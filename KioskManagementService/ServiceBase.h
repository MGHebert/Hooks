#pragma once

#include <atlbase.h>
#include "atlwin.h"
#include "atlstr.h"
#include "TCPServer.h"

class CServiceWindow : public CWindowImpl<CServiceWindow>
{
    public:
        DECLARE_WND_CLASS(_T("WRServiceWindow_0EC5E8DBA2C2463884EF404B50EEAC30"))

        BEGIN_MSG_MAP(CWRServiceWindow)
            MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
        END_MSG_MAP()

        LRESULT OnDestroy( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled )
        {
            PostQuitMessage(0);
            bHandled = FALSE;
            return 0;
        }

};

class CServiceBase : public CAtlModule 
{
    public :
        CServiceBase( const wchar_t* serviceName, const wchar_t* serviceTitle, const wchar_t* serviceDescription ) throw();
        virtual ~CServiceBase();

	    int WinMain(int nShowCmd) throw();
        HRESULT Install( bool registerAsWindowsService ) throw();
        HRESULT Uninstall() throw();

        // this is here purely for ATL
        //virtual HRESULT AddCommonRGSReplacements(IRegistrarBase* /*pRegistrar*/) throw(){return E_FAIL;};
	    virtual HRESULT AddCommonRGSReplacements(IRegistrarBase* pRegistrar) throw()
	    {
		    return pRegistrar->AddReplacement(L"APPID", GetAppId());
	    }

    protected:
        // derived class overridable functions
        virtual void OnStopRequested() throw();
        virtual void OnPauseRequested() throw();
        virtual void OnContinueRequested() throw();
        virtual void OnInterrogateRequested() throw();
        virtual void OnShutdownRequested() throw();
        virtual void OnSessionChange( DWORD dwEventType, DWORD dwSessionId ) throw();
        virtual void OnUnknownRequest(DWORD /*dwOpcode*/) throw();
        virtual HRESULT OnPostUninstall() throw();
        virtual HRESULT OnPostInstall() throw();
        virtual HRESULT OnInitialize();
        virtual HRESULT OnPreinitialize();
        virtual HRESULT OnUninitialize();
		HRESULT LoadDriver();
		HRESULT InjectDLL(LPCWSTR library);
		HRESULT UninjectDLL(LPCWSTR library);
		HRESULT Inject();
		HRESULT Uninject();
		HRESULT CopySysFile32();
		HRESULT CopySysFile64();
        HRESULT UnregisterAppId() throw();
        HRESULT RegisterAppId( bool registerAsWindowsService ) throw();

    
        virtual HRESULT WINAPI UpdateRegistryAppId(BOOL bRegister) throw()
	    {
            return E_NOTIMPL;
    	}

	    virtual LPCOLESTR GetAppId() throw()
	    {
		    return NULL;
	    }

	    virtual TCHAR* GetAppIdT() throw()
	    {
		    return NULL;
	    }

        HRESULT WaitForService( const wchar_t* serviceName, DWORD timeout );

    private:

	    HRESULT Run(int nShowCmd) throw();
	    void SetServiceStatus(DWORD dwState) throw();
	    BOOL IsInstalledInSCM() throw();
	    void Handler(DWORD dwOpcode, DWORD dwEventType, LPVOID lpEventData, LPVOID lpContext) throw();
        HRESULT SimulateRun( bool shouldUseControllingWindow, int nShowCmd ) throw();
        void RunMessageLoop() throw();
        HRESULT PostMessageLoop() throw();
        HRESULT PreMessageLoop(int /*nShowCmd*/) throw();
        void ServiceMain(DWORD dwArgc, LPWSTR* lpszArgv) throw();
	    static int WordCmpI(LPCTSTR psz1, LPCTSTR psz2) throw();
	    static LPCTSTR FindOneOf(LPCTSTR p1, LPCTSTR p2) throw();
	    static void WINAPI _ServiceMain(DWORD dwArgc, LPTSTR* lpszArgv) throw();
	    static DWORD WINAPI _Handler( DWORD dwOpcode, DWORD dwEventType, LPVOID lpEventData, LPVOID lpContext ) throw();

	    CAtlStringW m_szServiceTitle;
	    CAtlStringW m_szServiceDescription;
	    SERVICE_STATUS_HANDLE m_hServiceStatus;
	    SERVICE_STATUS m_status;
	    BOOL m_bService;
	    DWORD m_dwServiceThreadID;
	    DWORD m_dwStartupThreadID;
    	bool m_bComInitialized;    // Flag that indicates if ATL initialized COM
        CServiceWindow* m_window;

		//CTCPServer m_listener;
        static CServiceBase* m_serviceObject;

    protected:
	    TCHAR m_szServiceName[256];

};

// These macros allow the derived class to specify the appid in a resource
#define WR_DECLARE_REGISTRY_APPID_RESOURCEID(resid, appid) \
	LPCOLESTR GetAppId() throw() \
	{ \
		return OLESTR(appid); \
	} \
	TCHAR* GetAppIdT() throw() \
	{ \
		return _T(appid); \
	} \
	HRESULT WINAPI UpdateRegistryAppId(BOOL bRegister) throw() \
	{ \
		ATL::_ATL_REGMAP_ENTRY aMapEntries [] = \
		{ \
			{ OLESTR("APPID"), GetAppId() }, \
			{ NULL, NULL } \
		}; \
		return ATL::_pAtlModule->UpdateRegistryFromResource(resid, bRegister, aMapEntries); \
	}

#define WR_DECLARE_LIBID(libid) \
	void InitLibId() throw() \
	{ \
		ATL::CAtlModule::m_libid = libid; \
	}
