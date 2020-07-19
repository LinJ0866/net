#include <stdio.h>
#include <stdlib.h>
#include <Winsock2.h>
#include <windows.h>
#include <pthread.h>

#define BUF_SIZE 1024
#define CLIENT_SIZE 100

int client_fds[CLIENT_SIZE];
int client_cnt = 0;

//���߿ͻ���������ѯ 
int countClient()
{
    int sum=0;
    for(int i=0; i<CLIENT_SIZE; i++)
    {
        if (client_fds[i] != -1)
            sum++;
    }
    return sum;
}

//����ת��
void sendToAll(int currentfd, char *buff, int size)
{
    for(int i=0; i<CLIENT_SIZE; i++)
    {
        if(client_fds[i]!=currentfd && client_fds[i]!=-1)
        {
            int r = send(client_fds[i],buff,size,0);
            if(r<0){
                printf("��ϵͳ���ѡ� %d���˳�\n\n", client_fds[i]);
                close(client_fds[i]);
                client_fds[i]=-1;
            }
        }
    }
}

//�ͻ������ݽ��ս��� 
void* connectAndWait(void *arg)
{
    char revData[BUF_SIZE];
    int sockfd = *((int *)arg);

    send(sockfd, "���������� Hello, Client!", 25, 0);
    sendToAll(sockfd, "���������� ���µĿͻ��˽���.", 30);

    while(1)
    {
        memset(revData, 0, CLIENT_SIZE);
        int ret = recv(sockfd, revData, 256, 0);
        if(ret > 0)
        {
            revData[ret] = 0x00;
            printf("%s\n\n", revData);
            sendToAll(sockfd, revData, ret);
        }
    }

}

//��ͻ������ݷ��ͽ��� 
void* chat (void)
{
    while(1)
    {
        char SendBuffer[BUF_SIZE];
        char sendBuffer[BUF_SIZE];
        memset(SendBuffer, 0, BUF_SIZE);
		memset(sendBuffer, 0, BUF_SIZE);
        gets(sendBuffer);

        sprintf(SendBuffer, "����������: %s", sendBuffer);

		for(int i=0; i<CLIENT_SIZE; i++)
        {
            if(client_fds[i]!=-1)
            {
                int r = send(client_fds[i],SendBuffer,strlen(SendBuffer),0);
                //α�������Ƽ�����߿ͻ���״̬ 
                if(r<0){
                    printf("��ϵͳ���ѡ� %d���˳�\n\n", client_fds[i]);
                    close(client_fds[i]);
                    client_fds[i]=-1;
                }
            }
        }
		printf("\n");
	}
}

int main(int argc, char *argv[]) {
	WSADATA wsaData;
	SOCKET hServer, hClient;
	struct sockaddr_in servAddr;
	struct sockaddr_in clntAddr;

	//Init Windows Socket
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		printf("��ʼ��windows socketʧ��.\n");
		return -1;
	}

    pthread_t t_id;
    for(int i=0; i<CLIENT_SIZE; i++)
        client_fds[i] = -1;

    printf("---------------------------------------------\n");
    printf("            Socket �������˳���\n");
    printf("---------------------------------------------\n\n");

	//create Socket
	printf("��ϵͳ���ѡ� �������˿�ʼ����SOCKET\n\n");
	hServer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (hServer == INVALID_SOCKET)
	{
		printf("��ERROR�� ����Server Socketʧ��.\n");
		return -1;
	}

	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servAddr.sin_port = htons(8080);
	if (bind(hServer, (struct sockaddr*)&servAddr,
			sizeof(servAddr)) == SOCKET_ERROR)
	{
		printf("Server Socket��ʧ��.\n");
		return -1;
	}
	printf("��ϵͳ���ѡ� �������˿� 8080\n\n");


	// listen
	if (listen(hServer, 10) == SOCKET_ERROR)
	{
		printf("Server ����ʧ��.\n");
		return -1;
	}
	printf("��ϵͳ���ѡ� �������󶨼����ɹ�\n\n");
	printf("-------------------------\n\n");

	int rc = pthread_create(&t_id, NULL, chat, NULL);
    if (rc)
        printf("�����������߳�ʧ��.\n");
    pthread_detach(t_id);

    //������-ѭ�����տͻ�������
	int szClntAddr = sizeof(clntAddr);
	while(1)
    {
        if((hClient = accept(hServer, (struct sockaddr*)&clntAddr, &szClntAddr))<0)
        {
            printf("Server����Clientʧ��.\n");
            return -1;
        }

        client_fds[client_cnt++] = hClient;

        int rc = pthread_create(&t_id, NULL, connectAndWait, (void *)&hClient);
        if (rc)
            printf("�����������߳�ʧ��.\n");
        pthread_detach(t_id);
        printf("��ϵͳ���ѡ� ���յ�һ�����ԡ�%s�������ӣ�Ŀǰ����%d̨�ͻ�������.\n\n", inet_ntoa(clntAddr.sin_addr), countClient());
    }

	closesocket(hClient);
	closesocket(hServer);
	WSACleanup();

	return 0;
}
