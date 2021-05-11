#include <WinSock2.h>
#include <process.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#pragma comment(lib,"ws2_32.lib")

#define MAXLINE 256
#define PORT 23800




DWORD WINAPI t_func(void *data)
{

	SOCKET sockfd = (SOCKET)data;
	int readn, writen;
	int addrlen;
	struct sockaddr_in sock_addr;
	char Buffer[MAXLINE];


	printf("Thread Start : %d\n", GetCurrentThreadId());

	memset(Buffer, 0x00, MAXLINE);
	while ((readn = recv(sockfd, Buffer, MAXLINE, 0)) > 0)
	{
		addrlen = sizeof(sock_addr);
		getpeername(sockfd, (struct sockaddr *)&sock_addr, &addrlen);

		printf("[Len %d][Thread %d]Read Data %s(%d) : %s\n", readn, GetCurrentThreadId(), inet_ntoa(sock_addr.sin_addr), ntohs(sock_addr.sin_port), Buffer);

		writen = send(sockfd, Buffer, readn, 0);

		memset(Buffer, 0x00, MAXLINE);
	}
	closesocket(sockfd);
	printf("Thread Close : %d\n", GetCurrentThreadId());
	return 0;
}

int main(int argc, char** argv)
{
	WSADATA wsa;
	SOCKET listen_s, client_s;
	struct sockaddr_in server_addr, client_addr;
	HANDLE hThread;
	int addr_len;

	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	listen_s = socket(AF_INET, SOCK_STREAM, 0);

	if (listen_s == INVALID_SOCKET)
		return 1;

	ZeroMemory(&server_addr, sizeof(struct sockaddr_in));

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(listen_s, (struct sockaddr*)&server_addr, sizeof(struct sockaddr_in)) == SOCKET_ERROR)
		return 0;

	if (listen(listen_s, 5) == SOCKET_ERROR)
		return 0;

	ZeroMemory(&client_addr, sizeof(struct sockaddr_in));

	while (1)
	{
		addr_len = sizeof(struct sockaddr_in);
		client_s = accept(listen_s, (struct sockaddr*)&client_addr, &addr_len);
		hThread = CreateThread(NULL, 0, t_func, (void *)client_s, 0, NULL);
		CloseHandle(hThread);
	}

	closesocket(listen_s);
	WSACleanup();
	return 0;
}

