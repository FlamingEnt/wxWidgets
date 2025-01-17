/////////////////////////////////////////////////////////////////////////////
// Name:        dcscreen.cpp
// Purpose:     wxScreenDC class
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id: dcscreen.cpp,v 1.12 2000/03/01 11:05:47 JS Exp $
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "dcscreen.h"
#endif

#include "wx/window.h"
#include "wx/frame.h"
#include "wx/dcscreen.h"
#include "wx/utils.h"

#ifdef __VMS__
#pragma message disable nosimpint
#endif
#include <Xm/Xm.h>
#ifdef __VMS__
#pragma message enable nosimpint
#endif

#include "wx/motif/private.h"

IMPLEMENT_DYNAMIC_CLASS(wxScreenDC, wxWindowDC)

WXWindow wxScreenDC::sm_overlayWindow = 0;
int wxScreenDC::sm_overlayWindowX = 0;
int wxScreenDC::sm_overlayWindowY = 0;

// Create a DC representing the whole screen
wxScreenDC::wxScreenDC()
{
    m_display = wxGetDisplay();
    Display* display = (Display*) m_display;
    
    if (sm_overlayWindow)
    {
        m_pixmap = sm_overlayWindow;
        m_deviceOriginX = - sm_overlayWindowX;
        m_deviceOriginY = - sm_overlayWindowY;
    }
    else
        m_pixmap = (WXPixmap) RootWindow(display, DefaultScreen(display));
    
    XGCValues gcvalues;
    gcvalues.foreground = BlackPixel (display, DefaultScreen (display));
    gcvalues.background = WhitePixel (display, DefaultScreen (display));
    gcvalues.graphics_exposures = False;
    gcvalues.subwindow_mode = IncludeInferiors;
    gcvalues.line_width = 1;
    m_gc = XCreateGC (display, RootWindow (display, DefaultScreen (display)),
        GCForeground | GCBackground | GCGraphicsExposures | GCLineWidth | GCSubwindowMode,
        &gcvalues);
    
    m_backgroundPixel = (int) gcvalues.background;
    m_ok = TRUE;
}

wxScreenDC::~wxScreenDC()
{
    EndDrawingOnTop();
}

bool wxScreenDC::StartDrawingOnTop(wxWindow* window)
{
    wxRect rect;
    int x, y, width, height;
    window->GetPosition(& x, & y);
    if (window->GetParent() && !window->IsKindOf(CLASSINFO(wxFrame)))
        window->GetParent()->ClientToScreen(& x, & y);
    window->GetSize(& width, & height);
    rect.x = x; rect.y = y;
    rect.width = width; rect.height = height;
    
    return StartDrawingOnTop(& rect);
}

bool wxScreenDC::StartDrawingOnTop(wxRect* rect)
{
    if (sm_overlayWindow)
        return FALSE;
    
    Display *dpy = (Display*) wxGetDisplay();
    Pixmap screenPixmap = RootWindow(dpy, DefaultScreen(dpy));
    
    int x = 0;
    int y = 0;
    int width, height;
    wxDisplaySize(&width, &height);
    
    if (rect)
    {
        x = rect->x; y = rect->y;
        width = rect->width; height = rect->height;
    }
    sm_overlayWindowX = x;
    sm_overlayWindowY = y;
    
    XSetWindowAttributes attributes;
    attributes.override_redirect = True;
    unsigned long valueMask = CWOverrideRedirect;
    
    sm_overlayWindow = (WXWindow) XCreateWindow(dpy, screenPixmap, x, y, width, height, 0,
        wxDisplayDepth(), InputOutput,
        DefaultVisual(dpy, 0), valueMask,
        & attributes);
    
    if (sm_overlayWindow)
    {
        XMapWindow(dpy, (Window) sm_overlayWindow);
        return TRUE;
    }
    else
        return FALSE;
}

bool wxScreenDC::EndDrawingOnTop()
{
    if (sm_overlayWindow)
    {
        XDestroyWindow((Display*) wxGetDisplay(), (Window) sm_overlayWindow);
        sm_overlayWindow = 0;
        return TRUE;
    }
    else
        return FALSE;
}
