//
// 2008.08 Kyoung Shin Park 
//
#include "QUANTAinit.hxx"
#include "UDPServer.h"
#include <string.h>

CUDPServer::CUDPServer(int port, int numClients)
{
	QUANTAinit();
	m_UDPSocket = new QUANTAnet_udp_c; // obtain a udp socket
	m_UDPSocket->init(port); // bind the socket to the port
	m_UDPSocket->makeNonBlocking(); // set non-blocking mode
	m_NumOfClients=0;
	m_BunchSocket = new QUANTAnet_udp_c*[numClients];
}

CUDPServer::~CUDPServer()
{
	if (m_UDPSocket)
		delete m_UDPSocket;
	m_UDPSocket = NULL;
}

void CUDPServer::CheckClient(QUANTAnet_udp_c *m_UDPSocket) // check for new client (if so, make a copy)
{
	QUANTAnet_udp_c *eachClient;

	for (int i = 0; i < m_NumOfClients; i++)
	{
		eachClient = m_BunchSocket[i];
		if ((eachClient->getSendIP() == m_UDPSocket->getReceiveIP()) &&
		    (eachClient->getSendPort() == m_UDPSocket->getReceivePort())) return;
	}
	printf("NEW CLIENT\n\n");
	m_BunchSocket[m_NumOfClients] = m_UDPSocket->clone();
	m_BunchSocket[m_NumOfClients]->copyReceiveAddressToSendAddress();
	m_NumOfClients++;
}

void CUDPServer::ReplyToClients() // send data to all clients
{
	static int ok = 0; ok++;
	char mesg[256];
	for(int i = 0 ; i<m_NumOfClients; i++)
	{
		sprintf(mesg,"HOWDY CLIENT# %d\tTOTAL CLIENTS: %d MESSAGE: %d\n", i, m_NumOfClients, ok);
		m_BunchSocket[i]->send(mesg, strlen(mesg)+1,QUANTAnet_udp_c::NON_BLOCKING); // sendto
	}
}

void CUDPServer::Receive() // receive data & check new client & reply back (send)
{
	char recvline[MAXSIZE];

	while(1) {
		if (m_UDPSocket->receive(recvline, MAXSIZE,QUANTAnet_udp_c::BLOCKING) > 0) { // recvfrom
			printf("RECEIVED: %s",recvline);
		}

		CheckClient(m_UDPSocket);
		ReplyToClients();
	}
}

