#include "Utilities.h"

bool CUtilities::DebuggingEnabled()
{
	DWORD result = 0;
	DWORD debugging = 0;
	CRegKey* key;
	key = new CRegKey();
	if (NULL != key)
	{
		#ifdef _WIN64
		result = key->Open(HKEY_LOCAL_MACHINE, SERVICE_PRINTER_IPC_REG_KEY, KEY_WOW64_32KEY | KEY_READ);
        #else
		result = key->Open(HKEY_LOCAL_MACHINE, SERVICE_PRINTER_IPC_REG_KEY, KEY_READ);
        #endif

		if ( ERROR_SUCCESS == result )
		{
			key->QueryDWORDValue(TEXT("PrintMonitorLog"), debugging);
			key->Close();
		}
		delete key;
	}

	return ( 0 != debugging );
}

bool CUtilities:: OneLogPerProces()
{
	DWORD result = 0;
	DWORD perProcess = 0;
	CRegKey* key;
	key = new CRegKey();
	if (NULL != key)
	{
		#ifdef _WIN64
		result = key->Open(HKEY_LOCAL_MACHINE, SERVICE_PRINTER_IPC_REG_KEY, KEY_WOW64_32KEY | KEY_READ);
        #else
		result = key->Open(HKEY_LOCAL_MACHINE, SERVICE_PRINTER_IPC_REG_KEY, KEY_READ);
        #endif

		if ( ERROR_SUCCESS == result )
		{
			key->QueryDWORDValue(TEXT("PerProcess"), perProcess);
			key->Close();
		}
		delete key;
	}

	return ( 0 != perProcess );
}

CAtlString CUtilities:: LogFileDir()
{
	DWORD result = 0;
	char logFileDir[MAX_PATH]  = {0};
	ULONG stringSize = MAX_PATH;
	CRegKey* key;
	key = new CRegKey();
	if (NULL != key)
	{
		#ifdef _WIN64
		result = key->Open(HKEY_LOCAL_MACHINE, NETSTOP_REG_KEY, KEY_WOW64_32KEY | KEY_READ);
        #else
		result = key->Open(HKEY_LOCAL_MACHINE, NETSTOP_REG_KEY, KEY_READ);
        #endif
		if ( ERROR_SUCCESS == result )
		{
			key->QueryStringValue(TEXT("Log File - Log File Location"), logFileDir, &stringSize);
			key->Close();
		}
		delete key;
	}

	CAtlString out;
	out.Format("%s", logFileDir);
	return out;
}

CAtlString CUtilities::MonitoredProcess()
{
	DWORD result = 0;
	char process[MAX_PATH]  = {0};
	ULONG stringSize = MAX_PATH;
	CRegKey* key;
	key = new CRegKey();
	if (NULL != key)
	{
		#ifdef _WIN64
		result = key->Open(HKEY_LOCAL_MACHINE, SERVICE_PRINTER_IPC_REG_KEY, KEY_WOW64_32KEY | KEY_READ);
        #else
		result = key->Open(HKEY_LOCAL_MACHINE, SERVICE_PRINTER_IPC_REG_KEY, KEY_READ);
        #endif
		if ( ERROR_SUCCESS == result )
		{
			key->QueryStringValue(TEXT("MonitoredProcess"), process, &stringSize);
			key->Close();
		}
		delete key;
	}

	CAtlString out;
	out.Format("%s", process);
	return out;
}

void CUtilities::Dlog(CAtlString logString)
{
	CHiResTime now(true);

	char exeName[MAX_PATH + 1];
	GetModuleFileName(0, exeName, MAX_PATH);
	CAtlString myProcessName;
	myProcessName.Format("%s", exeName);
	
	CAtlString monitoredProcess = MonitoredProcess();
	bool oneLogPerProcess = OneLogPerProces();
	if ( (oneLogPerProcess) && ("" != monitoredProcess) && (-1 == myProcessName.MakeUpper().Find(monitoredProcess.MakeUpper())) )
	{
		return;
	}

	CAtlString logFileName = LogFileDir();
	if ('\\' != logFileName.GetAt(logFileName.GetLength() -1) )
	{
		logFileName = logFileName + '\\';
	}

	if (oneLogPerProcess)
	{
		//Trim down process name so that only file name is left
		int pos = myProcessName.Find('\\');
		while (-1 != pos)
		{
			myProcessName.Delete(0, (pos + 1));
			pos = myProcessName.Find('\\');
    	}
		pos = myProcessName.Find('.');
		myProcessName.Delete(pos, (myProcessName.GetLength() - pos) );
		logFileName = logFileName + "PrintMonitor" + myProcessName + ".log";
	}
	else
	{
		logFileName = logFileName + "PrintMonitor.log";
	}

}
