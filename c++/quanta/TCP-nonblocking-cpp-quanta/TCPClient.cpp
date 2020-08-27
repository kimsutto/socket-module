#include "QUANTAinit.hxx"
#include "TCPClient.h"

CTCPClient::CTCPClient(char* IP, int port) 
{
	QUANTAinit();
	m_Client = new QUANTAnet_tcpClient_c; // obtain a socket
	if (IP != NULL && port != 0) 
		m_Client->connectToServer(IP, port); // connect to the server (remote_ip, port)
}

CTCPClient::CTCPClient(QUANTAnet_tcpClient_c* client)
{
	m_Client = client;
}

CTCPClient::~CTCPClient() 
{
	if (m_Client) {
		delete m_Client;
	}
}

void CTCPClient::SetSendAddress(char* IP, int port)
{
	m_Client->connectToServer(IP, port);
}

void CTCPClient::Send(char* sendmsg, int& dataSize)
{
	m_Client->write(sendmsg, &dataSize, QUANTAnet_tcpClient_c::BLOCKING); // send data (write)
}

int CTCPClient::Receive(char* recvline, int& dataSize, int blockingType)
{
	int status = m_Client->read(recvline, &dataSize, blockingType); // receive data (read)
	return status;
}

void CTCPClient::setClient(QUANTAnet_tcpClient_c* client)
{
	m_Client = client;
}