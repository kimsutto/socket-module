#include "TCPClient.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>

#define LOCALHOST "127.0.0.1"
int port = 7002;
char hostname[128];
CTCPClient client;

void readInput(std::string& line)
{
	std::cout << "Enter a city name to find (or enter EXIT): ";
	std::getline(std::cin, line);
	if (line == "EXIT")
	{
			printf("TCP CLIENT ENDED\n");
			exit(EXIT_SUCCESS);
	}
}

int main(int argc, char** argv)
{
	strcpy(hostname, LOCALHOST);
	if (argc > 1) strcpy(hostname, argv[1]);
	if (argc > 2) port = atoi(argv[2]);
	printf("HOSTNAME: %s PORT: %d\n", hostname, port);

	client.SetSendAddress(hostname, port); // client connect to server

	std::string buffer;
	int ds = TRANSMITTED_DATA_SIZE;
	char recvbuffer[TRANSMITTED_DATA_SIZE];
	while (true) {
		readInput(buffer);
		client.Send((char*) buffer.c_str(), ds); // send data
		client.Receive(recvbuffer, ds); // receive data
		printf("TCP CLIENT RECEIVE: %s\n", recvbuffer);
	}
	return 0;
}