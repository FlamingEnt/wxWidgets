///////////////////////////////////////////////////////////////////////////////
// Name:        mgl/evtloop.cpp
// Purpose:     implements wxEventLoop for MGL
// Author:      Vaclav Slavik
// RCS-ID:      $Id: evtloop.cpp,v 1.9 2002/02/15 01:07:51 VS Exp $
// Copyright:   (c) 2001-2002 SciTech Software, Inc. (www.scitechsoft.com)
// License:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "evtloop.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/window.h"
    #include "wx/app.h"
    #include "wx/thread.h"
#endif //WX_PRECOMP

#include "wx/evtloop.h"
#include "wx/timer.h"
#include "wx/mgl/private.h"

// ----------------------------------------------------------------------------
// wxEventLoopImpl
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxEventLoopImpl
{
public:
    // ctor
    wxEventLoopImpl()
        {
            SetExitCode(0);
            SetKeepLooping(TRUE);
        }

    // process an event
    void Dispatch();

    // generate an idle event, return TRUE if more idle time requested
    bool SendIdleEvent();

    // set/get the exit code
    void SetExitCode(int exitcode) { m_exitcode = exitcode; }
    int GetExitCode() const { return m_exitcode; }
    
    void SetKeepLooping(bool k) { m_keepLooping = k; }
    bool GetKeepLooping() const { return m_keepLooping; }

private:

    // the exit code of the event loop
    int m_exitcode;
    // FALSE if the loop should end
    bool m_keepLooping;
};

// ============================================================================
// wxEventLoopImpl implementation
// ============================================================================

void wxEventLoopImpl::Dispatch()
{
    event_t evt;

    // VS: The code bellow is equal to MGL's EVT_halt implementation, with
    //     two things added: sleeping (busy waiting is stupid, lets make CPU's
    //     life a bit easier) and timers updating

    // EVT_halt(&evt, EVT_EVERYEVT);
    for (;;)
    {
#if wxUSE_TIMER
        wxTimer::NotifyTimers();
        MGL_wmUpdateDC(g_winMng);
#endif
        EVT_pollJoystick();
        if ( EVT_getNext(&evt, EVT_EVERYEVT) ) break;
        PM_sleep(10);
    }
    // end of EVT_halt
    
    MGL_wmProcessEvent(g_winMng, &evt);
}

bool wxEventLoopImpl::SendIdleEvent()
{
    wxIdleEvent event;

    return wxTheApp->ProcessEvent(event) && event.MoreRequested();
}

// ============================================================================
// wxEventLoop implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxEventLoop running and exiting
// ----------------------------------------------------------------------------

wxEventLoop *wxEventLoop::ms_activeLoop = NULL;

wxEventLoop::~wxEventLoop()
{
    wxASSERT_MSG( !m_impl, _T("should have been deleted in Run()") );
}

bool wxEventLoop::IsRunning() const
{
    return m_impl != NULL;
}

int wxEventLoop::Run()
{
    // event loops are not recursive, you need to create another loop!
    wxCHECK_MSG( !IsRunning(), -1, _T("can't reenter a message loop") );

    m_impl = new wxEventLoopImpl;
    
    wxEventLoop *oldLoop = ms_activeLoop;
    ms_activeLoop = this;

    for ( ;; )
    {
#if wxUSE_THREADS
        //wxMutexGuiLeaveOrEnter(); // FIXME_MGL - huh?
#endif // wxUSE_THREADS

        // generate and process idle events for as long as we don't have
        // anything else to do
        while ( !Pending() && m_impl->SendIdleEvent() ) {}

        // a message came or no more idle processing to do, sit in Dispatch()
        // waiting for the next message
        if ( !Dispatch() )
        {
            // app terminated
            break;
        }
    }

    int exitcode = m_impl->GetExitCode();
    delete m_impl;
    m_impl = NULL;

    ms_activeLoop = oldLoop;

    return exitcode;
}

void wxEventLoop::Exit(int rc)
{
    wxCHECK_RET( IsRunning(), _T("can't call Exit() if not running") );

    m_impl->SetExitCode(rc);
    m_impl->SetKeepLooping(FALSE);
    
    // Send a dummy event so that the app won't block in EVT_halt if there
    // are no user-generated events in the queue:
    EVT_post(0, EVT_USEREVT, 0, 0);
}

// ----------------------------------------------------------------------------
// wxEventLoop message processing dispatching
// ----------------------------------------------------------------------------

bool wxEventLoop::Pending() const
{
    // update the display here, so that wxYield refreshes display and 
    // changes take effect immediately, not after emptying events queue:
    MGL_wmUpdateDC(g_winMng);
    
    // is there an event in the queue?
    event_t evt;
    return EVT_peekNext(&evt, EVT_EVERYEVT);
}

bool wxEventLoop::Dispatch()
{
    wxCHECK_MSG( IsRunning(), FALSE, _T("can't call Dispatch() if not running") );

    m_impl->Dispatch();
    return m_impl->GetKeepLooping();
}
