//
// 2008.08 Kyoung Shin Park 
//

#ifndef __UDP_CLIENT_H__
#define __UDP_CLIENT_H__

#include "QUANTAnet_udp_c.hxx"

#define MAX_LINE 1024

class CUDPClient
{
public:
	CUDPClient(char* IP=NULL, int port=8001);
	virtual ~CUDPClient();

	void SetSendAddress(char* IP, int port);
	void Send(char* sendmsg);
	void Receive(char* recvline);

protected:
	QUANTAnet_udp_c* m_UDPSocket;
};

#endif 
