/////////////////////////////////////////////////////////////////////////////
// Name:        radiobut.h
// Purpose:     wxRadioButton class
// Author:      David Webster
// Modified by:
// Created:     10/12/99
// RCS-ID:      $Id: RADIOBUT.H,v 1.10.2.1 2002/12/27 14:49:42 JS Exp $
// Copyright:   (c) David Webster
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_RADIOBUT_H_
#define _WX_RADIOBUT_H_

#include "wx/control.h"

class WXDLLEXPORT wxRadioButton: public wxControl
{
public:
    inline wxRadioButton() { Init(); }
    inline wxRadioButton( wxWindow*          pParent
                         ,wxWindowID         vId
                         ,const wxString&    rsLabel
                         ,const wxPoint&     rPos = wxDefaultPosition
                         ,const wxSize&      rSize = wxDefaultSize
                         ,long               lStyle = 0
                         ,const wxValidator& rValidator = wxDefaultValidator
                         ,const wxString&    rsName = wxRadioButtonNameStr
                         )
    {
        Init();

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

    bool Create( wxWindow* pParent
                ,wxWindowID         vId
                ,const wxString&    rsLabel
                ,const wxPoint&     rPos = wxDefaultPosition
                ,const wxSize&      rSize = wxDefaultSize
                ,long               lStyle = 0
                ,const wxValidator& rValidator = wxDefaultValidator
                ,const wxString&    rsName = wxRadioButtonNameStr
               );

    virtual void SetLabel(const wxString& rsLabel);
    virtual void SetValue(bool bVal);
    virtual bool GetValue(void) const ;

    bool            OS2Command( WXUINT wParam
                               ,WXWORD wId
                              );
    void            Command(wxCommandEvent& rEvent);
    virtual MRESULT OS2WindowProc( WXUINT   uMsg
                                  ,WXWPARAM wParam
                                  ,WXLPARAM lParam
                                 );
    virtual void    SetFocus(void);

protected:
    virtual wxSize DoGetBestSize() const;

private:
    void Init(void);

    bool                            m_bFocusJustSet;

    DECLARE_DYNAMIC_CLASS(wxRadioButton)
}; // end of wxRadioButton

#endif
    // _WX_RADIOBUT_H_
