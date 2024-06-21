/////////////////////////////////////////////////////////////////////////////
// Name:        dcscreen.h
// Purpose:
// Author:      Vaclav Slavik
// Id:          $Id: dcscreen.h,v 1.6 2002/02/14 20:30:19 VS Exp $
// Copyright:   (c) 2001-2002 SciTech Software, Inc. (www.scitechsoft.com)
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __WX_DCSCREEN_H__
#define __WX_DCSCREEN_H__

#ifdef __GNUG__
#pragma interface "dcscreen.h"
#endif

#include "wx/dcclient.h"

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class WXDLLEXPORT wxScreenDC;

//-----------------------------------------------------------------------------
// wxScreenDC
//-----------------------------------------------------------------------------

class WXDLLEXPORT wxScreenDC: public wxDC
{
public:
    wxScreenDC();
    ~wxScreenDC();

    static bool StartDrawingOnTop(wxWindow *WXUNUSED(window)) { return TRUE; }
    static bool StartDrawingOnTop(wxRect *WXUNUSED(rect) = NULL) { return TRUE; }
    static bool EndDrawingOnTop() { return TRUE; }
  
private:
    DECLARE_DYNAMIC_CLASS(wxScreenDC)
};

#endif

    // __WX_DCSCREEN_H__

