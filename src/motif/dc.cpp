/////////////////////////////////////////////////////////////////////////////
// Name:        dc.cpp
// Purpose:     wxDC class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id: dc.cpp,v 1.17 1999/12/14 23:46:37 VS Exp $
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
    #pragma implementation "dc.h"
#endif

#include "wx/dc.h"
#include "wx/dcmemory.h"
#include "wx/defs.h"

    IMPLEMENT_ABSTRACT_CLASS(wxDC, wxObject)

//-----------------------------------------------------------------------------
// constants
//-----------------------------------------------------------------------------

#define mm2inches        0.0393700787402
#define inches2mm        25.4
#define mm2twips         56.6929133859
#define twips2mm         0.0176388888889
#define mm2pt            2.83464566929
#define pt2mm            0.352777777778

//-----------------------------------------------------------------------------
// wxDC
//-----------------------------------------------------------------------------

wxDC::wxDC()
{
    m_ok = FALSE;

    m_mm_to_pix_x = 1.0;
    m_mm_to_pix_y = 1.0;

    m_backgroundMode = wxTRANSPARENT;

    m_isInteractive = FALSE;
}

void wxDC::DoDrawIcon( const wxIcon &icon, wxCoord x, wxCoord y)
{
    wxCHECK_RET( Ok(), "invalid dc" );
    wxCHECK_RET( icon.Ok(), "invalid icon" );

    DoDrawBitmap(icon, x, y, TRUE);
}

void wxDC::DoDrawBitmap( const wxBitmap& bitmap, wxCoord x, wxCoord y, bool useMask )
{
    wxCHECK_RET( bitmap.Ok(), "invalid bitmap" );

    wxMemoryDC memDC;
    memDC.SelectObject(bitmap);

#if 0
    // Not sure if we need this. The mask should leave the masked areas as per
    // the original background of this DC.
    if (useMask)
    {
        // There might be transparent areas, so make these the same colour as this
        // DC
        memDC.SetBackground(* GetBackground());
        memDC.Clear();
    }
#endif // 0

    Blit(x, y, bitmap.GetWidth(), bitmap.GetHeight(), &memDC, 0, 0, wxCOPY, useMask);

    memDC.SelectObject(wxNullBitmap);
}

void wxDC::DoSetClippingRegion( wxCoord x, wxCoord y, wxCoord width, wxCoord height )
{
    m_clipping = TRUE;
    m_clipX1 = x;
    m_clipY1 = y;
    m_clipX2 = x + width;
    m_clipY2 = y + height;
}

void wxDC::DestroyClippingRegion()
{
    m_clipping = FALSE;
}

void wxDC::DoGetSize( int* width, int* height ) const
{
    if ( width )
        *width = m_maxX - m_minX;
    if ( height )
        *height = m_maxY - m_minY;
}

void wxDC::DoGetSizeMM( int* width, int* height ) const
{
    int w, h;
    GetSize( &w, &h );

    if ( width )
        *width = int( double(w) / (m_scaleX*m_mm_to_pix_x) );
    if ( height )
        *height = int( double(h) / (m_scaleY*m_mm_to_pix_y) );
}

// Resolution in pixels per logical inch
wxSize wxDC::GetPPI() const
{
    // TODO (should probably be pure virtual)
    return wxSize(0, 0);
}

void wxDC::SetMapMode( int mode )
{
    switch (mode)
    {
    case wxMM_TWIPS:
        SetLogicalScale( twips2mm*m_mm_to_pix_x, twips2mm*m_mm_to_pix_y );
        break;
    case wxMM_POINTS:
        SetLogicalScale( pt2mm*m_mm_to_pix_x, pt2mm*m_mm_to_pix_y );
        break;
    case wxMM_METRIC:
        SetLogicalScale( m_mm_to_pix_x, m_mm_to_pix_y );
        break;
    case wxMM_LOMETRIC:
        SetLogicalScale( m_mm_to_pix_x/10.0, m_mm_to_pix_y/10.0 );
        break;
    default:
    case wxMM_TEXT:
        SetLogicalScale( 1.0, 1.0 );
        break;
    }
    if (mode != wxMM_TEXT)
    {
        m_needComputeScaleX = TRUE;
        m_needComputeScaleY = TRUE;
    }
}

void wxDC::SetUserScale( double x, double y )
{
    // allow negative ? -> no
    m_userScaleX = x;
    m_userScaleY = y;
    ComputeScaleAndOrigin();
}

void wxDC::SetLogicalScale( double x, double y )
{
    // allow negative ?
    m_logicalScaleX = x;
    m_logicalScaleY = y;
    ComputeScaleAndOrigin();
}

void wxDC::SetLogicalOrigin( wxCoord x, wxCoord y )
{
    m_logicalOriginX = x * m_signX;   // is this still correct ?
    m_logicalOriginY = y * m_signY;
    ComputeScaleAndOrigin();
}

void wxDC::SetDeviceOrigin( wxCoord x, wxCoord y )
{
    // only wxPostScripDC has m_signX = -1, we override SetDeviceOrigin there
    m_deviceOriginX = x;
    m_deviceOriginY = y;
    ComputeScaleAndOrigin();
}

void wxDC::SetAxisOrientation( bool xLeftRight, bool yBottomUp )
{
    m_signX = xLeftRight ?  1 : -1;
    m_signY = yBottomUp  ? -1 :  1;
    ComputeScaleAndOrigin();
}

wxCoord wxDCBase::DeviceToLogicalX(wxCoord x) const
{
  return ((wxDC *)this)->XDEV2LOG(x);
}

wxCoord wxDCBase::DeviceToLogicalY(wxCoord y) const
{
  return ((wxDC *)this)->YDEV2LOG(y);
}

wxCoord wxDCBase::DeviceToLogicalXRel(wxCoord x) const
{
  return ((wxDC *)this)->XDEV2LOGREL(x);
}

wxCoord wxDCBase::DeviceToLogicalYRel(wxCoord y) const
{
  return ((wxDC *)this)->YDEV2LOGREL(y);
}

wxCoord wxDCBase::LogicalToDeviceX(wxCoord x) const
{
  return ((wxDC *)this)->XLOG2DEV(x);
}

wxCoord wxDCBase::LogicalToDeviceY(wxCoord y) const
{
  return ((wxDC *)this)->YLOG2DEV(y);
}

wxCoord wxDCBase::LogicalToDeviceXRel(wxCoord x) const
{
  return ((wxDC *)this)->XLOG2DEVREL(x);
}

wxCoord wxDCBase::LogicalToDeviceYRel(wxCoord y) const
{
  return ((wxDC *)this)->YLOG2DEVREL(y);
}

void wxDC::ComputeScaleAndOrigin()
{
    m_scaleX = m_logicalScaleX * m_userScaleX;
    m_scaleY = m_logicalScaleY * m_userScaleY;
}

