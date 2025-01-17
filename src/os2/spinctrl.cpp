/////////////////////////////////////////////////////////////////////////////
// Name:        msw/spinctrl.cpp
// Purpose:     wxSpinCtrl class implementation for Win32
// Author:      David Webster
// Modified by:
// Created:     10/15/99
// RCS-ID:      $Id: SPINCTRL.CPP,v 1.14.2.2 2003/09/20 22:58:39 SN Exp $
// Copyright:   (c) David Webster
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================


#ifdef __GNUG__
    #pragma implementation "spinctrlbase.h"
    #pragma implementation "spinctrl.h"
#endif

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// for compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#if wxUSE_SPINBTN

#include "wx/spinctrl.h"
#include "wx/os2/private.h"

// ----------------------------------------------------------------------------
// macros
// ----------------------------------------------------------------------------

extern void  wxAssociateWinWithHandle( HWND         hWnd
                                      ,wxWindowOS2* pWin
                                     );
static WXFARPROC fnWndProcSpinCtrl = (WXFARPROC)NULL;
wxArraySpins                        wxSpinCtrl::m_svAllSpins;

IMPLEMENT_DYNAMIC_CLASS(wxSpinCtrl, wxControl)

BEGIN_EVENT_TABLE(wxSpinCtrl, wxSpinButton)
    EVT_CHAR(wxSpinCtrl::OnChar)
    EVT_SET_FOCUS(wxSpinCtrl::OnSetFocus)
    EVT_SPIN(-1, wxSpinCtrl::OnSpinChange)
END_EVENT_TABLE()
// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// the margin between the up-down control and its buddy
static const int MARGIN_BETWEEN = 5;

// ============================================================================
// implementation
// ============================================================================
MRESULT EXPENTRY wxSpinCtrlWndProc(
  HWND                              hWnd
, UINT                              uMessage
, MPARAM                            wParam
, MPARAM                            lParam
)
{
    wxSpinCtrl*                    pSpin = (wxSpinCtrl *)::WinQueryWindowULong( hWnd
                                                                               ,QWL_USER
                                                                              );
    bool                            bProccesed = FALSE;
    MRESULT                         rc = (MRESULT)0;
    //
    // Forward some messages (the key ones only so far) to the spin ctrl
    //
    switch (uMessage )
    {
        case WM_CHAR:
            pSpin->OS2WindowProc( uMessage
                                 ,wParam
                                 ,lParam
                                );

            //
            // The control may have been deleted at this point, so check.
            //
            if (!(::WinIsWindow(vHabmain, hWnd) && ((wxSpinCtrl *)::WinQueryWindowULong( hWnd
                                                                                        ,QWL_USER
                                                                                       )
                                                   ) == pSpin))
                return 0;
            break;

    }
    return (fnWndProcSpinCtrl( hWnd
                              ,(ULONG)uMessage
                              ,(MPARAM)wParam
                              ,(MPARAM)lParam
                             )
           );
} // end of wxSpinCtrlWndProc

wxSpinCtrl::~wxSpinCtrl()
{
    m_svAllSpins.Remove(this);

    // This removes spurious memory leak reporting
    if (m_svAllSpins.GetCount() == 0)
        m_svAllSpins.Clear();
} // end of wxSpinCtrl::~wxSpinCtrl

// ----------------------------------------------------------------------------
// construction
// ----------------------------------------------------------------------------

bool wxSpinCtrl::Create(
  wxWindow*                         pParent
, wxWindowID                        vId
, const wxString&                   rsValue
, const wxPoint&                    rPos
, const wxSize&                     rSize
, long                              lStyle
, int                               nMin
, int                               nMax
, int                               nInitial
, const wxString&                   rsName
)
{
    SWP                             vSwp;

    if (vId == -1)
        m_windowId = NewControlId();
    else
        m_windowId = vId;
    m_backgroundColour = pParent->GetBackgroundColour();
    m_foregroundColour = pParent->GetForegroundColour();
    SetName(rsName);
    SetParent(pParent);
    m_windowStyle      = lStyle;

    int                             lSstyle = 0L;

    lSstyle = WS_TABSTOP      |
              SPBS_MASTER     | // We use only single field spin buttons
              SPBS_NUMERICONLY; // We default to numeric data

    if (m_windowStyle & wxCLIP_SIBLINGS )
        lSstyle |= WS_CLIPSIBLINGS;

    SPBCDATA                        vCtrlData;

    vCtrlData.cbSize = sizeof(SPBCDATA);
    vCtrlData.ulTextLimit = 10L;
    vCtrlData.lLowerLimit = 0L;
    vCtrlData.lUpperLimit = 100L;
    vCtrlData.idMasterSpb = vId;
    vCtrlData.pHWXCtlData = NULL;

    m_hWnd = (WXHWND)::WinCreateWindow( GetWinHwnd(pParent)
                                       ,WC_SPINBUTTON
                                       ,(PSZ)NULL
                                       ,lSstyle
                                       ,0L, 0L, 0L, 0L
                                       ,GetWinHwnd(pParent)
                                       ,HWND_TOP
                                       ,(HMENU)vId
                                       ,(PVOID)&vCtrlData
                                       ,NULL
                                      );
    if (m_hWnd == 0)
    {
        return FALSE;
    }
    m_hWndBuddy = m_hWnd; // One in the same for OS/2
    if(pParent)
        pParent->AddChild((wxSpinButton *)this);
    wxFont*                          pTextFont = new wxFont( 10
                                                            ,wxMODERN
                                                            ,wxNORMAL
                                                            ,wxNORMAL
                                                           );
    SetFont(*pTextFont);
    ::WinQueryWindowPos(m_hWnd, &vSwp);
    SetXComp(vSwp.x);
    SetYComp(vSwp.y);
    SetSize( rPos.x
            ,rPos.y
            ,rSize.x
            ,rSize.y
           );

    SetRange(nMin, nMax);
    SetValue(nInitial);

    //
    // For OS/2 we'll just set our handle into our long data
    //
    wxAssociateWinWithHandle( m_hWnd
                             ,(wxWindowOS2*)this
                            );
    ::WinSetWindowULong(GetHwnd(), QWL_USER, (LONG)this);
    fnWndProcSpinCtrl = (WXFARPROC)::WinSubclassWindow(m_hWnd, (PFNWP)wxSpinCtrlWndProc);
    m_svAllSpins.Add(this);
    delete pTextFont;
    return TRUE;
} // end of wxSpinCtrl::Create

wxSize wxSpinCtrl::DoGetBestSize() const
{
    wxSize                          vSizeBtn = wxSpinButton::DoGetBestSize();
    int                             nHeight;

    vSizeBtn.x += DEFAULT_ITEM_WIDTH + MARGIN_BETWEEN;

    wxGetCharSize( GetHWND()
                  ,NULL
                  ,&nHeight
                  ,(wxFont*)&GetFont()
                 );
    nHeight = EDIT_HEIGHT_FROM_CHAR_HEIGHT(nHeight);

    if (vSizeBtn.y < nHeight)
    {
        //
        // Make the text tall enough
        //
        vSizeBtn.y = nHeight;
    }
    return vSizeBtn;
} // end of wxSpinCtrl::DoGetBestSize

void wxSpinCtrl::DoGetPosition(
  int*                              pnX
, int*                              pnY
) const
{
    WXHWND                          hWnd = GetHWND();

    wxConstCast(this, wxSpinCtrl)->m_hWnd = m_hWndBuddy;
    wxSpinButton::DoGetPosition( pnX
                                ,pnY
                               );
    wxConstCast(this, wxSpinCtrl)->m_hWnd = hWnd;
} // end of wxpinCtrl::DoGetPosition

void wxSpinCtrl::DoGetSize(
  int*                              pnWidth
, int*                              pnHeight
) const
{
    RECTL                           vSpinrect;

    ::WinQueryWindowRect(GetHwnd(), &vSpinrect);

    if (pnWidth)
        *pnWidth = vSpinrect.xRight - vSpinrect.xLeft;
    if (pnHeight)
        *pnHeight = vSpinrect.yTop - vSpinrect.yBottom;
} // end of wxSpinCtrl::DoGetSize

void wxSpinCtrl::DoMoveWindow(
  int                               nX
, int                               nY
, int                               nWidth
, int                               nHeight
)
{
    wxWindowOS2*                    pParent = (wxWindowOS2*)GetParent();

    if (pParent)
    {
        int                         nOS2Height = GetOS2ParentHeight(pParent);

        nY = nOS2Height - (nY + nHeight);
    }
    else
    {
        RECTL                       vRect;

        ::WinQueryWindowRect(HWND_DESKTOP, &vRect);
        nY = vRect.yTop - (nY + nHeight);
    }
    ::WinSetWindowPos( GetHwnd()
                      ,HWND_TOP
                      ,nX
                      ,nY
                      ,nWidth
                      ,nHeight
                      ,SWP_SIZE | SWP_MOVE | SWP_ZORDER | SWP_SHOW
                     );
} // end of wxSpinCtrl::DoMoveWindow

bool wxSpinCtrl::Enable(
  bool                              bEnable
)
{
    if (!wxControl::Enable(bEnable))
    {
        return FALSE;
    }
    ::WinEnableWindow(GetHwnd(), bEnable);
    return TRUE;
} // end of wxSpinCtrl::Enable

wxSpinCtrl* wxSpinCtrl::GetSpinForTextCtrl(
  WXHWND                            hWndBuddy
)
{
    wxSpinCtrl*                     pSpin = (wxSpinCtrl *)::WinQueryWindowULong( (HWND)hWndBuddy
                                                                                ,QWL_USER
                                                                               );
    int                             i = m_svAllSpins.Index(pSpin);

    if (i == wxNOT_FOUND)
        return NULL;

    // sanity check
    wxASSERT_MSG( pSpin->m_hWndBuddy == hWndBuddy,
                  _T("wxSpinCtrl has incorrect buddy HWND!") );

    return pSpin;
} // end of wxSpinCtrl::GetSpinForTextCtrl

int wxSpinCtrl::GetValue() const
{
    long                            lVal = 0L;
    char                            zVal[10];

    ::WinSendMsg( GetHwnd()
                 ,SPBM_QUERYVALUE
                 ,MPFROMP(zVal)
                 ,MPFROM2SHORT( (USHORT)10
                               ,SPBQ_UPDATEIFVALID
                              )
                );
    lVal = atol(zVal);
    return (int)lVal;
} // end of wxSpinCtrl::GetValue

void wxSpinCtrl::OnChar (
  wxKeyEvent&                       rEvent
)
{
    switch (rEvent.KeyCode())
    {
        case WXK_RETURN:
            {
                wxCommandEvent              vEvent( wxEVT_COMMAND_TEXT_ENTER
                                                   ,m_windowId
                                                  );
                wxString                    sVal = wxGetWindowText(m_hWndBuddy);

                InitCommandEvent(vEvent);
                vEvent.SetString((char*)sVal.c_str());
                vEvent.SetInt(GetValue());
                if (GetEventHandler()->ProcessEvent(vEvent))
                    return;
                break;
            }

        case WXK_TAB:
            //
            // Always produce navigation event - even if we process TAB
            // ourselves the fact that we got here means that the user code
            // decided to skip processing of this TAB - probably to let it
            // do its default job.
            //
            {
                wxNavigationKeyEvent        vEventNav;

                vEventNav.SetDirection(!rEvent.ShiftDown());
                vEventNav.SetWindowChange(rEvent.ControlDown());
                vEventNav.SetEventObject(this);
                if (GetParent()->GetEventHandler()->ProcessEvent(vEventNav))
                    return;
            }
            break;
    }

    //
    // No, we didn't process it
    //
    rEvent.Skip();
} // end of wxSpinCtrl::OnChar

void wxSpinCtrl::OnSpinChange(
  wxSpinEvent&                      rEventSpin
)
{
    wxCommandEvent                  vEvent( wxEVT_COMMAND_SPINCTRL_UPDATED
                                           ,GetId()
                                          );

    vEvent.SetEventObject(this);
    vEvent.SetInt(rEventSpin.GetPosition());
    (void)GetEventHandler()->ProcessEvent(vEvent);
    if (rEventSpin.GetSkipped())
    {
        vEvent.Skip();
    }
} // end of wxSpinCtrl::OnSpinChange

void wxSpinCtrl::OnSetFocus (
  wxFocusEvent&                     rEvent
)
{
    //
    // When we get focus, give it to our buddy window as it needs it more than
    // we do
    //
    ::WinSetFocus(HWND_DESKTOP, (HWND)m_hWndBuddy);
    rEvent.Skip();
} // end of wxSpinCtrl::OnSetFocus

bool wxSpinCtrl::ProcessTextCommand(
  WXWORD                            wCmd
, WXWORD                            wId
)
{
    switch (wCmd)
    {
        case SPBN_CHANGE:
        {
            wxCommandEvent          vEvent( wxEVT_COMMAND_TEXT_UPDATED
                                           ,GetId()
                                          );
            vEvent.SetEventObject(this);

            wxString                sVal = wxGetWindowText(m_hWndBuddy);

            vEvent.SetString((char*)sVal.c_str());
            vEvent.SetInt(GetValue());
            return (GetEventHandler()->ProcessEvent(vEvent));
        }

        case SPBN_SETFOCUS:
        case SPBN_KILLFOCUS:
        {
            wxFocusEvent                vEvent( wCmd == EN_KILLFOCUS ? wxEVT_KILL_FOCUS : wxEVT_SET_FOCUS
                                               ,m_windowId
                                              );

            vEvent.SetEventObject(this);
            return(GetEventHandler()->ProcessEvent(vEvent));
        }
        default:
            break;
    }

    //
    // Not processed
    //
    return FALSE;
} // end of wxSpinCtrl::ProcessTextCommand

void wxSpinCtrl::SetFocus()
{
    ::WinSetFocus(HWND_DESKTOP, GetHwnd());
} // end of wxSpinCtrl::SetFocus

bool wxSpinCtrl::SetFont(
  const wxFont&                     rFont
)
{
    if (!wxWindowBase::SetFont(rFont))
    {
        // nothing to do
        return FALSE;
    }

    WXHANDLE                        hFont = GetFont().GetResourceHandle();
    wxOS2SetFont( m_hWnd
                 ,rFont
                );
    return TRUE;
} // end of wxSpinCtrl::SetFont

void wxSpinCtrl::SetValue(
  const wxString&                   rsText
)
{
    long                            lVal;

    lVal = atol(rsText.c_str());
    wxSpinButton::SetValue(lVal);
} // end of wxSpinCtrl::SetValue

bool wxSpinCtrl::Show(
  bool                              bShow
)
{
    if (!wxControl::Show(bShow))
    {
        return FALSE;
    }
    return TRUE;
} // end of wxSpinCtrl::Show

void wxSpinCtrl::SetSelection (
  long                              lFrom
, long                              lTo
)
{
    //
    // If from and to are both -1, it means (in wxWindows) that all text should
    // be selected - translate into Windows convention
    //
    if ((lFrom == -1) && (lTo == -1))
    {
        lFrom = 0;
    }
    ::WinSendMsg(m_hWnd, EM_SETSEL, MPFROM2SHORT((USHORT)lFrom, (USHORT)lTo), (MPARAM)0);
} // end of wxSpinCtrl::SetSelection

#endif //wxUSE_SPINBTN
