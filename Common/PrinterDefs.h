
#pragma once

#include <windows.h>

#pragma pack(push, r1, 1)

/* Version 5.0 
#define VERSION_NUMBER TEXT("5.0")
#define NETSTOP_BASE_KEY TEXT("Software\\Moonrise Systems Inc\\NetStopPro")
#define NETSTOP_REG_KEY TEXT("Software\\Moonrise Systems Inc\\NetStopPro\\5.0")
#define SERVICE_PRINTER_REG_KEY NETSTOP_REG_KEY;
#define SERVICE_PRINTER_IPC_REG_KEY TEXT("Software\\Moonrise Systems Inc\\NetStopPro\\5.0\\PrinterIPC")
*/

/* Version 6.0 */
#define VERSION_NUMBER TEXT("6.0")
#define NETSTOP_BASE_KEY TEXT("Software\\Moonrise Systems Inc\\NetStopPro")
#define NETSTOP_REG_KEY TEXT("Software\\Moonrise Systems Inc\\NetStopPro\\6.0")
#define SERVICE_PRINTER_REG_KEY NETSTOP_REG_KEY;
#define SERVICE_PRINTER_IPC_REG_KEY TEXT("Software\\Moonrise Systems Inc\\NetStopPro\\6.0\\PrinterIPC")
 

#define X86_INJECTION_DRIVER_FILE_NAME  L"KlInjDriver32.sys"
#define X64_INJECTION_DRIVER_FILE_NAME  L"KlInjDriver64.sys"
#define X86_INJECTION_LIBRARY_FILE_NAME  L"PrintMonitor32.dll"
#define X64_INJECTION_LIBRARY_FILE_NAME  L"PrintMonitor64.dll"

/*
#define X86_INJECTION_DRIVER_FILE_NAME  L"DemoDriver32.sys"
#define X64_INJECTION_DRIVER_FILE_NAME  L"DemoDriver64.sys"
#define X86_INJECTION_LIBRARY_FILE_NAME  L"HookProcessCreation32.dll"
#define X64_INJECTION_LIBRARY_FILE_NAME  L"HookProcessCreation64.dll"
*/
#define INJECTION_SERVICE_NAME L"KioskLogixAPI"
#define INJECTION_SERVICE_DESC L"KioskLogix Application Control"

enum PrintAPI:BYTE {paCreateDC, paStartDoc, paEndDoc, paStartPage, paEndPage, paNoActivity};

struct PrintNotification
{
    PrintAPI printAPI;
	char appName[255];
    char printerName[255];
	__int64 threadId;
    unsigned __int32 numberOfPagesPrinted;
    unsigned __int32 numberOfPagesRejected;
	unsigned __int32 copies;
};

struct PrintReply
{
    DWORD printingReceipt;
	DWORD pagesAllowed;
	DWORD allowAction;
};

struct PrintControl
{
    int pagesAllowed;
    int printingReceipt;
};

struct HookRec
{
    PrintControl printControl;
    PrintNotification printNotification;
};

#pragma pack(pop, r1)
