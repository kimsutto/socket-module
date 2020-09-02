#pragma comment(lib, "ws2_32")
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>


#define BUF_SIZE 1024
void ErrorHandling(char *message);

int main(int argc, char *argv[])
{
	WSADATA wsaData;
	SOCKET hSocket;
	char message[BUF_SIZE], m[BUF_SIZE] = "";
	int strLen;
	SOCKADDR_IN servAdr;



	if (argc != 3) {
		printf("Usage : %s <IP> <port>\n", argv[0]);
		exit(1);
	}

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		ErrorHandling("WSAStartup() error!");

	hSocket = socket(PF_INET, SOCK_STREAM, 0);
	if (hSocket == INVALID_SOCKET)
		ErrorHandling("socket() error");

	memset(&servAdr, 0, sizeof(servAdr));
	servAdr.sin_family = AF_INET;
	servAdr.sin_addr.s_addr = inet_addr(argv[1]);
	servAdr.sin_port = htons(atoi(argv[2]));

	if (connect(hSocket, (SOCKADDR*)&servAdr, sizeof(servAdr)) == SOCKET_ERROR)
		ErrorHandling("connect() error!");
	else
		puts("Connected...........");


	while (1)
	{

		for (int i = 0; i < 3; i++) {
			fputs("Input message(Q to quit): ", stdout);
			fgets(message, BUF_SIZE, stdin);
			if (!strcmp(message, "q\n") || !strcmp(message, "Q\n"))
				break;
			message[strlen(message) - 1] = '\0';
			strcat(m, message);
		}
		if (!strcmp(message, "q\n") || !strcmp(message, "Q\n"))
			break;
		send(hSocket, m, strlen(m), 0);
		strLen = recv(hSocket, m, BUF_SIZE - 1, 0);
		m[strLen] = 0;

		printf("Message from server: %s\n", m);
		m[0] = '\0';
	}


	closesocket(hSocket);
	WSACleanup();
	return 0;
}

void ErrorHandling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}