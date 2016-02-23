#pragma once

#include "ServiceBase.h"
#include <atlbase.h>
#include <atlstr.h>
#include <WinUser.h>
#include "madCHook.h"
#include "Utilities.h"
#include "PrinterDefs.h"

CServiceBase* CServiceBase::m_serviceObject = NULL;

// --------------------------------------------------
//  CWRServiceBase
// --------------------------------------------------
CServiceBase::CServiceBase( const wchar_t* serviceName, const wchar_t* serviceTitle, const wchar_t* serviceDescription ) throw() : CAtlModule(), 
   m_bService(FALSE),
   m_window(NULL)
{
    // make sure there is only ever one of us.
    if (m_serviceObject!=NULL)
        AtlThrow(S_FALSE);
    m_serviceObject = this;

    // set up the initial service status 
    m_hServiceStatus = NULL;
    m_status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    m_status.dwCurrentState = SERVICE_STOPPED;
    m_status.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SESSIONCHANGE;
    m_status.dwWin32ExitCode = 0;
    m_status.dwServiceSpecificExitCode = 0;
    m_status.dwCheckPoint = 0;
    m_status.dwWaitHint = 0;

    // the service name should be written here.
    wcscpy_s( m_szServiceName, sizeof(m_szServiceName)/sizeof(wchar_t), serviceName );
    m_szServiceTitle = serviceTitle;
    m_szServiceDescription = serviceDescription;

}

// --------------------------------------------------
//  ~CWRServiceBase
// --------------------------------------------------
CServiceBase::~CServiceBase()
{
    if (m_serviceObject==this)
        m_serviceObject = NULL;
}

// --------------------------------------------------
//  SetServiceStatus
//      This function sets the service's status in 
//      the Windows SCM
// --------------------------------------------------
void CServiceBase::SetServiceStatus(DWORD dwState) throw()
{
    m_status.dwCurrentState = dwState;
    ::SetServiceStatus(m_hServiceStatus, &m_status);
}

// --------------------------------------------------
//  RegisterAppId
// --------------------------------------------------
inline HRESULT CServiceBase::RegisterAppId( bool registerAsWindowsService ) throw()
{
	HRESULT hr = UpdateRegistryAppId(TRUE);
	if (FAILED(hr))
		return hr;

	CRegKey keyAppID;
	LONG lRes = keyAppID.Open(HKEY_CLASSES_ROOT, _T("AppID"), KEY_WRITE);
	if (lRes != ERROR_SUCCESS)
		return AtlHresultFromWin32(lRes);

	CRegKey key;

	lRes = key.Create(keyAppID, GetAppIdT());
	if (lRes != ERROR_SUCCESS)
		return AtlHresultFromWin32(lRes);

	key.DeleteValue(_T("LocalService"));

	if (registerAsWindowsService)
    	key.SetStringValue(_T("LocalService"), m_szServiceName);

	return S_OK;
}

HRESULT CServiceBase::LoadDriver()
{
	wchar_t sysFile32[MAX_PATH] = {0};
	wchar_t sysFile64[MAX_PATH] = {0};

	ExpandEnvironmentStrings(L"%SystemRoot%", sysFile32, MAX_PATH);
	ExpandEnvironmentStrings(L"%SystemRoot%", sysFile64, MAX_PATH);
	#ifdef _WIN64
	lstrcatW(sysFile32, L"\\SysWOW64\\Drivers\\");
	lstrcatW(sysFile32, X86_INJECTION_DRIVER_FILE_NAME);
	lstrcatW(sysFile64, L"\\SysWOW64\\Drivers\\");
	lstrcatW(sysFile64, X64_INJECTION_DRIVER_FILE_NAME);
	#else
    lstrcatW(sysFile32, L"\\System32\\Drivers\\");
	lstrcatW(sysFile32, X86_INJECTION_DRIVER_FILE_NAME);
	lstrcatW(sysFile64, L"\\System32\\Drivers\\");
	lstrcatW(sysFile64, X64_INJECTION_DRIVER_FILE_NAME);
	#endif

	CUtilities::Dlog(L"Attempting To Load Injection Driver");
	CUtilities::DlogArg(L"    32 Bit Driver: %s",sysFile32);
	CUtilities::DlogArg(L"    64 Bit Driver: %s",sysFile64);

	if (!LoadInjectionDriver(INJECTION_SERVICE_NAME, sysFile32, sysFile64))
	{
        CUtilities::Dlog(L"Loading Of Injection Driver Service Failed");
		return S_FALSE;
	}
	else
	{
		CUtilities::Dlog(L"Loading Of Injection Driver Service Succeeded");
	}
	return S_OK;
}

HRESULT CServiceBase::InjectDLL(LPCWSTR library)
{
	if (InjectLibraryW( INJECTION_SERVICE_NAME, library, ALL_SESSIONS, CUtilities::InjectIntoSystemProcesses() ))
	{
		CUtilities::DlogArg(L"Injected %s", library);
		return S_OK;
	}
	else
	{
		CUtilities::DlogArg(L"Failed to inject %s", library);
		return S_FALSE;
	}
}

HRESULT CServiceBase::UninjectDLL(LPCWSTR library)
{
	if (UninjectLibraryW(INJECTION_SERVICE_NAME, library, ALL_SESSIONS, CUtilities::InjectIntoSystemProcesses()))
	{
		CUtilities::DlogArg(L"Uninjected %s", library);
		return S_OK;
	}
	else
	{
		CUtilities::DlogArg(L"Failed to uninject %s", library);
		return S_FALSE;
	}
}

HRESULT CServiceBase::Inject()
{
	wchar_t lib32[MAX_PATH] = {0};
	lstrcatW(lib32, CUtilities::GetLibFile32().GetBuffer());
	CUtilities::DlogArg(L"32 Bit Library In Registry = %s.", lib32);
	if (!CUtilities::FileExists(lib32))
	{
		GetModuleFileName(NULL, lib32, MAX_PATH);
		//Strip out the exe name on the end
		for (int i1 = lstrlenW(lib32) - 1; i1 > 0; i1--)
		{
            if (lib32[i1] == L'\\') 
	    	{
				lib32[i1 + 1] = 0;
				break;
			}
		}

        lstrcatW(lib32, X86_INJECTION_LIBRARY_FILE_NAME);

		if (!CUtilities::FileExists(lib32))
		{
			CUtilities::DlogArg(L"File %s does not exist.", lib32);
			return S_FALSE;
    	}
	}

	if ( S_OK != InjectDLL(lib32) )
	{
		return S_FALSE;
	}

    #ifdef _WIN64
	wchar_t lib64[MAX_PATH] = {0};
	lstrcatW(lib64, CUtilities::GetLibFile64().GetBuffer());
	CUtilities::DlogArg(L"64 Bit Library In Registry = %s.", lib64);
	if (!CUtilities::FileExists(lib64))
	{
		GetModuleFileName(NULL, lib64, MAX_PATH);
		//Strip out the exe name on the end
		for (int i1 = lstrlenW(lib64) - 1; i1 > 0; i1--)
		{
            if (lib64[i1] == L'\\') 
	    	{
				lib64[i1 + 1] = 0;
				break;
			}
		}

        lstrcatW(lib64, X64_INJECTION_LIBRARY_FILE_NAME);

		if (!CUtilities::FileExists(lib64))
		{
			CUtilities::DlogArg(L"File %s does not exist.", lib64);
			return S_FALSE;
    	}
	}

	if ( S_OK != InjectDLL(lib64) )
	{
		return S_FALSE;
	}
    #endif

	return S_OK;
}

HRESULT CServiceBase::Uninject()
{
	wchar_t lib32[MAX_PATH] = {0};
	lstrcatW(lib32, CUtilities::GetLibFile32().GetBuffer());
	CUtilities::DlogArg(L"32 Bit Library In Registry = %s.", lib32);
	if (!CUtilities::FileExists(lib32))
	{
		GetModuleFileName(NULL, lib32, MAX_PATH);
		//Strip out the exe name on the end
		for (int i1 = lstrlenW(lib32) - 1; i1 > 0; i1--)
		{
            if (lib32[i1] == L'\\') 
	    	{
				lib32[i1 + 1] = 0;
				break;
			}
		}

        lstrcatW(lib32, X86_INJECTION_LIBRARY_FILE_NAME);

		if (!CUtilities::FileExists(lib32))
		{
			CUtilities::DlogArg(L"File %s does not exist.", lib32);
			return S_FALSE;
    	}
	}

	if ( S_OK != UninjectDLL(lib32) )
	{
		return S_FALSE;
	}

    #ifdef _WIN64
	wchar_t lib64[MAX_PATH] = {0};
	lstrcatW(lib64, CUtilities::GetLibFile64().GetBuffer());
	CUtilities::DlogArg(L"64 Bit Library In Registry = %s.", lib64);
	if (!CUtilities::FileExists(lib64))
	{
		GetModuleFileName(NULL, lib64, MAX_PATH);
		//Strip out the exe name on the end
		for (int i1 = lstrlenW(lib64) - 1; i1 > 0; i1--)
		{
            if (lib64[i1] == L'\\') 
	    	{
				lib64[i1 + 1] = 0;
				break;
			}
		}

        lstrcatW(lib64, X64_INJECTION_LIBRARY_FILE_NAME);

		if (!CUtilities::FileExists(lib64))
		{
			CUtilities::DlogArg(L"File %s does not exist.", lib64);
			return S_FALSE;
    	}
	}

	if ( S_OK != UninjectDLL(lib64) )
	{
		return S_FALSE;
	}
    #endif

	return S_OK;
}

HRESULT CServiceBase::CopySysFile32()
{
	wchar_t fileName[MAX_PATH] = {0};
	wchar_t sysFile[MAX_PATH] = {0};
	
	ExpandEnvironmentStrings(L"%SystemRoot%", sysFile, MAX_PATH);
	#ifdef _WIN64
	lstrcatW(sysFile, L"\\SysWOW64\\Drivers\\");
	lstrcatW(sysFile, X86_INJECTION_DRIVER_FILE_NAME);
	#else
    lstrcatW(sysFile, L"\\System32\\Drivers\\");
	lstrcatW(sysFile, X86_INJECTION_DRIVER_FILE_NAME);
	#endif

	lstrcatW(fileName, CUtilities::GetSysFile32().GetBuffer());
	CUtilities::DlogArg(L"File Name In Registry = %s.", fileName);
	if (!CUtilities::FileExists(fileName))
	{
		GetModuleFileName(NULL, fileName, MAX_PATH);
		//Strip out the exe name on the end
		for (int i1 = lstrlenW(fileName) - 1; i1 > 0; i1--)
		{
            if (fileName[i1] == L'\\') 
	    	{
				fileName[i1 + 1] = 0;
				break;
			}
		}

        lstrcatW(fileName, X86_INJECTION_DRIVER_FILE_NAME);

		if (!CUtilities::FileExists(fileName))
		{
			CUtilities::DlogArg(L"File %s does not exist.", fileName);
			return S_FALSE;
    	}
	}
	else
	{
		CUtilities::DlogArg(L"File %s exists.", fileName);
	}

	CUtilities::Dlog(L"Copying 32Bit Driver File.");
	CUtilities::DlogArg(L"    Source = %s", fileName);
	CUtilities::DlogArg(L"    Destination = %s", sysFile);
	if (!CopyFile(fileName, sysFile, false))
	{
		CUtilities::DlogArg(L"Copy Failed: %s", sysFile);
		return S_FALSE;
	}
	else
	{
		CUtilities::Dlog(L"File Copied");
	}

	return S_OK;
}

HRESULT CServiceBase::CopySysFile64()
{
	wchar_t fileName[MAX_PATH] = {0};
	wchar_t sysFile[MAX_PATH] = {0};
	
	ExpandEnvironmentStrings(L"%SystemRoot%", sysFile, MAX_PATH);
	#ifdef _WIN64
	lstrcatW(sysFile, L"\\SysWOW64\\Drivers\\");
	lstrcatW(sysFile, X64_INJECTION_DRIVER_FILE_NAME);
	#else
    lstrcatW(sysFile, L"\\System32\\Drivers\\");
	lstrcatW(sysFile, X64_INJECTION_DRIVER_FILE_NAME);
	#endif

	lstrcatW(fileName, CUtilities::GetSysFile64().GetBuffer());
	CUtilities::DlogArg(L"File Name In Registry = %s.", fileName);
	if (!CUtilities::FileExists(fileName))
	{
		GetModuleFileName(NULL, fileName, MAX_PATH);
		//Strip out the exe name on the end
		for (int i1 = lstrlenW(fileName) - 1; i1 > 0; i1--)
		{
            if (fileName[i1] == L'\\') 
	    	{
				fileName[i1 + 1] = 0;
				break;
			}
		}
		lstrcatW(fileName, X64_INJECTION_DRIVER_FILE_NAME);
        if (!CUtilities::FileExists(fileName))
		{
			CUtilities::DlogArg(L"File %s does not exist.", fileName);
			return S_FALSE;
    	}
	}
	else
	{
		CUtilities::DlogArg(L"File %s exists.", fileName);
	}

	CUtilities::Dlog(L"Copying 32Bit Driver File.");
	CUtilities::DlogArg(L"    Source = %s", fileName);
	CUtilities::DlogArg(L"    Destination = %s", sysFile);
	if (!CopyFile(fileName, sysFile, false))
	{
		CUtilities::DlogArg(L"Copy Failed: %s", sysFile);
		return S_FALSE;
	}
	else
	{
		CUtilities::Dlog(L"File Copied");
	}

	return S_OK;
}

// --------------------------------------------------
//  UnregisterAppId
// --------------------------------------------------
HRESULT CServiceBase::UnregisterAppId() throw()
{
	// First remove entries not in the RGS file.
	CRegKey keyAppID;
	LONG lRes = keyAppID.Open(HKEY_CLASSES_ROOT, _T("AppID"), KEY_WRITE);
	if (lRes != ERROR_SUCCESS)
		return AtlHresultFromWin32(lRes);

	CRegKey key;
	lRes = key.Open(keyAppID, GetAppIdT(), KEY_WRITE);
	if (lRes != ERROR_SUCCESS)
		return AtlHresultFromWin32(lRes);
	key.DeleteValue(_T("LocalService"));

	return UpdateRegistryAppId(FALSE);
}

// --------------------------------------------------
//  IsInstalledInSCM
// --------------------------------------------------
BOOL CServiceBase::IsInstalledInSCM() throw()
{
    BOOL bResult = FALSE;

    SC_HANDLE hSCM = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

    if (hSCM != NULL)
    {
	    SC_HANDLE hService = ::OpenService(hSCM, m_szServiceName, SERVICE_QUERY_CONFIG);
	    if (hService != NULL)
	    {
		    bResult = TRUE;
		    ::CloseServiceHandle(hService);
	    }
	    ::CloseServiceHandle(hSCM);
    }
    return bResult;
}

// --------------------------------------------------
//  Install
// --------------------------------------------------
HRESULT CServiceBase::Install( bool registerAsWindowsService ) throw()
{
    HRESULT returnValue = S_OK;

    // Get the executable file path
    WCHAR szFilePath[MAX_PATH + _ATL_QUOTES_SPACE];
    DWORD dwFLen = ::GetModuleFileName(NULL, szFilePath + 1, MAX_PATH);
    if( dwFLen == 0 || dwFLen == MAX_PATH )
        return S_FALSE;

    // Quote the FilePath before calling CreateService
    szFilePath[0] = L'\"';
    szFilePath[dwFLen + 1] = L'\"';
    szFilePath[dwFLen + 2] = 0;
    
    if ( (registerAsWindowsService) && (!IsInstalledInSCM()) )
    {
        SC_HANDLE hSCM = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
        if (hSCM == NULL)
        {
            /*
            WCHAR szBuf[1024];
            if (AtlLoadString(ATL_SERVICE_MANAGER_OPEN_ERROR, szBuf, 1024) == 0)
            Checked::wcscpy_s(szBuf, _countof(szBuf), L"Could not open Service Manager");
            MessageBox(NULL, szBuf, m_szServiceName, MB_OK);
            */
			CUtilities::DlogArg(L"OpenSCManager failed: %s", CUtilities::GetErrorString(GetLastError()) );
            return S_FALSE;
        }

        SC_HANDLE hService = ::CreateService(
            hSCM, m_szServiceName, m_szServiceTitle,
            SERVICE_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS,
            SERVICE_AUTO_START, SERVICE_ERROR_NORMAL,
            szFilePath, L"Base", NULL, NULL, NULL, NULL);

        if (hService == NULL)
        {
			CUtilities::DlogArg(L"CreateService failed: %s", CUtilities::GetErrorString(GetLastError()) );
            return S_FALSE;
        }
        else
        {
			CUtilities::Dlog(L"CreateService Succeeded");
            CRegKey obReg;
            CAtlStringW keyPath;
            keyPath = L"SYSTEM\\CurrentControlSet\\Services\\";
            keyPath += m_szServiceName;
		    if(obReg.Open(HKEY_LOCAL_MACHINE, keyPath, KEY_ALL_ACCESS) == ERROR_SUCCESS)
    			obReg.SetStringValue( L"Description", m_szServiceDescription);
			obReg.Close();
			if(obReg.Open(HKEY_LOCAL_MACHINE, L"SYSTEM\\CurrentControlSet\\Control\\SafeBoot\\Minimal", KEY_ALL_ACCESS) == ERROR_SUCCESS)
				obReg.SetKeyValue(m_szServiceName, L"Service");
			obReg.Close();
			if(obReg.Open(HKEY_LOCAL_MACHINE, L"SYSTEM\\CurrentControlSet\\Control\\SafeBoot\\Network", KEY_ALL_ACCESS) == ERROR_SUCCESS)
				obReg.SetKeyValue(m_szServiceName, L"Service");
			obReg.Close();
        }

        ::CloseServiceHandle(hService);
        ::CloseServiceHandle(hSCM);
    }

    // Now register the AppId (slightly different if we are registering as a service or as an app)
    returnValue = RegisterAppId( registerAsWindowsService );

	OnPostInstall();

    return returnValue;
}


// --------------------------------------------------
//  Uninstall
// --------------------------------------------------
HRESULT CServiceBase::Uninstall() throw()
{
    HRESULT returnValue = S_OK;

    returnValue = UnregisterAppId();

    SC_HANDLE hSCM = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if (hSCM == NULL)
    {
        return S_FALSE;
    }

    SC_HANDLE hService = ::OpenService(hSCM, m_szServiceName, SERVICE_STOP | DELETE);

    if (hService == NULL)
    {
        ::CloseServiceHandle(hSCM);
        return S_FALSE;
    }
    SERVICE_STATUS status;
    BOOL bRet = ::ControlService(hService, SERVICE_CONTROL_STOP, &status);
    if (!bRet)
    {
        DWORD dwError = GetLastError();
        if (!((dwError == ERROR_SERVICE_NOT_ACTIVE) ||  (dwError == ERROR_SERVICE_CANNOT_ACCEPT_CTRL && status.dwCurrentState == SERVICE_STOP_PENDING)))
        {
			//Put some logging here
        }
    }

    // TODO: investigate whether this causes the service to be sceduled for deletion if
    // we were unable to stop the service
	//TTP 25446: 6.0.1.14 > Safe mode > App pops "cannot connect to consumer service" message. This must be done before DeleteService.
	CRegKey obReg;	
	try
	{
		if(obReg.Open(HKEY_LOCAL_MACHINE, L"SYSTEM\\CurrentControlSet\\Control\\SafeBoot\\Minimal", KEY_ALL_ACCESS) == ERROR_SUCCESS)
			obReg.DeleteSubKey(m_szServiceName);
		obReg.Close();
		if(obReg.Open(HKEY_LOCAL_MACHINE, L"SYSTEM\\CurrentControlSet\\Control\\SafeBoot\\Network", KEY_ALL_ACCESS) == ERROR_SUCCESS)
			obReg.DeleteSubKey(m_szServiceName);
		obReg.Close();
	}
	catch(...)
	{
	}

    BOOL bDelete = ::DeleteService(hService);
    ::CloseServiceHandle(hService);
    ::CloseServiceHandle(hSCM);

    OnPostUninstall();

    if (bDelete)
        return S_OK;

    return S_FALSE;
}

// --------------------------------------------------
//  Handler
// --------------------------------------------------
void CServiceBase::Handler(DWORD dwOpcode, DWORD dwEventType, LPVOID lpEventData, LPVOID lpContext) throw()
{
    CServiceBase* pT = static_cast<CServiceBase*>(this);

    switch (dwOpcode)
    {
    case SERVICE_CONTROL_STOP:
	    pT->OnStopRequested();
	    break;
    case SERVICE_CONTROL_PAUSE:
	    pT->OnPauseRequested();
	    break;
    case SERVICE_CONTROL_CONTINUE:
	    pT->OnContinueRequested();
	    break;
    case SERVICE_CONTROL_INTERROGATE:
	    pT->OnInterrogateRequested();
	    break;
    case SERVICE_CONTROL_SHUTDOWN:
	    pT->OnShutdownRequested();
	    break;
	case SERVICE_CONTROL_SESSIONCHANGE:
		{
			WTSSESSION_NOTIFICATION* pSession = (WTSSESSION_NOTIFICATION*) lpEventData;
			pT->OnSessionChange( dwEventType, pSession->dwSessionId);
		}
		break;
    default:
	    pT->OnUnknownRequest(dwOpcode);
    }
}

// --------------------------------------------------
//  OnStopRequested
// --------------------------------------------------
void CServiceBase::OnStopRequested() throw()
{
    if (m_bService) 
        SetServiceStatus(SERVICE_STOP_PENDING);
    PostThreadMessage(m_dwServiceThreadID, WM_QUIT, 0, 0);
}

// --------------------------------------------------
//  OnPause
// --------------------------------------------------
void CServiceBase::OnPauseRequested() throw()
{
    //Will Put logging here but probably need to pause threads as well
}

// --------------------------------------------------
//  OnContinue
// --------------------------------------------------
void CServiceBase::OnContinueRequested() throw()
{
    //Will Put logging here but probably need to pause threads as well
}

// --------------------------------------------------
//  OnInterrogate
// --------------------------------------------------
void CServiceBase::OnInterrogateRequested() throw()
{
    //Will Put logging here but probably need to pause threads as well
}

// --------------------------------------------------
//  OnShutdownRequested
// --------------------------------------------------
void CServiceBase::OnShutdownRequested() throw()
{
    //Will Put logging here but probably need to pause threads as well
}

// --------------------------------------------------
//  OnSessionChange
// --------------------------------------------------
void CServiceBase::OnSessionChange( DWORD dwEventType, DWORD dwSessionId ) throw()
{
    //Will Put logging here but probably need to pause threads as well
}

// --------------------------------------------------
//  OnUnknownRequest
// --------------------------------------------------
void CServiceBase::OnUnknownRequest(DWORD /*dwOpcode*/) throw()
{
    //Will Put logging here but probably need to pause threads as well
}

// --------------------------------------------------
//  OnPostInstall
// --------------------------------------------------
HRESULT CServiceBase::OnPostInstall() throw()
{
    return S_OK;
}

// --------------------------------------------------
//  OnPostUninstall
// --------------------------------------------------
HRESULT CServiceBase::OnPostUninstall() throw()
{
    //Will Put logging here but probably need to pause threads as well
    return S_OK;
}

// --------------------------------------------------
//  PreMessageLoop
// --------------------------------------------------
HRESULT CServiceBase::PreMessageLoop(int /*nShowCmd*/) throw()
{
    HRESULT hr = S_OK;

    return hr;
}

// --------------------------------------------------
//  RunMessageLoop
// --------------------------------------------------
void CServiceBase::RunMessageLoop() throw()
{
    // Modify the message loop to handle the crazy case of stopping
    // the service prematurely.  The application basically sends a 
    // quit message to this message loop.
    // the change here will keep the message loop processing until a timeout
    // in which case the service will simply exit.
    // but first, all COM objects should be shutdown and disconnected immediately.

    // the original code allows COM to usurp our message QUEUE.  In essence,
    // the service could continue running indefinitely if there is a large
    // volume of COM activity happening.  (the WM_QUIT never gets processed)
    
    MSG msg;
    while (GetMessage(&msg, 0, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

// --------------------------------------------------
//  PostMessageLoop
// --------------------------------------------------
HRESULT CServiceBase::PostMessageLoop() throw()
{
    HRESULT hr = S_OK;			
    return hr;
}

// --------------------------------------------------
//  WinMain
// --------------------------------------------------
int CServiceBase::WinMain(int nShowCmd) throw()
{
	CUtilities::Dlog(L"Entering WinMain Event Of Base Service");
    HRESULT hr = S_OK;
    bool executedSpecialCommand = false;
    m_bService = FALSE;

	m_dwStartupThreadID = GetCurrentThreadId();

    LPTSTR lpCmdLine = GetCommandLine(); 

	CUtilities::DlogArg(L"Command Line: %s", lpCmdLine);

    TCHAR szTokens[] = _T("-/");

    LPCWSTR lpszToken = FindOneOf(lpCmdLine, szTokens);
    while (lpszToken != NULL)
    {
		CUtilities::DlogArg(L"Token: %s", lpszToken);
        if (WordCmpI(lpszToken, _T("InstallService"))==0)
        {
			CUtilities::Dlog(L"Executing Install");
            executedSpecialCommand = true;

            // first uninstall everything
            hr = Uninstall();

            // then install everything (as-a-service)
            hr = Install( true );

            break;
        }
        else if (WordCmpI(lpszToken, _T("Uninstall"))==0)
        {
            executedSpecialCommand = true;
            hr = this->Uninstall();
            break;
        }
        else if (WordCmpI(lpszToken, _T("Simulate"))==0)
        {
            executedSpecialCommand = true;
            hr = SimulateRun( true, nShowCmd );
            break;
        }
        lpszToken = FindOneOf(lpszToken, szTokens);
    }

    if (!executedSpecialCommand)
    {
        m_bService = TRUE;
        // no special command line, assume we are a service
        // and hook up the service entry point.
	    SERVICE_TABLE_ENTRY st[] =
	    {
		    { m_szServiceName, _ServiceMain },
		    { NULL, NULL }
	    };
	    if (::StartServiceCtrlDispatcher(st) == 0)
        {
            m_bService = FALSE;
		    m_status.dwWin32ExitCode = GetLastError();

            hr = m_status.dwWin32ExitCode;
        }
    }

    return hr;
}

// --------------------------------------------------
//  Run
//      This function executes the main service
//      run.  It basically runs a message loop in
//      the main thread.
// --------------------------------------------------
HRESULT CServiceBase::Run(int nShowCmd = SW_HIDE) throw()
{
	
    HRESULT hr = S_OK;

    hr = this->PreMessageLoop(nShowCmd);

    if (hr == S_OK)
	    this->RunMessageLoop();

    if (SUCCEEDED(hr))
    {
	    hr = this->PostMessageLoop();
    }

    return hr;
}

// --------------------------------------------------
//  WaitForService
// --------------------------------------------------
HRESULT CServiceBase::WaitForService( const wchar_t* serviceName, DWORD timeoutInSeconds )
{
    // Wait here for RPC service to initialize.  
    // Then allow the rest of this code to run.
    HRESULT returnValue = S_FALSE;
    BOOL rpcStarted = FALSE;
    unsigned int loopCount = 0; // in 1/4 seconds
    SERVICE_STATUS rpcServiceStatus;
    rpcServiceStatus.dwCurrentState = SERVICE_STOPPED;
    while ((rpcServiceStatus.dwCurrentState != SERVICE_RUNNING) && (loopCount < timeoutInSeconds*4)) // loop until rpc is initialized or timeoutInSeconds seconds (timeoutInSeconds*4 * 250ms) passes.
    {
        if (loopCount) 
        {
            Sleep(250); // wait 1/4 second, but only the second time through.
        }
        SC_HANDLE hSCM = ::OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
        if (hSCM != NULL)
        {
    	    SC_HANDLE hService = ::OpenService(hSCM, serviceName, SERVICE_QUERY_STATUS | SERVICE_INTERROGATE);
	        if (hService != NULL)
	        {                
        	    if (!::ControlService( hService, SERVICE_CONTROL_INTERROGATE, &rpcServiceStatus ))                
                {
                    //Log Last WindowsError
                } 
    		    ::CloseServiceHandle(hService);
	        }
            else
            {
				//Log an error here
            }
	        ::CloseServiceHandle(hSCM);
        }
        loopCount++;
    }

    if (loopCount >= timeoutInSeconds*4)
    {
        returnValue = S_FALSE;
    }
    else
    {
        returnValue = S_OK; 
    }

    return S_OK;
}

// --------------------------------------------------
//  SimulateRun
// --------------------------------------------------
HRESULT CServiceBase::SimulateRun( bool shouldUseControllingWindow, int nShowCmd = SW_HIDE ) throw()
{
    HRESULT hr = S_OK;
    CAtlStringW moduleFilename;

    wchar_t moduleFullPathFilename[MAX_PATH];
    if (::GetModuleFileNameW( NULL, moduleFullPathFilename, (sizeof(moduleFullPathFilename)/2) ))
    {
        moduleFilename = moduleFullPathFilename;
        moduleFilename = moduleFilename.Right(moduleFilename.GetLength() - moduleFilename.ReverseFind(L'\\') - 1);
    }
    else
        hr = S_FALSE;

    hr = OnPreinitialize();

    // Wait here for RPC service to initialize.  
    WaitForService( L"RpcSs", 60 );

    hr = OnInitialize();
        
    // create a dummy window so the user can close it. (thereby
    // ending the service)
    m_window = new CServiceWindow();
    RECT smallWindowRect = {100,100,500,500};
    m_window->Create( NULL, &smallWindowRect, moduleFilename, WS_OVERLAPPEDWINDOW );//, DWORD dwExStyle = 0, UINT nID = 0, LPVOID lpCreateParam = NULL)
    m_window->ShowWindow( SW_SHOW );

    // Run the service. Note: the close event of the window
    // Should post a PostThreadMessage(m_dwThreadID, WM_QUIT, 0, 0);
    // that way the message loop knows that it should exit.
    hr = this->Run(nShowCmd);    

    hr = OnUninitialize();

    return hr;
}

// --------------------------------------------------
//  ServiceMain
// --------------------------------------------------
void CServiceBase::ServiceMain(DWORD dwArgc, LPWSTR* lpszArgv) throw()
{
    lpszArgv;
    dwArgc;
    HRESULT hr = S_FALSE;

    // reset the main thread ID here because it changes when we 
    // are in the service main thread.
    m_dwServiceThreadID = GetCurrentThreadId();

    // Register the control request handler
    m_status.dwCurrentState = SERVICE_START_PENDING;
	m_hServiceStatus = RegisterServiceCtrlHandlerEx( m_szServiceName, _Handler, NULL);
    if (m_hServiceStatus == NULL)
    {
        //LogEvent(L"Handler not installed");
        return;
    }
    // tell the OS that we are now running.
    //SetServiceStatus(SERVICE_START_PENDING);
    SetServiceStatus(SERVICE_RUNNING);

    OnPreinitialize();

    // Wait here for RPC service to initialize.  
    WaitForService( L"RpcSs", 60 );

  
	OnInitialize();

	m_status.dwWin32ExitCode = S_OK;
    m_status.dwCheckPoint = 0;
    m_status.dwWaitHint = 0;

    CServiceBase* pT = static_cast<CServiceBase*>(this);
        
    // When the Run function returns, the service has stopped.
    m_status.dwWin32ExitCode = pT->Run(SW_HIDE);

    OnUninitialize();

    try //it will cause TTP 36810: crash.
    {
        SetServiceStatus(SERVICE_STOPPED);
    }
    catch(...)
    {
        //Log Exception
    }
}

// --------------------------------------------------
//  OnInitialize
// --------------------------------------------------
HRESULT CServiceBase::OnInitialize()
{
	//m_listener.ThreadStart(true);
	HRESULT returnValue = CopySysFile32();

	returnValue = CopySysFile64();

	returnValue = LoadDriver();

	if (S_OK == returnValue)
	{
		returnValue = Inject();
	}

    return returnValue;
	
	return S_OK;
}

// --------------------------------------------------
//  OnPreinitialize
// --------------------------------------------------
HRESULT CServiceBase::OnPreinitialize()
{
    HRESULT returnValue = S_OK;

    //Here we will spawn the logging class

    return returnValue;
}

// --------------------------------------------------
//  OnUninitialize
// --------------------------------------------------
HRESULT CServiceBase::OnUninitialize()
{
	//m_listener.ThreadStop();

    HRESULT returnValue = S_OK;

	
	Uninject();
	if (!StopInjectionDriver(INJECTION_SERVICE_NAME))
	{
		CUtilities::Dlog(L"Stopping driver failed");
	}
    
    //Here we will free the logging class

    return returnValue;
}

// --------------------------------------------------
//  WordCmpI
// --------------------------------------------------
int CServiceBase::WordCmpI(LPCTSTR psz1, LPCTSTR psz2) throw()
{

    TCHAR c1 = (TCHAR)CharUpper((LPTSTR)*psz1);
    TCHAR c2 = (TCHAR)CharUpper((LPTSTR)*psz2);
    while (c1 != NULL && c1 == c2 && c1 != ' ' && c1 != '\t')
    {
	    psz1 = CharNext(psz1);
	    psz2 = CharNext(psz2);
	    c1 = (TCHAR)CharUpper((LPTSTR)*psz1);
	    c2 = (TCHAR)CharUpper((LPTSTR)*psz2);
    }
    if ((c1 == NULL || c1 == ' ' || c1 == '\t') && (c2 == NULL || c2 == ' ' || c2 == '\t'))
	    return 0;

    return (c1 < c2) ? -1 : 1;
}

// --------------------------------------------------
//  FindOneOf
//      search for an occurence of string p2 in string p1
// --------------------------------------------------
LPCTSTR CServiceBase::FindOneOf(LPCTSTR p1, LPCTSTR p2) throw()
{
    while (p1 != NULL && *p1 != NULL)
    {
	    LPCTSTR p = p2;
	    while (p != NULL && *p != NULL)
	    {
		    if (*p1 == *p)
			    return CharNext(p1);
		    p = CharNext(p);
	    }
	    p1 = CharNext(p1);
    }
    return NULL;
}

void WINAPI CServiceBase::_ServiceMain(DWORD dwArgc, LPTSTR* lpszArgv) throw()
{
    dynamic_cast<CServiceBase*>(m_serviceObject)->ServiceMain(dwArgc, lpszArgv);
}

DWORD WINAPI CServiceBase::_Handler(DWORD dwOpcode, DWORD dwEventType, LPVOID lpEventData, LPVOID lpContext) throw()
{
    dynamic_cast<CServiceBase*>(m_serviceObject)->Handler(dwOpcode, dwEventType, lpEventData, lpContext); 

	return NO_ERROR;
}
