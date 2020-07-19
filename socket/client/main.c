#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <Windows.h>
#include <pthread.h>

#define MAX_LEN 1024
#define IPV4(a,b,c,d) ((a<<0)|(b<<8)|(c<<16)|(d<<24))

char SendBuffer[MAX_LEN];
char sendBuffer[MAX_LEN];
char RecvBuffer[MAX_LEN];
char name[30];

//客户端接收数据进程 
void* RecvMessage(void *arg)
{
    char revData[MAX_LEN];
    int sockfd = *((int *)arg);

    while(1)
    {
        int ret = recv(sockfd, revData, 255, 0);
        if(ret > 0)
        {
            revData[ret] = 0x00;
            printf("%s\n\n", revData);
        }
        memset(RecvBuffer, 0, MAX_LEN);
    }

}

int main(int argc, char *argv[])
{
	WSADATA Ws;
	SOCKET hClient;
	struct sockaddr_in ServerAddr;
	int Ret = 0;

	//Init Windows Socket
	if (WSAStartup(MAKEWORD(2, 2), &Ws) != 0)
	{
		printf("Init Windows Socket Failed.\n");
		return -1;
	}

    pthread_t t_id;

	//Create Socket
	hClient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (hClient == INVALID_SOCKET)
	{
		printf("Create Socket Failed\n");
		return -1;
	}

	int IP_1=127, IP_2=0, IP_3=0, IP_4=1;
	unsigned short PORT = 8080;
	printf("---------------------------------------------\n");
    printf("            Socket 客户端程序\n");
    printf("---------------------------------------------\n\n");

    printf("请输入您的昵称：\n");
    scanf("%s%*c", name);

	//Socket与端口信息手动输入处理 
    printf("\n请输入服务器的IP地址和端口号：（例: 127.0.0.1 8080）\n");
    scanf("%d.%d.%d.%d %hd%*c", &IP_1, &IP_2, &IP_3, &IP_4, &PORT);

    unsigned int value=IPV4(IP_1,IP_2,IP_3,IP_4);
    unsigned int ip;
    memcpy(&ip,&value,sizeof(value));

	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_addr.s_addr = inet_addr(inet_ntoa(*((struct in_addr*)&ip)));
	ServerAddr.sin_port = htons(PORT);
	Ret = connect(hClient, (struct sockaddr*)&ServerAddr, sizeof(ServerAddr));
	if(Ret == SOCKET_ERROR)
	{
		printf("连接失败，请检查是否已打开服务器端程序.\n");
		return -1;
	}
	else
		printf("\nConnect OK!\n\n");

    printf("------------------------------\n\n");

	//创建接收信息子进程 
    int rc = pthread_create(&t_id, NULL, RecvMessage, (void *)&hClient);
    if (rc)
        printf("创建新连接线程失败.\n");
    pthread_detach(t_id);

	//循环发送数据 
	while(1)
    {
        gets(sendBuffer);

        sprintf(SendBuffer, "[%s]: %s", name, sendBuffer);
		Ret = send(hClient, SendBuffer, (int)strlen(SendBuffer), 0);
		if(Ret == SOCKET_ERROR)
		{
			printf("Send Info Error.\n");
			return -1;
		}
		printf("\n");
		memset(SendBuffer, 0, MAX_LEN);
		memset(sendBuffer, 0, MAX_LEN);
	}
}
