/////////////////////////////////////////////////////////////////////////
// File:        taskbar.h
// Purpose:	    Defines wxTaskBarIcon class for manipulating icons on the
//              task bar. Optional.
// Author:      AUTHOR
// Modified by:
// Created:     ??/??/98
// RCS-ID:      $Id: taskbar.h,v 1.5 2002/08/31 11:29:13 GD Exp $
// Copyright:   (c)
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////

#ifndef _WX_TASKBAR_H_
#define _WX_TASKBAR_H_

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "taskbar.h"
#endif

#include "wx/list.h"
#include "wx/icon.h"

class wxTaskBarIcon: public wxObject
{
public:
	wxTaskBarIcon();
	virtual ~wxTaskBarIcon();

// Accessors

// Operations
    bool SetIcon(const wxIcon& icon, const wxString& tooltip = "");
    bool RemoveIcon();

// Overridables
    virtual void OnMouseMove();
    virtual void OnLButtonDown();
    virtual void OnLButtonUp();
    virtual void OnRButtonDown();
    virtual void OnRButtonUp();
    virtual void OnLButtonDClick();
    virtual void OnRButtonDClick();

// Data members
protected:
};

#endif
    // _WX_TASKBAR_H_
