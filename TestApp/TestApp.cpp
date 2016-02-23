// TestApp.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Utilities.h"
#include "PrinterIPC.h"

#define INJECTION_SERVICE_NAME  L"KioskLogixAPI"
#define X86_INJECTION_DRIVER_FILE_NAME  L"KlInjDriver32.sys"
#define X64_INJECTION_DRIVER_FILE_NAME  L"KlInjDriver64.sys"

// ***************************************************************

WCHAR driverFile32 [MAX_PATH + 1];
WCHAR driverFile64 [MAX_PATH + 1];

void InitDriver32File()
// dllFile is set to "ourExePath\*.dll"
{
  GetModuleFileNameW(NULL, driverFile32, MAX_PATH);
  for (int i1 = lstrlenW(driverFile32) - 1; i1 > 0; i1--)
    if (driverFile32[i1] == L'\\') {
      driverFile32[i1 + 1] = 0;
      break;
    }
  lstrcatW(driverFile32, X86_INJECTION_DRIVER_FILE_NAME);
}

void InitDriver64File()
// dllFile is set to "ourExePath\*.dll"
{
  GetModuleFileNameW(NULL, driverFile64, MAX_PATH);
  for (int i1 = lstrlenW(driverFile64) - 1; i1 > 0; i1--)
    if (driverFile64[i1] == L'\\') {
      driverFile64[i1 + 1] = 0;
      break;
    }
  lstrcatW(driverFile64, X86_INJECTION_DRIVER_FILE_NAME);
}

int _tmain(int argc, _TCHAR* argv[])
{

	//CPrinterIPC map;
	//map.SetPrintAPI( paEndDoc );

	//return 0;

	PrintNotification* pn = new PrintNotification();
	pn->numberOfPagesPrinted = 1;
	pn->numberOfPagesRejected = 2;
	pn->printAPI = paCreateDC;
	strcpy(pn->printerName, CW2A(TEXT("Test PRINTER")));

	DWORD size;
	PrintReply* reply = new PrintReply();

	DLogArg(L"Size Of PrintReply = %d", sizeof(PrintReply));

	if ( 0 == CallNamedPipe(TEXT("\\\\.\\pipe\\PrintMonitorPipe"), pn, sizeof(PrintNotification), reply, sizeof(PrintReply), &size, NMPWAIT_WAIT_FOREVER) )
	{
		DLogArg(L"%s", CUtilities::GetErrorString( GetLastError() ));
	}
	else
	{
		DLogArg(L"Error Code = %d", reply->allowAction);
	}

	delete pn;
	delete reply;

	//CUtilities::PrintLog(logLine);
	return 0;
}

