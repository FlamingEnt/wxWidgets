/////////////////////////////////////////////////////////////////////////////
// Name:        dcmemory.cpp
// Purpose:     wxMemoryDC class
// Author:      David Webster
// Modified by:
// Created:     10/14/99
// RCS-ID:      $Id: DCMEMORY.CPP,v 1.18 2002/05/10 04:09:27 DW Exp $
// Copyright:   (c) David Webster
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/utils.h"
#include "wx/app.h"
#include "wx/log.h"
#endif

#include "wx/os2/private.h"

#include "wx/dcmemory.h"

IMPLEMENT_DYNAMIC_CLASS(wxMemoryDC, wxDC)

/////////////////////////////////////////////////////////////////////////////
// Memory DC
/////////////////////////////////////////////////////////////////////////////

wxMemoryDC::wxMemoryDC(void)
{
    CreateCompatible(NULL);
    Init();
} // end of wxMemoryDC::wxMemoryDC

wxMemoryDC::wxMemoryDC(
  wxDC*                             pOldDC
)
{
    pOldDC->BeginDrawing();
    CreateCompatible(pOldDC);
    pOldDC->EndDrawing();
    Init();
} // end of wxMemoryDC::wxMemoryDC

void wxMemoryDC::Init()
{
    if (m_ok)
    {
        SetBrush(*wxWHITE_BRUSH);
        SetPen(*wxBLACK_PEN);

        // the background mode is only used for text background and is set in
        // DrawText() to OPAQUE as required, otherwise always TRANSPARENT
        ::GpiSetBackMix( GetHPS(), BM_LEAVEALONE );
    }
    memset(&m_vRclPaint, 0, sizeof(m_vRclPaint));
} // end of wxMemoryDC::Init

bool wxMemoryDC::CreateCompatible(
  wxDC*                             pDC
)
{
    HDC                             hDC;
    HPS                             hPS;
    DEVOPENSTRUC                    vDOP = {0L, "DISPLAY", NULL, 0L, 0L, 0L, 0L, 0L, 0L};
    SIZEL                           vSize = {0, 0};

    //
    // Create a memory device context
    //
    hDC = ::DevOpenDC(vHabmain, OD_MEMORY, "*", 5L, (PDEVOPENDATA)&vDOP, NULLHANDLE);
    if (hDC != DEV_ERROR)
    {
        hPS = ::GpiCreatePS(vHabmain, hDC, &vSize, PU_PELS | GPIT_MICRO | GPIA_ASSOC);
        if (hPS != GPI_ERROR)
        {
            m_hPS = hPS;
            m_hDC = hDC;
            m_ok = TRUE;
            m_bOwnsDC = TRUE;
            //
            // Set the wxWindows color table
            //
            ::GpiCreateLogColorTable( m_hPS
                                     ,0L
                                     ,LCOLF_CONSECRGB
                                     ,0L
                                     ,(LONG)wxTheColourDatabase->m_nSize
                                     ,(PLONG)wxTheColourDatabase->m_palTable
                                    );
            ::GpiCreateLogColorTable( m_hPS
                                     ,0L
                                     ,LCOLF_RGB
                                     ,0L
                                     ,0L
                                     ,NULL
                                    );
        }
        else
        {
            m_hPS = NULLHANDLE;
            m_hDC = NULLHANDLE;
            m_ok  = FALSE;
            m_bOwnsDC = FALSE;
        }
    }
    else
    {
        m_hPS = NULLHANDLE;
        m_hDC = NULLHANDLE;
        m_ok  = FALSE;
        m_bOwnsDC = FALSE;
    }

    //
    // As we created the DC, we must delete it in the dtor
    //
    m_bOwnsDC = TRUE;
    m_ok = m_hDC != 0;
    return m_ok;
} // end of wxMemoryDC::CreateCompatible

void wxMemoryDC::SelectObject(
  const wxBitmap&                   rBitmap
)
{
    //
    // Select old bitmap out of the device context
    //
    if (m_hOldBitmap)
    {
        ::GpiSetBitmap(m_hPS, NULLHANDLE);
        if (m_vSelectedBitmap.Ok())
        {
            m_vSelectedBitmap.SetSelectedInto(NULL);
            m_vSelectedBitmap = wxNullBitmap;
        }
    }

    //
    // Check for whether the bitmap is already selected into a device context
    //
    wxCHECK_RET( !rBitmap.GetSelectedInto() ||
                 (rBitmap.GetSelectedInto() == this),
                 wxT("Bitmap is selected in another wxMemoryDC, delete the first wxMemoryDC or use SelectObject(NULL)") );

    WXHBITMAP                       hBmp = rBitmap.GetHBITMAP();

    if (!hBmp)
    {
        //
        // Bmps drawn to are upside down, so flip it before committing
        //
        POINTL                      vPoint[4] = { 0, m_vSelectedBitmap.GetHeight(),
                                                  m_vSelectedBitmap.GetWidth(), 0,
                                                  0, 0, m_vSelectedBitmap.GetWidth(), m_vSelectedBitmap.GetHeight()
                                                };


        ::GpiBitBlt( m_hPS
                    ,m_hPS
                    ,4
                    ,vPoint
                    ,ROP_SRCCOPY
                    ,BBO_IGNORE
                   );
        m_vSelectedBitmap.SetSelectedInto(NULL);
    }
    m_vSelectedBitmap = rBitmap;


    if (!hBmp)
    {

        m_hOldBitmap = (WXHBITMAP)::GpiSetBitmap(m_hPS, NULLHANDLE);
        return;
    }
    m_vSelectedBitmap.SetSelectedInto(this);
    m_hOldBitmap = (WXHBITMAP)::GpiSetBitmap(m_hPS, (HBITMAP)hBmp);

    if (m_hOldBitmap == HBM_ERROR)
    {
        wxLogLastError(wxT("SelectObject(memDC, bitmap)"));
        wxFAIL_MSG(wxT("Couldn't select a bitmap into wxMemoryDC"));
    }
} // end of wxMemoryDC::SelectObject

void wxMemoryDC::DoGetSize(
  int*                              pWidth
, int*                              pHeight
) const
{
    if (!m_vSelectedBitmap.Ok())
    {
        *pWidth  = 0;
        *pHeight = 0;
        return;
    }
    *pWidth = m_vSelectedBitmap.GetWidth();
    *pHeight = m_vSelectedBitmap.GetHeight();
} // end of wxMemoryDC::DoGetSize

