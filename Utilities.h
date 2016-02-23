#pragma once

#include "printerdefs.h"
#include "HiResTime.h"

class CUtilities
{
public:
	static DWORD pagesPrinted;
    static DWORD copies;
    static DWORD pagesRejected;
    static bool startPage;
    static bool rejectingPages;
	static bool CUtilities::DebuggingEnabled();
    static bool CUtilities:: OneLogPerProces();
    static CAtlString CUtilities:: LogFileDir();
    static CAtlString CUtilities::MonitoredProcess();
    static void CUtilities::Dlog(CAtlString logString);
};
