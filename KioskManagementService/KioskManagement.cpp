#include <atlbase.h>
#include <shellapi.h>
#include "ServiceBase.h"
#include "Utilities.h"

HANDLE _hMutex = NULL;

BOOL IsServiceAlreadyRunning()
{
    _hMutex = CreateMutex(NULL, TRUE, L"KioskManagement_Single_Instance_Mutex");
    return (GetLastError() == ERROR_ALREADY_EXISTS);
}

//-------------------------------------------------------------
//  _tWinMain
//    This function is the main entry point of the application.
//    A service object is created that represents the lifetime 
//    of this process.
//-------------------------------------------------------------
extern "C"
int WINAPI _tWinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nShowCmd)
{
	CUtilities::Dlog(L"Entering WinMain");
    int retVal = 0;
    CServiceBase * svc;

    // Check if WRConsumerService is running already.
    if (IsServiceAlreadyRunning())
    {
        //LogError that it is already running
		if (_hMutex)
			CloseHandle(_hMutex);
        return 0;
    }

	CUtilities::Dlog(L"Starting Service");

    svc = new CServiceBase(L"NetstopPrintingService", L"Netstop Print Service", L"Netstop Printer Control");
    retVal = svc -> WinMain (nShowCmd);
    delete svc;

	if (_hMutex)
		CloseHandle(_hMutex);

    return retVal;
}