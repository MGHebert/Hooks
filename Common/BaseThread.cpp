/***************************************************************************
 * PROPRIETARY/CONFIDENTIAL.  TBD                                          *
 ***************************************************************************
 * File:                                                                   *
 *   WRThread.cpp                                                          *
 * Date:                                                                   *
 *   1/22/2007                                                             *
 * Author:                                                                 *
 *   smcbride                                                              *
 * Description:                                                            *
 *   This Class is an implementation of a Thread that has                  *
 *   MiniDump support.                                                     *
 *                                                                         *
 ***************************************************************************/

#include "BaseThread.h"
#include "process.h"

#undef  WR_CURRENT_FILE_ID
#define WR_CURRENT_FILE_ID WR_FILE_WRThread_cpp
 
// --------------------------------------------------
//  CWRThread
// --------------------------------------------------
CBaseThread::CBaseThread():
    m_threadHandle(NULL),
    m_threadId(0),
    m_finishedEvent(NULL),
    m_shouldFinishEvent(NULL),
    m_terminated(true),
    m_started(false)
{
    
}

// --------------------------------------------------
//  ThreadStart
// --------------------------------------------------
HRESULT CBaseThread::ThreadStart(bool bWaitTillStarted, DWORD dwMilliseconds)
{
    // first make sure we are not already running
    if (m_started)
        return S_FALSE;

    // Create my mutexes
    m_finishedEvent = CreateEvent( NULL, TRUE, FALSE, NULL );
    m_shouldFinishEvent = CreateEvent( NULL, TRUE, FALSE, NULL );

    if (!m_finishedEvent)
        return S_FALSE;

	// Set Started event
	m_startedEvent = CreateEvent( NULL, TRUE, FALSE, NULL );

    m_terminated = false;
    m_threadHandle = (HANDLE)_beginthreadex(NULL, 0, _ThreadProc, (void*)this, 0, &m_threadId);
    if (m_threadHandle)
    {
        m_started = true;

		DWORD status = 0;
		if(bWaitTillStarted)
			status = ::WaitForSingleObject(m_startedEvent, dwMilliseconds);

        return S_OK;
    }
    else
    {
        m_threadId = 0;
        m_threadHandle = NULL;
        if (m_finishedEvent)
            ::CloseHandle(m_finishedEvent);

		if (m_startedEvent)
			::CloseHandle(m_startedEvent);

        m_terminated = true;
        return S_FALSE;
    }
}

HRESULT CBaseThread::ThreadStop( bool waitTillFinished )
{
    HRESULT returnValue = S_OK;
    DWORD result = 0;

    if ((m_finishedEvent)&&(m_threadId))
    {
        // signal anyone waiting or looping that this thread should exit
        m_terminated = true;
        ::SetEvent(m_shouldFinishEvent);

        // make sure that the thread is processing messages to get the termination flag.
        ::PostThreadMessage( m_threadId, WM_QUIT, 0, 0 );
        if ( waitTillFinished )
        {
            result = ::WaitForSingleObject(m_finishedEvent, INFINITE );
            if (result==WAIT_OBJECT_0)
                returnValue = S_OK;
            else
                returnValue = S_FALSE;
            ::CloseHandle(m_finishedEvent);
            ::CloseHandle(m_shouldFinishEvent);
			::CloseHandle(m_startedEvent);
            m_finishedEvent = NULL;
            m_threadHandle = 0;
        }
    }
    else
        returnValue = S_FALSE;

    return returnValue;
}

// --------------------------------------------------
//  ~CWRThread
// --------------------------------------------------
CBaseThread::~CBaseThread()
{
    // we must always wait for the thread to be gone before destroying
    ThreadStop();
}

// --------------------------------------------------
//  OnThreadBegin
// --------------------------------------------------
HRESULT CBaseThread::OnThreadBegin()
{
    return S_OK;
}

// --------------------------------------------------
//  OnThreadEnd
// --------------------------------------------------
HRESULT CBaseThread::OnThreadEnd()
{
    return S_OK;
}

// --------------------------------------------------
//  OnThreadExecute
// --------------------------------------------------
HRESULT CBaseThread::OnThreadExecute()
{
    return S_OK;
}

// --------------------------------------------------
//  ThreadProc
//      Note: This function WILL be running in
//      the WORKER thread, not the creating thread.
// --------------------------------------------------
HRESULT CBaseThread::ThreadProc()
{

    OnThreadExecute();

    // now that we are done, set our event to notify the
    // creating class that we are done.
    if (m_finishedEvent)
        ::SetEvent(m_finishedEvent);

    return S_OK;

}

// --------------------------------------------------
//  _ThreadProc
// --------------------------------------------------
unsigned _stdcall  CBaseThread::_ThreadProc( void *ArgList )
{
    CBaseThread* selfClass = NULL;
    selfClass = reinterpret_cast<CBaseThread*>(ArgList);

    if (selfClass!=NULL)
    {
        HANDLE finishedEvent = NULL;
        selfClass->ThreadProc(  );
    }

    _endthreadex( 0 );

	return 0;
}
