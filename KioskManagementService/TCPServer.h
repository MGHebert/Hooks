#pragma once

#include "BaseThread.h"
#include "threaded-server.h"

class CTCPServer: public CBaseThread
{
public:
	
protected:
	// BaseThread overrides
    HRESULT OnThreadBegin() throw();
    HRESULT OnThreadEnd() throw();
	HRESULT OnThreadExecute() throw();
private:
	SOCKET m_socket;
};
