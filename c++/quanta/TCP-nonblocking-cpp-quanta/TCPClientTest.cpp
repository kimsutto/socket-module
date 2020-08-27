#include "TCPClient.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>

#define LOCALHOST "127.0.0.1"
int port = 7001;
char hostname[128];

void readInput(char* buffer)
{
	printf("Type your text: ");
	fgets(buffer, 512, stdin);
}

int main(int argc, char** argv)
{
	strcpy(hostname, LOCALHOST);
	if (argc > 1) strcpy(hostname, argv[1]);
	if (argc > 2) port = atoi(argv[2]);
	printf("HOSTNAME: %s PORT: %d\n", hostname, port);

	CTCPClient client(hostname, port); // client connect to server

	char temp[512];
	int ds = 512;
#ifdef WIN32
	while (!GetAsyncKeyState(VK_ESCAPE)) {
#else
	while (true) {
#endif
		readInput(temp);
		client.Send(temp, ds); // send data
		client.Receive(temp, ds); // receive data
		printf("TCP CLIENT RECEIVE: %s\n", temp);
	}
	return 0;
}
