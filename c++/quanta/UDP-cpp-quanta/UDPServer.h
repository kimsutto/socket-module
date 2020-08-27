//
// 2008.08 Kyoung Shin Park 
//
#ifndef __UDP_SERVER_H__
#define __UDP_SERVER_H__

#include "QUANTAnet_udp_c.hxx"
#include "QUANTAts_thread_c.hxx"

#define MAXSIZE 256
#define BUNCHOFCLIENTS 5

class CUDPServer
{
public:
	CUDPServer(int port=8001, int numClients = BUNCHOFCLIENTS);
	virtual ~CUDPServer();
	void CheckClient(QUANTAnet_udp_c *mUDPSocket);
	void ReplyToClients();
	void Receive();

protected:
	int m_NumOfClients;
	QUANTAnet_udp_c* m_UDPSocket;
	QUANTAnet_udp_c** m_BunchSocket;
};

#endif 
