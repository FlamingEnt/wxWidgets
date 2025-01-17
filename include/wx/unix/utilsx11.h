/////////////////////////////////////////////////////////////////////////////
// Name:        wx/unix/utilsx11.h
// Purpose:     Miscellaneous X11 functions
// Author:      Mattia Barbon, Vaclav Slavik
// Modified by:
// Created:     25.03.02
// RCS-ID:      $Id: utilsx11.h,v 1.1.2.1 2003/04/06 17:43:12 JS Exp $
// Copyright:   (c) wxWindows team
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_UNIX_UTILSX11_H_
#define _WX_UNIX_UTILSX11_H_

#include "wx/defs.h"
#include "wx/gdicmn.h"

// NB: Content of this header is for wxWindows' private use! It is not
//     part of public API and may be modified or even disappear in the future!

#if defined(__WXMOTIF__) || defined(__WXGTK__) || defined(__WXX11__)

#if defined(__WXGTK__)
typedef void WXDisplay;
typedef void* WXWindow;
#endif

class wxIconBundle;

void wxSetIconsX11( WXDisplay* display, WXWindow window,
                    const wxIconBundle& ib );


enum wxX11FullScreenMethod
{
    wxX11_FS_AUTODETECT = 0,
    wxX11_FS_WMSPEC,
    wxX11_FS_KDE,
    wxX11_FS_GENERIC
};

wxX11FullScreenMethod wxGetFullScreenMethodX11(WXDisplay* display,
                                               WXWindow rootWindow);

void wxSetFullScreenStateX11(WXDisplay* display, WXWindow rootWindow,
                             WXWindow window, bool show, wxRect *origSize,
                             wxX11FullScreenMethod method);

#endif
    // __WXMOTIF__, __WXGTK__, __WXX11__

#endif
    // _WX_UNIX_UTILSX11_H_
