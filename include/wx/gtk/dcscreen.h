/////////////////////////////////////////////////////////////////////////////
// Name:        dcscreen.h
// Purpose:
// Author:      Robert Roebling
// Id:          $Id: dcscreen.h,v 1.8 2000/10/30 16:43:37 vadz Exp $
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __GTKDCSCREENH__
#define __GTKDCSCREENH__

#include "wx/dcclient.h"

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class wxScreenDC;

//-----------------------------------------------------------------------------
// wxScreenDC
//-----------------------------------------------------------------------------

class wxScreenDC : public wxPaintDC
{
public:
    wxScreenDC();
    virtual ~wxScreenDC();

    static bool StartDrawingOnTop( wxWindow *window );
    static bool StartDrawingOnTop( wxRect *rect = (wxRect *) NULL );
    static bool EndDrawingOnTop();

    // implementation

    static GdkWindow  *sm_overlayWindow;
    static int         sm_overlayWindowX;
    static int         sm_overlayWindowY;

protected:
    virtual void DoGetSize(int *width, int *height) const;

private:
    DECLARE_DYNAMIC_CLASS(wxScreenDC)
};

#endif

    // __GTKDCSCREENH__

