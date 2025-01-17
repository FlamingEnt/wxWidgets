/////////////////////////////////////////////////////////////////////////////
// Name:        cursor.cpp
// Purpose:     wxCursor class
// Author:      David Webster
// Modified by:
// Created:     10/13/99
// RCS-ID:      $Id: CURSOR.CPP,v 1.8.2.1 2003/04/17 17:24:44 DW Exp $
// Copyright:   (c) David Webster
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include <stdio.h>
#include "wx/setup.h"
#include "wx/list.h"
#include "wx/utils.h"
#include "wx/app.h"
#include "wx/cursor.h"
#include "wx/icon.h"
#include "wx/resource.h"
#endif

#include "wx/os2/private.h"
#include "wx/image.h"

#include "assert.h"

IMPLEMENT_DYNAMIC_CLASS(wxCursor, wxBitmap)

wxCursorRefData::wxCursorRefData(void)
{
  m_nWidth = 32;
  m_nHeight = 32;
  m_hCursor = 0 ;
  m_bDestroyCursor = FALSE;
}

void wxCursorRefData::Free()
{
    if (m_hCursor)
    {
        if (m_bDestroyCursor)
            ::WinDestroyPointer((HPOINTER)m_hCursor);
        m_hCursor = 0;
    }
} // end of wxCursorRefData::Free

// Cursors
wxCursor::wxCursor(void)
{
}

wxCursor::wxCursor(
  const char                        WXUNUSED(bits)[]
, int                               WXUNUSED(width)
, int                               WXUNUSED(height)
, int                               WXUNUSED(hotSpotX)
, int                               WXUNUSED(hotSpotY)
, const char                        WXUNUSED(maskBits)[]
)
{
}

wxCursor::wxCursor(
  const wxImage&                    rImage
)
{
    wxImage                         vImage32 = rImage.Scale(32,32);
    int                             nWidth   = vImage32.GetWidth();
    int                             nHeight  = vImage32.GetHeight();

    //
    // Need a bitmap handle somehow
    //
    HBITMAP                      hBitmap = wxBitmap(vImage32).GetHBITMAP();
    int                          nHotSpotX = vImage32.GetOptionInt(wxCUR_HOTSPOT_X);
    int                          nHotSpotY = vImage32.GetOptionInt(wxCUR_HOTSPOT_Y);

    if (nHotSpotX < 0 || nHotSpotX >= nWidth)
            nHotSpotX = 0;
    if (nHotSpotY < 0 || nHotSpotY >= nHeight)
            nHotSpotY = 0;


    wxCursorRefData*                pRefData = new wxCursorRefData;

    m_refData = pRefData;
    pRefData->m_hCursor = (WXHCURSOR) ::WinCreatePointer( HWND_DESKTOP
                                                         ,hBitmap
                                                         ,TRUE
                                                         ,nHotSpotY
                                                         ,nHotSpotX
                                                        );

} // end of wxCursor::wxCursor

wxCursor::wxCursor(
  const wxString&                   rsCursorFile
, long                              lFlags
, int                               nHotSpotX
, int                               nHotSpotY
)
{
    wxCursorRefData*                pRefData = new wxCursorRefData;

    pRefData = new wxCursorRefData;
    m_refData = pRefData;
    pRefData->m_bDestroyCursor = FALSE;
    if (lFlags == wxBITMAP_TYPE_CUR_RESOURCE)
    {
        pRefData->m_hCursor = (WXHCURSOR) ::WinLoadPointer( HWND_DESKTOP
                                                           ,0
                                                           ,(ULONG)lFlags // if OS/2 this should be the resource Id
                                                          );
    }
} // end of wxCursor::wxCursor

// Cursors by stock number
wxCursor::wxCursor(
  int                               nCursorType
)
{
    wxCursorRefData*                pRefData = new wxCursorRefData;

    m_refData = pRefData;
    switch (nCursorType)
    {
        case wxCURSOR_ARROWWAIT:
            pRefData->m_hCursor = (WXHCURSOR) ::WinQuerySysPointer( HWND_DESKTOP
                                                                   ,(ULONG)SPTR_WAIT
                                                                   ,FALSE
                                                                  );
            break;

        case wxCURSOR_WAIT:
            pRefData->m_hCursor = (WXHCURSOR) ::WinQuerySysPointer( HWND_DESKTOP
                                                                   ,(ULONG)SPTR_WAIT
                                                                   ,FALSE
                                                                  );
            break;

        case wxCURSOR_IBEAM:
            pRefData->m_hCursor = (WXHCURSOR) ::WinQuerySysPointer( HWND_DESKTOP
                                                                   ,(ULONG)SPTR_TEXT
                                                                   ,FALSE
                                                                  );
            break;

        case wxCURSOR_CROSS:
            pRefData->m_hCursor = (WXHCURSOR) ::WinQuerySysPointer( HWND_DESKTOP
                                                                   ,(ULONG)SPTR_MOVE
                                                                   ,FALSE
                                                                  );
            break;

        case wxCURSOR_SIZENWSE:
            pRefData->m_hCursor = (WXHCURSOR) ::WinQuerySysPointer( HWND_DESKTOP
                                                                   ,(ULONG)SPTR_SIZENWSE
                                                                   ,FALSE
                                                                  );
            break;

        case wxCURSOR_SIZENESW:
            pRefData->m_hCursor = (WXHCURSOR) ::WinQuerySysPointer( HWND_DESKTOP
                                                                   ,(ULONG)SPTR_SIZENESW
                                                                   ,FALSE
                                                                  );
            break;

        case wxCURSOR_SIZEWE:
            pRefData->m_hCursor = (WXHCURSOR) ::WinQuerySysPointer( HWND_DESKTOP
                                                                   ,(ULONG)SPTR_SIZEWE
                                                                   ,FALSE
                                                                  );
            break;

        case wxCURSOR_SIZENS:
            pRefData->m_hCursor = (WXHCURSOR) ::WinQuerySysPointer( HWND_DESKTOP
                                                                   ,(ULONG)SPTR_SIZENS
                                                                   ,FALSE
                                                                  );
            break;

        case wxCURSOR_CHAR:
            pRefData->m_hCursor = (WXHCURSOR) ::WinQuerySysPointer( HWND_DESKTOP
                                                                   ,(ULONG)SPTR_ARROW
                                                                   ,FALSE
                                                                  );
            break;

        case wxCURSOR_HAND:
            pRefData->m_hCursor = (WXHCURSOR) ::WinLoadPointer( HWND_DESKTOP
                                                               ,0
                                                               ,(ULONG)wxCURSOR_HAND
                                                              );
            break;

        case wxCURSOR_BULLSEYE:
            pRefData->m_hCursor = (WXHCURSOR) ::WinLoadPointer( HWND_DESKTOP
                                                               ,0
                                                               ,(ULONG)wxCURSOR_BULLSEYE
                                                              );
            break;

        case wxCURSOR_PENCIL:
            pRefData->m_hCursor = (WXHCURSOR) ::WinLoadPointer( HWND_DESKTOP
                                                               ,0
                                                               ,(ULONG)wxCURSOR_PENCIL
                                                              );
            break;

        case wxCURSOR_MAGNIFIER:
            pRefData->m_hCursor = (WXHCURSOR) ::WinLoadPointer( HWND_DESKTOP
                                                               ,0
                                                               ,(ULONG)wxCURSOR_MAGNIFIER
                                                              );
            break;

        case wxCURSOR_NO_ENTRY:
            pRefData->m_hCursor = (WXHCURSOR) ::WinLoadPointer( HWND_DESKTOP
                                                               ,0
                                                               ,(ULONG)wxCURSOR_NO_ENTRY
                                                              );
            break;

        case wxCURSOR_LEFT_BUTTON:
            pRefData->m_hCursor = (WXHCURSOR) ::WinQuerySysPointer( HWND_DESKTOP
                                                                   ,(ULONG)SPTR_ARROW
                                                                   ,FALSE
                                                                  );
            break;

        case wxCURSOR_RIGHT_BUTTON:
            pRefData->m_hCursor = (WXHCURSOR) ::WinQuerySysPointer( HWND_DESKTOP
                                                                   ,(ULONG)SPTR_ARROW
                                                                   ,FALSE
                                                                  );
            break;

        case wxCURSOR_MIDDLE_BUTTON:
            pRefData->m_hCursor = (WXHCURSOR) ::WinQuerySysPointer( HWND_DESKTOP
                                                                   ,(ULONG)SPTR_ARROW
                                                                   ,FALSE
                                                                  );
            break;

        case wxCURSOR_SIZING:
            pRefData->m_hCursor = (WXHCURSOR) ::WinQuerySysPointer( HWND_DESKTOP
                                                                   ,(ULONG)SPTR_SIZE
                                                                   ,FALSE
                                                                  );
            break;

        case wxCURSOR_WATCH:
            pRefData->m_hCursor = (WXHCURSOR) ::WinLoadPointer( HWND_DESKTOP
                                                               ,0
                                                               ,(ULONG)wxCURSOR_WATCH
                                                              );
            break;

        case wxCURSOR_SPRAYCAN:
            pRefData->m_hCursor = (WXHCURSOR) ::WinLoadPointer( HWND_DESKTOP
                                                              ,0
                                                              ,(ULONG)WXCURSOR_ROLLER
                                                             );
            break;

        case wxCURSOR_PAINT_BRUSH:
            pRefData->m_hCursor = (WXHCURSOR) ::WinLoadPointer( HWND_DESKTOP
                                                               ,0
                                                               ,(ULONG)WXCURSOR_PBRUSH
                                                              );
            break;

        case wxCURSOR_POINT_LEFT:
            pRefData->m_hCursor = (WXHCURSOR) ::WinLoadPointer( HWND_DESKTOP
                                                               ,0
                                                               ,(ULONG)WXCURSOR_PLEFT
                                                              );
            break;

        case wxCURSOR_POINT_RIGHT:
            pRefData->m_hCursor = (WXHCURSOR) ::WinLoadPointer( HWND_DESKTOP
                                                               ,0
                                                               ,(ULONG)WXCURSOR_PRIGHT
                                                              );
            break;

        case wxCURSOR_QUESTION_ARROW:
            pRefData->m_hCursor = (WXHCURSOR) ::WinLoadPointer( HWND_DESKTOP
                                                               ,0
                                                               ,(ULONG)WXCURSOR_QARROW
                                                              );
            break;

        case wxCURSOR_BLANK:
            pRefData->m_hCursor = (WXHCURSOR) ::WinLoadPointer( HWND_DESKTOP
                                                               ,0
                                                               ,(ULONG)WXCURSOR_BLANK
                                                              );
            break;

        default:
        case wxCURSOR_ARROW:
            pRefData->m_hCursor = (WXHCURSOR) ::WinQuerySysPointer( HWND_DESKTOP
                                                                   ,(ULONG)SPTR_ARROW
                                                                   ,FALSE
                                                                  );
            break;
    }
    ((wxCursorRefData *)m_refData)->m_bDestroyCursor = FALSE;
} // end of wxCursor::wxCursor

// Global cursor setting
void wxSetCursor(const wxCursor& cursor)
{
    extern wxCursor *g_globalCursor;

    if ( cursor.Ok() && cursor.GetHCURSOR() )
    {
//        ::SetCursor((HCURSOR) cursor.GetHCURSOR());

        if ( g_globalCursor )
            (*g_globalCursor) = cursor;
    }
}

