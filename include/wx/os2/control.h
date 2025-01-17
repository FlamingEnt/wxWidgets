/////////////////////////////////////////////////////////////////////////////
// Name:        control.h
// Purpose:     wxControl class
// Author:      David Webster
// Modified by:
// Created:     09/17/99
// RCS-ID:      $Id: CONTROL.H,v 1.15.2.1 2002/12/27 14:49:42 JS Exp $
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_CONTROL_H_
#define _WX_CONTROL_H_

#include "wx/dynarray.h"

// General item class
class WXDLLEXPORT wxControl : public wxControlBase
{
    DECLARE_ABSTRACT_CLASS(wxControl)

public:
   wxControl();
   wxControl( wxWindow*          pParent
             ,wxWindowID         vId
             ,const wxPoint&     rPos = wxDefaultPosition
             ,const wxSize&      rSize = wxDefaultSize
             ,long               lStyle = 0
             ,const wxValidator& rValidator = wxDefaultValidator
             ,const wxString&    rsName = wxControlNameStr
            )
    {
        Create( pParent
               ,vId
               ,rPos
               ,rSize
               ,lStyle
               ,rValidator
               ,rsName
              );
    }
    virtual ~wxControl();

    bool Create( wxWindow*          pParent
                ,wxWindowID         vId
                ,const wxPoint&     rPos = wxDefaultPosition
                ,const wxSize&      rSize = wxDefaultSize
                ,long               lStyle = 0
                ,const wxValidator& rValidator = wxDefaultValidator
                ,const wxString&    rsName = wxControlNameStr
               );

    //
    // Simulates an event
    //
    virtual void Command(wxCommandEvent& rEvent) { ProcessCommand(rEvent); }

    //
    // Implementation from now on
    // --------------------------
    //

    //
    // Calls the callback and appropriate event handlers
    //
    bool ProcessCommand(wxCommandEvent& rEvent);

    //
    // For ownerdraw items
    //
    virtual bool OS2OnDraw(WXDRAWITEMSTRUCT* WXUNUSED(pItem)) { return FALSE; };
    virtual long OS2OnMeasure(WXMEASUREITEMSTRUCT* WXUNUSED(pItem)) { return 0L; };

    wxArrayLong&     GetSubcontrols() { return m_aSubControls; }
    void             OnEraseBackground(wxEraseEvent& rEvent);
    virtual WXHBRUSH OnCtlColor( WXHDC    hDC
                                ,WXHWND   pWnd
                                ,WXUINT   nCtlColor
                                ,WXUINT   uMessage
                                ,WXWPARAM wParam
                                ,WXLPARAM lParam
                               );

#if WXWIN_COMPATIBILITY
    virtual void SetButtonColour(const wxColour& WXUNUSED(rCol)) { }
    wxColour*    GetButtonColour(void) const { return NULL; }

    inline virtual void SetLabelFont(const wxFont& rFont);
    inline virtual void SetButtonFont(const wxFont& rFont);
    inline wxFont&      GetLabelFont(void) const;
    inline wxFont&      GetButtonFont(void) const;

    //
    // Adds callback
    //
    inline void Callback(const wxFunction function);
    wxFunction  GetCallback(void) { return m_callback; }

protected:
    wxFunction                      m_callback;     // Callback associated with the window
#endif // WXWIN_COMPATIBILITY

public:
    //
    // For controls like radiobuttons which are really composite
    //
    wxArrayLong m_aSubControls;

    virtual wxSize DoGetBestSize(void) const;

    //
    // Create the control of the given PM class
    //
    bool OS2CreateControl( const wxChar*   zClassname
                          ,const wxString& rsLabel
                          ,const wxPoint&  rPos
                          ,const wxSize&   rSize
                          ,long            lStyle
                         );
    //
    // Create the control of the given class with the given style, returns FALSE
    // if creation failed.
    //
    bool OS2CreateControl( const wxChar*   zClassname
                          ,WXDWORD         dwStyle
                          ,const wxPoint&  rPos = wxDefaultPosition
                          ,const wxSize&   rSize = wxDefaultSize
                          ,const wxString& rsLabel = wxEmptyString
                          ,WXDWORD         dwExstyle = (WXDWORD)-1
                         );

    //
    // Default style for the control include WS_TABSTOP if it AcceptsFocus()
    //
    virtual WXDWORD OS2GetStyle( long     lStyle
                                ,WXDWORD* pdwExstyle
                               ) const;

    inline int  GetXComp(void) const {return m_nXComp;}
    inline int  GetYComp(void) const {return m_nYComp;}
    inline void SetXComp(const int nXComp) {m_nXComp = nXComp;}
    inline void SetYComp(const int nYComp) {m_nYComp = nYComp;}

private:
    int                             m_nXComp;
    int                             m_nYComp;
   DECLARE_EVENT_TABLE()
}; // end of wxControl

#if WXWIN_COMPATIBILITY
    inline void wxControl::Callback(const wxFunction f) { m_callback = f; };
    inline wxFont& wxControl::GetLabelFont(void) const { return GetFont(); }
    inline wxFont& wxControl::GetButtonFont(void) const { return GetFont(); }
    inline void wxControl::SetLabelFont(const wxFont& rFont) { SetFont(rFont); }
    inline void wxControl::SetButtonFont(const wxFont& rFont) { SetFont(rFont); }
#endif // WXWIN_COMPATIBILITY

#endif // _WX_CONTROL_H_

