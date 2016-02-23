#pragma once

#include <windows.h>
#include <sys/types.h>  // For stat().
#include <sys/stat.h>   // For stat().
#include "PrinterDefs.h"
#include "HiResTime.h"


#define LOG_BUFFER_WIDTH 255
#define FUNCTION_NAME_WIDTH 20

#define DLog( message ) _Trace( __FUNCTION__, message )
#define DLogArg( message, ... ) _TraceArg( __FUNCTION__, message, __VA_ARGS__ )

void _TraceArg( const char* functionName, const WCHAR* message, ... );
void _Trace( const char* functionName, const WCHAR* message);

class CUtilities
{
public:
	static bool DebuggingEnabled();
    static bool OneLogPerProces();
	static CAtlStringW GetApplicationName();
	static CAtlStringW GetApplicationPath();
	static CAtlStringW GetApplicationDirecotry(bool includeTrailingDelimiter);
    static CAtlStringW LogFileDir();
	static bool InjectIntoSystemProcesses();
	static bool FileExists(wchar_t* strFilename); 
	static bool DirectoryExists(wchar_t* strDirectoryName); 
	static CAtlStringW GetSysFile32();
	static CAtlStringW GetSysFile64();
	static CAtlStringW GetLibFile32();
	static CAtlStringW GetLibFile64();
    static CAtlStringW MonitoredProcess();
	static bool WriteToDebug();
	static void PrintLogArg( const WCHAR* message, ... );
	static void PrintLog(const WCHAR* logString);
	static CAtlStringW GetErrorString( DWORD err );
	static bool GetPipeControl();
	static bool GetUseIPC();
	static bool GetLockable();
	static DWORD GetDelayTime();
	static bool IsIE8Job();
    static void Dlog(const WCHAR* logString);
    static void DlogArg( const WCHAR* message, ... );
};
