// ***************************************************************
//  HookProcessCreation.dll   version: 1.1.0  ·  date: 2010-01-10
//  -------------------------------------------------------------
//  hook all process creation calls and ask for confirmation
//  -------------------------------------------------------------
//  Copyright (C) 1999 - 2010 www.madshi.net, All Rights Reserved
// ***************************************************************

// 2010-01-10 1.1.0 adjustments for madCodeHook 3
// 2003-06-15 1.0   initial release

// ***************************************************************
#include <atlstr.h>
#include <windows.h>
#include <winerror.h>
#include <atlbase.h>
#include "madCHook.h"
#include "Utilities.h"
#include "PrinterIPC.h"

// ***************************************************************

CPrinterIPC *FMap;

DWORD pagesPrinted;
DWORD copies;
DWORD pagesRejected;
bool startPage;
bool rejectingPages;
DWORD pagesAllowed;
DWORD printingReceipt;
bool lockable = false;
bool isPS;
bool isIE8Job;

CRITICAL_SECTION m_CriticalSection;

/******************************************************************
  Printer Redirects
******************************************************************/
DWORD (WINAPI *CreateDCANext) (LPCSTR lpszDriver,  __in  LPCSTR lpszDevice, LPCSTR lpszOutput,  __in  const DEVMODEA *lpInitData);
DWORD (WINAPI *CreateDCWNext) (LPWSTR lpszDriver,  __in  LPWSTR lpszDevice, LPWSTR lpszOutput,  __in  const DEVMODEW *lpInitData);
int (WINAPI *StartDocANext) (HDC dc,  const DOCINFOA* lpdi);
int (WINAPI *StartDocWNext) (HDC dc,  const DOCINFOW* lpdi);
int (WINAPI *EndDocNext) (HDC dc);
int (WINAPI *StartPageNext) (HDC dc);
int (WINAPI *EndPageNext) (HDC dc);

bool IsPostScriptPrinter( HDC hDC )
{
	int		nEscapeCode;
	wchar_t	szTechnology[MAX_PATH] = TEXT("");

	// If it supports POSTSCRIPT_PASSTHROUGH, it must be PS.
	nEscapeCode = POSTSCRIPT_PASSTHROUGH;
	if( ExtEscape( hDC, QUERYESCSUPPORT, sizeof(int), (LPCSTR)&nEscapeCode, 0, NULL ) > 0 ) {
		DLog(L"POSTSCRIPT_PASSTHROUGH supported, must be postscript printer");
		return true;
	}

	// If it doesn't support GETTECHNOLOGY, we won't be able to tell.
	nEscapeCode = GETTECHNOLOGY;
	if( ExtEscape( hDC, QUERYESCSUPPORT, sizeof(int), (LPCSTR)&nEscapeCode, 0, NULL ) <= 0 ) {
		DLog(L"GETTECHNOLOGY not supported, can't tell printer type");
		return false;
	}
		

	// Get the technology string and check to see if the word "postscript" is in it.
	if( ExtEscape( hDC, GETTECHNOLOGY, 0, NULL, MAX_PATH, (LPSTR)szTechnology ) <= 0 ) {
		DLog(L"ExtEscape call failed with GETTECHNOLOGY");
		return false;
	}

	DLogArg(L"Technonlgy String = %s", szTechnology);
	_wcsupr( szTechnology );
	if( wcsstr( szTechnology, L"POSTSCRIPT" ) == NULL )
		return false;

	// The word "postscript" was not found and it didn't support 
	//   POSTSCRIPT_PASSTHROUGH, so it's not a PS printer.
	return false;
}

int AllowPage(HDC dc)
{
	DLog(L"Allowing Page");
    int result = StartPageNext( dc );

    DLogArg(L"StartPageNext Result = %i", result);

    if (result <= 0)
	{
		DLogArg(L"Error Returned: %s", CUtilities::GetErrorString(GetLastError()));
	}
	rejectingPages = false;
	return result;
}

int RejectPage(HDC dc)
{
	int result;
    DLog(L"Rejecting Page");
	if (isPS && isIE8Job && (!rejectingPages))
	{
		result = EndPage(dc);
		DLogArg(L"IE8Job/PS - Reject Page - EndPage Result = %d", result);
        if (!CancelDC(dc))
		{
			DLog(L"CancelDC failed");
		}
	}
    result = EndDocNext(dc);
    if (result <= 0)
	{
		DLogArg(L"EndDocNext Failed: %s", CUtilities::GetErrorString(GetLastError()));
	}
    rejectingPages = true;
	return result;
}

int ProcessPage(HDC dc)
{
	int result = 0;
	int pageLimit;
	int allowed;

	DLog(L"ProcessPage - Enter");
    try
	{
		// We check if there is enough balance to allow this page to print,
		// taking into consideration the number of copies requested.
		pageLimit = pagesPrinted + copies;
		if (CUtilities::GetPipeControl())
		{
			allowed = pagesAllowed;
		}
		else
		{
			allowed = FMap->GetPagesAllowed();
		}

		DLogArg(L"Pages Allowed = %i", allowed);
		DLogArg(L"NumberOfPagesPrinted = %i", pagesPrinted);
		DLogArg(L"NumberOfCopies = %i", copies);
        
		if ((allowed >= 0) && (allowed < pageLimit))
		{
			DLog(L"Page Limit Exceeded");
			result = RejectPage(dc);
			//result = 0;
		}
		else
		{
			// Changed the following line on 9/13/2005 to fix problems with host based
			// printers crashing NetStop when the result returned was a raw integer. Now, when
			// pages are rejected, the End Document hook is called.
			// At least 8908790980`5one more page can be printed
			DLog(L"Pages Allowed Less Than Limit");
            result = AllowPage(dc);
		}
        DLog(L"ProcessPage - Exit");
	}
    catch(char *str)
	{
		DLogArg(L"Exception raised in ProcessPage: %s", str);
        throw;
	}

	return result;
}

bool IsWindowsXP()
{
	OSVERSIONINFO osvi;
    ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

    GetVersionEx(&osvi);

    return  ( (osvi.dwMajorVersion == 5) && (osvi.dwMinorVersion >= 1) );
}

void Lock()
{
	if (lockable)
		EnterCriticalSection(&m_CriticalSection);
}

void Unlock()
{
	if (lockable)
	  LeaveCriticalSection(&m_CriticalSection);
}

PrintReply NotifyApplicationPipe(PrintAPI pPrintAPI,  LPCSTR pPrinterName)
{

	PrintNotification vPrintNotification;;  // Record with the info sent to the application

	DLog(L"NotifyApplication - Enter");

	vPrintNotification.printAPI = pPrintAPI;
	DLog(L"printAPI Set");
	if (pPrinterName)
	{
        strcpy(vPrintNotification.printerName, pPrinterName);
		DLog(L"pPrinterName Set");
	}

	GetModuleFileNameA(0, vPrintNotification.appName, 255);
	vPrintNotification.threadId =  GetCurrentThreadId();
	
    vPrintNotification.numberOfPagesPrinted = pagesPrinted;
	vPrintNotification.numberOfPagesRejected = pagesRejected;
	vPrintNotification.copies = copies;
	
	DLogArg(L"    Pages Printed = %d", vPrintNotification.numberOfPagesPrinted);
	DLogArg(L"    Pages Rejected =  %d", vPrintNotification.numberOfPagesRejected);
  
	FMap->SetPrintAPI(pPrintAPI);
	DLogArg(L"printAPI Set to %d", pPrintAPI);
	FMap->SetPrinterName( CA2W(pPrinterName) );
	FMap->SetPagesPrinted(vPrintNotification.numberOfPagesPrinted);
	FMap->SetPagesRejected(vPrintNotification.numberOfPagesRejected);


    // Now send the composed strings to our log window
    // hopefully there's an instance running in the specified session
    DLog(L"    Sending IPC Message to PrintMonitorIPC");
    //SendIpcMessage("PrintMonitorIPC", &vPrintNotification, sizeof(PrintNotification));
	DWORD size = 0;
	PrintReply reply;
	CallNamedPipe(TEXT("\\\\.\\pipe\\PrintMonitorPipe"), &vPrintNotification, sizeof(PrintNotification), &reply, sizeof(PrintReply), &size, NMPWAIT_WAIT_FOREVER);


	pagesAllowed = reply.pagesAllowed;
	printingReceipt = reply.printingReceipt;
	//Sleep(1000);

    DLog(L"NotifyApplication - Exit");

	return reply;
}

PrintReply NotifyApplicationIPC(PrintAPI pPrintAPI,  LPCSTR pPrinterName)
{

	PrintNotification vPrintNotification;;  // Record with the info sent to the application

	DLog(L"NotifyApplicationIPC - Enter");

	vPrintNotification.printAPI = pPrintAPI;
	DLog(L"printAPI Set");
	if (pPrinterName)
	{
        strcpy(vPrintNotification.printerName, pPrinterName);
		DLog(L"pPrinterName Set");
	}

	GetModuleFileNameA(0, vPrintNotification.appName, 255);
	vPrintNotification.threadId =  GetCurrentThreadId();
	
    vPrintNotification.numberOfPagesPrinted = pagesPrinted;
	vPrintNotification.numberOfPagesRejected = pagesRejected;
	vPrintNotification.copies = copies;
	
	DLogArg(L"    Pages Printed = %d", vPrintNotification.numberOfPagesPrinted);
	DLogArg(L"    Pages Rejected =  %d", vPrintNotification.numberOfPagesRejected);
  
	FMap->SetPrintAPI(pPrintAPI);
	DLogArg(L"printAPI Set to %d", pPrintAPI);
	FMap->SetPrinterName( CA2W(pPrinterName) );
	FMap->SetPagesPrinted(vPrintNotification.numberOfPagesPrinted);
	FMap->SetPagesRejected(vPrintNotification.numberOfPagesRejected);


    // Now send the composed strings to our log window
    // hopefully there's an instance running in the specified session
    DLog(L"    Sending IPC Message to PrintMonitorIPC");
    SendIpcMessage("PrintMonitorIPC", &vPrintNotification, sizeof(PrintNotification));
	DWORD delay = CUtilities::GetDelayTime();
	if (0 != delay)
	{
		DLogArg(L"    Delaying %d milliseconds", delay);
		Sleep(delay);
	}
	PrintReply reply;
	//CallNamedPipe(TEXT("\\\\.\\pipe\\PrintMonitorPipe"), &vPrintNotification, sizeof(PrintNotification), &reply, sizeof(PrintReply), &size, NMPWAIT_WAIT_FOREVER);


	pagesAllowed = 0;
	printingReceipt = 0;
	//Sleep(1000);

    DLog(L"NotifyApplication - Exit");

	return reply;
}

PrintReply AppNotify(PrintAPI printAPI,  LPCSTR printerName)
{
	if (!CUtilities::GetUseIPC())
		return NotifyApplicationPipe(printAPI,  printerName);
	else
		return NotifyApplicationIPC(printAPI,  printerName);
}

PrintReply AppNotifyW(PrintAPI printAPI,  LPWSTR printerName)
{ 
	if (!CUtilities::GetUseIPC())
		return NotifyApplicationPipe(printAPI, CW2A(printerName));
	else
		return NotifyApplicationIPC(printAPI, CW2A(printerName));
}

DWORD WINAPI CreateDCACallback(LPCSTR lpszDriver,  __in  LPCSTR lpszDevice, LPCSTR lpszOutput,  __in  const DEVMODEA *lpInitDat)
{
	DWORD result;
	Lock();
	__try
	{
		char vPrinterName[MAX_PATH];

    	DLog(L"CreateDCACallback - Enter");
        result = CreateDCANext(lpszDriver, lpszDevice, lpszOutput, lpInitDat);

		bool receiptPrinting;
		if (CUtilities::GetPipeControl())
		{
			receiptPrinting = (0 != printingReceipt);
		}
		else
		{
			receiptPrinting = FMap->GetPrintingReceipt();
		}

    	if ( (lpszDevice) && (!receiptPrinting) )
	    {
		    strcpy(vPrinterName, lpszDevice);
		    vPrinterName[11] = 0;

		    // We don't want to display dcs
		    if ( 0 != strcmp("\\\\.\\DISPLAY", vPrinterName) )
		    {
			    // We notify the application only if it is a printer DC creation
			    if ( (lpszDevice) && (!IsBadReadPtr(lpszDevice, 1)) && (lpszDevice != "") )
		        {
			        // If the number of copies was set, use the value.
					//if ((lpInitDat) && ( ((*lpInitDat).dmFields & DM_COPIES) > 0) )
				    if ((lpInitDat) && ( ((*lpInitDat).dmFields & DM_COPIES) == DM_COPIES) )
    				{
	    				copies = (*lpInitDat).dmCopies;
		    			DLogArg(L"    Set Number of Copies = %d", (*lpInitDat).dmCopies);
			    	}
				    else
				    {
					    copies = 0;
    				}

                    strcpy(vPrinterName, lpszDevice);
		    		DLogArg(L"    Sending paCreateDC notification to application, Printer = %s", lpszDevice);
                    AppNotify(paCreateDC, vPrinterName);
    			}
	    	}
	    }
        DLog(L"CreateDCACallback - Exit");
	}
	__finally
	{
		Unlock();
	}
	return result;
}

DWORD WINAPI CreateDCWCallback(LPWSTR lpszDriver,  __in  LPWSTR lpszDevice, LPWSTR lpszOutput,  __in  const DEVMODEW *lpInitData)
{
	wchar_t vPrinterName[MAX_PATH];
	DWORD result;

	Lock();

    __try
	{
		
		DLog(L"CreateDCWCallback - Enter");
        result = CreateDCWNext(lpszDriver, lpszDevice, lpszOutput, lpInitData);
        
		bool receiptPrinting;
		if (CUtilities::GetPipeControl())
		{
			receiptPrinting = (0 != printingReceipt);
		}
		else
		{
			receiptPrinting = FMap->GetPrintingReceipt();
		}

    	if ( (lpszDevice) && (!receiptPrinting) )
	    {
		    wcscpy(vPrinterName, lpszDevice);
    		vPrinterName[11] = 0;

			
	    	// We don't want to display dcs
		    if ( 0 != wcscmp(L"\\\\.\\DISPLAY", vPrinterName) )
    		{
	     		// We notify the application only if it is a printer DC creation
		    	if ( (lpszDevice) && (!IsBadReadPtr(lpszDevice, 2)) && (lpszDevice != L"") )
		        {
					
			        // If the number of copies was set, use the value.
     				if ((lpInitData) && ( ((*lpInitData).dmFields & DM_COPIES) > 0) )
	    			{
		    			copies = (*lpInitData).dmCopies;
			    		//PrintLog('    Set Number of Copies = ' + IntToStr(PDeviceModeA(DM)^.dmCopies));
				    }
     				else
	    			{
		    			copies = 0;
			    	} 

                    wcscpy(vPrinterName, lpszDevice);
    				DLogArg(L"    Sending paCreateDC notification to application, Printer = %s", lpszDevice);
                    AppNotifyW(paCreateDC, vPrinterName); 
    			} 
		    } 
    	}
	}
	__finally
	{
		Unlock();
	}
    DLog(L"CreateDCWCallback - Exit");
	return result;
}

int WINAPI StartDocACallback(HDC dc, const DOCINFOA* lpdi)
{
	int result;

    Lock();
	__try
	{
		DLog(L"StartDocACallback - Enter");

		isPS = IsPostScriptPrinter(dc);
        isIE8Job = CUtilities::IsIE8Job();

		DLog(_TEXT("----------------------"));
		if (isPS) {
			DLog(_TEXT("POSTSCRIPT Printer"));
		} else {
			DLog(_TEXT("NON-POSTSCRIPT Printer"));
		}

		if (isIE8Job) {
			DLog(_TEXT("IE8 Job"));
		}  else {
			DLog(_TEXT("NON-IE8 Job"));
		}
        DLog(L"----------------------");

		pagesPrinted = 0;
		pagesRejected = 0;

		bool receiptPrinting;
		if (CUtilities::GetPipeControl())
		{
			receiptPrinting = (0 != printingReceipt);
		}
		else
		{
			receiptPrinting = FMap->GetPrintingReceipt();
		}

		if (!receiptPrinting)
		{
			DLog(L"    Sending paStartDoc notification");
			AppNotify(paStartDoc, NULL);
	    }
        result = StartDocANext(dc, lpdi);
        DLog(L"StartDocACallback - Exit");
	}
	__finally
	{
		Unlock();
	}
	return result;
}

int WINAPI StartDocWCallback(HDC dc, const DOCINFOW* lpdi)
{
	int result;

    Lock();
	__try
	{
		DLog(L"StartDocWCallback - Enter");
		
        isPS = IsPostScriptPrinter(dc);
        isIE8Job = CUtilities::IsIE8Job();

		DLog(_TEXT("----------------------"));
		if (isPS) {
			DLog(_TEXT("POSTSCRIPT Printer"));
		} else {
			DLog(_TEXT("NON-POSTSCRIPT Printer"));
		}

		if (isIE8Job) {
			DLog(_TEXT("IE8 Job"));
		}  else {
			DLog(_TEXT("NON-IE8 Job"));
		}
        DLog(L"----------------------");

		pagesPrinted = 0;
		pagesRejected = 0;

		bool receiptPrinting;
		if (CUtilities::GetPipeControl())
		{
			receiptPrinting = (0 != printingReceipt);
		}
		else
		{
			receiptPrinting = FMap->GetPrintingReceipt();
		}

		if (!receiptPrinting)
	   {
	    	DLog(L"    Sending paStartDoc notification");
		    AppNotifyW(paStartDoc, NULL);
	    }
        result = StartDocWNext(dc, lpdi);
	}
	__finally
	{
		Unlock();
	}
    DLog(L"StartDocACallback - Exit");
	return result;
}

int WINAPI EndDocCallback(HDC dc)
{
	int result;

    Lock();
	__try
	{
		DLog(L"EndDocCallback - Enter");
		
		bool receiptPrinting;
		if (CUtilities::GetPipeControl())
		{
			receiptPrinting = (0 != printingReceipt);
		}
		else
		{
			receiptPrinting = FMap->GetPrintingReceipt();
		}

		if (!receiptPrinting) 
    	{
	        // We notify the application that the document is finished - the application then
		    // it knows it is time to charge for the printing (deduct from balance)
    		AppNotify(paEndDoc, NULL);

    		// The application has been notified of these pages so we can reset
            pagesPrinted = 0;
            pagesRejected = 0;
		    FMap->SetNumberOfCopies(copies);
    		FMap->SetUseRegistryCopyCounter(0);
            //SetCopies(GetNumberOfCopies,False); // ACME PRINT ISSUE Set Registry Copy Count Flag to False
	    }
        result = EndDocNext(dc);
	}
	__finally
	{
		Unlock();
	}

    DLog(L"EndDocCallback - Exit");

	return result;
}

// This API tells us when a page is to be printed. This is where we reject pages.
int WINAPI StartPageCallback(HDC dc)
{
	int result = 0;

    	Lock();
		DLog(L"StartPageCallback - Enter");
        try
	    {
			//PrintReply pr = AppNotifyW(paStartPage, NULL);

			if (FMap->GetUseRegistryCopyCounter())
            {
		         copies = FMap->GetNumberOfCopies();
            }
            startPage = true;

			bool receiptPrinting;
			if (CUtilities::GetPipeControl())
		    {
			//	receiptPrinting = (0 != pr.printingReceipt);
    		}
	    	else
		    {
			    receiptPrinting = FMap->GetPrintingReceipt();
    		}

	        if (receiptPrinting)
	        {
     		    result = StartPageNext(dc);
	        }
            else
	        {
	            result = ProcessPage(dc);
    	    }
			DLogArg(L"StartPageCallback - Exit (Result: %d)", result);
	    }
        catch(char *str)
        {
	        DLogArg(L"Exception in StartPageCallback: %s", str);
        }
		Unlock();
	return result;
}

int WINAPI EndPageCallback(HDC dc)
{
	int result = 0;
	int allowed;
	int tempCopies;
	Lock();
	__try
	{
		allowed = FMap->GetPagesAllowed();

		DLog(L"EndPageCallback - Enter");
		bool receiptPrinting;
		if (CUtilities::GetPipeControl())
		{
			receiptPrinting = (0 != printingReceipt);
		}
		else
		{
			receiptPrinting = FMap->GetPrintingReceipt();
		}

        if (receiptPrinting)
    	{
            result = EndPageNext(dc);
    	}
	    else
	    {
		    if (rejectingPages)
    		{
	     		DLog(L"Rejecting Pages, Ending Document");
		    	// Changed the following line on 9/27/05 to fix an occasional problem
			    // in the "Prepaid Minutes & Pages" usage mode where NetStop would
			    // not charge for the pages printed.
    			result = EndDocNext(dc); // old code before 4.8.2 Result := 1 (issue no 765)

    			// Only one End page allowed per StartPage.
	    		if (startPage)
			    {
				     // If a valid copies value is detected, we use it. Else increment the page count by 1
    				if (copies > 0)
	    			{
		     			pagesRejected = pagesRejected + copies;
			    	}
				    else
    				{
	    				pagesRejected = pagesRejected + 1;
		    		}
				    DLogArg(L"Starting Page, Pages Rejected = %d", pagesRejected);
    			}
	    	}
            else
    		{
	    		DLog(L"Not Rejecting Pages, Ending Page");
    			result = EndPageNext(dc);

	    		// Only one End page allowed per StartPage.
		    	if (startPage) 
			    {
					if (isPS && isIE8Job && (0 == allowed))
					{
						pagesPrinted = 0;
						pagesRejected = 1;
					}
					if (allowed > 0)
					{
                        tempCopies = copies;
						if (0 == tempCopies) 
						{
							tempCopies = 1;
						}
						if (isPS && isIE8Job && ((pagesPrinted % tempCopies) == (allowed % tempCopies)) && (0 == pagesRejected)) 
						{
							pagesPrinted = pagesPrinted - FMap->GetNumberOfCopies();
                            pagesRejected = pagesRejected + FMap->GetNumberOfCopies();
						}

						 // If a valid copies value is detected, we use it. Else increment the page count by 1
                         if (copies > 0) 
						 {
							 pagesPrinted = pagesPrinted + copies;
						 }
						 else
						 {
							 pagesPrinted = pagesPrinted + 1;
						 }
            		    DLogArg(L"Ending Page, Pages Rejected = %d", pagesRejected);
	    	    	    DLogArg(L"Ending Page, Pages Printed = %d", pagesPrinted);
					}
				}
			    else
    			{
	    			DLog(L"StartPage = FALSE");
		    	}
		    }
    		startPage = false;
	    }
	}
	__finally
	{
		Unlock();
	}
	DLog(L"EndPageCallback - Exit");
	return result;
}

// ***************************************************************

BOOL (WINAPI *CreateProcessANext)  (LPCTSTR               lpApplicationName,
                                    LPTSTR                lpCommandLine,
                                    LPSECURITY_ATTRIBUTES lpProcessAttributes,
                                    LPSECURITY_ATTRIBUTES lpThreadAttributes,
                                    BOOL                  bInheritHandles,
                                    DWORD                 dwCreationFlags,
                                    LPVOID                lpEnvironment,
                                    LPCTSTR               lpCurrentDirectory,
                                    LPSTARTUPINFO         lpStartupInfo,
                                    LPPROCESS_INFORMATION lpProcessInformation);
BOOL (WINAPI *CreateProcessWNext)  (LPCWSTR               lpApplicationName,
                                    LPWSTR                lpCommandLine,
                                    LPSECURITY_ATTRIBUTES lpProcessAttributes,
                                    LPSECURITY_ATTRIBUTES lpThreadAttributes,
                                    BOOL                  bInheritHandles,
                                    DWORD                 dwCreationFlags,
                                    LPVOID                lpEnvironment,
                                    LPCWSTR               lpCurrentDirectory,
                                    LPSTARTUPINFO         lpStartupInfo,
                                    LPPROCESS_INFORMATION lpProcessInformation);
UINT (WINAPI *WinExecNext)         (LPCSTR                lpCmdLine,
                                    UINT                  uCmdShow);

// ***************************************************************

/*
BOOL WINAPI CreateProcessACallback(LPCTSTR               lpApplicationName,
                                   LPTSTR                lpCommandLine,
                                   LPSECURITY_ATTRIBUTES lpProcessAttributes,
                                   LPSECURITY_ATTRIBUTES lpThreadAttributes,
                                   BOOL                  bInheritHandles,
                                   DWORD                 dwCreationFlags,
                                   LPVOID                lpEnvironment,
                                   LPCTSTR               lpCurrentDirectory,
                                   LPSTARTUPINFO         lpStartupInfo,
                                   LPPROCESS_INFORMATION lpProcessInformation)
{
  if (!IsAllowed(lpApplicationName, lpCommandLine, NULL, NULL)) {
    SetLastError(ERROR_ACCESS_DENIED);
    return false;
  } else {
    BOOL result = CreateProcessANext(lpApplicationName, lpCommandLine,
                                     lpProcessAttributes, lpThreadAttributes,
                                     bInheritHandles, dwCreationFlags,
                                     lpEnvironment, lpCurrentDirectory,
                                     lpStartupInfo, lpProcessInformation);
    // CreateProcess hooks are used very often, so to be sure we renew the hook
    RenewHook((PVOID*) &CreateProcessANext);
    return result;
  }
}

BOOL WINAPI CreateProcessWCallback(LPCWSTR               lpApplicationName,
                                   LPWSTR                lpCommandLine,
                                   LPSECURITY_ATTRIBUTES lpProcessAttributes,
                                   LPSECURITY_ATTRIBUTES lpThreadAttributes,
                                   BOOL                  bInheritHandles,
                                   DWORD                 dwCreationFlags,
                                   LPVOID                lpEnvironment,
                                   LPCWSTR               lpCurrentDirectory,
                                   LPSTARTUPINFO         lpStartupInfo,
                                   LPPROCESS_INFORMATION lpProcessInformation)
{
  if (!IsAllowed(NULL, NULL, lpApplicationName, lpCommandLine)) {
    SetLastError(ERROR_ACCESS_DENIED);
    return false;
  } else {
    BOOL result = CreateProcessWNext(lpApplicationName, lpCommandLine,
                                     lpProcessAttributes, lpThreadAttributes,
                                     bInheritHandles, dwCreationFlags,
                                     lpEnvironment, lpCurrentDirectory,
                                     lpStartupInfo, lpProcessInformation);
    // CreateProcess hooks are used very often, so to be sure we renew the hook
    RenewHook((PVOID*) &CreateProcessWNext);
    return result;
  }
}

UINT WINAPI WinExecCallback(LPCSTR lpCmdLine, UINT uCmdShow)
{
  if (!IsAllowed(NULL, (LPTSTR) lpCmdLine, NULL, NULL))
    return ERROR_ACCESS_DENIED;
  else {
    DWORD result = WinExecNext(lpCmdLine, uCmdShow);
    RenewHook((PVOID*) &WinExecNext);
    return result;
  }
}
*/
// ***************************************************************

BOOL WINAPI DllMain(HANDLE hModule, DWORD fdwReason, LPVOID lpReserved)
{
	if (fdwReason == DLL_PROCESS_ATTACH) 
	{
		FMap = new CPrinterIPC();
		// InitializeMadCHook is needed only if you're using the static madCHook.lib
		InitializeMadCHook();

		//HookAPI("kernel32.dll", "CreateProcessA", CreateProcessACallback, (PVOID*) &CreateProcessANext);
		//HookAPI("kernel32.dll", "CreateProcessW", CreateProcessWCallback, (PVOID*) &CreateProcessWNext);
		//HookAPI("kernel32.dll",        "WinExec",        WinExecCallback, (PVOID*)        &WinExecNext);

		InitializeCriticalSection(&m_CriticalSection);

		lockable = CUtilities::GetLockable();

		DLog(L"Hooking APIs");
        HookAPI("gdi32.dll", "CreateDCA", CreateDCACallback, (PVOID*) &CreateDCANext);
		HookAPI("gdi32.dll", "CreateDCW", CreateDCWCallback, (PVOID*) &CreateDCWNext);
		HookAPI("gdi32.dll", "StartDocA", StartDocACallback, (PVOID*) &StartDocANext);
		HookAPI("gdi32.dll", "StartDocW", StartDocWCallback, (PVOID*) &StartDocWNext);
		HookAPI("gdi32.dll", "EndDoc",    EndDocCallback,    (PVOID*) &EndDocNext);
        HookAPI("gdi32.dll", "StartPage", StartPageCallback, (PVOID*) &StartPageNext);
        HookAPI("gdi32.dll", "EndPage",   EndPageCallback,   (PVOID*) &EndPageNext);
	} 
    else if (fdwReason == DLL_PROCESS_DETACH)
    {
		DeleteCriticalSection(&m_CriticalSection);
	    // FinalizeMadCHook is needed only if you're using the static madCHook.lib
	    FinalizeMadCHook();
		delete FMap;
    }

    return true;
}
