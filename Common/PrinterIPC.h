#pragma once

#include <atlstr.h>
#include <windows.h>
#include <atlbase.h>
#include "PrinterDefs.h"

class CPrinterIPC
{
private:
	void CreateRegistryKey();
	CAtlStringW GetRegistryString(HKEY rootKey, WCHAR* keyName, WCHAR* valueName);
	void SetRegistryString(HKEY rootKey, WCHAR* keyName, WCHAR* valueName, WCHAR* value);
	int GetRegistryInt(HKEY rootKey, WCHAR* keyName, WCHAR* valueName);
	void SetRegistryInt(HKEY rootKey, WCHAR* keyName, WCHAR* valueName, int value);
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
    void SetPrinterName(WCHAR* printerName);
    int GetPagesPrinted();
    void SetPagesPrinted(int pagesPrinted);
    int GetPagesRejected();
    void SetPagesRejected(int pagesRejected);
	int GetNumberOfCopies();
	void SetNumberOfCopies(int copies);
	bool GetUseRegistryCopyCounter();
	void SetUseRegistryCopyCounter(int useRegistryCopyCounter);
	bool GetPipeControl();
};
