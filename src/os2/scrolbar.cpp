/////////////////////////////////////////////////////////////////////////////
// Name:        scrolbar.cpp
// Purpose:     wxScrollBar
// Author:      David Webster
// Modified by:
// Created:     10/15/99
// RCS-ID:      $Id: SCROLBAR.CPP,v 1.7.6.3 2003/04/17 17:24:44 DW Exp $
// Copyright:   (c) David Webster
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/defs.h"
#include "wx/utils.h"
#endif

#include "wx/scrolbar.h"
#include "wx/os2/private.h"

IMPLEMENT_DYNAMIC_CLASS(wxScrollBar, wxControl)

// Scrollbar
bool wxScrollBar::Create (
  wxWindow*                         pParent
, wxWindowID                        vId
, const wxPoint&                    rPos
, const wxSize&                     rSize
, long                              lStyle
, const wxValidator&                rValidator
, const wxString&                   rsName
)
{
    int                             nX = rPos.x;
    int                             nY = rPos.y;
    int                             nWidth = rSize.x;
    int                             nHeight = rSize.y;

    if (!pParent)
        return FALSE;
    pParent->AddChild(this);
    SetName(rsName);
#if wxUSE_VALIDATORS
    SetValidator(rValidator);
#endif
    SetBackgroundColour(pParent->GetBackgroundColour()) ;
    SetForegroundColour(pParent->GetForegroundColour()) ;

    if (vId == -1L)
        m_windowId = (int)NewControlId();
    else
        m_windowId = vId;

    if (nWidth == -1)
    {
        if (lStyle & wxHORIZONTAL)
            nWidth = 140;
        else
            nWidth = 14;
    }
    if (nHeight == -1)
    {
        if (lStyle & wxVERTICAL)
            nHeight = 140;
        else
            nHeight = 14;
    }

    DWORD                           dwStyle = WS_VISIBLE;

    if (GetWindowStyleFlag() & wxCLIP_SIBLINGS)
        dwStyle |= WS_CLIPSIBLINGS;

    DWORD                           dwDirection = (lStyle & wxHORIZONTAL) ? SBS_HORZ: SBS_VERT;

    HWND                            hScrollBar = ::WinCreateWindow( (HWND)GetHwndOf(pParent)
                                                                   ,WC_SCROLLBAR
                                                                   ,(PSZ)NULL
                                                                   ,dwDirection | dwStyle
                                                                   ,0, 0, 0, 0
                                                                   ,(HWND)GetHwndOf(pParent)
                                                                   ,HWND_TOP
                                                                   ,(HMENU)m_windowId
                                                                   ,NULL
                                                                   ,NULL
                                                                  );

    m_nPageSize = 1;
    m_nViewSize = 1;
    m_nObjectSize = 1;
    ::WinSendMsg( hScrollBar
                 ,SBM_SETSCROLLBAR
                 ,(MPARAM)0
                 ,MPFROM2SHORT(0,1)
                );
    ::WinShowWindow( hScrollBar
                    ,TRUE
                   );
    SetFont(*wxSMALL_FONT);

    m_hWnd = hScrollBar;

    //
    // Subclass again for purposes of dialog editing mode
    //
    SubclassWin((WXHWND)hScrollBar);
    SetSize( nX
            ,nY
            ,nWidth
            ,nHeight
           );
    return TRUE;
} // end of wxScrollBar::Create

wxScrollBar::~wxScrollBar()
{
}

bool wxScrollBar::OS2OnScroll (
  int                               nOrientation
, WXWORD                            wParam
, WXWORD                            wPos
, WXHWND                            hControl
)
{
    int                             nPosition;
    int                             nMaxPos;
    int                             nTrackPos = wPos;
    int                             nMinPos;
    int                             nScrollInc;
    wxEventType                     vScrollEvent = wxEVT_NULL;

    MRESULT                         vRange;

    //
    // When we're dragging the scrollbar we can't use pos parameter because it
    // is limited to 16 bits
    //
    if (wParam == SB_SLIDERPOSITION || wParam == SB_SLIDERTRACK)
    {
        SBCDATA                     vScrollInfo;

        vScrollInfo.sHilite = SB_SLIDERTRACK;

        ::WinSendMsg((HWND)GetHwnd(), WM_QUERYWINDOWPARAMS, (PVOID)&vScrollInfo, NULL);

        nTrackPos = vScrollInfo.posThumb;
        nPosition = vScrollInfo.posFirst;
        nMaxPos   = vScrollInfo.posLast;
    }
    else
    {
        nPosition = (int)(MRESULT)::WinSendMsg((HWND)GetHwnd(), SBM_QUERYPOS, (MPARAM)NULL, (MPARAM)NULL);
        vRange = ::WinSendMsg((HWND)GetHwnd(), SBM_QUERYRANGE, (MPARAM)NULL, (MPARAM)NULL);
        nMinPos = SHORT1FROMMR(vRange);
        nMaxPos = SHORT2FROMMR(vRange);
    }
    //
    // A page size greater than one has the effect of reducing the effective
    // range, therefore the range has already been boosted artificially - so
    // reduce it again.
    //
    if (m_nPageSize > 1)
        nMaxPos -= (m_nPageSize - 1);
    switch (wParam)
    {
        case SB_LINEUP:
            nScrollInc   = -1;
            vScrollEvent = wxEVT_SCROLL_LINEUP;
            break;

        case SB_LINEDOWN:
            nScrollInc   = 1;
            vScrollEvent = wxEVT_SCROLL_LINEDOWN;
            break;

        case SB_PAGEUP:
            nScrollInc   = -GetPageSize();
            vScrollEvent = wxEVT_SCROLL_PAGEUP;
            break;

        case SB_PAGEDOWN:
            nScrollInc   = GetPageSize();
            vScrollEvent = wxEVT_SCROLL_PAGEDOWN;
            break;

        case SB_SLIDERTRACK:
            nScrollInc   = nTrackPos - nPosition;
            vScrollEvent = wxEVT_SCROLL_THUMBTRACK;
            break;

        case SB_ENDSCROLL:
            nScrollInc   = 0;
            vScrollEvent = wxEVT_SCROLL_ENDSCROLL;
            break;

        default:
            nScrollInc = 0;
    }
    if (nScrollInc)
    {
        nPosition += nScrollInc;

        if (nPosition < 0)
            nPosition = 0;
        if (nPosition > nMaxPos)
            nPosition = nMaxPos;
        SetThumbPosition(nPosition);
    }
    else if ( vScrollEvent != wxEVT_SCROLL_THUMBRELEASE &&
              vScrollEvent != wxEVT_SCROLL_ENDSCROLL
            )
    {
        //
        // Don't process the event if there is no displacement,
        // unless this is a thumb release or end scroll event.
        //
        return FALSE;
    }

    wxScrollEvent                   vEvent( vScrollEvent
                                           ,m_windowId
                                          );

    vEvent.SetOrientation(IsVertical() ? wxVERTICAL : wxHORIZONTAL);
    vEvent.SetPosition(nPosition);
    vEvent.SetEventObject(this);
    return GetEventHandler()->ProcessEvent(vEvent);
} // end of wxScrollBar::OS2OnScroll

void wxScrollBar::SetThumbPosition (
  int                               nViewStart
)
{
    SBCDATA                        vInfo;

    memset(&vInfo, '\0', sizeof(SBCDATA));
    vInfo.cb       = sizeof(SBCDATA);
    vInfo.posThumb = nViewStart;

    ::WinSendMsg((HWND)GetHwnd(), WM_SETWINDOWPARAMS, (MPARAM)&vInfo, (MPARAM)NULL);
    ::WinSendMsg((HWND)GetHwnd(), SBM_SETPOS, (MPARAM)nViewStart, (MPARAM)NULL);
} // end of wxScrollBar::SetThumbPosition

int wxScrollBar::GetThumbPosition() const
{
    return((int)(MRESULT)::WinSendMsg((HWND)GetHwnd(), SBM_QUERYPOS, (MPARAM)NULL, (MPARAM)NULL));
} // end of wxScrollBar::GetThumbPosition

void wxScrollBar::SetScrollbar (
  int                               nPosition
, int                               nThumbSize
, int                               nRange
, int                               nPageSize
, bool                              bRefresh
)
{
    SBCDATA                         vInfo;
    //
    // The lRange (number of scroll steps) is the
    // object length minus the page size.
    //
    int                             nRange1 = wxMax((m_nObjectSize - m_nPageSize), 0);

    m_nViewSize   = nPageSize;
    m_nPageSize   = nThumbSize;
    m_nObjectSize = nRange;


    //
    // Try to adjust the lRange to cope with page size > 1
    // (see comment for SetPageLength)
    //
    if (m_nPageSize > 1 )
    {
        nRange1 += (m_nPageSize - 1);
    }
    vInfo.cb = sizeof(SBCDATA);
    vInfo.cVisible = m_nPageSize;
    vInfo.posFirst = 0;
    vInfo.posLast  = nRange1;
    vInfo.posThumb = nPosition;

    ::WinSendMsg((HWND)GetHwnd(), WM_SETWINDOWPARAMS, (MPARAM)&vInfo, (MPARAM)NULL);
} // end of wxScrollBar::SetScrollbar

WXHBRUSH wxScrollBar::OnCtlColor (
  WXHDC                             hDC
, WXHWND                            hWnd
, WXUINT                            uCtlColor
, WXUINT                            uMessage
, WXWPARAM                          wParam
, WXLPARAM                          lParam
)
{
    //
    // Does nothing under OS/2
    //
    return 0;
} // end of wxScrollBar::OnCtlColor

void wxScrollBar::Command (
  wxCommandEvent&                   rEvent
)
{
    SetThumbPosition(rEvent.m_commandInt);
    ProcessCommand(rEvent);
} // end of wxScrollBar::Command

