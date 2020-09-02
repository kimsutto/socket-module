
#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h> 
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include <math.h>
.

void extractLinks(char* pgBuf, int numBytes, int upper) {
	printf("\n");
	char* tok;
	char* tok2;

	printf("Links (uppercase): \n");

	char* start = strstr(pgBuf, "HREF=\"");
	char* start2 = strstr(pgBuf, ".html\">");

	char* str1;
	str1 = (char*)malloc(20000);
	char* str2;
	str2 = (char*)malloc(20000);

	while (start != NULL) {

		strcpy(str1, start);
		strcpy(str2, start2);

		tok2 = strtok(str1 + 6, "\"");
		tok = strtok(str2 + 7, "<");

		printf("%s - %s\n", tok, tok2);

		start = strstr(start + 1, "HREF=\"");
		start2 = strstr(start2 + 1, ".html\">");
	}

	printf("Links (lowercase): \n");



}


int main(int argc, char** argv) {

	char* host;
	char* svc;
	int status;
	int s;
	struct sockaddr_in server_addr;
	struct sockaddr_in ad;
	const int maxbytes = 256;
	const int maxpagesize = 20480;
	char pageBuffer[maxpagesize];
	char command[maxbytes];
	char* cmdptr;
	int addr;
	struct addrinfo h;
	struct addrinfo* ip;



	if (argc < 3) {
		printf("Usage: nc <host> <svc>\n");
		exit(1);
	}

	host = argv[1];
	svc = argv[2];


	memset(&h, 0, sizeof(h));
	h.ai_family = AF_UNSPEC;
	h.ai_socktype = SOCK_STREAM; 

	addr = getaddrinfo(host, svc, &h, &ip);

	struct hostent* host_ip;
	host_ip = gethostbyname(host);

	unsigned int ret = 0;
	char* c_ip;
	char* c;

	c_ip = inet_ntoa(*(struct in_addr*)host_ip->h_addr_list[0]);
	for (int i = 3;i >= 0;i--) {
		c = strtok(c_ip, ".");
		ret += pow(256, i) * atoi(c);
		c_ip = NULL;
	}

	printf("Addr: %08X, port: %s\n", ret, svc);


!
	s = socket(PF_INET, SOCK_STREAM, 0);

	if (s < 0) {
		printf("Socket call failed\n");
		exit(1);

	}

	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(80);

	memcpy((void*)(&server_addr.sin_addr), (void*)(&((struct sockaddr_in*)ip->ai_addr)->sin_addr), sizeof(server_addr.sin_addr));

	status = connect(s, (struct sockaddr*) & server_addr, sizeof(server_addr));

	if (status < 0) {
		printf("connect call failed\n");
		exit(1);
	}

	command[0] = 0;
	printf("Command: ");
	//http method + url 
	cmdptr = fgets(command, maxbytes, stdin);


	printf("Sending: |%s|, len=%d\n", command, (unsigned int)strlen(command));



	send(s, cmdptr, strlen(cmdptr) + 1, 0);

	printf("Waiting:\n");
	sleep(1);


	//
	memset(pageBuffer, '\0', maxpagesize + 1);
	read(s, pageBuffer, maxpagesize);

	printf("%s\n", pageBuffer);





	close(s);

	extractLinks(pageBuffer, maxpagesize, 0);

	printf("Done!\n");
}
