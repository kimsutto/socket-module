//
// 2008.08 Kyoung Shin Park 
//

#include "UDPClient.h"
#include <string.h>
#include <stdio.h>
#include <string>
#include <iostream>

#define LOCALHOST "127.0.0.1"
int port = 8001;
char hostname[128];

void readInput(std::string& buffer)
{
	std::cout << "Type your text: ";
	std::getline(std::cin, buffer);
}

int main (int argc, char** argv)
{
	strcpy(hostname, LOCALHOST);
	if (argc > 1) strcpy(hostname, argv[1]);
	if (argc > 2) port = atoi(argv[2]);	
	printf("HOSTNAME: %s PORT: %d\n", hostname, port);

	CUDPClient client(hostname, port);

	char temp[512];
#ifdef WIN32
	while (!GetAsyncKeyState(VK_ESCAPE)) {
#else
	while (true) {
#endif
		std::string buffer;
		readInput(buffer);
		client.Send((char *)buffer.c_str());
		client.Receive(temp);
		printf("UDP CLIENT RECEIVE: %s\n", temp);
	}
	return 0;
}

