//
// 2008.08 Kyoung Shin Park 
//
#include "QUANTAinit.hxx"
#include "UDPClient.h"
#include <string.h>

CUDPClient::CUDPClient(char* IP, int port)
{
	QUANTAinit();
	m_UDPSocket = new QUANTAnet_udp_c; // obtain a udp socket

	// open a dynamic port since we are a client and don't need to commit to any one port
	m_UDPSocket->init();

	// we recommend that you use the blocking mode option in 
	// the send() and recieve() instead of calling makeNonBlocking()
	m_UDPSocket->makeNonBlocking();

	// we want to use the socket to send data to the server
	m_UDPSocket->setSendAddress(IP, port);
}

CUDPClient::~CUDPClient()
{
	if (m_UDPSocket)
		delete m_UDPSocket;
	m_UDPSocket = NULL;
}

void CUDPClient::SetSendAddress(char* IP, int port)
{
	if (m_UDPSocket) {
		m_UDPSocket->setSendAddress(IP, port);
	}
}

void CUDPClient::Send(char* sendmsg)
{
	if (m_UDPSocket) {
		int n = strlen(sendmsg) + 1;
		printf("UDPCLIENT sendmessage:%s len:%d\n", sendmsg, n);
		if (m_UDPSocket->send(sendmsg, n) != n) // sendto
			printf("Client sendto error\n");
	}
}

void CUDPClient::Receive(char* recvline) 
{
	if (m_UDPSocket) {
		int n = m_UDPSocket->receive(recvline, MAX_LINE); // recvfrom
		if (n < 0)  
			if (n == QUANTAnet_udp_c::NON_BLOCKING_HAS_NO_DATA)
				printf("Nonblocking has no data\n");
			else
				printf("Client receive error\n");
		else {
			printf("RECV SIZE %d ",n);
			printf("%s\n", recvline);
		}
	}
}
