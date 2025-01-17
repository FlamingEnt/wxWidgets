/////////////////////////////////////////////////////////////////////////////
// Name:        privx.h
// Purpose:     Private declarations common to X11 and Motif ports
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id: privx.h,v 1.8.2.1 2002/11/09 15:20:33 RR Exp $
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PRIVX_H_
#define _WX_PRIVX_H_

#include "wx/defs.h"
#include "wx/utils.h"
#include "wx/colour.h"

#include "X11/Xlib.h"
#include "X11/Xatom.h"
#include "X11/Xutil.h"

class wxMouseEvent;
class wxKeyEvent;
class wxWindow;

// ----------------------------------------------------------------------------
// key events related functions
// ----------------------------------------------------------------------------

extern int wxCharCodeXToWX(KeySym keySym);
extern KeySym wxCharCodeWXToX(int id);

int wxGetBestMatchingPixel(Display *display, XColor *desiredColor, Colormap cmap);
Pixmap XCreateInsensitivePixmap( Display *display, Pixmap pixmap );

extern XColor g_itemColors[];
extern int wxComputeColours (Display *display, wxColour * back, wxColour * fore);

// For convenience
inline Display* wxGlobalDisplay() { return (Display*) wxGetDisplay(); }

#define wxMAX_RGB           0xff
#define wxMAX_SV            1000
#define wxSIGN(x)           ((x < 0) ? -x : x)
#define wxH_WEIGHT          4
#define wxS_WEIGHT          1
#define wxV_WEIGHT          2

typedef struct wx_hsv {
                        int h,s,v;
                      } wxHSV;

#define wxMax3(x,y,z) ((x > y) ? ((x > z) ? x : z) : ((y > z) ? y : z))
#define wxMin3(x,y,z) ((x < y) ? ((x < z) ? x : z) : ((y < z) ? y : z))

void wxHSVToXColor(wxHSV *hsv,XColor *xcolor);
void wxXColorToHSV(wxHSV *hsv,XColor *xcolor);
void wxAllocNearestColor(Display *display,Colormap colormap,XColor *xcolor);
void wxAllocColor(Display *display,Colormap colormap,XColor *xcolor);

// For debugging
#ifdef __WXDEBUG__
wxString wxGetXEventName(XEvent& event);
#endif

// Is the window visible?
bool wxWindowIsVisible(Window win);

#if wxUSE_NANOX
#define XEventGetWindow(event) event->general.wid
#define XEventGetType(event) event->general.type
#define XConfigureEventGetX(event) ((int) event->update.x)
#define XConfigureEventGetY(event) ((int) event->update.y)
#define XConfigureEventGetWidth(event) ((int) event->update.width)
#define XConfigureEventGetHeight(event) ((int) event->update.height)
#define XExposeEventGetX(event) event->exposure.x
#define XExposeEventGetY(event) event->exposure.y
#define XExposeEventGetWidth(event) event->exposure.width
#define XExposeEventGetHeight(event) event->exposure.height
#define XButtonEventGetTime(event) (wxGetLocalTime())
#define XButtonEventLChanged(event) (event->button.changebuttons & GR_BUTTON_L)
#define XButtonEventMChanged(event) (event->button.changebuttons & GR_BUTTON_M)
#define XButtonEventRChanged(event) (event->button.changebuttons & GR_BUTTON_R)
#define XButtonEventLIsDown(x) ((x)->button.buttons & GR_BUTTON_L)
#define XButtonEventMIsDown(x) ((x)->button.buttons & GR_BUTTON_M)
#define XButtonEventRIsDown(x) ((x)->button.buttons & GR_BUTTON_R)
#define XButtonEventShiftIsDown(x) (x->button.modifiers & MWKMOD_SHIFT)
#define XButtonEventCtrlIsDown(x) (x->button.modifiers & MWKMOD_CTRL)
#define XButtonEventAltIsDown(x) (x->button.modifiers & MWKMOD_ALT)
#define XButtonEventMetaIsDown(x) (x->button.modifiers & MWKMOD_META)
#define XButtonEventGetX(event) (event->button.x)
#define XButtonEventGetY(event) (event->button.y)
#define XKeyEventGetTime(event) (wxGetLocalTime())
#define XKeyEventGetX(event) (event->keystroke.x)
#define XKeyEventGetY(event) (event->keystroke.y)
#define XKeyEventShiftIsDown(x) (x->keystroke.modifiers & MWKMOD_SHIFT)
#define XKeyEventCtrlIsDown(x) (x->keystroke.modifiers & MWKMOD_CTRL)
#define XKeyEventAltIsDown(x) (x->keystroke.modifiers & MWKMOD_ALT)
#define XKeyEventMetaIsDown(x) (x->keystroke.modifiers & MWKMOD_META)
#define XFontStructGetAscent(f) f->info.baseline

#else

#define XEventGetWindow(event) event->xany.window
#define XEventGetType(event) event->xany.type
#define XConfigureEventGetX(event) event->xconfigure.x
#define XConfigureEventGetY(event) event->xconfigure.y
#define XConfigureEventGetWidth(event) event->xconfigure.width
#define XConfigureEventGetHeight(event) event->xconfigure.height
#define XExposeEventGetX(event) event->xexpose.x
#define XExposeEventGetY(event) event->xexpose.y
#define XExposeEventGetWidth(event) event->xexpose.width
#define XExposeEventGetHeight(event) event->xexpose.height
#define XButtonEventGetTime(event) (event->xbutton.time)
#define XButtonEventLChanged(event) (event->xbutton.button == Button1)
#define XButtonEventMChanged(event) (event->xbutton.button == Button2)
#define XButtonEventRChanged(event) (event->xbutton.button == Button3)
#define XButtonEventLIsDown(x) ((x)->xbutton.state & Button1Mask)
#define XButtonEventMIsDown(x) ((x)->xbutton.state & Button2Mask)
#define XButtonEventRIsDown(x) ((x)->xbutton.state & Button3Mask)
#define XButtonEventShiftIsDown(x) (x->xbutton.state & ShiftMask)
#define XButtonEventCtrlIsDown(x) (x->xbutton.state & ControlMask)
#define XButtonEventAltIsDown(x) (x->xbutton.state & Mod3Mask)
#define XButtonEventMetaIsDown(x) (x->xbutton.state & Mod1Mask)
#define XButtonEventGetX(event) (event->xbutton.x)
#define XButtonEventGetY(event) (event->xbutton.y)
#define XKeyEventGetTime(event) (event->xkey.time)
#define XKeyEventShiftIsDown(x) (x->xkey.state & ShiftMask)
#define XKeyEventCtrlIsDown(x) (x->xkey.state & ControlMask)
#define XKeyEventAltIsDown(x) (x->xkey.state & Mod3Mask)
#define XKeyEventMetaIsDown(x) (x->xkey.state & Mod1Mask)
#define XKeyEventGetX(event) (event->xkey.x)
#define XKeyEventGetY(event) (event->xkey.y)
#define XFontStructGetAscent(f) f->ascent
#endif

#endif
// _WX_PRIVX_H_
