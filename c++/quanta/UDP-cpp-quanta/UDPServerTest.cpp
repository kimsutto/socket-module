//
// 2008.08 Kyoung Shin Park 
//

#include "UDPServer.h"
int port = 8001;

int main (int argc, char** argv)
{
	if (argc > 1) {
		port = atoi(argv[1]);
	}
	printf("PORT: %d\n", port);

	CUDPServer server(port);
	server.Receive(); // receive data & check new client & reply back (send)
	return 0;
}

