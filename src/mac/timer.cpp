/////////////////////////////////////////////////////////////////////////////
// Name:        timer.cpp
// Purpose:     wxTimer implementation
// Author:      AUTHOR
// Modified by:
// Created:     ??/??/98
// RCS-ID:      $Id: timer.cpp,v 1.12.2.1 2003/06/20 20:48:17 GD Exp $
// Copyright:   (c) AUTHOR
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "timer.h"
#endif

#include "wx/timer.h"

#if !USE_SHARED_LIBRARY
IMPLEMENT_ABSTRACT_CLASS(wxTimer, wxObject)
#endif

#ifdef __WXMAC__
#include "wx/mac/private.h"
#endif
#ifndef __DARWIN__
#include <Timer.h>
#endif

#include "wx/dynarray.h"

typedef struct MacTimerInfo
{
    TMTask m_task;
    wxMacNotifierTableRef m_table ;
    wxTimer* m_timer ;
} ;

static void wxProcessTimer( unsigned long event , void *data ) ;

static pascal void MacTimerProc( TMTask * t )
{
	MacTimerInfo * tm = (MacTimerInfo*)  t ;
	wxMacAddEvent( tm->m_table , wxProcessTimer, 0 , (void*) tm->m_timer , TRUE ) ;
}

wxArrayPtrVoid gTimersInProcess ;

static void wxProcessTimer( unsigned long event , void *data )
{
	if ( !data )
		return ;
		
	wxTimer* timer = (wxTimer*) data ;
	
	if ( timer->IsOneShot() )
		timer->Stop() ;
		
	gTimersInProcess.Add( timer ) ;
	 
    timer->Notify();

    int index = gTimersInProcess.Index( timer ) ;
    
    if ( index != wxNOT_FOUND )
    {
        gTimersInProcess.RemoveAt( index ) ;
        
        if ( !timer->IsOneShot() && timer->m_info->m_task.tmAddr )
        {
    	    PrimeTime( (QElemPtr)  &timer->m_info->m_task , timer->GetInterval() ) ;
        }
    
    }
}

void wxTimer::Init()
{
    m_info = new MacTimerInfo() ;
	m_info->m_task.tmAddr = NULL ;
	m_info->m_task.tmWakeUp = 0 ;
	m_info->m_task.tmReserved = 0 ;
	m_info->m_task.qType = 0 ;
	m_info->m_table = wxMacGetNotifierTable() ;
	m_info->m_timer = this ;
}

bool wxTimer::IsRunning() const 
{
	return ( m_info->m_task.qType & kTMTaskActive ) ;
}

wxTimer::~wxTimer()
{
    Stop();
    if (m_info != NULL) {
        delete m_info ;
        m_info = NULL ;
    }
    int index = gTimersInProcess.Index( this ) ;
    if ( index != wxNOT_FOUND )
        gTimersInProcess.RemoveAt( index ) ;  
}

bool wxTimer::Start(int milliseconds,bool mode)
{
    (void)wxTimerBase::Start(milliseconds, mode);

    wxCHECK_MSG( m_milli > 0, FALSE, wxT("invalid value for timer timeour") );
    wxCHECK_MSG( m_info->m_task.tmAddr == NULL , FALSE, wxT("attempting to restart a timer") );

    m_milli = milliseconds;
#if defined(UNIVERSAL_INTERFACES_VERSION) && (UNIVERSAL_INTERFACES_VERSION >= 0x0340)
    m_info->m_task.tmAddr = NewTimerUPP( MacTimerProc ) ;
#else
    m_info->m_task.tmAddr = NewTimerProc( MacTimerProc ) ;
#endif
    m_info->m_task.tmWakeUp = 0 ;
    m_info->m_task.tmReserved = 0 ;
	m_info->m_task.qType = 0 ;
	m_info->m_timer = this ;
    InsXTime((QElemPtr) &m_info->m_task ) ;
    PrimeTime( (QElemPtr) &m_info->m_task , m_milli ) ;
    return TRUE;
}

void wxTimer::Stop()
{
    m_milli = 0 ;
    if ( m_info->m_task.tmAddr )
    {
    	RmvTime(  (QElemPtr) &m_info->m_task ) ;
    	DisposeTimerUPP(m_info->m_task.tmAddr) ;
    	m_info->m_task.tmAddr = NULL ;
    }
    wxMacRemoveAllNotifiersForData( wxMacGetNotifierTable() , this ) ;
}



