/////////////////////////////////////////////////////////////////////////////
// Name:        dcscreen.cpp
// Purpose:     wxScreenDC class
// Author:      David Webster
// Modified by:
// Created:     10/14/99
// RCS-ID:      $Id: DCSCREEN.CPP,v 1.7 2002/03/15 23:19:49 DW Exp $
// Copyright:   (c) David Webster
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "dcscreen.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#define INCL_DEV
#define INCL_GPI
#define INCL_PM
#include<os2.h>

#ifndef WX_PRECOMP
   #include "wx/string.h"
   #include "wx/window.h"
#endif

#include "wx/os2/private.h"

#include "wx/dcscreen.h"

IMPLEMENT_DYNAMIC_CLASS(wxScreenDC, wxWindowDC)

// Create a DC representing the whole screen
wxScreenDC::wxScreenDC()
{
    m_hDC = ::WinOpenWindowDC(HWND_DESKTOP);
    m_hPS = ::WinGetScreenPS(HWND_DESKTOP);
    ::GpiSetBackMix(m_hPS, BM_LEAVEALONE);
} // end of wxScreenDC::wxScreenDC()

void wxScreenDC::DoGetSize(
  int*                              pnWidth
, int*                              pnHeight
) const
{
    //
    // Skip wxWindowDC version because it doesn't work without a valid m_canvas
    // (which we don't have)
    //
    wxDC::DoGetSize( pnWidth
                    ,pnHeight
                   );
} // end of wxScreenDC::DoGetSize

