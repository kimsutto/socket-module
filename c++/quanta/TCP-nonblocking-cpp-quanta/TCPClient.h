#ifndef __TCP_CLIENT_H__
#define __TCP_CLIENT_H__

#include "QUANTAnet_tcp_c.hxx"

/* This is the size of all messages sent from clients to servers. */
#define TRANSMITTED_DATA_SIZE 512

/* This is the size of all messages sent from servers to clients. */
#define BROADCAST_DATA_SIZE 1024

class CTCPClient
{
public:
	CTCPClient(char* IP = NULL, int port = 7001);
	CTCPClient(QUANTAnet_tcpClient_c* client);
	virtual ~CTCPClient();

	void SetSendAddress(char* IP, int port);
	void Send(char* sendmsg, int& dataSize);
	int Receive(char* recvline, int& dataSize, int blockingType = QUANTAnet_tcpClient_c::BLOCKING);

	void setClient(QUANTAnet_tcpClient_c* client);

protected:
	QUANTAnet_tcpClient_c* m_Client;
};
#endif