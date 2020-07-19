#include <stdio.h>
#include <stdlib.h>
#include <Winsock2.h>
#include <windows.h>
#include <pthread.h>

#define BUF_SIZE 1024
#define CLIENT_SIZE 100

int client_fds[CLIENT_SIZE];
int client_cnt = 0;

//在线客户端数量查询 
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

//推送转发
void sendToAll(int currentfd, char *buff, int size)
{
    for(int i=0; i<CLIENT_SIZE; i++)
    {
        if(client_fds[i]!=currentfd && client_fds[i]!=-1)
        {
            int r = send(client_fds[i],buff,size,0);
            if(r<0){
                printf("【系统提醒】 %d已退出\n\n", client_fds[i]);
                close(client_fds[i]);
                client_fds[i]=-1;
            }
        }
    }
}

//客户端数据接收进程 
void* connectAndWait(void *arg)
{
    char revData[BUF_SIZE];
    int sockfd = *((int *)arg);

    send(sockfd, "【服务器】 Hello, Client!", 25, 0);
    sendToAll(sockfd, "【服务器】 有新的客户端接入.", 30);

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

//与客户端数据发送进程 
void* chat (void)
{
    while(1)
    {
        char SendBuffer[BUF_SIZE];
        char sendBuffer[BUF_SIZE];
        memset(SendBuffer, 0, BUF_SIZE);
		memset(sendBuffer, 0, BUF_SIZE);
        gets(sendBuffer);

        sprintf(SendBuffer, "【服务器】: %s", sendBuffer);

		for(int i=0; i<CLIENT_SIZE; i++)
        {
            if(client_fds[i]!=-1)
            {
                int r = send(client_fds[i],SendBuffer,strlen(SendBuffer),0);
                //伪心跳机制监测在线客户端状态 
                if(r<0){
                    printf("【系统提醒】 %d已退出\n\n", client_fds[i]);
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
		printf("初始化windows socket失败.\n");
		return -1;
	}

    pthread_t t_id;
    for(int i=0; i<CLIENT_SIZE; i++)
        client_fds[i] = -1;

    printf("---------------------------------------------\n");
    printf("            Socket 服务器端程序\n");
    printf("---------------------------------------------\n\n");

	//create Socket
	printf("【系统提醒】 服务器端开始创建SOCKET\n\n");
	hServer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (hServer == INVALID_SOCKET)
	{
		printf("【ERROR】 创建Server Socket失败.\n");
		return -1;
	}

	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servAddr.sin_port = htons(8080);
	if (bind(hServer, (struct sockaddr*)&servAddr,
			sizeof(servAddr)) == SOCKET_ERROR)
	{
		printf("Server Socket绑定失败.\n");
		return -1;
	}
	printf("【系统提醒】 服务器端口 8080\n\n");


	// listen
	if (listen(hServer, 10) == SOCKET_ERROR)
	{
		printf("Server 监听失败.\n");
		return -1;
	}
	printf("【系统提醒】 服务器绑定监听成功\n\n");
	printf("-------------------------\n\n");

	int rc = pthread_create(&t_id, NULL, chat, NULL);
    if (rc)
        printf("创建新连接线程失败.\n");
    pthread_detach(t_id);

    //主进程-循环接收客户端连接
	int szClntAddr = sizeof(clntAddr);
	while(1)
    {
        if((hClient = accept(hServer, (struct sockaddr*)&clntAddr, &szClntAddr))<0)
        {
            printf("Server接受Client失败.\n");
            return -1;
        }

        client_fds[client_cnt++] = hClient;

        int rc = pthread_create(&t_id, NULL, connectAndWait, (void *)&hClient);
        if (rc)
            printf("创建新连接线程失败.\n");
        pthread_detach(t_id);
        printf("【系统提醒】 接收到一个来自【%s】的连接，目前共有%d台客户端在线.\n\n", inet_ntoa(clntAddr.sin_addr), countClient());
    }

	closesocket(hClient);
	closesocket(hServer);
	WSACleanup();

	return 0;
}
