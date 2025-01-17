/////////////////////////////////////////////////////////////////////////////
// Name:        palette.cpp
// Purpose:     wxPalette
// Author:      AUTHOR
// Modified by:
// Created:     ??/??/98
// RCS-ID:      $Id: PALETTE.CPP,v 1.9 2001/04/22 20:32:01 SN Exp $
// Copyright:   (c) AUTHOR
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include <stdio.h>
#include "wx/defs.h"
#include "wx/setup.h"
#include "wx/string.h"
#include "wx/os2/private.h"
#include "wx/palette.h"
#include "wx/app.h"
#endif

#define INCL_PM
#define INCL_GPI

#include "assert.h"

IMPLEMENT_DYNAMIC_CLASS(wxPalette, wxGDIObject)

/*
 * Palette
 *
 */

wxPaletteRefData::wxPaletteRefData()
{
    m_hPalette = NULLHANDLE;
    m_hPS      = NULLHANDLE;
} // end of wxPaletteRefData::wxPaletteRefData

wxPaletteRefData::~wxPaletteRefData()
{
    if ( m_hPalette )
        return;
} // end of wxPaletteRefData::~wxPaletteRefData

wxPalette::wxPalette()
{
} // end of wxPalette::wxPalette

wxPalette::wxPalette(
  int                               n
, const unsigned char*              pRed
, const unsigned char*              pGreen
, const unsigned char*              pBlue
)
{
    Create( n
           ,pRed
           ,pGreen
           ,pBlue
          );
} // end of wxPalette::wxPalette

wxPalette::~wxPalette()
{
} // end of wxPalette::~wxPalette

bool wxPalette::FreeResource(
  bool                              bForce
)
{
    if ( M_PALETTEDATA && M_PALETTEDATA->m_hPalette)
    {
        ::GpiSelectPalette(M_PALETTEDATA->m_hPS, NULLHANDLE);
        ::GpiDeletePalette((HPAL)M_PALETTEDATA->m_hPalette);
    }
    return TRUE;
} // end of wxPalette::FreeResource

bool wxPalette::Create(
  int                               n
, const unsigned char*              pRed
, const unsigned char*              pGreen
, const unsigned char*              pBlue
)
{
    PULONG                          pualTable;

    UnRef();

    m_refData = new wxPaletteRefData;
    pualTable = new ULONG[n];
    if (!pualTable)
        return(FALSE);

    for (int i = 0; i < n; i ++)
    {
        pualTable[i] = (PC_RESERVED * 16777216) + ((int)pRed[i] * 65536) + ((int)pGreen[i] * 256) + (int)pBlue[i];
    }
    M_PALETTEDATA->m_hPalette = (WXHPALETTE)::GpiCreatePalette( vHabmain
                                                               ,LCOL_PURECOLOR
                                                               ,LCOLF_CONSECRGB
                                                               ,(LONG)n
                                                               ,pualTable
                                                              );
    delete [] pualTable;
    return TRUE;
} // end of wxPalette::Create

int wxPalette::GetPixel(
  const unsigned char               cRed
, const unsigned char               cGreen
, const unsigned char               cBlue
) const
{
    bool                            bFound = FALSE;
    PULONG                          pualTable = NULL;
    ULONG                           ulNumEntries;
    ULONG                           ulRGB = (PC_RESERVED * 16777216) +
                                            ((int)cRed * 65536) +
                                            ((int)cGreen * 256) +
                                             (int)cBlue;

    if (!m_refData)
        return FALSE;

    //
    // Get number of entries first
    //
    ulNumEntries = ::GpiQueryPaletteInfo( M_PALETTEDATA->m_hPalette
                                         ,M_PALETTEDATA->m_hPS
                                         ,0                    // No options
                                         ,0                    // No start index
                                         ,0                    // Force return of number entries
                                         ,NULL                 // No array
                                        );

    pualTable = new ULONG[ulNumEntries];

    //
    // Now get the entries
    //
    ulNumEntries = ::GpiQueryPaletteInfo( M_PALETTEDATA->m_hPalette
                                         ,M_PALETTEDATA->m_hPS
                                         ,0                    // No options
                                         ,0                    // start at 0
                                         ,ulNumEntries         // Force return of number entries
                                         ,pualTable            // Palette entry array with RGB values
                                        );
    //
    // Now loop through and find the matching entry
    //
    int                             i;
    for (i = 0; i < ulNumEntries; i++)
    {
        if (pualTable[i] == ulRGB)
        {
            bFound = TRUE;
            break;
        }
    }
    if (!bFound)
        return 0;
    return (i + 1);
} // end of wxPalette::GetPixel

bool wxPalette::GetRGB(
  int                               nIndex
, unsigned char*                    pRed
, unsigned char*                    pGreen
, unsigned char*                    pBlue
) const
{
    PULONG                          pualTable = NULL;
    RGB2                            vRGB;
    ULONG                           ulNumEntries;

    if (!m_refData)
        return FALSE;

    if (nIndex < 0 || nIndex > 255)
        return FALSE;
    //
    // Get number of entries first
    //
    ulNumEntries = ::GpiQueryPaletteInfo( M_PALETTEDATA->m_hPalette
                                         ,M_PALETTEDATA->m_hPS
                                         ,0                    // No options
                                         ,0                    // No start index
                                         ,0                    // Force return of number entries
                                         ,NULL                 // No array
                                        );

    pualTable = new ULONG[ulNumEntries];

    //
    // Now get the entries
    //
    ulNumEntries = ::GpiQueryPaletteInfo( M_PALETTEDATA->m_hPalette
                                         ,M_PALETTEDATA->m_hPS
                                         ,0                    // No options
                                         ,0                    // start at 0
                                         ,ulNumEntries         // Force return of number entries
                                         ,pualTable            // Palette entry array with RGB values
                                        );

    memcpy(&vRGB, &pualTable[nIndex], sizeof(RGB2));
    *pBlue  = vRGB.bBlue;
    *pGreen = vRGB.bGreen;
    *pRed   = vRGB.bRed;
    return TRUE;
} // end of wxPalette::GetRGB

void wxPalette::SetHPALETTE(
  WXHPALETTE                        hPal
)
{
    if ( !m_refData )
        m_refData = new wxPaletteRefData;

    M_PALETTEDATA->m_hPalette = hPal;
} // end of wxPalette::SetHPALETTE

void wxPalette::SetPS(
  HPS                               hPS
)
{
    if ( !m_refData )
        m_refData = new wxPaletteRefData;

    ::GpiSelectPalette(M_PALETTEDATA->m_hPS, M_PALETTEDATA->m_hPalette);
    M_PALETTEDATA->m_hPS = hPS;
} // end of wxPalette::SetHPALETTE

