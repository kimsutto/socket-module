#include "QUANTAinit.hxx"
#include "TCPServer.h"

const int CTCPServer::OK = 1;
const int CTCPServer::FAIL = 0;

CTCPServer::CTCPServer(int port, int numClients)
{
	QUANTAinit();
	m_Server = new QUANTAnet_tcpServer_c; // obtain a tcp socket
	m_Server->init(port); // bind the socket to the port & listen
	m_NumOfClients = 0;
	m_BunchSocket = new CTCPClient*[numClients];
}

CTCPServer::~CTCPServer()
{
	if (m_Server) delete m_Server;
}

CTCPClient* CTCPServer::Listen() // check for new connection from clients
{
	QUANTAnet_tcpClient_c* client = m_Server->checkForNewConnections(); // accept new connection from client
	if (client) {
		CTCPClient* tcpClient = new CTCPClient(client);
		m_BunchSocket[m_NumOfClients] = tcpClient;
		m_NumOfClients++;
		return tcpClient;
	}
	return NULL;
}

int CTCPServer::Receive(char* recvmesg, int& dataSize) // receive data from all connected clients
{
	for (int i = 0; i < m_NumOfClients; i++) {
		int ds = dataSize;

		int status = m_BunchSocket[i]->Receive(recvmesg, ds, QUANTAnet_tcpClient_c::NON_BLOCKING); // receive data (read)
		if (status == QUANTAnet_tcpClient_c::OK) {
			dataSize = ds;
			return OK;
		}
	}
	return FAIL;
}

void CTCPServer::ReplyToClients(char* sendmesg, int dataSize) // send data to all connected clients
{
	for (int i = 0; i < m_NumOfClients; i++) {
		int ds = dataSize;

		m_BunchSocket[i]->Send(sendmesg, ds); // send data (write)
	}
}