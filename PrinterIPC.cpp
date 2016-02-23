#include "PrinterIPC.h"

CPrinterIPC::CPrinterIPC(void)
{
	CreateRegistryKey();
}

CPrinterIPC::~CPrinterIPC(void)
{
}

void CPrinterIPC::CreateRegistryKey()
{
	CRegKey* key = NULL;
	DWORD result = 0;
	key = new CRegKey();
	if (NULL != key)
	{
        #ifdef _WIN64
		key->Create(HKEY_CURRENT_USER, SERVICE_PRINTER_IPC_REG_KEY, REG_NONE, REG_OPTION_NON_VOLATILE, KEY_WOW64_32KEY | KEY_READ | KEY_WRITE);
        #else
		key->Create(HKEY_CURRENT_USER, SERVICE_PRINTER_IPC_REG_KEY, REG_NONE, REG_OPTION_NON_VOLATILE, KEY_READ | KEY_WRITE);
        #endif

		if ( ERROR_SUCCESS == result )
		{
			key->Close();
		}
		delete key;
	}
}

CAtlString CPrinterIPC::GetRegistryString(HKEY rootKey, LPCSTR keyName, LPCSTR valueName)
{
	DWORD result = 0;
	char value[MAX_PATH]  = {0};
	ULONG stringSize = MAX_PATH;
	CRegKey* key;
	key = new CRegKey();
	if (NULL != key)
	{
		#ifdef _WIN64
		result = key->Open(rootKey, keyName, KEY_WOW64_32KEY | KEY_READ);
        #else
		result = key->Open(rootKey, keyName, KEY_READ);
        #endif
		if ( ERROR_SUCCESS == result )
		{
			key->QueryStringValue(valueName, value, &stringSize);
			key->Close();
		}
		delete key;
	}

	CAtlString out;
	out.Format("%s", value);
	return out;
}

void CPrinterIPC::SetRegistryString(HKEY rootKey, LPCSTR keyName, LPCSTR valueName, LPCSTR value)
{
	DWORD result = 0;
	CRegKey* key;
	key = new CRegKey();
	if (NULL != key)
	{
		#ifdef _WIN64
		result = key->Open(rootKey, keyName, KEY_WOW64_32KEY | KEY_READ | KEY_WRITE);
        #else
		result = key->Open(rootKey, keyName, KEY_READ | KEY_WRITE);
        #endif
		if ( ERROR_SUCCESS == result )
		{
			key->SetStringValue(valueName, value, REG_SZ);
			key->Close();
		}
		delete key;
	}
}

int CPrinterIPC::GetRegistryInt(HKEY rootKey, LPCSTR keyName, LPCSTR valueName)
{
	DWORD result = 0;
	DWORD value = 0;
	CRegKey* key;
	key = new CRegKey();
	if (NULL != key)
	{
		#ifdef _WIN64
		result = key->Open(rootKey, keyName, KEY_WOW64_32KEY | KEY_READ);
        #else
		result = key->Open(rootKey, keyName, KEY_READ);
        #endif

		if ( ERROR_SUCCESS == result )
		{
			key->QueryDWORDValue(valueName, value);
			key->Close();
		}
		delete key;
	}

	return value;
}

void CPrinterIPC::SetRegistryInt(HKEY rootKey, LPCSTR keyName, LPCSTR valueName, int value)
{
	DWORD result = 0;
	CRegKey* key;
	key = new CRegKey();
	if (NULL != key)
	{
		#ifdef _WIN64
		result = key->Open(rootKey, keyName, KEY_WOW64_32KEY | KEY_READ);
        #else
		result = key->Open(rootKey, keyName, KEY_READ);
        #endif

		if ( ERROR_SUCCESS == result )
		{
			key->SetDWORDValue(valueName, value);
			key->Close();
		}
		delete key;
	}
}

int CPrinterIPC::GetPagesAllowed()
{
	return GetRegistryInt(HKEY_CURRENT_USER, SERVICE_PRINTER_IPC_REG_KEY, TEXT("PagesAllowed"));
}

void CPrinterIPC::SetPagesAllowed(int pagesAllowed)
{
	SetRegistryInt(HKEY_CURRENT_USER, SERVICE_PRINTER_IPC_REG_KEY, TEXT("PagesAllowed"), pagesAllowed);
}

bool CPrinterIPC::GetPrintingReceipt()
{
	return ( 0 != GetRegistryInt(HKEY_CURRENT_USER, SERVICE_PRINTER_IPC_REG_KEY, TEXT("PrintingReceipt")) );
}

void CPrinterIPC::SetPrintingReceipt(bool printingReceipt)
{
	SetRegistryInt(HKEY_CURRENT_USER, SERVICE_PRINTER_IPC_REG_KEY, TEXT("PrintingReceipt"), printingReceipt);
}

PrintAPI CPrinterIPC::GetPrintAPI()
{
	return ( static_cast<PrintAPI>( GetRegistryInt(HKEY_CURRENT_USER, SERVICE_PRINTER_IPC_REG_KEY, TEXT("PrintAPI")) ) );
}

void CPrinterIPC::SetPrintAPI(PrintAPI value)
{
	SetRegistryInt(HKEY_CURRENT_USER, SERVICE_PRINTER_IPC_REG_KEY, TEXT("PrintAPI"), static_cast<int>(value) );
}

CAtlString CPrinterIPC::GetPrinterName()
{
	return GetRegistryString( HKEY_CURRENT_USER, SERVICE_PRINTER_IPC_REG_KEY, TEXT("PrinterName") );
}

void CPrinterIPC::SetPrinterName(LPCSTR printerName)
{
	SetRegistryString( HKEY_CURRENT_USER, SERVICE_PRINTER_IPC_REG_KEY, TEXT("PrinterName"), printerName );
}

int CPrinterIPC::GetPagesPrinted()
{
	return GetRegistryInt(HKEY_CURRENT_USER, SERVICE_PRINTER_IPC_REG_KEY, TEXT("PagesPrintedForJob"));
}

void CPrinterIPC::SetPagesPrinted(int pagesPrinted)
{
	SetRegistryInt(HKEY_CURRENT_USER, SERVICE_PRINTER_IPC_REG_KEY, TEXT("PagesPrintedForJob"), pagesPrinted);
}

int CPrinterIPC::GetPagesRejected()
{
	return GetRegistryInt(HKEY_CURRENT_USER, SERVICE_PRINTER_IPC_REG_KEY, TEXT("PagesRejectedForJob"));
}

void CPrinterIPC::SetPagesRejected(int pagesRejected)
{
	SetRegistryInt(HKEY_CURRENT_USER, SERVICE_PRINTER_IPC_REG_KEY, TEXT("PagesRejectedForJob"), pagesRejected);
}