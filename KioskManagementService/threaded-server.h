#include "ws-util.h"
#include "Utilities.h"
#include <winsock.h>
#include <iostream>

//// Prototypes ////////////////////////////////////////////////////////

SOCKET SetUpListener(const char* pcAddress, int nPort);
void ShutDownListener(SOCKET s);
void AcceptConnections(SOCKET ListeningSocket);
bool EchoIncomingPackets(SOCKET sd);
int DoWinsock(const char* pcAddress, int nPort);