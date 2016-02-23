#include "TCPServer.h"

HRESULT CTCPServer::OnThreadBegin() throw()
{
	m_socket = SetUpListener("127.0.0.1", 9999);
	return S_OK;
}


HRESULT CTCPServer::OnThreadEnd() throw()
{
	ShutDownListener(m_socket);
	return S_OK;
}

HRESULT CTCPServer::OnThreadExecute() throw()
{
	OnThreadBegin();

	while (!m_terminated)
	{
		AcceptConnections(m_socket);
	}

	OnThreadEnd();
	return S_OK;
}
