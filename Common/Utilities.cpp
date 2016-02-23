#include "Utilities.h"


void _TraceArg( const char* functionName, const WCHAR* message, ... )
{
	wchar_t logLine[LOG_BUFFER_WIDTH] = {0};
	va_list ap;
	va_start(ap, message);

	_vsnwprintf_s(logLine, LOG_BUFFER_WIDTH, LOG_BUFFER_WIDTH-1, message, ap );
	logLine[LOG_BUFFER_WIDTH-1] = L'\0';

	va_end(ap);

	_Trace(functionName, logLine);
}

void _Trace(const char* functionName, const WCHAR* logString)
{
	if (!CUtilities::DebuggingEnabled())
	{
		return;
	}

	CHiResTime now(true);

	wchar_t exeName[MAX_PATH + 1];
	char szFunctionName[FUNCTION_NAME_WIDTH];

	_snprintf_s(szFunctionName, FUNCTION_NAME_WIDTH, FUNCTION_NAME_WIDTH-1, functionName );
    szFunctionName[FUNCTION_NAME_WIDTH-1] = L'\0';

	GetModuleFileName(0, exeName, MAX_PATH);
	CAtlStringW myProcessName;
	myProcessName.Format(L"%s", exeName);
	
	CAtlStringW monitoredProcess = CUtilities::MonitoredProcess();
	bool oneLogPerProcess = CUtilities::OneLogPerProces();
	if ( (oneLogPerProcess) && (L"" != monitoredProcess) && (-1 == myProcessName.MakeUpper().Find(monitoredProcess.MakeUpper())) )
	{
		return;
	}

	CAtlStringW logFileName = CUtilities::LogFileDir();

	//Strip FileName
	int logPos = logFileName.ReverseFind('\\');
	if (-1 != logPos) 
	{
		logFileName.Delete(logPos, (logFileName.GetLength() - logPos) );
	}

	if (CUtilities::DirectoryExists( logFileName.GetBuffer() ))
	{
		if ('\\' != logFileName.GetAt(logFileName.GetLength() -1) )
	    {
		    logFileName = logFileName + '\\';
    	}
	}
	else
	{
		return;
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
		logFileName = logFileName + L"PrintMonitor" + myProcessName + ".log";
	}
	else
	{
		logFileName = logFileName + L"PrintMonitor.log";
	}

	CAtlStringW thread;
	thread.Format(L"0x%.8x/0x%.8x", GetCurrentProcessId(), GetCurrentThreadId());
	thread.MakeUpper();

	CAtlStringW logEntry;
	logEntry.Format(L"%s %s %-20.20S:%s\n", now.AsCAtlString(), thread, szFunctionName, logString);
	
	FILE *fp = _wfopen(logFileName, L"a+t"); /* to append CString contents to file*/
    if ( fp )
	{
		fputws( logEntry.GetBuffer(), fp);
		fclose(fp);
	}

	if (CUtilities::WriteToDebug())
	{
		OutputDebugString(logEntry.GetBuffer());
	}
}

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

//----------------------------------------------
// GetApplicationPath()
//
// Returns application exe anme with no path
// trailing delimiter
//----------------------------------------------
CAtlStringW CUtilities::GetApplicationName()
{
	wchar_t exeName[MAX_PATH + 1];
	GetModuleFileName(0, exeName, MAX_PATH);
	CAtlStringW myProcessName;
	myProcessName.Format(L"%s", exeName);

	//Trim down process name so that only file name is left
	int pos = myProcessName.Find('\\');
	while (-1 != pos)
	{
		myProcessName.Delete(0, (pos + 1));
		pos = myProcessName.Find('\\');
    }
	return myProcessName;
}

//----------------------------------------------
// GetApplicationPath()
//
// Returns application name including full path
//----------------------------------------------
CAtlStringW CUtilities::GetApplicationPath()
{
	wchar_t exeName[MAX_PATH + 1];
	GetModuleFileName(0, exeName, MAX_PATH);
	CAtlStringW myProcessName;
	myProcessName.Format(L"%s", exeName);
	return myProcessName;
}

//----------------------------------------------
// GetApplicationPath()
//
// Returns application directory with optional 
// trailing delimiter
//----------------------------------------------
CAtlStringW CUtilities::GetApplicationDirecotry(bool includeTrailingDelimiter)
{
	wchar_t exeName[MAX_PATH + 1];
	GetModuleFileName(0, exeName, MAX_PATH);
	CAtlStringW myProcessName;
	myProcessName.Format(L"%s", exeName);

	myProcessName.MakeReverse();

	int pos = myProcessName.Find('\\');
	if (-1 != pos)
	{
		myProcessName.Delete(0, (pos + 1));
    }

	myProcessName.MakeReverse();
	if (includeTrailingDelimiter)
		myProcessName += "\\";

	return myProcessName;
}

CAtlStringW CUtilities::LogFileDir()
{
	DWORD result = 0;
	wchar_t logFileDir[MAX_PATH]  = {0};
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

	CAtlStringW out;
	out.Format(L"%s", logFileDir);
	return out;
}

bool CUtilities::InjectIntoSystemProcesses()
{
	DWORD result = 0;
	DWORD sysProcess = 0;
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
			key->QueryDWORDValue(TEXT("DLL Injection - System Processes"), sysProcess);
			key->Close();
		}
		delete key;
	}

	return ( 0 != sysProcess );
}

CAtlStringW CUtilities::MonitoredProcess()
{
	DWORD result = 0;
	wchar_t process[MAX_PATH]  = {0};
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
	out.Format(L"%s", process);
	return out;
}

bool CUtilities::WriteToDebug()
{
	DWORD result = 0;
	DWORD toDebug = 0;
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
			result = key->QueryDWORDValue(TEXT("Log File - Write To Debug"), toDebug);
			key->Close();
		}
		delete key;
	}

	return ((0 != toDebug) && (ERROR_SUCCESS == result));
}

bool CUtilities::FileExists(wchar_t* strFilename) 
{ 
    return (-1 != _waccess(strFilename, 0));
}

bool CUtilities::DirectoryExists(wchar_t* strDirectoryName) 
{
	if ( 0 == _waccess( strDirectoryName, 0 ) )
    {
        struct _stat64i32 status;
        _wstat( strDirectoryName, &status );

        if ( status.st_mode & S_IFDIR )
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}

CAtlStringW CUtilities::GetSysFile32()
{
	DWORD result = 0;
	wchar_t logFileDir[MAX_PATH]  = {0};
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
			key->QueryStringValue(TEXT("InjectionDriver32"), logFileDir, &stringSize);
			key->Close();
		}
		delete key;
	}

	CAtlStringW out;
	out.Format(L"%s", logFileDir);
	return out;
}

CAtlStringW CUtilities::GetSysFile64()
{
	DWORD result = 0;
	wchar_t logFileDir[MAX_PATH]  = {0};
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
			key->QueryStringValue(TEXT("InjectionDriver64"), logFileDir, &stringSize);
			key->Close();
		}
		delete key;
	}

	CAtlStringW out;
	out.Format(L"%s", logFileDir);
	return out;
}

CAtlStringW CUtilities::GetLibFile32()
{
	DWORD result = 0;
	wchar_t logFileDir[MAX_PATH]  = {0};
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
			key->QueryStringValue(TEXT("InjectionLibrary32"), logFileDir, &stringSize);
			key->Close();
		}
		delete key;
	}

	CAtlStringW out;
	out.Format(L"%s", logFileDir);
	return out;
}

CAtlStringW CUtilities::GetLibFile64()
{
	DWORD result = 0;
	wchar_t logFileDir[MAX_PATH]  = {0};
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
			key->QueryStringValue(TEXT("InjectionLibrary64"), logFileDir, &stringSize);
			key->Close();
		}
		delete key;
	}

	CAtlStringW out;
	out.Format(L"%s", logFileDir);
	return out;
}


void CUtilities::DlogArg( const WCHAR* message, ... )
{
	wchar_t logLine[LOG_BUFFER_WIDTH] = {0};
	va_list ap;
	va_start(ap, message);

	_vsnwprintf_s(logLine, LOG_BUFFER_WIDTH, LOG_BUFFER_WIDTH-1, message, ap );
	logLine[LOG_BUFFER_WIDTH-1] = L'\0';

	va_end(ap);

	Dlog(logLine);
}

void CUtilities::Dlog(const WCHAR* logString)
{
	CHiResTime now(true);

	wchar_t exeName[MAX_PATH + 1];
	GetModuleFileName(0, exeName, MAX_PATH);
	CAtlStringW myProcessName;
	myProcessName.Format(L"%s", exeName);
	
	CAtlStringW logFileName = LogFileDir();

	//Strip FileName
	int logPos = logFileName.ReverseFind('\\');
	if (-1 != logPos) 
	{
		logFileName.Delete(logPos, (logFileName.GetLength() - logPos) );
	}

	if (DirectoryExists( logFileName.GetBuffer() ) )
	{
		if ('\\' != logFileName.GetAt(logFileName.GetLength() -1) )
	    {
		    logFileName = logFileName + '\\';
    	}
	}
	else
	{
		logFileName = GetApplicationDirecotry(true);
	}

    //Trim down process name so that only file name is left
	int pos = myProcessName.Find('\\');
	while (-1 != pos)
	{
		myProcessName.Delete(0, (pos + 1));
		pos = myProcessName.Find('\\');
    }
	pos = myProcessName.Find('.');
	myProcessName.Delete(pos, (myProcessName.GetLength() - pos) );
	logFileName = logFileName + myProcessName + ".log";

	CAtlStringW thread;
	thread.Format(L"0x%.8x/0x%.8x", GetCurrentProcessId(), GetCurrentThreadId());
	thread.MakeUpper();

	CAtlStringW logEntry;
	logEntry.Format(L"%s %s %s\n", now.AsCAtlString(), thread, logString);
	
	FILE *fp = _wfopen(logFileName, L"a+t"); //
    if ( fp )
	{
		fputws( logEntry.GetBuffer(), fp);
		fclose(fp);
	}
	if (WriteToDebug())
	{
		OutputDebugString(logEntry.GetBuffer());
	}
}


void CUtilities::PrintLogArg( const WCHAR* message, ... )
{
	wchar_t logLine[LOG_BUFFER_WIDTH] = {0};
	va_list ap;
	va_start(ap, message);

	_vsnwprintf_s(logLine, LOG_BUFFER_WIDTH, LOG_BUFFER_WIDTH-1, message, ap );
	logLine[LOG_BUFFER_WIDTH-1] = L'\0';

	va_end(ap);

	PrintLog(logLine);
}

void CUtilities::PrintLog(const WCHAR* logString)
{
	if (!DebuggingEnabled())
	{
		return;
	}

	CHiResTime now(true);

	wchar_t exeName[MAX_PATH + 1];
	GetModuleFileName(0, exeName, MAX_PATH);
	CAtlStringW myProcessName;
	myProcessName.Format(L"%s", exeName);
	
	CAtlStringW monitoredProcess = MonitoredProcess();
	bool oneLogPerProcess = OneLogPerProces();
	if ( (oneLogPerProcess) && (L"" != monitoredProcess) && (-1 == myProcessName.MakeUpper().Find(monitoredProcess.MakeUpper())) )
	{
		return;
	}

	CAtlStringW logFileName = LogFileDir();

	//Strip FileName
	int logPos = logFileName.ReverseFind('\\');
	if (-1 != logPos) 
	{
		logFileName.Delete(logPos, (logFileName.GetLength() - logPos) );
	}

	if (DirectoryExists( logFileName.GetBuffer() ))
	{
		if ('\\' != logFileName.GetAt(logFileName.GetLength() -1) )
	    {
		    logFileName = logFileName + '\\';
    	}
	}
	else
	{
		return;
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
		logFileName = logFileName + L"PrintMonitor" + myProcessName + ".log";
	}
	else
	{
		logFileName = logFileName + L"PrintMonitor.log";
	}

	CAtlStringW thread;
	thread.Format(L"0x%.8x/0x%.8x", GetCurrentProcessId(), GetCurrentThreadId());
	thread.MakeUpper();

	CAtlStringW logEntry;
	logEntry.Format(L"%s %s %s\n", now.AsCAtlString(), thread, logString);
	
	FILE *fp = _wfopen(logFileName, L"a+t"); /* to append CString contents to file*/
    if ( fp )
	{
		fputws( logEntry.GetBuffer(), fp);
		fclose(fp);
	}

	if (WriteToDebug())
	{
		OutputDebugString(logEntry.GetBuffer());
	}
}

CString CUtilities::GetErrorString(DWORD err)
{
	CAtlString Error;
    LPTSTR s;
    if( 0 == FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, err, 0, (LPTSTR)&s, 0, NULL) )
    { 
		/* failed */
		// Unknown error code %08x (%d)
        CAtlString t;
        t.Format(L"Unknown error 0x%08x (%d)", err, LOWORD(err) );
        Error = t;
    } 
    else
    {
		/* success */
        LPTSTR p = _tcschr(s, _T('\r'));
        if(p != NULL)
        { 
			/* lose CRLF */
            *p = _T('\0');
        } 
        Error = s;
        LocalFree(s);
    } 
    return Error;
}

bool CUtilities::GetPipeControl()
{
	DWORD result = 0;
	DWORD pipeControl = 0;
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
			key->QueryDWORDValue(TEXT("PipeControl"), pipeControl);
			key->Close();
		}
		delete key;
	}

	return ( 0 != pipeControl );
}

bool CUtilities::GetUseIPC()
{
	DWORD result = 0;
	DWORD useIPC = 0;
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
			key->QueryDWORDValue(TEXT("UseIPC"), useIPC);
			key->Close();
		}
		delete key;
	}

	return ( 0 != useIPC );
}

bool CUtilities::GetLockable()
{
	DWORD result = 0;
	DWORD lockable = 0;
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
			key->QueryDWORDValue(TEXT("Lockable"), lockable);
			key->Close();
		}
		delete key;
	}

	return ( 0 != lockable );
}

DWORD CUtilities::GetDelayTime()
{
	DWORD result = 0;
	DWORD delay = 0;
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
			if (ERROR_SUCCESS != key->QueryDWORDValue(TEXT("IPCDelay"), delay))
			{
				delay = 0;
			}
			key->Close();
		}
		delete key;
	}

	return delay;
}

bool CUtilities::IsIE8Job()
{
	DWORD result = 0;
	DWORD isIE8Job = 0;
	CRegKey* key;
	key = new CRegKey();
	if (NULL != key)
	{
		#ifdef _WIN64
		result = key->Open(HKEY_CURRENT_USER, SERVICE_PRINTER_IPC_REG_KEY, KEY_WOW64_32KEY | KEY_READ);
        #else
		result = key->Open(HKEY_CURRENT_USER, SERVICE_PRINTER_IPC_REG_KEY, KEY_READ);
        #endif

		if ( ERROR_SUCCESS == result )
		{
			if (ERROR_SUCCESS != key->QueryDWORDValue(TEXT("IsIE8Job"), isIE8Job))
			{
				isIE8Job = 0;
			}
			key->Close();
		}
		delete key;
	}

	return (0 != isIE8Job);
}

