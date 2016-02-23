/***************************************************************************
 * PROPRIETARY/CONFIDENTIAL.  TBD                                          *
 ***************************************************************************
 * File:                                                                   *
 *   WRThread.h                                                            *
 * Date:                                                                   *
 *   1/22/2007                                                             *
 * Author:                                                                 *
 *   smcbride                                                              *
 * Description:                                                            *
 *   This Class is an implementation of a Thread that has                  *
 *   MiniDump support.                                                     *
 *                                                                         *
 ***************************************************************************/

#pragma once

#include <windows.h>

class CBaseThread
{
    
    public:        
        CBaseThread();
        virtual ~CBaseThread();

        virtual HRESULT ThreadStart (bool bWaitTillStarted = false, DWORD dwMilliseconds = INFINITE);
        virtual HRESULT ThreadStop ( bool waitTillFinished = true );
    protected:
        virtual HRESULT OnThreadExecute () throw();
        virtual HRESULT OnThreadBegin () throw();
        virtual HRESULT OnThreadEnd () throw();

        HANDLE m_threadHandle;
        unsigned m_threadId;
        HANDLE m_finishedEvent;
        HANDLE m_shouldFinishEvent;
        HANDLE m_startedEvent;
		bool m_terminated;
        bool m_started;
	
    private:
        virtual HRESULT ThreadProc ();
        static unsigned _stdcall _ThreadProc( void *ArgList );

};

