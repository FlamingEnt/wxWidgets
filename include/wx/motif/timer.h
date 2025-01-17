/////////////////////////////////////////////////////////////////////////////
// Name:        timer.h
// Purpose:     wxTimer class
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id: timer.h,v 1.6 2002/02/05 16:34:31 JS Exp $
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_TIMER_H_
#define _WX_TIMER_H_

#ifdef __GNUG__
#pragma interface "timer.h"
#endif

class WXDLLEXPORT wxTimer : public wxTimerBase
{
    friend void wxTimerCallback(wxTimer * timer);
    
public:
    wxTimer() { Init(); }
    wxTimer(wxEvtHandler *owner, int id = -1) : wxTimerBase(owner, id)
    { Init(); }
    ~wxTimer();
    
    virtual bool Start(int milliseconds = -1, bool oneShot = FALSE);
    virtual void Stop();
    
    virtual bool IsRunning() const { return m_id != 0; }
    
protected:
    void Init();
    
    long m_id;
    
private:
    DECLARE_DYNAMIC_CLASS(wxTimer)
};

#endif
// _WX_TIMER_H_
