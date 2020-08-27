#ifndef __TCP_SERVER_H__
#define __TCP_SERVER_H__

#include "QUANTAnet_tcp_c.hxx"
#include "TCPClient.h"

#define BUNCHOFCLIENTS 5

class CTCPServer
{
public:
	static const int OK;
	static const int FAIL;

	CTCPServer(int port=7001, int numClients = BUNCHOFCLIENTS);
	virtual ~CTCPServer();

	CTCPClient* Listen();
	int Receive(char* recvmesg, int& dataSize); 
	void ReplyToClients(char* sendmesg, int dataSize);

protected:
	QUANTAnet_tcpServer_c* m_Server;
	int m_NumOfClients;
	CTCPClient** m_BunchSocket;
};

#endif