/////////////////////////////////////////////////////////////////////////
// File:        taskbar.cpp
// Purpose:	    Implements wxTaskBarIcon class for manipulating icons on
//              the task bar. Optional.
// Author:      David Webster
// Modified by:
// Created:     10/17/99
// RCS-ID:      $Id: TASKBAR.CPP,v 1.5 2001/12/14 17:09:03 DW Exp $
// Copyright:   (c) David Webster
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/defs.h"
#include "wx/window.h"
#include "wx/frame.h"
#include "wx/utils.h"
#include "wx/menu.h"
#endif

#define INCL_PM
#include <os2.h>
#include <string.h>
#include <wx/os2/taskbar.h>
#include <wx/os2/private.h>

MRESULT wxTaskBarIconWindowProc( HWND hWnd, UINT msg, MPARAM wParam, MPARAM lParam );

wxChar *wxTaskBarWindowClass = wxT("wxTaskBarWindowClass");

wxList wxTaskBarIcon::sm_taskBarIcons;
bool   wxTaskBarIcon::sm_registeredClass = FALSE;
UINT   wxTaskBarIcon::sm_taskbarMsg = 0;


BEGIN_EVENT_TABLE(wxTaskBarIcon, wxEvtHandler)
    EVT_TASKBAR_MOVE         (wxTaskBarIcon::_OnMouseMove)
    EVT_TASKBAR_LEFT_DOWN    (wxTaskBarIcon::_OnLButtonDown)
    EVT_TASKBAR_LEFT_UP      (wxTaskBarIcon::_OnLButtonUp)
    EVT_TASKBAR_RIGHT_DOWN   (wxTaskBarIcon::_OnRButtonDown)
    EVT_TASKBAR_RIGHT_UP     (wxTaskBarIcon::_OnRButtonUp)
    EVT_TASKBAR_LEFT_DCLICK  (wxTaskBarIcon::_OnLButtonDClick)
    EVT_TASKBAR_RIGHT_DCLICK (wxTaskBarIcon::_OnRButtonDClick)
END_EVENT_TABLE()

IMPLEMENT_DYNAMIC_CLASS(wxTaskBarIcon, wxEvtHandler)


wxTaskBarIcon::wxTaskBarIcon(void)
{
    m_hWnd = 0;
    m_iconAdded = FALSE;

    AddObject(this);
// TODO:
/*
    if (RegisterWindowClass())
        m_hWnd = CreateTaskBarWindow();
*/
}

wxTaskBarIcon::~wxTaskBarIcon(void)
{
// TODO:
/*
    RemoveObject(this);

    if (m_iconAdded)
    {
        RemoveIcon();
    }

    if (m_hWnd)
    {
        ::DestroyWindow((HWND) m_hWnd);
        m_hWnd = 0;
    }
*/
}

// Operations
bool wxTaskBarIcon::SetIcon(const wxIcon& icon, const wxString& tooltip)
{
    if (!IsOK())
        return FALSE;
// TODO:
/*
    NOTIFYICONDATA notifyData;

    memset(&notifyData, 0, sizeof(notifyData));
    notifyData.cbSize = sizeof(notifyData);
    notifyData.hWnd = (HWND) m_hWnd;
    notifyData.uCallbackMessage = sm_taskbarMsg;
    notifyData.uFlags = NIF_MESSAGE ;
    if (icon.Ok())
    {
        notifyData.uFlags |= NIF_ICON;
        notifyData.hIcon = (HICON) icon.GetHICON();
    }

    if (((const wxChar*) tooltip != NULL) && (tooltip != wxT("")))
    {
        notifyData.uFlags |= NIF_TIP ;
        lstrcpyn(notifyData.szTip, WXSTRINGCAST tooltip, sizeof(notifyData.szTip));
    }

    notifyData.uID = 99;

    if (m_iconAdded)
        return (Shell_NotifyIcon(NIM_MODIFY, & notifyData) != 0);
    else
    {
        m_iconAdded = (Shell_NotifyIcon(NIM_ADD, & notifyData) != 0);
        return m_iconAdded;
    }
*/
    return FALSE;
}

bool wxTaskBarIcon::RemoveIcon(void)
{
    if (!m_iconAdded)
        return FALSE;
//TODO:
/*
    NOTIFYICONDATA notifyData;

    memset(&notifyData, 0, sizeof(notifyData));
    notifyData.cbSize = sizeof(notifyData);
    notifyData.hWnd = (HWND) m_hWnd;
    notifyData.uCallbackMessage = sm_taskbarMsg;
    notifyData.uFlags = NIF_MESSAGE;
    notifyData.hIcon = 0 ; // hIcon;
    notifyData.uID = 99;
    m_iconAdded = FALSE;

    return (Shell_NotifyIcon(NIM_DELETE, & notifyData) != 0);
*/
    return FALSE;
}

bool wxTaskBarIcon::PopupMenu(wxMenu *menu) //, int x, int y);
{
    // OK, so I know this isn't thread-friendly, but
    // what to do? We need this check.

    static bool s_inPopup = FALSE;

    if (s_inPopup)
        return FALSE;

    s_inPopup = TRUE;

    bool        rval = FALSE;
    wxWindow*   win;
    int         x, y;
    wxGetMousePosition(&x, &y);

    // is wxFrame the best window type to use???
    win = new wxFrame(NULL, -1, "", wxPoint(x,y), wxSize(-1,-1), 0);
    win->PushEventHandler(this);

    // Remove from record of top-level windows, or will confuse wxWindows
    // if we try to exit right now.
    wxTopLevelWindows.DeleteObject(win);

    menu->UpdateUI();

    rval = win->PopupMenu(menu, 0, 0);

    win->PopEventHandler(FALSE);
    win->Destroy();
    delete win;

    s_inPopup = FALSE;

    return rval;
}

// Overridables
void wxTaskBarIcon::OnMouseMove(wxEvent&)
{
}

void wxTaskBarIcon::OnLButtonDown(wxEvent&)
{
}

void wxTaskBarIcon::OnLButtonUp(wxEvent&)
{
}

void wxTaskBarIcon::OnRButtonDown(wxEvent&)
{
}

void wxTaskBarIcon::OnRButtonUp(wxEvent&)
{
}

void wxTaskBarIcon::OnLButtonDClick(wxEvent&)
{
}

void wxTaskBarIcon::OnRButtonDClick(wxEvent&)
{
}

void wxTaskBarIcon::_OnMouseMove(wxEvent& e)      { OnMouseMove(e);     }
void wxTaskBarIcon::_OnLButtonDown(wxEvent& e)    { OnLButtonDown(e);   }
void wxTaskBarIcon::_OnLButtonUp(wxEvent& e)      { OnLButtonUp(e);     }
void wxTaskBarIcon::_OnRButtonDown(wxEvent& e)    { OnRButtonDown(e);   }
void wxTaskBarIcon::_OnRButtonUp(wxEvent& e)      { OnRButtonUp(e);     }
void wxTaskBarIcon::_OnLButtonDClick(wxEvent& e)  { OnLButtonDClick(e); }
void wxTaskBarIcon::_OnRButtonDClick(wxEvent& e)  { OnRButtonDClick(e); }


wxTaskBarIcon* wxTaskBarIcon::FindObjectForHWND(WXHWND hWnd)
{
    wxNode*node = sm_taskBarIcons.First();
    while (node)
    {
        wxTaskBarIcon* obj = (wxTaskBarIcon*) node->Data();
        if (obj->GetHWND() == hWnd)
            return obj;
        node = node->Next();
    }
    return NULL;
}

void wxTaskBarIcon::AddObject(wxTaskBarIcon* obj)
{
    sm_taskBarIcons.Append(obj);
}

void wxTaskBarIcon::RemoveObject(wxTaskBarIcon* obj)
{
    sm_taskBarIcons.DeleteObject(obj);
}

bool wxTaskBarIcon::RegisterWindowClass()
{
    if (sm_registeredClass)
        return TRUE;

    // Also register the taskbar message here
// TODO:
/*
    sm_taskbarMsg = ::RegisterWindowMessage(wxT("wxTaskBarIconMessage"));

    WNDCLASS  wc;
    bool      rc;

    HINSTANCE hInstance = GetModuleHandle(NULL);

    //
    // set up and register window class
    //
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = (WNDPROC) wxTaskBarIconWindowProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = 0;
    wc.hCursor = 0;
    wc.hbrBackground = 0;
    wc.lpszMenuName = NULL;
    wc.lpszClassName = wxTaskBarWindowClass ;
    rc = (::RegisterClass( &wc ) != 0);

    sm_registeredClass = (rc != 0);

    return( (rc != 0) );
*/
    return FALSE;
}

WXHWND wxTaskBarIcon::CreateTaskBarWindow()
{
// TODO:
/*
    HINSTANCE hInstance = GetModuleHandle(NULL);

    HWND hWnd = CreateWindowEx (0, wxTaskBarWindowClass,
            wxT("wxTaskBarWindow"),
            WS_OVERLAPPED,
            0,
            0,
            10,
            10,
            NULL,
            (HMENU) 0,
            hInstance,
            NULL);

    return (WXHWND) hWnd;
*/
    return (WXHWND)0;
}

MRESULT wxTaskBarIcon::WindowProc( WXHWND hWnd, UINT msg, MPARAM wParam, MPARAM lParam )
{
    wxEventType eventType = 0;
// TODO:
/*
    if (msg != sm_taskbarMsg)
        return DefWindowProc((HWND) hWnd, msg, wParam, lParam);

    switch (lParam)
    {
        case WM_LBUTTONDOWN:
            eventType = wxEVT_TASKBAR_LEFT_DOWN;
            break;

        case WM_LBUTTONUP:
            eventType = wxEVT_TASKBAR_LEFT_UP;
            break;

        case WM_RBUTTONDOWN:
            eventType = wxEVT_TASKBAR_RIGHT_DOWN;
            break;

        case WM_RBUTTONUP:
            eventType = wxEVT_TASKBAR_RIGHT_UP;
            break;

        case WM_LBUTTONDBLCLK:
            eventType = wxEVT_TASKBAR_LEFT_DCLICK;
            break;

        case WM_RBUTTONDBLCLK:
            eventType = wxEVT_TASKBAR_RIGHT_DCLICK;
            break;

        case WM_MOUSEMOVE:
            eventType = wxEVT_TASKBAR_MOVE;
            break;

        default:
            break;
    }
    if (eventType)
    {
        wxEvent event;
        event.SetEventType(eventType);
        event.SetEventObject(this);

        ProcessEvent(event);
    }
*/
    return 0;
}

MRESULT wxTaskBarIconWindowProc(
  HWND                              hWnd
, UINT                              msg
, MPARAM                            wParam
, MPARAM                            lParam
)
{
    wxTaskBarIcon* obj = wxTaskBarIcon::FindObjectForHWND((WXHWND) hWnd);
    if (obj)
        return obj->WindowProc((WXHWND) hWnd, msg, wParam, lParam);
    else
        return (MRESULT)0;
//        return DefWindowProc(hWnd, msg, wParam, lParam);
}

