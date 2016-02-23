#pragma once

#include <atlstr.h>
#include <windows.h>
#include <atlbase.h>
#include "printerdefs.h"

class CPrinterIPC
{
private:
	void CreateRegistryKey();
	CAtlString GetRegistryString(HKEY rootKey, LPCSTR keyName, LPCSTR valueName);
	void SetRegistryString(HKEY rootKey, LPCSTR keyName, LPCSTR valueName, LPCSTR value);
	int GetRegistryInt(HKEY rootKey, LPCSTR keyName, LPCSTR valueName);
	void SetRegistryInt(HKEY rootKey, LPCSTR keyName, LPCSTR valueName, int value);
public:
	CPrinterIPC(void);
	~CPrinterIPC(void);
	int GetPagesAllowed();
    void SetPagesAllowed(int pagesAllowed);
    bool GetPrintingReceipt();
    void SetPrintingReceipt(bool printingReceipt);
    PrintAPI GetPrintAPI();
    void SetPrintAPI(PrintAPI value);
    CAtlString GetPrinterName();
    void SetPrinterName(LPCSTR printerName);
    int GetPagesPrinted();
    void SetPagesPrinted(int pagesPrinted);
    int GetPagesRejected();
    void SetPagesRejected(int pagesRejected);
};
