/////////////////////////////////////////////////////////////////////////////
// Name:        button.h
// Purpose:     wxButton class
// Author:      David Webster
// Modified by:
// Created:     10/13/99
// RCS-ID:      $Id: BUTTON.H,v 1.13.2.1 2002/12/27 14:49:41 JS Exp $
// Copyright:   (c) David Webster
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_BUTTON_H_
#define _WX_BUTTON_H_

#include "wx/control.h"

// Pushbutton
class WXDLLEXPORT wxButton: public wxButtonBase
{
 public:
    inline wxButton() {}
    inline wxButton( wxWindow*          pParent
                    ,wxWindowID         vId
                    ,const wxString&    rsLabel
                    ,const wxPoint&     rPos = wxDefaultPosition
                    ,const wxSize&      rSize = wxDefaultSize
                    ,long               lStyle = 0
                    ,const wxValidator& rValidator = wxDefaultValidator
                    ,const wxString&    rsName = wxButtonNameStr
                   )
    {
        Create( pParent
               ,vId
               ,rsLabel
               ,rPos
               ,rSize
               ,lStyle
               ,rValidator
               ,rsName
              );
    }

    bool Create( wxWindow*          pParent
                ,wxWindowID         vId
                ,const wxString&    rsLabel
                ,const wxPoint&     rPos = wxDefaultPosition
                ,const wxSize&      rSize = wxDefaultSize
                ,long               lStyle = 0
                ,const wxValidator& rValidator = wxDefaultValidator
                ,const wxString&    rsName = wxButtonNameStr
               );

    virtual ~wxButton();

    virtual void     SetDefault(void);
    static wxSize    GetDefaultSize(void);
    virtual void     Command(wxCommandEvent& rEvent);
    virtual bool     OS2Command( WXUINT uParam
                                ,WXWORD vId
                               );
    virtual WXHBRUSH OnCtlColor( WXHDC    hDC
                                ,WXHWND   hWnd
                                ,WXUINT   uCtlColor
                                ,WXUINT   uMessage
                                ,WXWPARAM wParam
                                ,WXLPARAM lParam
                               );
    void          MakeOwnerDrawn(void);

    virtual MRESULT WindowProc( WXUINT   uMsg
                               ,WXWPARAM wParam
                               ,WXLPARAM lParam
                              );

protected:

    bool            SendClickEvent(void);
    void            SetTmpDefault(void);
    void            UnsetTmpDefault(void);

    static void     SetDefaultStyle( wxButton* pButton
                                    ,bool      bOn
                                   );

    virtual wxSize  DoGetBestSize(void) const;
    virtual WXDWORD OS2GetStyle( long     style
                                ,WXDWORD* exstyle
                               ) const;
private:
  DECLARE_DYNAMIC_CLASS(wxButton)
}; // end of CLASS wxButton

#endif
    // _WX_BUTTON_H_

