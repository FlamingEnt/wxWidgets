/////////////////////////////////////////////////////////////////////////////
// Name:        src/os2/thread.cpp
// Purpose:     wxThread Implementation
// Author:      Original from Wolfram Gloger/Guilhem Lavaux/David Webster
// Modified by: Stefan Neis
// Created:     04/22/98
// RCS-ID:      $Id: THREAD.CPP,v 1.16.2.6 2003/09/07 12:48:05 SN Exp $
// Copyright:   (c) Stefan Neis (2003)
//                         
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
    #pragma implementation "thread.h"
#endif

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_THREADS

#include <stdio.h>

#include "wx/app.h"
#include "wx/module.h"
#include "wx/intl.h"
#include "wx/utils.h"
#include "wx/log.h"
#include "wx/thread.h"

#define INCL_DOSSEMAPHORES
#define INCL_DOSPROCESS
#define INCL_DOSMISC
#define INCL_ERRORS
#include <os2.h>
#ifndef __EMX__
#include <bseerr.h>
#endif
// the possible states of the thread ("=>" shows all possible transitions from
// this state)
enum wxThreadState
{
    STATE_NEW,          // didn't start execution yet (=> RUNNING)
    STATE_RUNNING,      // thread is running (=> PAUSED, CANCELED)
    STATE_PAUSED,       // thread is temporarily suspended (=> RUNNING)
    STATE_CANCELED,     // thread should terminate a.s.a.p. (=> EXITED)
    STATE_EXITED        // thread is terminating
};

// ----------------------------------------------------------------------------
// this module's globals
// ----------------------------------------------------------------------------

// id of the main thread - the one which can call GUI functions without first
// calling wxMutexGuiEnter()
static ULONG                        s_ulIdMainThread = 1;
wxMutex*                            p_wxMainMutex;

// OS2 substitute for Tls pointer the current parent thread object
wxThread*                           m_pThread;    // pointer to the wxWindows thread object

// if it's FALSE, some secondary thread is holding the GUI lock
static bool gs_bGuiOwnedByMainThread = TRUE;

// critical section which controls access to all GUI functions: any secondary
// thread (i.e. except the main one) must enter this crit section before doing
// any GUI calls
static wxCriticalSection *gs_pCritsectGui = NULL;

// critical section which protects s_nWaitingForGui variable
static wxCriticalSection *gs_pCritsectWaitingForGui = NULL;

// number of threads waiting for GUI in wxMutexGuiEnter()
static size_t gs_nWaitingForGui = 0;

// are we waiting for a thread termination?
static bool gs_bWaitingForThread = FALSE;

// ============================================================================
// OS/2 implementation of thread and related classes
// ============================================================================

// ----------------------------------------------------------------------------
// wxMutex implementation
// ----------------------------------------------------------------------------
class wxMutexInternal
{
public:
    wxMutexInternal(wxMutexType mutexType);
    ~wxMutexInternal();

    bool IsOk() const { return m_vMutex != NULL; }

    wxMutexError Lock() { return LockTimeout(SEM_INDEFINITE_WAIT); }
    wxMutexError TryLock() { return LockTimeout(SEM_IMMEDIATE_RETURN); }
    wxMutexError Unlock();

private:
    wxMutexError LockTimeout(ULONG ulMilliseconds);
    HMTX                            m_vMutex;
};

// all mutexes are "pseudo-"recursive under OS2 so we don't use mutexType
// (Calls to DosRequestMutexSem and DosReleaseMutexSem can be nested, but
//  the request count for a semaphore cannot exceed 65535. If an attempt is
//  made to exceed this number, ERROR_TOO_MANY_SEM_REQUESTS is returned.)
wxMutexInternal::wxMutexInternal(
  wxMutexType                       WXUNUSED(eMutexType)
)
{
    APIRET                          ulrc;

    ulrc = ::DosCreateMutexSem(NULL, &m_vMutex, 0L, FALSE);
    if (ulrc != 0)
    {
        wxLogSysError(_("Can not create mutex."));
        m_vMutex = NULL;
    }
}

wxMutexInternal::~wxMutexInternal()
{
    if (m_vMutex)
    {
        if (::DosCloseMutexSem(m_vMutex))
            wxLogLastError(_T("DosCloseMutexSem(mutex)"));
    }
}

wxMutexError wxMutexInternal::LockTimeout(ULONG ulMilliseconds)
{
    APIRET                          ulrc;

    ulrc = ::DosRequestMutexSem(m_vMutex, ulMilliseconds);

    switch (ulrc)
    {
        case ERROR_TIMEOUT:
        case ERROR_TOO_MANY_SEM_REQUESTS:
            return wxMUTEX_BUSY;

        case NO_ERROR:
            // ok
            break;

        case ERROR_INVALID_HANDLE:
        case ERROR_INTERRUPT:
        case ERROR_SEM_OWNER_DIED:
            wxLogSysError(_("Couldn't acquire a mutex lock"));
            return wxMUTEX_MISC_ERROR;

        default:
            wxFAIL_MSG(wxT("impossible return value in wxMutex::Lock"));
            return wxMUTEX_MISC_ERROR;
     }
    return wxMUTEX_NO_ERROR;
}

wxMutexError wxMutexInternal::Unlock()
{
    APIRET                          ulrc;

    ulrc = ::DosReleaseMutexSem(m_vMutex);
    if (ulrc != 0)
    {
        wxLogSysError(_("Couldn't release a mutex"));
        return wxMUTEX_MISC_ERROR;
    }
    return wxMUTEX_NO_ERROR;
}

// --------------------------------------------------------------------------
// wxSemaphore
// --------------------------------------------------------------------------

// a trivial wrapper around OS2 event semaphore
class wxSemaphoreInternal
{
public:
    wxSemaphoreInternal(int initialcount, int maxcount);
    ~wxSemaphoreInternal();

    bool IsOk() const { return m_vEvent != NULL; }

    wxSemaError Wait() { return WaitTimeout(SEM_INDEFINITE_WAIT); }
    wxSemaError TryWait() { return WaitTimeout(SEM_IMMEDIATE_RETURN); }
    wxSemaError WaitTimeout(unsigned long milliseconds);

    wxSemaError Post();

private:
    HEV m_vEvent;
    HMTX m_vMutex;
    int m_count;
    int m_maxcount;
};

wxSemaphoreInternal::wxSemaphoreInternal(int initialcount, int maxcount)
{
    APIRET ulrc;
    if ( maxcount == 0 )
    {
        // make it practically infinite
        maxcount = INT_MAX;
    }

    m_count = initialcount;
    m_maxcount = maxcount;
    ulrc = ::DosCreateMutexSem(NULL, &m_vMutex, 0L, FALSE);
    if (ulrc != 0)
    {
        wxLogLastError(_T("DosCreateMutexSem()"));
        m_vMutex = NULL;
        m_vEvent = NULL;
        return;
    }
    ulrc = ::DosCreateEventSem(NULL, &m_vEvent, 0L, FALSE);
    if ( ulrc != 0)
    {
        wxLogLastError(_T("DosCreateEventSem()"));
        ::DosCloseMutexSem(m_vMutex);
        m_vMutex = NULL;
        m_vEvent = NULL;
    }
    if (initialcount)
        ::DosPostEventSem(m_vEvent);
}

wxSemaphoreInternal::~wxSemaphoreInternal()
{
    if ( m_vEvent )
    {
        if ( ::DosCloseEventSem(m_vEvent) )
        {
            wxLogLastError(_T("DosCloseEventSem(semaphore)"));
        }
        if ( ::DosCloseMutexSem(m_vMutex) )
        {
            wxLogLastError(_T("DosCloseMutexSem(semaphore)"));
        }
        else
            m_vEvent = NULL;
    }
}

wxSemaError wxSemaphoreInternal::WaitTimeout(unsigned long ulMilliseconds)
{
    APIRET ulrc;
    do {
        ulrc = ::DosWaitEventSem(m_vEvent, ulMilliseconds );
        switch ( ulrc )
        {
            case NO_ERROR:
                break;

            case ERROR_TIMEOUT:
                if (ulMilliseconds == SEM_IMMEDIATE_RETURN)
                    return wxSEMA_BUSY;
                else
                    return wxSEMA_TIMEOUT;

            default:
                wxLogLastError(_T("DosWaitEventSem(semaphore)"));
                return wxSEMA_MISC_ERROR;
        }
        ulrc = :: DosRequestMutexSem(m_vMutex, ulMilliseconds);
        switch ( ulrc )
        {
            case NO_ERROR:
                // ok
                break;

            case ERROR_TIMEOUT:
            case ERROR_TOO_MANY_SEM_REQUESTS:
                if (ulMilliseconds == SEM_IMMEDIATE_RETURN)
                    return wxSEMA_BUSY;
                else
                    return wxSEMA_TIMEOUT;

            default:
                wxFAIL_MSG(wxT("DosRequestMutexSem(mutex)"));
                return wxSEMA_MISC_ERROR;
        }
        bool OK = false;
        if (m_count > 0)
        {
            m_count--;
            OK = true;
        }
        else
        {
            ULONG ulPostCount;
            ::DosResetEventSem(m_vEvent, &ulPostCount);
        }
        ::DosReleaseMutexSem(m_vMutex);
        if (OK)
            return wxSEMA_NO_ERROR;
    } while (ulMilliseconds == SEM_INDEFINITE_WAIT);

    if (ulMilliseconds == SEM_IMMEDIATE_RETURN)
        return wxSEMA_BUSY;
    return wxSEMA_TIMEOUT;
}

wxSemaError wxSemaphoreInternal::Post()
{
    APIRET ulrc;
    ulrc = ::DosRequestMutexSem(m_vMutex, SEM_INDEFINITE_WAIT);
    if (ulrc != NO_ERROR)
        return wxSEMA_MISC_ERROR;
    bool OK = false;
    if (m_count < m_maxcount)
    {
        m_count++;
        ulrc = ::DosPostEventSem(m_vEvent);
        OK = true;
    }
    ::DosReleaseMutexSem(m_vMutex);
    if (!OK)
        return wxSEMA_OVERFLOW;
    if ( ulrc != NO_ERROR && ulrc != ERROR_ALREADY_POSTED )
    {
        wxLogLastError(_T("DosPostEventSem(semaphore)"));

        return wxSEMA_MISC_ERROR;
    }

    return wxSEMA_NO_ERROR;
}

// ----------------------------------------------------------------------------
// wxThread implementation
// ----------------------------------------------------------------------------

// wxThreadInternal class
// ----------------------

class wxThreadInternal
{
public:
    inline wxThreadInternal()
    {
        m_hThread = 0;
        m_eState = STATE_NEW;
        m_nPriority = WXTHREAD_DEFAULT_PRIORITY;
    }

    ~wxThreadInternal()
    {
        m_hThread = 0;
    }

    // create a new (suspended) thread (for the given thread object)
    bool Create( wxThread*    pThread
                ,unsigned int uStackSize
               );

    // suspend/resume/terminate
    bool Suspend();
    bool Resume();
    inline void Cancel() { m_eState = STATE_CANCELED; }

    // thread state
    inline void SetState(wxThreadState eState) { m_eState = eState; }
    inline wxThreadState GetState() const { return m_eState; }

    // thread priority
    void                SetPriority(unsigned int nPriority);
    inline unsigned int GetPriority() const { return m_nPriority; }

    // thread handle and id
    inline TID GetHandle() const { return m_hThread; }
    TID  GetId() const { return m_hThread; }

    // thread function
    static void OS2ThreadStart(void* pParam);

private:
    // Threads in OS/2 have only an ID, so m_hThread is both it's handle and ID
    // PM also has no real Tls mechanism to index pointers by so we'll just
    // keep track of the wxWindows parent object here.
    TID                             m_hThread;    // handle and ID of the thread
    wxThreadState                   m_eState;     // state, see wxThreadState enum
    unsigned int                    m_nPriority;  // thread priority in "wx" units
};

void wxThreadInternal::OS2ThreadStart(
  void * pParam
)
{
    DWORD                           dwRet;
    bool bWasCancelled;

    wxThread *pThread = (wxThread *)pParam;

    // first of all, wait for the thread to be started.
    pThread->m_critsect.Enter();
    pThread->m_critsect.Leave();
    // Now check whether we hadn't been cancelled already and don't
    // start the user code at all in this case.
    if ( pThread->m_internal->GetState() == STATE_EXITED )
    {
        dwRet = (DWORD)-1;
        bWasCancelled = TRUE;
    }
    else // do run thread
    {
        HAB     vHab;
        vHab = ::WinInitialize(0);
        dwRet = (DWORD)pThread->Entry();
        ::WinTerminate(vHab);

	// enter m_critsect before changing the thread state
	pThread->m_critsect.Enter();

	bWasCancelled = pThread->m_internal->GetState() == STATE_CANCELED;

	pThread->m_internal->SetState(STATE_EXITED);
	pThread->m_critsect.Leave();
    }
    pThread->OnExit();

    // if the thread was cancelled (from Delete()), then it the handle is still
    // needed there
    if (pThread->IsDetached() && !bWasCancelled)
    {
        // auto delete
        delete pThread;
    }
    //else: the joinable threads handle will be closed when Wait() is done
    return;
}

void wxThreadInternal::SetPriority(
  unsigned int                      nPriority
)
{
    // translate wxWindows priority to the PM one
    ULONG                           ulOS2_PriorityClass;
    ULONG                           ulOS2_SubPriority;
    ULONG                           ulrc;

    m_nPriority = nPriority;
    if (m_nPriority <= 25)
        ulOS2_PriorityClass = PRTYC_IDLETIME;
    else if (m_nPriority <= 50)
        ulOS2_PriorityClass = PRTYC_REGULAR;
    else if (m_nPriority <= 75)
        ulOS2_PriorityClass = PRTYC_TIMECRITICAL;
    else if (m_nPriority <= 100)
        ulOS2_PriorityClass = PRTYC_FOREGROUNDSERVER;
    else
    {
        wxFAIL_MSG(wxT("invalid value of thread priority parameter"));
        ulOS2_PriorityClass = PRTYC_REGULAR;
    }
    ulOS2_SubPriority = (ULONG) (((m_nPriority - 1) % 25 + 1) * 31.0 / 25);
    ulrc = ::DosSetPriority( PRTYS_THREAD
                            ,ulOS2_PriorityClass
                            ,ulOS2_SubPriority
                            ,(ULONG)m_hThread
                           );
    if (ulrc != 0)
    {
        wxLogSysError(_("Can't set thread priority"));
    }
}

bool wxThreadInternal::Create(
  wxThread*                         pThread
, unsigned int                      uStackSize
)
{
    int                          tid;
    
    if (!uStackSize)
      uStackSize = 131072;
    pThread->m_critsect.Enter();
    tid = _beginthread(wxThreadInternal::OS2ThreadStart,
			     NULL, uStackSize, pThread);
    if(tid == -1)
    {
        wxLogSysError(_("Can't create thread"));

        return FALSE;
    }
    m_hThread = tid;
    if (m_nPriority != WXTHREAD_DEFAULT_PRIORITY)
    {
        SetPriority(m_nPriority);
    }

    return(TRUE);
}

bool wxThreadInternal::Suspend()
{
    ULONG                           ulrc = ::DosSuspendThread(m_hThread);

    if (ulrc != 0)
    {
        wxLogSysError(_("Can not suspend thread %lu"), m_hThread);
        return FALSE;
    }
    m_eState = STATE_PAUSED;
    return TRUE;
}

bool wxThreadInternal::Resume()
{
    ULONG                           ulrc = ::DosResumeThread(m_hThread);

    if (ulrc != 0)
    {
        wxLogSysError(_("Can not resume thread %lu"), m_hThread);
        return FALSE;
    }

    // don't change the state from STATE_EXITED because it's special and means
    // we are going to terminate without running any user code - if we did it,
    // the codei n Delete() wouldn't work
    if ( m_eState != STATE_EXITED )
    {
        m_eState = STATE_RUNNING;
    }

    return TRUE;
}

// static functions
// ----------------

wxThread *wxThread::This()
{
    wxThread*                       pThread = m_pThread;
    return pThread;
}

bool wxThread::IsMain()
{
    PTIB                            ptib;
    PPIB                            ppib;

    ::DosGetInfoBlocks(&ptib, &ppib);

    if (ptib->tib_ptib2->tib2_ultid == s_ulIdMainThread)
        return TRUE;
    return FALSE;
}

#ifdef Yield
    #undef Yield
#endif

void wxThread::Yield()
{
    ::DosSleep(0);
}

void wxThread::Sleep(
  unsigned long                     ulMilliseconds
)
{
    ::DosSleep(ulMilliseconds);
}

int wxThread::GetCPUCount()
{
    ULONG CPUCount;
    APIRET ulrc;
    ulrc = ::DosQuerySysInfo(26, 26, (void *)&CPUCount, sizeof(ULONG));
    // QSV_NUMPROCESSORS(26) is typically not defined in header files

    if (ulrc != 0)
        CPUCount = 1;

    return CPUCount;
}

unsigned long wxThread::GetCurrentId()
{
    PTIB                            ptib;
    PPIB                            ppib;

    ::DosGetInfoBlocks(&ptib, &ppib);
    return (unsigned long) ptib->tib_ptib2->tib2_ultid;
}

bool wxThread::SetConcurrency(size_t level)
{
    wxASSERT_MSG( IsMain(), _T("should only be called from the main thread") );

    // ok only for the default one
    if ( level == 0 )
        return 0;

    // Don't know how to realize this on OS/2.
    return level == 1;
}

// ctor and dtor
// -------------

wxThread::wxThread(wxThreadKind kind)
{
    m_internal = new wxThreadInternal();

    m_isDetached = kind == wxTHREAD_DETACHED;
}

wxThread::~wxThread()
{
    delete m_internal;
}

// create/start thread
// -------------------

wxThreadError wxThread::Create(
  unsigned int                      uStackSize
)
{
    wxCriticalSectionLocker lock((wxCriticalSection &)m_critsect);

    if ( !m_internal->Create(this, uStackSize) )
        return wxTHREAD_NO_RESOURCE;

    return wxTHREAD_NO_ERROR;
}

wxThreadError wxThread::Run()
{
    wxCriticalSectionLocker         lock((wxCriticalSection &)m_critsect);

    if ( m_internal->GetState() != STATE_NEW )
    {
        // actually, it may be almost any state at all, not only STATE_RUNNING
        return wxTHREAD_RUNNING;
    }
    return Resume();
}

// suspend/resume thread
// ---------------------

wxThreadError wxThread::Pause()
{
    wxCriticalSectionLocker         lock((wxCriticalSection &)m_critsect);

    return m_internal->Suspend() ? wxTHREAD_NO_ERROR : wxTHREAD_MISC_ERROR;
}

wxThreadError wxThread::Resume()
{
    if (m_internal->GetState() == STATE_NEW)
    {
	m_internal->SetState(STATE_RUNNING);
        m_critsect.Leave();
	return wxTHREAD_NO_ERROR;
    }

    wxCriticalSectionLocker         lock((wxCriticalSection &)m_critsect);

    return m_internal->Resume() ? wxTHREAD_NO_ERROR : wxTHREAD_MISC_ERROR;
}

// stopping thread
// ---------------

wxThread::ExitCode wxThread::Wait()
{
    // although under Windows we can wait for any thread, it's an error to
    // wait for a detached one in wxWin API
    wxCHECK_MSG( !IsDetached(), (ExitCode)-1,
                 _T("can't wait for detached thread") );
    ExitCode rc = (ExitCode)-1;
    (void)Delete(&rc);
    return(rc);
}

wxThreadError wxThread::Delete(ExitCode *pRc)
{
    ExitCode rc = 0;

    // Delete() is always safe to call, so consider all possible states

    // we might need to resume the thread, but we might also not need to cancel
    // it if it doesn't run yet
    bool shouldResume = FALSE,
         shouldCancel = TRUE,
         isRunning = FALSE;

    // check if the thread already started to run
    {
        wxCriticalSectionLocker         lock((wxCriticalSection &)m_critsect);

        if ( m_internal->GetState() == STATE_NEW )
        {
            // WinThreadStart() will see it and terminate immediately, no need
            // to cancel the thread - but we still need to resume it to let it
            // run
            m_internal->SetState(STATE_EXITED);

            Resume();   // it knows about STATE_EXITED special case

            shouldCancel = FALSE;
            isRunning = TRUE;

            // shouldResume is correctly set to FALSE here
        }
        else
        {
            shouldResume = IsPaused();
        }
    }

    // resume the thread if it is paused
    if ( shouldResume )
        Resume();

    TID                             hThread = m_internal->GetHandle();

    if ( isRunning || IsRunning())
    {
        if (IsMain())
        {
            // set flag for wxIsWaitingForThread()
            gs_bWaitingForThread = TRUE;
	}

        // ask the thread to terminate
        if ( shouldCancel )
        {
            wxCriticalSectionLocker lock(m_critsect);

            m_internal->Cancel();
        }

#if wxUSE_GUI
        // we can't just wait for the thread to terminate because it might be
        // calling some GUI functions and so it will never terminate before we
        // process the Windows messages that result from these functions
        DWORD result = 0;       // suppress warnings from broken compilers
        do
        {
            if ( IsMain() )
            {
                // give the thread we're waiting for chance to do the GUI call
                // it might be in
                if ( (gs_nWaitingForGui > 0) && wxGuiOwnedByMainThread() )
                {
                    wxMutexGuiLeave();
                }
            }

            result = ::DosWaitThread(&hThread, DCWW_NOWAIT);
	    // FIXME: We ought to have a message processing loop here!!

            switch ( result )
	    {
  	        case ERROR_INTERRUPT:
		case ERROR_THREAD_NOT_TERMINATED:
		    break;
		case ERROR_INVALID_THREADID:
                case NO_ERROR:
                    // thread we're waiting for just terminated
  		    // or even does not exist any more.
		    result = NO_ERROR;
		    break;
                default:
                    wxFAIL_MSG(wxT("unexpected result of DosWaitThread"));
            }
	    if ( IsMain() )
	    {
		// event processing - needed if we are the main thread
		// to give other threads a chance to do remaining GUI
		// processing and terminate cleanly.
		wxTheApp->HandleSockets();
		if (wxTheApp->Pending())
		  if ( !wxTheApp->DoMessage() )
		  {
		      // WM_QUIT received: kill the thread
		      Kill();

		      return wxTHREAD_KILLED;
		  }
		  else
		    wxUsleep(10);
	    }
	    else
	        wxUsleep(10);
        } while ( result != NO_ERROR );
#else // !wxUSE_GUI
        // simply wait for the thread to terminate
        //
        // OTOH, even console apps create windows (in wxExecute, for WinSock
        // &c), so may be use MsgWaitForMultipleObject() too here?
        if ( ::DosWaitThread(&hThread, DCWW_WAIT) != NO_ERROR )
        {
            wxFAIL_MSG(wxT("unexpected result of DosWaitThread"));
        }
#endif // wxUSE_GUI/!wxUSE_GUI

        if ( IsMain() )
        {
            gs_bWaitingForThread = FALSE;
        }
    }

#if 0
    // although the thread might be already in the EXITED state it might not
    // have terminated yet and so we are not sure that it has actually
    // terminated if the "if" above hadn't been taken
    do
    {
        if ( !::GetExitCodeThread(hThread, (LPDWORD)&rc) )
        {
            wxLogLastError(wxT("GetExitCodeThread"));

            rc = (ExitCode)-1;
        }
    } while ( (DWORD)rc == STILL_ACTIVE );
#endif

    if ( IsDetached() )
    {
        // if the thread exits normally, this is done in WinThreadStart, but in
        // this case it would have been too early because
        // MsgWaitForMultipleObject() would fail if the thread handle was
        // closed while we were waiting on it, so we must do it here
        delete this;
    }

    if ( pRc )
        *pRc = rc;

    return rc == (ExitCode)-1 ? wxTHREAD_MISC_ERROR : wxTHREAD_NO_ERROR;
}

wxThreadError wxThread::Kill()
{
    if (!IsRunning())
        return wxTHREAD_NOT_RUNNING;

    ::DosKillThread(m_internal->GetHandle());
    if (IsDetached())
    {
        delete this;
    }
    return wxTHREAD_NO_ERROR;
}

void wxThread::Exit(
  ExitCode                          pStatus
)
{
    delete this;
    _endthread();
    wxFAIL_MSG(wxT("Couldn't return from DosExit()!"));
}

void wxThread::SetPriority(
  unsigned int                      nPrio
)
{
    wxCriticalSectionLocker         lock((wxCriticalSection &)m_critsect);

    m_internal->SetPriority(nPrio);
}

unsigned int wxThread::GetPriority() const
{
    wxCriticalSectionLocker         lock((wxCriticalSection &)m_critsect);

    return m_internal->GetPriority();
}

unsigned long wxThread::GetId() const
{
    wxCriticalSectionLocker lock((wxCriticalSection &)m_critsect); // const_cast

    return (unsigned long)m_internal->GetId();
}

bool wxThread::IsRunning() const
{
    wxCriticalSectionLocker         lock((wxCriticalSection &)m_critsect);

    return(m_internal->GetState() == STATE_RUNNING);
}

bool wxThread::IsAlive() const
{
    wxCriticalSectionLocker         lock((wxCriticalSection &)m_critsect);

    return (m_internal->GetState() == STATE_RUNNING) ||
           (m_internal->GetState() == STATE_PAUSED);
}

bool wxThread::IsPaused() const
{
    wxCriticalSectionLocker         lock((wxCriticalSection &)m_critsect);

    return (m_internal->GetState() == STATE_PAUSED);
}

bool wxThread::TestDestroy()
{
    wxCriticalSectionLocker         lock((wxCriticalSection &)m_critsect);

    return m_internal->GetState() == STATE_CANCELED;
}

// ----------------------------------------------------------------------------
// Automatic initialization for thread module
// ----------------------------------------------------------------------------

class wxThreadModule : public wxModule
{
public:
    virtual bool OnInit();
    virtual void OnExit();

private:
    DECLARE_DYNAMIC_CLASS(wxThreadModule)
};

IMPLEMENT_DYNAMIC_CLASS(wxThreadModule, wxModule)

bool wxThreadModule::OnInit()
{
    gs_pCritsectWaitingForGui = new wxCriticalSection();

    gs_pCritsectGui = new wxCriticalSection();
    gs_pCritsectGui->Enter();

    PTIB                            ptib;
    PPIB                            ppib;

    ::DosGetInfoBlocks(&ptib, &ppib);

    s_ulIdMainThread = ptib->tib_ptib2->tib2_ultid;
    return TRUE;
}

void wxThreadModule::OnExit()
{
    if (gs_pCritsectGui)
    {
        gs_pCritsectGui->Leave();
#if (!(defined(__VISAGECPP__) && (__IBMCPP__ < 400 || __IBMC__ < 400 )))
        delete gs_pCritsectGui;
#endif
        gs_pCritsectGui = NULL;
    }

#if (!(defined(__VISAGECPP__) && (__IBMCPP__ < 400 || __IBMC__ < 400 )))
    wxDELETE(gs_pCritsectWaitingForGui);
#endif
}

// ----------------------------------------------------------------------------
// Helper functions
// ----------------------------------------------------------------------------

// wake up the main thread if it's in ::GetMessage()
void WXDLLEXPORT wxWakeUpMainThread()
{
#if 0
    if ( !::WinPostQueueMsg(wxTheApp->m_hMq, WM_NULL, 0, 0) )
    {
        // should never happen
        wxLogLastError(wxT("WinPostMessage(WM_NULL)"));
    }
#endif
}

void WXDLLEXPORT wxMutexGuiEnter()
{
    // this would dead lock everything...
    wxASSERT_MSG( !wxThread::IsMain(),
                  wxT("main thread doesn't want to block in wxMutexGuiEnter()!") );

    // the order in which we enter the critical sections here is crucial!!

    // set the flag telling to the main thread that we want to do some GUI
    {
        wxCriticalSectionLocker enter(*gs_pCritsectWaitingForGui);

        gs_nWaitingForGui++;
    }

    wxWakeUpMainThread();

    // now we may block here because the main thread will soon let us in
    // (during the next iteration of OnIdle())
    gs_pCritsectGui->Enter();
}

void WXDLLEXPORT wxMutexGuiLeave()
{
    wxCriticalSectionLocker enter(*gs_pCritsectWaitingForGui);

    if ( wxThread::IsMain() )
    {
        gs_bGuiOwnedByMainThread = FALSE;
    }
    else
    {
        // decrement the number of waiters now
        wxASSERT_MSG(gs_nWaitingForGui > 0,
                      wxT("calling wxMutexGuiLeave() without entering it first?") );

        gs_nWaitingForGui--;

        wxWakeUpMainThread();
    }

    gs_pCritsectGui->Leave();
}

void WXDLLEXPORT wxMutexGuiLeaveOrEnter()
{
    wxASSERT_MSG( wxThread::IsMain(),
                  wxT("only main thread may call wxMutexGuiLeaveOrEnter()!") );

    wxCriticalSectionLocker enter(*gs_pCritsectWaitingForGui);

    if (gs_nWaitingForGui == 0)
    {
        // no threads are waiting for GUI - so we may acquire the lock without
        // any danger (but only if we don't already have it)
        if (!wxGuiOwnedByMainThread())
        {
            gs_pCritsectGui->Enter();

            gs_bGuiOwnedByMainThread = TRUE;
        }
        //else: already have it, nothing to do
    }
    else
    {
        // some threads are waiting, release the GUI lock if we have it
        if (wxGuiOwnedByMainThread())
        {
            wxMutexGuiLeave();
        }
        //else: some other worker thread is doing GUI
    }
}

bool WXDLLEXPORT wxGuiOwnedByMainThread()
{
    return gs_bGuiOwnedByMainThread;
}

bool WXDLLEXPORT wxIsWaitingForThread()
{
    return gs_bWaitingForThread;
}

// ----------------------------------------------------------------------------
// include common implementation code
// ----------------------------------------------------------------------------

#include "wx/thrimpl.cpp"

#endif
  // wxUSE_THREADS
