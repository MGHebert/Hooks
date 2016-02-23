/***********************************************************************
 threaded-server.cpp - Implements a simple Winsock server that accepts
    connections and spins each one off into its own thread, where it's
    treated as a blocking socket.

    Each connection thread reads data off the socket and echoes it
    back verbatim.

 Compiling:
    VC++: cl -GX threaded-server.cpp main.cpp ws-util.cpp wsock32.lib
    BC++: bcc32 threaded-server.cpp main.cpp ws-util.cpp
    
 This program is hereby released into the public domain.  There is
 ABSOLUTELY NO WARRANTY WHATSOEVER for this product.  Caveat hacker.
***********************************************************************/

/*
#include "ws-util.h"

#include "Utilities.h"

#include <winsock.h>

#include <iostream>
*/
#include "threaded-server.h"
using namespace std;


//// Constants /////////////////////////////////////////////////////////

const int kBufferSize = 1024;
        

//// DoWinsock /////////////////////////////////////////////////////////
// The module's driver function -- we just call other functions and
// interpret their results.

int DoWinsock(const char* pcAddress, int nPort)
{
	CUtilities::Dlog(L"Establishing the listener...");
    SOCKET ListeningSocket = SetUpListener(pcAddress, htons(nPort));
    if (ListeningSocket == INVALID_SOCKET) 
	{
        CUtilities::DlogArg(L"%s", WSAGetLastErrorMessage("establish listener"));
        return 3;
    }

    CUtilities::Dlog(L"Waiting for connections...");
    while (1) 
	{
        AcceptConnections(ListeningSocket);
        CUtilities::Dlog(L"Acceptor restarting...");
    }

#if defined(_MSC_VER)
    return 0;   // warning eater
#endif
}


//// SetUpListener /////////////////////////////////////////////////////
// Sets up a listener on the given interface and port, returning the
// listening socket if successful; if not, returns INVALID_SOCKET.

SOCKET SetUpListener(const char* pcAddress, int nPort)
{
    u_long nInterfaceAddr = inet_addr(pcAddress);
    if (nInterfaceAddr != INADDR_NONE)
	{
        SOCKET sd = socket(AF_INET, SOCK_STREAM, 0);
        if (sd != INVALID_SOCKET) 
		{
            sockaddr_in sinInterface;
            sinInterface.sin_family = AF_INET;
            sinInterface.sin_addr.s_addr = nInterfaceAddr;
            sinInterface.sin_port = nPort;
            if (bind(sd, (sockaddr*)&sinInterface, sizeof(sockaddr_in)) != SOCKET_ERROR) 
			{
                listen(sd, SOMAXCONN);
                return sd;
            }
            else 
			{
                CUtilities::DlogArg(L"%s", WSAGetLastErrorMessage("bind() failed"));
            }
        }
    }

    return INVALID_SOCKET;
}

void ShutDownListener(SOCKET s)
{
	closesocket(s);
}

//// EchoHandler ///////////////////////////////////////////////////////
// Handles the incoming data by reflecting it back to the sender.

DWORD WINAPI EchoHandler(void* sd_) 
{
    int nRetval = 0;
    SOCKET sd = (SOCKET)sd_;

    if (!EchoIncomingPackets(sd)) 
	{
        //CUtilities::DlogArg(L"%s", WSAGetLastErrorMessage("Echo incoming packets failed"));
        nRetval = 3;
    }

    //CUtilities::Dlog(L"Shutting connection down...");
    if (ShutdownConnection(sd)) 
	{
        //CUtilities::Dlog(L"Connection is down.");
    }
    else 
	{
        //CUtilities::DlogArg( L"%s", WSAGetLastErrorMessage("Connection shutdown failed") );
        nRetval = 3;
    }

    return nRetval;
}


//// AcceptConnections /////////////////////////////////////////////////
// Spins forever waiting for connections.  For each one that comes in, 
// we create a thread to handle it and go back to waiting for
// connections.  If an error occurs, we return.

void AcceptConnections(SOCKET ListeningSocket)
{
    sockaddr_in sinRemote;
    int nAddrSize = sizeof(sinRemote);

    while (1) {
        SOCKET sd = accept(ListeningSocket, (sockaddr*)&sinRemote,
                &nAddrSize);
        if (sd != INVALID_SOCKET) 
		{
			CUtilities::DlogArg( L"Accepted connection from %s:%d.", inet_ntoa(sinRemote.sin_addr), ntohs(sinRemote.sin_port) );

            DWORD nThreadID;
            CreateThread(0, 0, EchoHandler, (void*)sd, 0, &nThreadID);
        }
        else 
		{
            CUtilities::DlogArg( L"%s", WSAGetLastErrorMessage("accept() failed"));
            return;
        }
    }
}


//// EchoIncomingPackets ///////////////////////////////////////////////
// Bounces any incoming packets back to the client.  We return false
// on errors, or true if the client closed the socket normally.

bool EchoIncomingPackets(SOCKET sd)
{
    // Read data from client
    char acReadBuffer[kBufferSize];
    int nReadBytes;
    do {
        nReadBytes = recv(sd, acReadBuffer, kBufferSize, 0);
        if (nReadBytes > 0) {
            cout << "Received " << nReadBytes << 
                    " bytes from client." << endl;
        
            int nSentBytes = 0;
            while (nSentBytes < nReadBytes) {
                int nTemp = send(sd, acReadBuffer + nSentBytes,
                        nReadBytes - nSentBytes, 0);
                if (nTemp > 0) {
                    cout << "Sent " << nTemp << 
                            " bytes back to client." << endl;
                    nSentBytes += nTemp;
                }
                else if (nTemp == SOCKET_ERROR) {
                    return false;
                }
                else {
                    // Client closed connection before we could reply to
                    // all the data it sent, so bomb out early.
                    cout << "Peer unexpectedly dropped connection!" << 
                            endl;
                    return true;
                }
            }
        }
        else if (nReadBytes == SOCKET_ERROR) {
            return false;
        }
    } while (nReadBytes != 0);

    cout << "Connection closed by peer." << endl;
    return true;
}


