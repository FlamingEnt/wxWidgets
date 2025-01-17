/////////////////////////////////////////////////////////////////////////////
// Name:        spinctrl.h
// Purpose:     wxSpinCtrl class
// Author:      Robert Roebling
// Modified by:
// RCS-ID:      $Id: spinctrl.h,v 1.8 2002/09/07 12:28:46 GD Exp $
// Copyright:   (c) Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __GTKSPINCTRLH__
#define __GTKSPINCTRLH__

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface
#endif

#include "wx/defs.h"

#if wxUSE_SPINCTRL

#include "wx/control.h"

//-----------------------------------------------------------------------------
// wxSpinCtrl
//-----------------------------------------------------------------------------

class wxSpinCtrl : public wxControl
{
public:
    wxSpinCtrl() {}
    wxSpinCtrl(wxWindow *parent,
               wxWindowID id = -1,
               const wxString& value = wxEmptyString,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = wxSP_ARROW_KEYS,
               int min = 0, int max = 100, int initial = 0,
               const wxString& name = _T("wxSpinCtrl"))
    {
        Create(parent, id, value, pos, size, style, min, max, initial, name);
    }

    bool Create(wxWindow *parent,
                wxWindowID id = -1,
                const wxString& value = wxEmptyString,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxSP_ARROW_KEYS,
                int min = 0, int max = 100, int initial = 0,
                const wxString& name = _T("wxSpinCtrl"));

    void SetValue(const wxString& text);

    virtual int GetValue() const;
    virtual void SetValue( int value );
    virtual void SetRange( int minVal, int maxVal );
    virtual int GetMin() const;
    virtual int GetMax() const;

    // implementation
    void OnChar( wxKeyEvent &event );
    
    bool IsOwnGtkWindow( GdkWindow *window );
    void ApplyWidgetStyle();
    void GtkDisableEvents();
    void GtkEnableEvents();

    GtkAdjustment  *m_adjust;
    float           m_oldPos;

protected:
    virtual wxSize DoGetBestSize() const;

private:
    DECLARE_DYNAMIC_CLASS(wxSpinCtrl)
    DECLARE_EVENT_TABLE()
};

#endif
    // wxUSE_SPINCTRL

#endif
    // __GTKSPINCTRLH__
