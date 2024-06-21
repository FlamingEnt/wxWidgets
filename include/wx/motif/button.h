/////////////////////////////////////////////////////////////////////////////
// Name:        button.h
// Purpose:     wxButton class
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id: button.h,v 1.7 2002/02/05 16:34:30 JS Exp $
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_BUTTON_H_
#define _WX_BUTTON_H_

#ifdef __GNUG__
#pragma interface "button.h"
#endif

#include "wx/control.h"
#include "wx/gdicmn.h"

WXDLLEXPORT_DATA(extern const char*) wxButtonNameStr;

// Pushbutton
class WXDLLEXPORT wxButton: public wxControl
{
    DECLARE_DYNAMIC_CLASS(wxButton)
        
public:
    wxButton() { }
    wxButton(wxWindow *parent,
        wxWindowID id,
        const wxString& label,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize, long style = 0,
        const wxValidator& validator = wxDefaultValidator,
        const wxString& name = wxButtonNameStr)
    {
        Create(parent, id, label, pos, size, style, validator, name);
    }
    
    bool Create(wxWindow *parent, wxWindowID id, const wxString& label,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize, long style = 0,
        const wxValidator& validator = wxDefaultValidator,
        const wxString& name = wxButtonNameStr);
    
    virtual void SetDefault();
    virtual void Command(wxCommandEvent& event);
    
    static wxSize GetDefaultSize();
    
    // Implementation
    virtual void ChangeFont(bool keepOriginalSize = TRUE);
    virtual void ChangeBackgroundColour();
    virtual void ChangeForegroundColour();
};

#endif
// _WX_BUTTON_H_
