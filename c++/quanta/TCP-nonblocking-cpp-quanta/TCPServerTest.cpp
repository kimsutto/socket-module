#include "TCPServer.h"
#include <string.h>

int port = 7001;

int main(int argc, char** argv)
{
	if (argc > 1) {
		port = atoi(argv[1]);
	}
	printf("PORT: %d\n", port);
	CTCPServer server(port);

#ifdef WIN32
	while (!GetAsyncKeyState(VK_ESCAPE))
#else
	while (true)
#endif
	{
		server.Listen(); // check for new connection
		char temp[512];
		int ds = 512;
		if (server.Receive(temp, ds) == CTCPServer::OK) { // receive data
			printf("%s\n", temp);
			char newData[512];
			strcpy(newData, "Received: ");
			strcat(newData, temp);
			ds = 512;
			server.ReplyToClients(newData, ds); // send data to all connected clients
		}
	}
	return 0;
}
