/////////////////////////////////////////////////////////////////////////////
// Name:        dc.cpp
// Purpose:     wxDC class
// Author:      AUTHOR
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id: dc.cpp,v 1.56.2.11 2003/09/18 11:35:32 SC Exp $
// Copyright:   (c) AUTHOR
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////
#ifdef __GNUG__
#pragma implementation "dc.h"
#endif
#include "wx/dc.h"
#include "wx/app.h"
#include "wx/mac/uma.h"
#include "wx/dcmemory.h"
#include "wx/dcprint.h"
#include "wx/region.h"
#include "wx/image.h"
#include "wx/log.h"

#if __MSL__ >= 0x6000
#include "math.h"
using namespace std ;
#endif
#include "wx/mac/private.h"
#include "ATSUnicode.h"
#include "TextCommon.h"
#include "TextEncodingConverter.h"
#if !USE_SHARED_LIBRARY
IMPLEMENT_ABSTRACT_CLASS(wxDC, wxObject)
#endif
//-----------------------------------------------------------------------------
// constants
//-----------------------------------------------------------------------------
#define mm2inches		0.0393700787402
#define inches2mm		25.4
#define mm2twips		56.6929133859
#define twips2mm		0.0176388888889
#define mm2pt			2.83464566929
#define pt2mm			0.352777777778
#ifndef __DARWIN__
const double M_PI = 3.14159265358979 ;
#endif
const double RAD2DEG  = 180.0 / M_PI;
const short kEmulatedMode = -1 ;
const short kUnsupportedMode = -2 ;

// set to 0 if problems arise
#define wxMAC_EXPERIMENTAL_DC 1

wxMacPortSetter::wxMacPortSetter( const wxDC* dc ) :
	m_ph( (GrafPtr) dc->m_macPort )
{
	wxASSERT( dc->Ok() ) ;
	m_dc = dc ;
	dc->MacSetupPort(&m_ph) ;
}
wxMacPortSetter::~wxMacPortSetter()
{
	m_dc->MacCleanupPort(&m_ph) ;
}

#if wxMAC_EXPERIMENTAL_DC
class wxMacFastPortSetter
{
public :
    wxMacFastPortSetter( const wxDC *dc ) 
    {
	    wxASSERT( dc->Ok() ) ;
	    GetPort( &m_oldPort ) ;
	    SetPort( (GrafPtr) dc->m_macPort ) ;
	    m_clipRgn = NewRgn() ;
	    GetClip( m_clipRgn ) ;
	    m_dc = dc ;
	    dc->MacSetupPort( NULL ) ;
    }
    ~wxMacFastPortSetter()
    {
        SetPort( (GrafPtr) m_dc->m_macPort ) ;
        SetClip( m_clipRgn ) ;
	    SetPort( m_oldPort ) ;
	    m_dc->MacCleanupPort( NULL ) ;
	    DisposeRgn( m_clipRgn ) ;
    }
private :
    RgnHandle m_clipRgn ;
    GrafPtr m_oldPort ;
    const wxDC*   m_dc ;
} ;

#else
typedef wxMacPortSetter wxMacFastPortSetter ;
#endif

wxMacWindowClipper::wxMacWindowClipper( const wxWindow* win )
{
    m_formerClip = NewRgn() ;
    m_newClip = NewRgn() ;
    GetClip( m_formerClip ) ;

    if ( win )
    {
#if 0
    	// this clipping area was set to the parent window's drawing area, lead to problems
    	// with MacOSX controls drawing outside their wx' rectangle
        RgnHandle insidergn = NewRgn() ;
        int x = 0 , y = 0;
        wxWindow *parent = win->GetParent() ;
        parent->MacWindowToRootWindow( &x,&y ) ;
        wxSize size = parent->GetSize() ;
        SetRectRgn( insidergn , parent->MacGetLeftBorderSize() , parent->MacGetTopBorderSize() ,
      	  size.x - parent->MacGetRightBorderSize(),
      	  size.y - parent->MacGetBottomBorderSize()) ;
        CopyRgn( (RgnHandle) parent->MacGetVisibleRegion(false).GetWXHRGN() , m_newClip ) ;
    	SectRgn( m_newClip , insidergn , m_newClip ) ;
        OffsetRgn( m_newClip , x , y ) ;
        SetClip( m_newClip ) ;
    	DisposeRgn( insidergn ) ;
#endif
        int x = 0 , y = 0;
        win->MacWindowToRootWindow( &x,&y ) ;
        CopyRgn( (RgnHandle) ((wxWindow*)win)->MacGetVisibleRegion().GetWXHRGN() , m_newClip ) ;
        OffsetRgn( m_newClip , x , y ) ;
        SetClip( m_newClip ) ;
	}
}
wxMacWindowClipper::~wxMacWindowClipper()
{
    SetClip( m_formerClip ) ;
	DisposeRgn( m_newClip ) ;
	DisposeRgn( m_formerClip ) ;
}
//-----------------------------------------------------------------------------
// Local functions
//-----------------------------------------------------------------------------
static inline double dmin(double a, double b) { return a < b ? a : b; }
static inline double dmax(double a, double b) { return a > b ? a : b; }
static inline double DegToRad(double deg) { return (deg * M_PI) / 180.0; }
//-----------------------------------------------------------------------------
// wxDC
//-----------------------------------------------------------------------------
// this function emulates all wx colour manipulations, used to verify the implementation
// by setting the mode in the blitting functions to kEmulatedMode
void wxMacCalculateColour( int logical_func , const RGBColor &srcColor , RGBColor &dstColor ) ;
void wxMacCalculateColour( int logical_func , const RGBColor &srcColor , RGBColor &dstColor )
{
    switch ( logical_func )
    {
    	case wxAND:        // src AND dst
    	    dstColor.red = dstColor.red & srcColor.red ;
    	    dstColor.green = dstColor.green & srcColor.green ;
    	    dstColor.blue = dstColor.blue & srcColor.blue ;
    	    break ;
    	case wxAND_INVERT: // (NOT src) AND dst
    	    dstColor.red = dstColor.red & ~srcColor.red ;
    	    dstColor.green = dstColor.green & ~srcColor.green ;
    	    dstColor.blue = dstColor.blue & ~srcColor.blue ;
    	    break ;
    	case wxAND_REVERSE:// src AND (NOT dst)
    	    dstColor.red = ~dstColor.red & srcColor.red ;
    	    dstColor.green = ~dstColor.green & srcColor.green ;
    	    dstColor.blue = ~dstColor.blue & srcColor.blue ;
    	    break ;
    	case wxCLEAR:      // 0
    	    dstColor.red = 0 ;
    	    dstColor.green = 0 ;
    	    dstColor.blue = 0 ;
    	    break ;
        case wxCOPY:       // src
    	    dstColor.red = srcColor.red ;
    	    dstColor.green = srcColor.green ;
    	    dstColor.blue = srcColor.blue ;
    		break ;
    	case wxEQUIV:      // (NOT src) XOR dst
    	    dstColor.red = dstColor.red ^ ~srcColor.red ;
    	    dstColor.green = dstColor.green ^ ~srcColor.green ;
    	    dstColor.blue = dstColor.blue ^ ~srcColor.blue ;
    	    break ;
    	case wxINVERT:     // NOT dst
    	    dstColor.red = ~dstColor.red ;
    	    dstColor.green = ~dstColor.green ;
    	    dstColor.blue = ~dstColor.blue ;
    		break ;
    	case wxNAND:       // (NOT src) OR (NOT dst)
    	    dstColor.red = ~dstColor.red | ~srcColor.red ;
    	    dstColor.green = ~dstColor.green | ~srcColor.green ;
    	    dstColor.blue = ~dstColor.blue | ~srcColor.blue ;
    	    break ;
    	case wxNOR:        // (NOT src) AND (NOT dst)
    	    dstColor.red = ~dstColor.red & ~srcColor.red ;
    	    dstColor.green = ~dstColor.green & ~srcColor.green ;
    	    dstColor.blue = ~dstColor.blue & ~srcColor.blue ;
    	    break ;
    	case wxNO_OP:      // dst
    	    break ;
    	case wxOR:         // src OR dst
    	    dstColor.red = dstColor.red | srcColor.red ;
    	    dstColor.green = dstColor.green | srcColor.green ;
    	    dstColor.blue = dstColor.blue | srcColor.blue ;
    	    break ;
    	case wxOR_INVERT:  // (NOT src) OR dst
    	    dstColor.red = dstColor.red | ~srcColor.red ;
    	    dstColor.green = dstColor.green | ~srcColor.green ;
    	    dstColor.blue = dstColor.blue | ~srcColor.blue ;
    	    break ;
    	case wxOR_REVERSE: // src OR (NOT dst)
    	    dstColor.red = ~dstColor.red | srcColor.red ;
    	    dstColor.green = ~dstColor.green | srcColor.green ;
    	    dstColor.blue = ~dstColor.blue | srcColor.blue ;
    		break ;
    	case wxSET:        // 1
    	    dstColor.red = 0xFFFF ;
    	    dstColor.green = 0xFFFF ;
    	    dstColor.blue = 0xFFFF ;
    	    break ;
    	case wxSRC_INVERT: // (NOT src)
    	    dstColor.red = ~srcColor.red ;
    	    dstColor.green = ~srcColor.green ;
    	    dstColor.blue = ~srcColor.blue ;
    		break ;
    	case wxXOR:        // src XOR dst
    	    dstColor.red = dstColor.red ^ srcColor.red ;
    	    dstColor.green = dstColor.green ^ srcColor.green ;
    	    dstColor.blue = dstColor.blue ^ srcColor.blue ;
    		break ;
    }
}
wxDC::wxDC()
{
  m_ok = FALSE;
  m_colour = TRUE;
  m_mm_to_pix_x = mm2pt;
  m_mm_to_pix_y = mm2pt;
  m_internalDeviceOriginX = 0;
  m_internalDeviceOriginY = 0;
  m_externalDeviceOriginX = 0;
  m_externalDeviceOriginY = 0;
  m_logicalScaleX = 1.0;
  m_logicalScaleY = 1.0;
  m_userScaleX = 1.0;
  m_userScaleY = 1.0;
  m_scaleX = 1.0;
  m_scaleY = 1.0;
  m_needComputeScaleX = FALSE;
  m_needComputeScaleY = FALSE;
  m_macPort = NULL ;
  m_macMask = NULL ;
  m_ok = FALSE ;
	m_macFontInstalled = false ;
	m_macBrushInstalled = false ;
	m_macPenInstalled = false ;
	m_macLocalOrigin.x = m_macLocalOrigin.y = 0 ;
	m_macBoundaryClipRgn = NewRgn() ;
	m_macCurrentClipRgn = NewRgn() ;
	SetRectRgn( (RgnHandle) m_macBoundaryClipRgn , -32000 , -32000 , 32000 , 32000 ) ;
	SetRectRgn( (RgnHandle) m_macCurrentClipRgn , -32000 , -32000 , 32000 , 32000 ) ;
    m_pen = *wxBLACK_PEN;
    m_font = *wxNORMAL_FONT;
    m_brush = *wxWHITE_BRUSH;
    m_macCurrentPortStateHelper = NULL ;
    m_macATSUIStyle = NULL ;
    m_macAliasWasEnabled = false;
    m_macForegroundPixMap = NULL ;
    m_macBackgroundPixMap = NULL ;
}
wxDC::~wxDC(void)
{
    DisposeRgn( (RgnHandle) m_macBoundaryClipRgn ) ;
    DisposeRgn( (RgnHandle) m_macCurrentClipRgn ) ;
}
void wxDC::MacSetupPort(wxMacPortStateHelper* help) const
{
    wxASSERT( m_macCurrentPortStateHelper == NULL ) ;
    m_macCurrentPortStateHelper = help ;
	SetClip( (RgnHandle) m_macCurrentClipRgn);
#if ! wxMAC_EXPERIMENTAL_DC
	m_macFontInstalled = false ;
	m_macBrushInstalled = false ;
	m_macPenInstalled = false ;
#endif
}
void wxDC::MacCleanupPort(wxMacPortStateHelper* help) const
{
    wxASSERT( m_macCurrentPortStateHelper == help ) ;
    m_macCurrentPortStateHelper = NULL ;
    if( m_macATSUIStyle )
    {
        ::ATSUDisposeStyle((ATSUStyle)m_macATSUIStyle);
        m_macATSUIStyle = NULL ;
    }
    if ( m_macAliasWasEnabled )
    {
        SetAntiAliasedTextEnabled(m_macFormerAliasState, m_macFormerAliasSize);
        m_macAliasWasEnabled = false ;
    }
    if ( m_macForegroundPixMap )
    {
        Pattern blackColor ;
        ::PenPat(GetQDGlobalsBlack(&blackColor));
        DisposePixPat( (PixPatHandle) m_macForegroundPixMap ) ;
        m_macForegroundPixMap = NULL ;
    }
    if ( m_macBackgroundPixMap )
    {
        Pattern whiteColor ;
        ::BackPat(GetQDGlobalsWhite(&whiteColor));
        DisposePixPat( (PixPatHandle) m_macBackgroundPixMap ) ;
        m_macBackgroundPixMap = NULL ;
    }
}
void wxDC::DoDrawBitmap( const wxBitmap &bmp, wxCoord x, wxCoord y, bool useMask )
{
     wxCHECK_RET( Ok(), wxT("invalid window dc") );
     wxCHECK_RET( bmp.Ok(), wxT("invalid bitmap") );
     wxMacPortSetter helper(this) ;
     wxCoord xx = XLOG2DEVMAC(x);
     wxCoord yy = YLOG2DEVMAC(y);
     wxCoord w = bmp.GetWidth();
     wxCoord h = bmp.GetHeight();
     wxCoord ww = XLOG2DEVREL(w);
     wxCoord hh = YLOG2DEVREL(h);
     // Set up drawing mode
     short  mode = (m_logicalFunction == wxCOPY ? srcCopy :
                    //m_logicalFunction == wxCLEAR ? WHITENESS :
                    //m_logicalFunction == wxSET ? BLACKNESS :
                    m_logicalFunction == wxINVERT ? hilite :
                   //m_logicalFunction == wxAND ? MERGECOPY :
                    m_logicalFunction == wxOR ? srcOr :
                    m_logicalFunction == wxSRC_INVERT ? notSrcCopy :
                    m_logicalFunction == wxXOR ? srcXor :
                    m_logicalFunction == wxOR_REVERSE ? notSrcOr :
                    //m_logicalFunction == wxAND_REVERSE ? SRCERASE :
                    //m_logicalFunction == wxSRC_OR ? srcOr :
                    //m_logicalFunction == wxSRC_AND ? SRCAND :
                    srcCopy );
     if ( bmp.GetBitmapType() == kMacBitmapTypePict ) {
         Rect bitmaprect = { 0 , 0 , hh, ww };
         ::OffsetRect( &bitmaprect, xx, yy ) ;
         ::DrawPicture( (PicHandle) bmp.GetPict(), &bitmaprect ) ;
    }
     else if ( bmp.GetBitmapType() == kMacBitmapTypeGrafWorld )
     {
         GWorldPtr    bmapworld = MAC_WXHBITMAP( bmp.GetHBITMAP() );
         PixMapHandle bmappixels ;
         // Set foreground and background colours (for bitmaps depth = 1)
         if(bmp.GetDepth() == 1)
        {
             RGBColor fore = MAC_WXCOLORREF(m_textForegroundColour.GetPixel());
             RGBColor back = MAC_WXCOLORREF(m_textBackgroundColour.GetPixel());
             RGBForeColor(&fore);
             RGBBackColor(&back);
         }
         else
         {
             RGBColor white = { 0xFFFF, 0xFFFF,0xFFFF} ;
             RGBColor black = { 0,0,0} ;
             RGBForeColor( &black ) ;
             RGBBackColor( &white ) ;
         }
         bmappixels = GetGWorldPixMap( bmapworld ) ;
         wxCHECK_RET(LockPixels(bmappixels),
                     wxT("DoDrawBitmap:  Unable to lock pixels"));
         Rect source = { 0, 0, h, w };
         Rect dest   = { yy, xx, yy + hh, xx + ww };
         if ( useMask && bmp.GetMask() )
         {
             if( LockPixels(GetGWorldPixMap(MAC_WXHBITMAP(bmp.GetMask()->GetMaskBitmap()))))
             {
                 CopyDeepMask
                     (
                      GetPortBitMapForCopyBits(bmapworld),
                      GetPortBitMapForCopyBits(MAC_WXHBITMAP(bmp.GetMask()->GetMaskBitmap())),
                      GetPortBitMapForCopyBits( MAC_WXHBITMAP(m_macPort) ),
                      &source, &source, &dest, mode, NULL
                      );
                 UnlockPixels(GetGWorldPixMap(MAC_WXHBITMAP(bmp.GetMask()->GetMaskBitmap())));
             }
         }
         else {
             CopyBits( GetPortBitMapForCopyBits( bmapworld ),
                       GetPortBitMapForCopyBits( MAC_WXHBITMAP(m_macPort) ),
                       &source, &dest, mode, NULL ) ;
         }
         UnlockPixels( bmappixels ) ;
     }
     else if ( bmp.GetBitmapType() == kMacBitmapTypeIcon )
     {
        Rect bitmaprect = { 0 , 0 , bmp.GetHeight(), bmp.GetWidth() } ;
        OffsetRect( &bitmaprect, xx, yy ) ;
        PlotCIconHandle( &bitmaprect , atNone , ttNone , MAC_WXHICON(bmp.GetHICON()) ) ;
     }
     m_macPenInstalled = false ;
     m_macBrushInstalled = false ;
     m_macFontInstalled = false ;
}
void wxDC::DoDrawIcon( const wxIcon &icon, wxCoord x, wxCoord y )
{
     wxCHECK_RET(Ok(), wxT("Invalid dc  wxDC::DoDrawIcon"));
     wxCHECK_RET(icon.Ok(), wxT("Invalid icon wxDC::DoDrawIcon"));
    DoDrawBitmap( icon , x , y , icon.GetMask() != NULL ) ;
}
void wxDC::DoSetClippingRegion( wxCoord x, wxCoord y, wxCoord width, wxCoord height )
{
    wxCHECK_RET(Ok(), wxT("wxDC::DoSetClippingRegion  Invalid DC"));
    wxCoord xx, yy, ww, hh;
    xx = XLOG2DEVMAC(x);
    yy = YLOG2DEVMAC(y);
    ww = XLOG2DEVREL(width);
    hh = YLOG2DEVREL(height);
    SetRectRgn( (RgnHandle) m_macCurrentClipRgn , xx , yy , xx + ww , yy + hh ) ;
    SectRgn( (RgnHandle) m_macCurrentClipRgn , (RgnHandle) m_macBoundaryClipRgn , (RgnHandle) m_macCurrentClipRgn ) ;
    if( m_clipping )
    {
        m_clipX1 = wxMax( m_clipX1 , xx );
        m_clipY1 = wxMax( m_clipY1 , yy );
        m_clipX2 = wxMin( m_clipX2, (xx + ww));
        m_clipY2 = wxMin( m_clipY2, (yy + hh));
    }
    else
    {
        m_clipping = TRUE;
        m_clipX1 = xx;
        m_clipY1 = yy;
        m_clipX2 = xx + ww;
        m_clipY2 = yy + hh;
    }
}
void wxDC::DoSetClippingRegionAsRegion( const wxRegion &region  )
{
    wxCHECK_RET( Ok(), wxT("invalid window dc") ) ;
    if (region.Empty())
    {
        DestroyClippingRegion();
        return;
    }
  	wxMacPortSetter helper(this) ;
    wxCoord x, y, w, h;
    region.GetBox( x, y, w, h );
    wxCoord xx, yy, ww, hh;
    xx = XLOG2DEVMAC(x);
    yy = YLOG2DEVMAC(y);
    ww = XLOG2DEVREL(w);
    hh = YLOG2DEVREL(h);
    // if we have a scaling that we cannot map onto native regions
    // we must use the box
    if ( ww != w || hh != h )
    {
        wxDC::DoSetClippingRegion( x, y, w, h );
    }
    else
    {
        CopyRgn( (RgnHandle) region.GetWXHRGN() , (RgnHandle) m_macCurrentClipRgn ) ;
        if ( xx != x || yy != y )
        {
            OffsetRgn( (RgnHandle) m_macCurrentClipRgn , xx - x , yy - y ) ;
        }
        SectRgn( (RgnHandle) m_macCurrentClipRgn , (RgnHandle) m_macBoundaryClipRgn , (RgnHandle) m_macCurrentClipRgn ) ;
        if( m_clipping )
        {
            m_clipX1 = wxMax( m_clipX1 , xx );
            m_clipY1 = wxMax( m_clipY1 , yy );
            m_clipX2 = wxMin( m_clipX2, (xx + ww));
            m_clipY2 = wxMin( m_clipY2, (yy + hh));
        }
        else
        {
            m_clipping = TRUE;
            m_clipX1 = xx;
            m_clipY1 = yy;
            m_clipX2 = xx + ww;
            m_clipY2 = yy + hh;
        }
    }
}
void wxDC::DestroyClippingRegion()
{
  wxMacPortSetter helper(this) ;
  CopyRgn( (RgnHandle) m_macBoundaryClipRgn , (RgnHandle) m_macCurrentClipRgn ) ;
  m_clipping = FALSE;
}
void wxDC::DoGetSize( int* width, int* height ) const
{
  *width = m_maxX-m_minX;
  *height = m_maxY-m_minY;
}
void wxDC::DoGetSizeMM( int* width, int* height ) const
{
  int w = 0;
  int h = 0;
  GetSize( &w, &h );
  *width = long( double(w) / (m_scaleX*m_mm_to_pix_x) );
  *height = long( double(h) / (m_scaleY*m_mm_to_pix_y) );
}
void wxDC::SetTextForeground( const wxColour &col )
{
    wxCHECK_RET(Ok(), wxT("Invalid DC"));
    m_textForegroundColour = col;
    m_macFontInstalled = false ;
}
void wxDC::SetTextBackground( const wxColour &col )
{
    wxCHECK_RET(Ok(), wxT("Invalid DC"));
    m_textBackgroundColour = col;
    m_macFontInstalled = false ;
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
  m_externalDeviceOriginX = x;
  m_externalDeviceOriginY = y;
  ComputeScaleAndOrigin();
}
#if 0
void wxDC::SetInternalDeviceOrigin( long x, long y )
{
  m_internalDeviceOriginX = x;
  m_internalDeviceOriginY = y;
  ComputeScaleAndOrigin();
}
void wxDC::GetInternalDeviceOrigin( long *x, long *y )
{
  if (x) *x = m_internalDeviceOriginX;
  if (y) *y = m_internalDeviceOriginY;
}
#endif
void wxDC::SetAxisOrientation( bool xLeftRight, bool yBottomUp )
{
  m_signX = (xLeftRight ?  1 : -1);
  m_signY = (yBottomUp  ? -1 :  1);
  ComputeScaleAndOrigin();
}
wxSize wxDC::GetPPI() const
{
    return wxSize(72, 72);
}
int wxDC::GetDepth() const
{
    if ( IsPortColor( (CGrafPtr) m_macPort ) )
    {
        return ( (**GetPortPixMap( (CGrafPtr) m_macPort)).pixelSize ) ;
    }
    return 1 ;
}
void wxDC::ComputeScaleAndOrigin()
{
  // CMB: copy scale to see if it changes
  double origScaleX = m_scaleX;
  double origScaleY = m_scaleY;
  m_scaleX = m_logicalScaleX * m_userScaleX;
  m_scaleY = m_logicalScaleY * m_userScaleY;
  m_deviceOriginX = m_internalDeviceOriginX + m_externalDeviceOriginX;
  m_deviceOriginY = m_internalDeviceOriginY + m_externalDeviceOriginY;
  // CMB: if scale has changed call SetPen to recalulate the line width
  if (m_scaleX != origScaleX || m_scaleY != origScaleY)
  {
    // this is a bit artificial, but we need to force wxDC to think
    // the pen has changed
    wxPen* pen = & GetPen();
    wxPen tempPen;
    m_pen = tempPen;
    SetPen(* pen);
  }
}
void  wxDC::SetPalette( const wxPalette& palette )
{
}
void  wxDC::SetBackgroundMode( int mode )
{
	m_backgroundMode = mode ;
}
void  wxDC::SetFont( const wxFont &font )
{
    m_font = font;
    m_macFontInstalled = false ;
}
void  wxDC::SetPen( const wxPen &pen )
{
	if ( m_pen == pen )
		return ;
  m_pen = pen;
  m_macPenInstalled = false ;
}
void  wxDC::SetBrush( const wxBrush &brush )
{
  if (m_brush == brush)
  	return;
  m_brush = brush;
  m_macBrushInstalled = false ;
}
void  wxDC::SetBackground( const wxBrush &brush )
{
  if (m_backgroundBrush == brush)
  	return;
  m_backgroundBrush = brush;
  if (!m_backgroundBrush.Ok())
  	return;
   m_macBrushInstalled = false ;
}
void  wxDC::SetLogicalFunction( int function )
{
 	if (m_logicalFunction == function)
 		return;
	m_logicalFunction = function ;
	m_macFontInstalled = false ;
	m_macBrushInstalled = false ;
	m_macPenInstalled = false ;
}
extern bool wxDoFloodFill(wxDC *dc, wxCoord x, wxCoord y,
                          const wxColour & col, int style);
bool wxDC::DoFloodFill(wxCoord x, wxCoord y,
                       const wxColour& col, int style)
{
    return wxDoFloodFill(this, x, y, col, style);
}
bool  wxDC::DoGetPixel( wxCoord x, wxCoord y, wxColour *col ) const
{
    wxCHECK_MSG( Ok(), false, wxT("wxDC::DoGetPixel  Invalid DC") );
    wxMacFastPortSetter helper(this) ;
    RGBColor colour;
    GetCPixel( XLOG2DEVMAC(x), YLOG2DEVMAC(y), &colour );
    // Convert from Mac colour to wx
    col->Set( colour.red   >> 8,
              colour.green >> 8,
              colour.blue  >> 8);
    return true ;
}
void  wxDC::DoDrawLine( wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2 )
{
  wxCHECK_RET(Ok(), wxT("Invalid DC"));
  wxMacFastPortSetter helper(this) ;
  if (m_pen.GetStyle() != wxTRANSPARENT)
  {
		MacInstallPen() ;
		wxCoord offset = ( (m_pen.GetWidth() == 0 ? 1 :
                            m_pen.GetWidth() ) * (wxCoord)m_scaleX - 1) / 2;
        wxCoord xx1 = XLOG2DEVMAC(x1) - offset;
        wxCoord yy1 = YLOG2DEVMAC(y1) - offset;
        wxCoord xx2 = XLOG2DEVMAC(x2) - offset;
        wxCoord yy2 = YLOG2DEVMAC(y2) - offset;
        if ((m_pen.GetCap() == wxCAP_ROUND) &&
            (m_pen.GetWidth() <= 1))
    	{
    	    // Implement LAST_NOT for MAC at least for
    	    // orthogonal lines. RR.
    	 	if (xx1 == xx2)
    	 	{
    	 	  	if (yy1 < yy2)
    	 	  		yy2--;
    	 	    if (yy1 > yy2)
    	 	    	yy2++;
    	 	}
    	 	if (yy1 == yy2)
    	 	{
    	 	  	if (xx1 < xx2)
    	 	  		xx2--;
    	 	    if (xx1 > xx2)
    	 	    	xx2++;
    	 	}
    	}
		::MoveTo(xx1, yy1);
		::LineTo(xx2, yy2);
  }
}
void  wxDC::DoCrossHair( wxCoord x, wxCoord y )
{
    wxCHECK_RET( Ok(), wxT("wxDC::DoCrossHair  Invalid window dc") );
    wxMacFastPortSetter helper(this) ;
    if (m_pen.GetStyle() != wxTRANSPARENT)
    {
        int w = 0;
        int h = 0;
        GetSize( &w, &h );
        wxCoord xx = XLOG2DEVMAC(x);
        wxCoord yy = YLOG2DEVMAC(y);
        MacInstallPen();
        ::MoveTo( XLOG2DEVMAC(0), yy );
        ::LineTo( XLOG2DEVMAC(w), yy );
        ::MoveTo( xx, YLOG2DEVMAC(0) );
        ::LineTo( xx, YLOG2DEVMAC(h) );
        CalcBoundingBox(x, y);
        CalcBoundingBox(x+w, y+h);
    }
}
/*
 * To draw arcs properly the angles need to be converted from the WX style:
 * Angles start on the +ve X axis and go anti-clockwise (As you would draw on
 * a normal axis on paper).
 * TO
 * the Mac style:
 * Angles start on the +ve y axis and go clockwise.
 */
static double wxConvertWXangleToMACangle(double angle)
{
    double newAngle = 90 - angle ;
    if ( newAngle < 0 )
        newAngle += 360 ;
    return newAngle ;
}
void  wxDC::DoDrawArc( wxCoord x1, wxCoord y1,
                           wxCoord x2, wxCoord y2,
                           wxCoord xc, wxCoord yc )
{
    wxCHECK_RET(Ok(), wxT("wxDC::DoDrawArc  Invalid DC"));
    wxMacFastPortSetter helper(this) ;
    wxCoord xx1 = XLOG2DEVMAC(x1);
    wxCoord yy1 = YLOG2DEVMAC(y1);
    wxCoord xx2 = XLOG2DEVMAC(x2);
    wxCoord yy2 = YLOG2DEVMAC(y2);
    wxCoord xxc = XLOG2DEVMAC(xc);
    wxCoord yyc = YLOG2DEVMAC(yc);
    double dx = xx1 - xxc;
    double dy = yy1 - yyc;
    double radius = sqrt((double)(dx*dx+dy*dy));
    wxCoord rad   = (wxCoord)radius;
    double radius1, radius2;
    if (xx1 == xx2 && yy1 == yy2)
    {
        radius1 = 0.0;
        radius2 = 360.0;
    }
    else if (radius == 0.0)
    {
        radius1 = radius2 = 0.0;
    }
    else
    {
        radius1 = (xx1 - xxc == 0) ?
            (yy1 - yyc < 0) ? 90.0 : -90.0 :
            -atan2(double(yy1-yyc), double(xx1-xxc)) * RAD2DEG;
         radius2 = (xx2 - xxc == 0) ?
            (yy2 - yyc < 0) ? 90.0 : -90.0 :
            -atan2(double(yy2-yyc), double(xx2-xxc)) * RAD2DEG;
     }
     wxCoord alpha2 = wxCoord(radius2 - radius1);
    wxCoord alpha1 = wxCoord(wxConvertWXangleToMACangle(radius1));
     if( (xx1 > xx2) || (yy1 > yy2) ) {
        alpha2 *= -1;
    }
    Rect r = { yyc - rad, xxc - rad, yyc + rad, xxc + rad };
    if(m_brush.GetStyle() != wxTRANSPARENT) {
        MacInstallBrush();
        PaintArc(&r, alpha1, alpha2);
    }
    if(m_pen.GetStyle() != wxTRANSPARENT) {
        MacInstallPen();
        FrameArc(&r, alpha1, alpha2);
    }
}
void  wxDC::DoDrawEllipticArc( wxCoord x, wxCoord y, wxCoord w, wxCoord h,
                                   double sa, double ea )
{
    wxCHECK_RET(Ok(), wxT("wxDC::DoDrawEllepticArc  Invalid DC"));
    wxMacFastPortSetter helper(this) ;
    Rect r;
    double angle = sa - ea;  // Order important Mac in opposite direction to wx
    // we have to make sure that the filling is always counter-clockwise
    if ( angle > 0 )
        angle -= 360 ;
    wxCoord xx = XLOG2DEVMAC(x);
    wxCoord yy = YLOG2DEVMAC(y);
    wxCoord ww = m_signX * XLOG2DEVREL(w);
    wxCoord hh = m_signY * YLOG2DEVREL(h);
    // handle -ve width and/or height
    if (ww < 0) { ww = -ww; xx = xx - ww; }
    if (hh < 0) { hh = -hh; yy = yy - hh; }
    sa = wxConvertWXangleToMACangle(sa);
    r.top    = yy;
    r.left   = xx;
    r.bottom = yy + hh;
    r.right  = xx + ww;
    if(m_brush.GetStyle() != wxTRANSPARENT) {
        MacInstallBrush();
        PaintArc(&r, (short)sa, (short)angle);
    }
    if(m_pen.GetStyle() != wxTRANSPARENT) {
        MacInstallPen();
        FrameArc(&r, (short)sa, (short)angle);
    }
}
void  wxDC::DoDrawPoint( wxCoord x, wxCoord y )
{
  wxCHECK_RET(Ok(), wxT("Invalid DC"));
  wxMacFastPortSetter helper(this) ;
  if (m_pen.GetStyle() != wxTRANSPARENT)
  {
        wxCoord xx1 = XLOG2DEVMAC(x);
        wxCoord yy1 = YLOG2DEVMAC(y);
        RGBColor pencolor = MAC_WXCOLORREF( m_pen.GetColour().GetPixel());
        ::SetCPixel( xx1,yy1,&pencolor) ;
        CalcBoundingBox(x, y);
  }
}
void  wxDC::DoDrawLines(int n, wxPoint points[],
                             wxCoord xoffset, wxCoord yoffset)
{
  wxCHECK_RET(Ok(), wxT("Invalid DC"));
  wxMacFastPortSetter helper(this) ;
  if (m_pen.GetStyle() == wxTRANSPARENT)
  	return;
  MacInstallPen() ;
  wxCoord offset = ( (m_pen.GetWidth() == 0 ? 1 :
                      m_pen.GetWidth() ) * (wxCoord)m_scaleX - 1) / 2 ;
  wxCoord x1, x2 , y1 , y2 ;
  x1 = XLOG2DEVMAC(points[0].x + xoffset);
  y1 = YLOG2DEVMAC(points[0].y + yoffset);
  ::MoveTo(x1 - offset, y1 - offset );
  for (int i = 0; i < n-1; i++)
  {
    x2 = XLOG2DEVMAC(points[i+1].x + xoffset);
    y2 = YLOG2DEVMAC(points[i+1].y + yoffset);
    ::LineTo( x2 - offset, y2 - offset );
  }
}
void  wxDC::DoDrawPolygon(int n, wxPoint points[],
                          wxCoord xoffset, wxCoord yoffset,
                          int fillStyle )
{
  	wxCHECK_RET(Ok(), wxT("Invalid DC"));
  	wxMacFastPortSetter helper(this) ;
  	wxCoord x1, x2 , y1 , y2 ;
    if ( m_brush.GetStyle() == wxTRANSPARENT && m_pen.GetStyle() == wxTRANSPARENT )
        return ;
	PolyHandle polygon = OpenPoly();
	x2 = x1 = XLOG2DEVMAC(points[0].x + xoffset);
	y2 = y1 = YLOG2DEVMAC(points[0].y + yoffset);
	::MoveTo(x1,y1);
	for (int i = 1; i < n; i++)
	{
		x2 = XLOG2DEVMAC(points[i].x + xoffset);
		y2 = YLOG2DEVMAC(points[i].y + yoffset);
		::LineTo(x2, y2);
	}
    // close the polyline if necessary
    if ( x1 != x2 || y1 != y2 )
    {
        ::LineTo(x1,y1 ) ;
    }
	ClosePoly();
  	if (m_brush.GetStyle() != wxTRANSPARENT)
  	{
		MacInstallBrush();
		::PaintPoly( polygon );
	}
	if (m_pen.GetStyle() != wxTRANSPARENT)
	{
		MacInstallPen() ;
		::FramePoly( polygon ) ;
	}
	KillPoly( polygon );
}
void wxDC::DoDrawRectangle(wxCoord x, wxCoord y, wxCoord width, wxCoord height)
{
    wxCHECK_RET(Ok(), wxT("Invalid DC"));
    wxMacFastPortSetter helper(this) ;
	wxCoord xx = XLOG2DEVMAC(x);
	wxCoord yy = YLOG2DEVMAC(y);
	wxCoord ww = m_signX * XLOG2DEVREL(width);
	wxCoord hh = m_signY * YLOG2DEVREL(height);
	// CMB: draw nothing if transformed w or h is 0
	if (ww == 0 || hh == 0)
		return;
	// CMB: handle -ve width and/or height
	if (ww < 0)
	{
		ww = -ww;
		xx = xx - ww;
	}
	if (hh < 0)
	{
		hh = -hh;
		yy = yy - hh;
	}
	Rect rect = { yy , xx , yy + hh , xx + ww } ;
	if (m_brush.GetStyle() != wxTRANSPARENT)
	{
		MacInstallBrush() ;
		::PaintRect( &rect ) ;
	}
	if (m_pen.GetStyle() != wxTRANSPARENT)
	{
		MacInstallPen() ;
		::FrameRect( &rect ) ;
	}
}
void  wxDC::DoDrawRoundedRectangle(wxCoord x, wxCoord y,
                                        wxCoord width, wxCoord height,
                                        double radius)
{
    wxCHECK_RET(Ok(), wxT("Invalid DC"));
    wxMacFastPortSetter helper(this) ;
    if (radius < 0.0)
  	    radius = - radius * ((width < height) ? width : height);
	wxCoord xx = XLOG2DEVMAC(x);
	wxCoord yy = YLOG2DEVMAC(y);
	wxCoord ww = m_signX * XLOG2DEVREL(width);
	wxCoord hh = m_signY * YLOG2DEVREL(height);
	// CMB: draw nothing if transformed w or h is 0
	if (ww == 0 || hh == 0)
		return;
	// CMB: handle -ve width and/or height
	if (ww < 0)
	{
		ww = -ww;
		xx = xx - ww;
	}
	if (hh < 0)
	{
		hh = -hh;
		yy = yy - hh;
	}
	Rect rect = { yy , xx , yy + hh , xx + ww } ;
	if (m_brush.GetStyle() != wxTRANSPARENT)
	{
		MacInstallBrush() ;
		::PaintRoundRect( &rect , int(radius * 2) , int(radius * 2) ) ;
	}
	if (m_pen.GetStyle() != wxTRANSPARENT)
	{
		MacInstallPen() ;
		::FrameRoundRect( &rect , int(radius * 2) , int(radius * 2) ) ;
	}
}
void  wxDC::DoDrawEllipse(wxCoord x, wxCoord y, wxCoord width, wxCoord height)
{
    wxCHECK_RET(Ok(), wxT("Invalid DC"));
    wxMacFastPortSetter helper(this) ;
	wxCoord xx = XLOG2DEVMAC(x);
	wxCoord yy = YLOG2DEVMAC(y);
	wxCoord ww = m_signX * XLOG2DEVREL(width);
	wxCoord hh = m_signY * YLOG2DEVREL(height);
	// CMB: draw nothing if transformed w or h is 0
	if (ww == 0 || hh == 0)
		return;
	// CMB: handle -ve width and/or height
	if (ww < 0)
	{
		ww = -ww;
		xx = xx - ww;
	}
	if (hh < 0)
	{
		hh = -hh;
		yy = yy - hh;
	}
	Rect rect = { yy , xx , yy + hh , xx + ww } ;
	if (m_brush.GetStyle() != wxTRANSPARENT)
	{
		MacInstallBrush() ;
		::PaintOval( &rect ) ;
	}
	if (m_pen.GetStyle() != wxTRANSPARENT)
	{
		MacInstallPen() ;
		::FrameOval( &rect ) ;
	}
}


bool  wxDC::CanDrawBitmap(void) const
{
	return true ;
}

bool  wxDC::DoBlit(wxCoord xdest, wxCoord ydest, wxCoord width, wxCoord height,
                        wxDC *source, wxCoord xsrc, wxCoord ysrc, int logical_func , bool useMask,
                        wxCoord xsrcMask,  wxCoord ysrcMask )
{
    wxCHECK_MSG(Ok(), false, wxT("wxDC::DoBlit Illegal dc"));
    wxCHECK_MSG(source->Ok(), false, wxT("wxDC::DoBlit  Illegal source DC"));
    if ( logical_func == wxNO_OP )
        return TRUE ;
    if (xsrcMask == -1 && ysrcMask == -1)
    {
        xsrcMask = xsrc; ysrcMask = ysrc;
    }
    // correct the parameter in case this dc does not have a mask at all
    if ( useMask && !source->m_macMask )
        useMask = false ;
	Rect srcrect , dstrect ;
	srcrect.top = source->YLOG2DEVMAC(ysrc) ;
	srcrect.left = source->XLOG2DEVMAC(xsrc)  ;
	srcrect.right = source->XLOG2DEVMAC(xsrc + width ) ;
	srcrect.bottom = source->YLOG2DEVMAC(ysrc + height) ;
	dstrect.top = YLOG2DEVMAC(ydest) ;
	dstrect.left = XLOG2DEVMAC(xdest) ;
	dstrect.bottom = YLOG2DEVMAC(ydest + height )  ;
	dstrect.right = XLOG2DEVMAC(xdest + width ) ;
    short mode = kUnsupportedMode ;
    bool invertDestinationFirst = false ;
    switch ( logical_func )
    {
		case wxAND:        // src AND dst
		    mode = srcOr ; // ok
		    break ;
		case wxAND_INVERT: // (NOT src) AND dst
		    mode = notSrcOr  ; // ok
		    break ;
		case wxAND_REVERSE:// src AND (NOT dst)
		    invertDestinationFirst = true ;
		    mode = srcOr ;
		    break ;
		case wxCLEAR:      // 0
		    mode = kEmulatedMode ;
		    break ;
        case wxCOPY:       // src
			mode = srcCopy ; // ok
			break ;
		case wxEQUIV:      // (NOT src) XOR dst
		    mode = srcXor ; // ok
		    break ;
		case wxINVERT:     // NOT dst
			mode = kEmulatedMode ; //or hilite ;
			break ;
		case wxNAND:       // (NOT src) OR (NOT dst)
		    invertDestinationFirst = true ;
		    mode = srcBic ;
		    break ;
		case wxNOR:        // (NOT src) AND (NOT dst)
		    invertDestinationFirst = true ;
		    mode = notSrcOr ;
		    break ;
		case wxNO_OP:      // dst
		    mode = kEmulatedMode ; // this has already been handled upon entry
		    break ;
		case wxOR:         // src OR dst
		    mode = notSrcBic ;
		    break ;
		case wxOR_INVERT:  // (NOT src) OR dst
		    mode = srcBic ;
		    break ;
		case wxOR_REVERSE: // src OR (NOT dst)
		    invertDestinationFirst = true ;
			mode = notSrcBic ;
			break ;
		case wxSET:        // 1
		    mode = kEmulatedMode ;
		    break ;
		case wxSRC_INVERT: // (NOT src)
			mode = notSrcCopy ; // ok
			break ;
		case wxXOR:        // src XOR dst
			mode = notSrcXor ; // ok
			break ;
        default :
			break ;
    }
    if ( mode == kUnsupportedMode )
    {
        wxFAIL_MSG("unsupported blitting mode" );
        return FALSE ;
    }
	CGrafPtr			sourcePort = (CGrafPtr) source->m_macPort ;
	PixMapHandle	bmappixels =  GetGWorldPixMap( sourcePort ) ;
	if ( LockPixels(bmappixels) )
	{
        wxMacPortSetter helper(this) ;
    	if ( source->GetDepth() == 1 )
    	{
    	    RGBForeColor( &MAC_WXCOLORREF(m_textForegroundColour.GetPixel()) ) ;
    	    RGBBackColor( &MAC_WXCOLORREF(m_textBackgroundColour.GetPixel()) ) ;
        }
        else
        {
            // the modes need this, otherwise we'll end up having really nice colors...
    	    RGBColor	white = { 0xFFFF, 0xFFFF,0xFFFF} ;
    	    RGBColor	black = { 0,0,0} ;
    	    RGBForeColor( &black ) ;
    	    RGBBackColor( &white ) ;
        }
		if ( useMask && source->m_macMask )
		{
		    if ( mode == srcCopy )
		    {
    			if ( LockPixels( GetGWorldPixMap( MAC_WXHBITMAP(source->m_macMask) ) ) )
    			{
    				CopyMask( GetPortBitMapForCopyBits( sourcePort ) ,
    				            GetPortBitMapForCopyBits( MAC_WXHBITMAP(source->m_macMask) ) ,
    				            GetPortBitMapForCopyBits( MAC_WXHBITMAP(m_macPort) ) ,
    					        &srcrect, &srcrect , &dstrect ) ;
    				UnlockPixels( GetGWorldPixMap( MAC_WXHBITMAP(source->m_macMask) )  ) ;
    			}
		    }
		    else
		    {
                RgnHandle clipRgn = NewRgn() ;
                LockPixels( GetGWorldPixMap( MAC_WXHBITMAP(source->m_macMask) ) ) ;
                BitMapToRegion( clipRgn , (BitMap*) *GetGWorldPixMap( MAC_WXHBITMAP(source->m_macMask) ) ) ;
                UnlockPixels( GetGWorldPixMap( MAC_WXHBITMAP(source->m_macMask) ) ) ;
                OffsetRgn( clipRgn , -srcrect.left + dstrect.left, -srcrect.top +  dstrect.top ) ;
                if ( mode == kEmulatedMode )
                {
                    Pattern pat ;
		            ::PenPat(GetQDGlobalsBlack(&pat));
                    if ( logical_func == wxSET )
                    {
                        RGBColor col= { 0xFFFF, 0xFFFF, 0xFFFF } ;
		                ::RGBForeColor( &col  ) ;
                        ::PaintRgn( clipRgn ) ;
                    }
                    else if ( logical_func == wxCLEAR )
                    {
                        RGBColor col= { 0x0000, 0x0000, 0x0000 } ;
		                ::RGBForeColor( &col  ) ;
                        ::PaintRgn( clipRgn ) ;
                    }
                    else if ( logical_func == wxINVERT )
                    {
                        MacInvertRgn( clipRgn ) ;
                    }
                    else
                    {
                        for ( int y = 0 ; y < srcrect.right - srcrect.left ; ++y )
                        {
                            for ( int x = 0 ; x < srcrect.bottom - srcrect.top ; ++x )
                            {
                                Point dstPoint = { dstrect.top + y , dstrect.left + x } ;
                                Point srcPoint = { srcrect.top + y , srcrect.left + x } ;
                                if ( PtInRgn( dstPoint , clipRgn ) )
                                {
                                    RGBColor srcColor ;
                                    RGBColor dstColor ;
                                    SetPort( (GrafPtr) sourcePort ) ;
                                    GetCPixel(  srcPoint.h , srcPoint.v , &srcColor) ;
                                    SetPort( (GrafPtr) m_macPort ) ;
                                    GetCPixel( dstPoint.h , dstPoint.v , &dstColor ) ;
                                    wxMacCalculateColour( logical_func , srcColor ,  dstColor ) ;
                                    SetCPixel( dstPoint.h , dstPoint.v , &dstColor ) ;
                                }
                            }
                        }
                    }
                }
                else
                {
                    if ( invertDestinationFirst )
                    {
                        MacInvertRgn( clipRgn ) ;
                    }
        			CopyBits( GetPortBitMapForCopyBits( sourcePort ) ,
        			    GetPortBitMapForCopyBits( MAC_WXHBITMAP(m_macPort) ) ,
        				&srcrect, &dstrect, mode, clipRgn ) ;
        	    }
    		    DisposeRgn( clipRgn ) ;
    		}
		}
		else
		{
            RgnHandle clipRgn = NewRgn() ;
            SetRectRgn( clipRgn , dstrect.left , dstrect.top , dstrect.right , dstrect.bottom ) ;
		    if ( mode == kEmulatedMode )
		    {
                Pattern pat ;
	            ::PenPat(GetQDGlobalsBlack(&pat));
                if ( logical_func == wxSET )
                {
                    RGBColor col= { 0xFFFF, 0xFFFF, 0xFFFF } ;
	                ::RGBForeColor( &col  ) ;
                    ::PaintRgn( clipRgn ) ;
                }
                else if ( logical_func == wxCLEAR )
                {
                    RGBColor col= { 0x0000, 0x0000, 0x0000 } ;
	                ::RGBForeColor( &col  ) ;
                    ::PaintRgn( clipRgn ) ;
                }
                else if ( logical_func == wxINVERT )
                {
                    MacInvertRgn( clipRgn ) ;
                }
                else
                {
                    for ( int y = 0 ; y < srcrect.right - srcrect.left ; ++y )
                    {
                        for ( int x = 0 ; x < srcrect.bottom - srcrect.top ; ++x )
                        {
                            Point dstPoint = { dstrect.top + y , dstrect.left + x } ;
                            Point srcPoint = { srcrect.top + y , srcrect.left + x } ;
                            {
                                RGBColor srcColor ;
                                RGBColor dstColor ;
                                SetPort( (GrafPtr) sourcePort ) ;
                                GetCPixel(  srcPoint.h , srcPoint.v , &srcColor) ;
                                SetPort( (GrafPtr) m_macPort ) ;
                                GetCPixel( dstPoint.h , dstPoint.v , &dstColor ) ;
                                wxMacCalculateColour( logical_func , srcColor ,  dstColor ) ;
                                SetCPixel( dstPoint.h , dstPoint.v , &dstColor ) ;
                            }
                        }
                    }
                }
    		}
    		else
    		{
                if ( invertDestinationFirst )
                {
                    MacInvertRgn( clipRgn ) ;
                }
    			CopyBits( GetPortBitMapForCopyBits( sourcePort ) ,
    			    GetPortBitMapForCopyBits( MAC_WXHBITMAP(m_macPort) ) ,
    				&srcrect, &dstrect, mode, NULL ) ;
    		}
    		DisposeRgn( clipRgn ) ;
		}
		UnlockPixels( bmappixels ) ;
	}
	m_macPenInstalled = false ;
	m_macBrushInstalled = false ;
	m_macFontInstalled = false ;
  return TRUE;
}
inline Fixed	IntToFixed( int inInt )
	{
		return (((SInt32) inInt) << 16);
	}
inline int	FixedToInt( Fixed inFixed )
	{
		return (((SInt32) inFixed) >> 16);
	}

void  wxDC::DoDrawRotatedText(const wxString& str, wxCoord x, wxCoord y,
                              double angle)
{
    wxCHECK_RET( Ok(), wxT("wxDC::DoDrawRotatedText  Invalid window dc") );
    if (angle == 0.0 )
    {
        DrawText(str, x, y);
        return;
    }
    if ( str.Length() == 0 )
        return ;
    wxMacFastPortSetter helper(this) ;
    MacInstallFont() ;
	wxString text ;
	if ( wxApp::s_macDefaultEncodingIsPC )
	{
		text = wxMacMakeMacStringFromPC( str ) ;
	}
	else
	{
		text = str ;
	}
	wxFontRefData * font = (wxFontRefData*) m_font.GetRefData() ;
    if ( 0 )
    {
        m_macFormerAliasState = IsAntiAliasedTextEnabled(&m_macFormerAliasSize);
        SetAntiAliasedTextEnabled(true, SInt16(m_scaleY * font->m_macFontSize));
        m_macAliasWasEnabled = true ;
    }
    OSStatus status = noErr ;
	TECObjectRef ec;
	status = TECCreateConverter(&ec, kTextEncodingMacRoman, kTextEncodingUnicodeDefault);
	wxASSERT_MSG( status == noErr , "couldn't start converter" ) ;
    ByteCount byteOutLen ;
    ByteCount byteInLen = text.Length() ;
    ByteCount byteBufferLen = byteInLen *2 ;
    char* buf = new char[byteBufferLen] ;
	status = TECConvertText(ec, (ConstTextPtr)text.c_str() , byteInLen, &byteInLen,
	    (TextPtr)buf, byteBufferLen, &byteOutLen);
	wxASSERT_MSG( status == noErr , "couldn't convert text" ) ;
	status = TECDisposeConverter(ec);
	wxASSERT_MSG( status == noErr , "couldn't dispose converter" ) ;
	ATSUTextLayout atsuLayout ;
	UniCharCount chars = byteOutLen / 2 ;
    status = ::ATSUCreateTextLayoutWithTextPtr( (UniCharArrayPtr) buf , 0 , byteOutLen / 2 , byteOutLen / 2 , 1 ,
        &chars , (ATSUStyle*) &m_macATSUIStyle , &atsuLayout ) ;
	wxASSERT_MSG( status == noErr , "couldn't create the layout of the rotated text" );
    int iAngle = int( angle );
    int drawX = XLOG2DEVMAC(x) ;
    int drawY = YLOG2DEVMAC(y) ;

    ATSUTextMeasurement textBefore ;
    ATSUTextMeasurement textAfter ;
    ATSUTextMeasurement ascent ;
    ATSUTextMeasurement descent ;


    if ( abs(iAngle) > 0 )
    {
        Fixed atsuAngle = IntToFixed( iAngle ) ;
		ATSUAttributeTag atsuTags[] =
		{
		    kATSULineRotationTag ,
		} ;
	    ByteCount atsuSizes[sizeof(atsuTags)/sizeof(ATSUAttributeTag)] =
	    {
	        sizeof( Fixed ) ,
	    } ;
		ATSUAttributeValuePtr	atsuValues[sizeof(atsuTags)/sizeof(ATSUAttributeTag)] =
		{
		    &atsuAngle ,
		} ;
    	status = ::ATSUSetLayoutControls(atsuLayout , sizeof(atsuTags)/sizeof(ATSUAttributeTag),
		    atsuTags, atsuSizes, atsuValues ) ;
    }
	status = ::ATSUMeasureText( atsuLayout, kATSUFromTextBeginning, kATSUToTextEnd,
					&textBefore , &textAfter, &ascent , &descent );

    drawX += (int)( sin(angle/RAD2DEG) * FixedToInt(ascent) ) ;
    drawY += (int)( cos(angle/RAD2DEG) * FixedToInt(ascent) ) ;
    status = ::ATSUDrawText( atsuLayout, kATSUFromTextBeginning, kATSUToTextEnd,
				IntToFixed(drawX) , IntToFixed(drawY) );
	wxASSERT_MSG( status == noErr , "couldn't draw the rotated text" );
    Rect rect ;
	status = ::ATSUMeasureTextImage( atsuLayout, kATSUFromTextBeginning, kATSUToTextEnd,
					IntToFixed(drawX) , IntToFixed(drawY) , &rect );
	wxASSERT_MSG( status == noErr , "couldn't measure the rotated text" );
    OffsetRect( &rect , -m_macLocalOrigin.x , -m_macLocalOrigin.y ) ;
    CalcBoundingBox(XDEV2LOG(rect.left), YDEV2LOG(rect.top) );
    CalcBoundingBox(XDEV2LOG(rect.right), YDEV2LOG(rect.bottom) );
    ::ATSUDisposeTextLayout(atsuLayout);
    delete[] buf ;
}
void  wxDC::DoDrawText(const wxString& strtext, wxCoord x, wxCoord y)
{
    wxCHECK_RET(Ok(), wxT("wxDC::DoDrawText  Invalid DC"));
    wxMacFastPortSetter helper(this) ;
	long xx = XLOG2DEVMAC(x);
	long yy = YLOG2DEVMAC(y);
#if TARGET_CARBON
	bool useDrawThemeText = ( DrawThemeTextBox != (void*) kUnresolvedCFragSymbolAddress ) ;
	if ( IsKindOf(CLASSINFO( wxPrinterDC ) ) ||  m_font.GetNoAntiAliasing() )
	    useDrawThemeText = false ;
#endif
	MacInstallFont() ;
    if ( 0 )
    {
        m_macFormerAliasState = IsAntiAliasedTextEnabled(&m_macFormerAliasSize);
        SetAntiAliasedTextEnabled(true, 8);
        m_macAliasWasEnabled = true ;
    }
	FontInfo fi ;
	::GetFontInfo( &fi ) ;
#if TARGET_CARBON
	if ( !useDrawThemeText )
#endif
	    yy += fi.ascent ;
	::MoveTo( xx , yy );
	if (  m_backgroundMode == wxTRANSPARENT )
	{
		::TextMode( srcOr) ;
	}
	else
	{
		::TextMode( srcCopy ) ;
	}
	const char *text = NULL ;
	int length = 0 ;
	wxString macText ;
	if ( wxApp::s_macDefaultEncodingIsPC )
	{
		macText = wxMacMakeMacStringFromPC( strtext ) ;
		text = macText ;
		length = macText.Length() ;
	}
	else
	{
		text = strtext ;
		length = strtext.Length() ;
	}
	int laststop = 0 ;
	int i = 0 ;
	int line = 0 ;
    {
    	while( i < length )
    	{
    		if( text[i] == 13 || text[i] == 10)
    		{
#if TARGET_CARBON
            	if ( useDrawThemeText )
            	{
    	            Rect frame = { yy + line*(fi.descent + fi.ascent + fi.leading)  ,xx , yy + (line+1)*(fi.descent + fi.ascent + fi.leading) , xx + 10000 } ;
                    CFStringRef mString = CFStringCreateWithBytes( NULL , (UInt8*) text + laststop , i - laststop , CFStringGetSystemEncoding(), false ) ;
		            if ( m_backgroundMode != wxTRANSPARENT )
		            {
			            Point bounds={0,0} ;
			            Rect background = frame ;
			            SInt16 baseline ;
			    		::GetThemeTextDimensions( mString,
			    							kThemeCurrentPortFont,
			    							kThemeStateActive,
			    							false,
			    							&bounds,
			    							&baseline );
			    		background.right = background.left + bounds.h ;
			    		background.bottom = background.top + bounds.v ;
			    		::EraseRect( &background ) ;
		            }
            		::DrawThemeTextBox( mString,
            							kThemeCurrentPortFont,
            							kThemeStateActive,
            							false,
            							&frame,
            							teJustLeft,
            							nil );
            	    CFRelease( mString ) ;
            	    line++ ;
                }
    		    else
#endif
    		    {
        			::DrawText( text , laststop , i - laststop ) ;
            	    line++ ;
        			::MoveTo( xx , yy + line*(fi.descent + fi.ascent + fi.leading) );
    			}
        		laststop = i+1 ;
    		}
    		i++ ;
    	}
#if TARGET_CARBON
        if ( useDrawThemeText )
    	{
    	    Rect frame = { yy + line*(fi.descent + fi.ascent + fi.leading)  ,xx , yy + (line+1)*(fi.descent + fi.ascent + fi.leading) , xx + 10000 } ;
            CFStringRef mString = CFStringCreateWithCString( NULL , text + laststop , kCFStringEncodingMacRoman ) ;
            if ( m_backgroundMode != wxTRANSPARENT )
            {
	            Point bounds={0,0} ;
	            Rect background = frame ;
	            SInt16 baseline ;
	    		::GetThemeTextDimensions( mString,
	    							kThemeCurrentPortFont,
	    							kThemeStateActive,
	    							false,
	    							&bounds,
	    							&baseline );
	    		background.right = background.left + bounds.h ;
	    		background.bottom = background.top + bounds.v ;
	    		::EraseRect( &background ) ;
            }
    		::DrawThemeTextBox( mString,
    							kThemeCurrentPortFont,
    							kThemeStateActive,
    							false,
    							&frame,
    							teJustLeft,
    							nil );
            CFRelease( mString ) ;
        }
        else
#endif
        {
    	    ::DrawText( text , laststop , i - laststop ) ;
        }
	}
	::TextMode( srcOr ) ;
}
bool  wxDC::CanGetTextExtent() const
{
	wxCHECK_MSG(Ok(), false, wxT("Invalid DC"));
	return true ;
}
void  wxDC::DoGetTextExtent( const wxString &string, wxCoord *width, wxCoord *height,
                     wxCoord *descent, wxCoord *externalLeading ,
                     wxFont *theFont ) const
{
   wxCHECK_RET(Ok(), wxT("Invalid DC"));
   wxMacFastPortSetter helper(this) ;
 	wxFont formerFont = m_font ;
	if ( theFont )
	{
	    // work around the constness
       *((wxFont*)(&m_font)) = *theFont ;
	}
	MacInstallFont() ;
	FontInfo fi ;
	::GetFontInfo( &fi ) ;
#if TARGET_CARBON
	bool useGetThemeText = ( GetThemeTextDimensions != (void*) kUnresolvedCFragSymbolAddress ) ;
	if ( IsKindOf(CLASSINFO( wxPrinterDC ) ) || ((wxFont*)&m_font)->GetNoAntiAliasing() )
	    useGetThemeText = false ;
#endif
	if ( height )
		*height = YDEV2LOGREL( fi.descent + fi.ascent ) ;
	if ( descent )
		*descent =YDEV2LOGREL( fi.descent );
	if ( externalLeading )
		*externalLeading = YDEV2LOGREL( fi.leading ) ;
	const char *text = NULL ;
	int length = 0 ;
	wxString macText ;
	if ( wxApp::s_macDefaultEncodingIsPC )
	{
		macText = wxMacMakeMacStringFromPC( string ) ;
		text = macText ;
		length = macText.Length() ;
	}
	else
	{
		text = string ;
		length = string.Length() ;
	}
	int laststop = 0 ;
	int i = 0 ;
	int curwidth = 0 ;
	if ( width )
	{
		*width = 0 ;
		while( i < length )
		{
			if( text[i] == 13 || text[i] == 10)
			{
				if ( height )
					*height += YDEV2LOGREL( fi.descent + fi.ascent + fi.leading ) ;
#if TARGET_CARBON
                if ( useGetThemeText )
                {
                    Point bounds={0,0} ;
                    SInt16 baseline ;
                    CFStringRef mString = CFStringCreateWithBytes( NULL , (UInt8*) text + laststop , i - laststop , CFStringGetSystemEncoding(), false ) ;
            		::GetThemeTextDimensions( mString,
            							kThemeCurrentPortFont,
            							kThemeStateActive,
            							false,
            							&bounds,
            							&baseline );
            	    CFRelease( mString ) ;
            	    curwidth = bounds.h ;
                }
                else
#endif
                {
				    curwidth = ::TextWidth( text , laststop , i - laststop ) ;
				}
				if ( curwidth > *width )
					*width = XDEV2LOGREL( curwidth ) ;
				laststop = i+1 ;
			}
			i++ ;
		}

#if TARGET_CARBON
        if ( useGetThemeText )
        {
            Point bounds={0,0} ;
            SInt16 baseline ;
            CFStringRef mString = CFStringCreateWithBytes( NULL , (UInt8*) text + laststop , i - laststop , CFStringGetSystemEncoding(), false ) ;
    		::GetThemeTextDimensions( mString,
    							kThemeCurrentPortFont,
    							kThemeStateActive,
    							false,
    							&bounds,
    							&baseline );
    	    CFRelease( mString ) ;
    	    curwidth = bounds.h ;
        }
        else
#endif
        {
		    curwidth = ::TextWidth( text , laststop , i - laststop ) ;
		}
		if ( curwidth > *width )
			*width = XDEV2LOGREL( curwidth ) ;
	}
	if ( theFont )
	{
	    // work around the constness
       *((wxFont*)(&m_font)) = formerFont ;
		m_macFontInstalled = false ;
	}
}
wxCoord   wxDC::GetCharWidth(void) const
{
    wxCHECK_MSG(Ok(), 1, wxT("Invalid DC"));
   wxMacFastPortSetter helper(this) ;
	MacInstallFont() ;
    int width = 0 ;
#if TARGET_CARBON
	bool useGetThemeText = ( GetThemeTextDimensions != (void*) kUnresolvedCFragSymbolAddress ) ;
	if ( ((wxFont*)&m_font)->GetNoAntiAliasing() )
		useGetThemeText = false ;
#endif
	char text[] = "H" ;
#if TARGET_CARBON
    if ( useGetThemeText )
    {
        Point bounds={0,0} ;
        SInt16 baseline ;
        CFStringRef mString = CFStringCreateWithBytes( NULL , (UInt8*) text , 1 , CFStringGetSystemEncoding(), false ) ;
		::GetThemeTextDimensions( mString,
							kThemeCurrentPortFont,
							kThemeStateActive,
							false,
							&bounds,
							&baseline );
	    CFRelease( mString ) ;
	    width = bounds.h ;
    }
    else
#endif
    {
	    width = ::TextWidth( text , 0 , 1 ) ;
	}
	return YDEV2LOGREL(width) ;
}
wxCoord   wxDC::GetCharHeight(void) const
{
    wxCHECK_MSG(Ok(), 1, wxT("Invalid DC"));
    wxMacFastPortSetter helper(this) ;
	MacInstallFont() ;
	FontInfo fi ;
	::GetFontInfo( &fi ) ;
	return YDEV2LOGREL( fi.descent + fi.ascent );
}
void  wxDC::Clear(void)
{
    wxCHECK_RET(Ok(), wxT("Invalid DC"));
    wxMacFastPortSetter helper(this) ;
	Rect rect = { -31000 , -31000 , 31000 , 31000 } ;
	if (m_backgroundBrush.GetStyle() != wxTRANSPARENT)
	{
	    ::PenNormal() ;
	    //MacInstallBrush() ;
        MacSetupBackgroundForCurrentPort( m_backgroundBrush ) ;
		::EraseRect( &rect ) ;
	}
}
void wxDC::MacInstallFont() const
{
    wxCHECK_RET(Ok(), wxT("Invalid DC"));
    if ( m_macFontInstalled )
        return ;
        
	Pattern blackColor ;
	MacSetupBackgroundForCurrentPort(m_backgroundBrush) ;
	wxFontRefData * font = (wxFontRefData*) m_font.GetRefData() ;
	if ( font )
	{
		::TextFont( font->m_macFontNum ) ;
		::TextSize( short(m_scaleY * font->m_macFontSize) ) ;
		::TextFace( font->m_macFontStyle ) ;
		m_macFontInstalled = true ;
		m_macBrushInstalled = false ;
		m_macPenInstalled = false ;
		RGBColor forecolor = MAC_WXCOLORREF( m_textForegroundColour.GetPixel());
		RGBColor backcolor = MAC_WXCOLORREF( m_textBackgroundColour.GetPixel());
		::RGBForeColor( &forecolor );
		::RGBBackColor( &backcolor );
	}
	else
	{
        FontFamilyID fontId ;
    	Str255 fontName ;
    	SInt16 fontSize ;
    	Style fontStyle ;
    	GetThemeFont(kThemeSmallSystemFont , GetApplicationScript() , fontName , &fontSize , &fontStyle ) ;
        GetFNum( fontName, &fontId );
		::TextFont( fontId ) ;
		::TextSize( short(m_scaleY * fontSize) ) ;
		::TextFace( fontStyle ) ;
		// todo reset after spacing changes - or store the current spacing somewhere
		m_macFontInstalled = true ;
		m_macBrushInstalled = false ;
		m_macPenInstalled = false ;
		RGBColor forecolor = MAC_WXCOLORREF( m_textForegroundColour.GetPixel());
		RGBColor backcolor = MAC_WXCOLORREF( m_textBackgroundColour.GetPixel());
		::RGBForeColor( &forecolor );
		::RGBBackColor( &backcolor );
	}
	short mode = patCopy ;
	// todo :
	switch( m_logicalFunction )
	{
		case wxCOPY:       // src
			mode = patCopy ;
			break ;
		case wxINVERT:     // NOT dst
			::PenPat(GetQDGlobalsBlack(&blackColor));
			mode = patXor ;
			break ;
		case wxXOR:        // src XOR dst
			mode = patXor ;
			break ;
		case wxOR_REVERSE: // src OR (NOT dst)
			mode = notPatOr ;
			break ;
		case wxSRC_INVERT: // (NOT src)
			mode = notPatCopy ;
			break ;
		// unsupported TODO
		case wxCLEAR:      // 0
		case wxAND_REVERSE:// src AND (NOT dst)
		case wxAND:        // src AND dst
		case wxAND_INVERT: // (NOT src) AND dst
		case wxNO_OP:      // dst
		case wxNOR:        // (NOT src) AND (NOT dst)
		case wxEQUIV:      // (NOT src) XOR dst
		case wxOR_INVERT:  // (NOT src) OR dst
		case wxNAND:       // (NOT src) OR (NOT dst)
		case wxOR:         // src OR dst
		case wxSET:        // 1
//		case wxSRC_OR:     // source _bitmap_ OR destination
//		case wxSRC_AND:     // source _bitmap_ AND destination
			break ;
	}
	::PenMode( mode ) ;

    OSStatus status = noErr ;
    Fixed atsuSize = IntToFixed( int(m_scaleY * font->m_macFontSize) ) ;
    Style qdStyle = font->m_macFontStyle ;
	ATSUFontID	atsuFont = font->m_macATSUFontID ;
    status = ::ATSUCreateStyle((ATSUStyle *)&m_macATSUIStyle) ;
	wxASSERT_MSG( status == noErr , "couldn't create ATSU style" ) ;
	ATSUAttributeTag atsuTags[] =
	{
	    kATSUFontTag ,
	    kATSUSizeTag ,
//	    kATSUColorTag ,
//	    kATSUBaselineClassTag ,
	    kATSUVerticalCharacterTag,
	    kATSUQDBoldfaceTag ,
	    kATSUQDItalicTag ,
	    kATSUQDUnderlineTag ,
	    kATSUQDCondensedTag ,
	    kATSUQDExtendedTag ,
	} ;
    ByteCount atsuSizes[sizeof(atsuTags)/sizeof(ATSUAttributeTag)] =
    {
        sizeof( ATSUFontID ) ,
        sizeof( Fixed ) ,
//        sizeof( RGBColor ) ,
//        sizeof( BslnBaselineClass ) ,
        sizeof( ATSUVerticalCharacterType),
        sizeof( Boolean ) ,
        sizeof( Boolean ) ,
        sizeof( Boolean ) ,
        sizeof( Boolean ) ,
        sizeof( Boolean ) ,
    } ;
    Boolean kTrue = true ;
    Boolean kFalse = false ;
//    BslnBaselineClass kBaselineDefault = kBSLNHangingBaseline ;
    ATSUVerticalCharacterType kHorizontal = kATSUStronglyHorizontal;
	ATSUAttributeValuePtr	atsuValues[sizeof(atsuTags)/sizeof(ATSUAttributeTag)] =
	{
	    &atsuFont ,
	    &atsuSize ,
//	    &MAC_WXCOLORREF( m_textForegroundColour.GetPixel() ) ,
//	    &kBaselineDefault ,
	    &kHorizontal,
	    (qdStyle & bold) ? &kTrue : &kFalse ,
	    (qdStyle & italic) ? &kTrue : &kFalse ,
	    (qdStyle & underline) ? &kTrue : &kFalse ,
	    (qdStyle & condense) ? &kTrue : &kFalse ,
	    (qdStyle & extend) ? &kTrue : &kFalse ,
	} ;
	status = ::ATSUSetAttributes((ATSUStyle)m_macATSUIStyle, sizeof(atsuTags)/sizeof(ATSUAttributeTag),
	    atsuTags, atsuSizes, atsuValues);
	wxASSERT_MSG( status == noErr , "couldn't set create ATSU style" ) ;
}
Pattern gHatchPatterns[] =
{
     { { 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF , 0xFF } },
     { { 0x01 , 0x02 , 0x04 , 0x08 , 0x10 , 0x20 , 0x40 , 0x80 } },
     { { 0x80 , 0x40 , 0x20 , 0x10 , 0x08 , 0x04 , 0x02 , 0x01 } },
     { { 0x10 , 0x10 , 0x10 , 0xFF , 0x10 , 0x10 , 0x10 , 0x10 } },
     { { 0x00 , 0x00 , 0x00 , 0xFF , 0x00 , 0x00 , 0x00 , 0x00 } },
     { { 0x10 , 0x10 , 0x10 , 0x10 , 0x10 , 0x10 , 0x10 , 0x10 } },
     { { 0x81 , 0x42 , 0x24 , 0x18 , 0x18 , 0x24 , 0x42 , 0x81 } }
} ;
static void wxMacGetHatchPattern(int hatchStyle, Pattern *pattern)
{
	int theIndex = 1 ;
	switch(hatchStyle)
	{
		case wxBDIAGONAL_HATCH:
			theIndex = 2;
			break;
		case wxFDIAGONAL_HATCH:
			theIndex = 3;
			break;
		case wxCROSS_HATCH:
			theIndex = 4;
			break;
		case wxHORIZONTAL_HATCH:
			theIndex = 5;
			break;
		case wxVERTICAL_HATCH:
			theIndex = 6;
			break;
		case wxCROSSDIAG_HATCH:
			theIndex = 7;
			break;
		default:
			theIndex = 1; // solid pattern
			break;
	}
	*pattern = gHatchPatterns[theIndex-1] ;
}
void wxDC::MacInstallPen() const
{
    wxCHECK_RET(Ok(), wxT("Invalid DC"));

    if ( m_macPenInstalled )
		return ;

	Pattern	 blackColor;
	RGBColor forecolor = MAC_WXCOLORREF( m_pen.GetColour().GetPixel());
	RGBColor backcolor = MAC_WXCOLORREF( m_backgroundBrush.GetColour().GetPixel());
	::RGBForeColor( &forecolor );
	::RGBBackColor( &backcolor );
	::PenNormal() ;
    int penWidth = (int) (m_pen.GetWidth() * m_scaleX) ; ;
	// null means only one pixel, at whatever resolution
	if ( penWidth == 0 )
		penWidth = 1 ;
	::PenSize(penWidth, penWidth);
	int penStyle = m_pen.GetStyle();
	if (penStyle == wxSOLID)
	{
		::PenPat(GetQDGlobalsBlack(&blackColor));
	}
	else if (IS_HATCH(penStyle))
	{
		Pattern pat ;
		wxMacGetHatchPattern(penStyle, &pat);
		::PenPat(&pat);
	}
	else
	{
	    Pattern pat = *GetQDGlobalsBlack(&blackColor) ;
	    switch( penStyle )
	    {
	        case wxDOT :
                for ( int i = 0 ; i < 8 ; ++i )
                {
                    pat.pat[i] = 0xCC ;
                }
	            break ;
	        case wxLONG_DASH :
                for ( int i = 0 ; i < 8 ; ++i )
                {
                    pat.pat[i] = 0xFE ;
                }
	            break ;
	        case wxSHORT_DASH :
                for ( int i = 0 ; i < 8 ; ++i )
                {
                    pat.pat[i] = 0xEE ;
                }
	            break ;
	        case wxDOT_DASH :
                for ( int i = 0 ; i < 8 ; ++i )
                {
                    pat.pat[i] = 0x6F ;
                }
	            break ;
	        case wxUSER_DASH :
	            {
	                wxDash* dash ;
                    m_pen.GetDashes(&dash) ;
	                // right now we don't allocate larger pixmaps
	                // int number =
	                m_pen.GetDashes(&dash) ;
	                for ( int i = 0 ; i < 8 ; ++i )
	                {
	                    pat.pat[i] = dash[0] ;
	                }
	            }
	            break ;
	    }
		::PenPat(&pat);
	}
	short mode = patCopy ;
	// todo :
	switch( m_logicalFunction )
	{
		case wxCOPY:       // only foreground color, leave background (thus not patCopy)
			mode = patOr ;
			break ;
		case wxINVERT:     // NOT dst
//			::PenPat(GetQDGlobalsBlack(&blackColor));
			mode = patXor ;
			break ;
		case wxXOR:        // src XOR dst
			mode = patXor ;
			break ;
		case wxOR_REVERSE: // src OR (NOT dst)
			mode = notPatOr ;
			break ;
		case wxSRC_INVERT: // (NOT src)
			mode = notPatCopy ;
			break ;
		// unsupported TODO
		case wxCLEAR:      // 0
		case wxAND_REVERSE:// src AND (NOT dst)
		case wxAND:        // src AND dst
		case wxAND_INVERT: // (NOT src) AND dst
		case wxNO_OP:      // dst
		case wxNOR:        // (NOT src) AND (NOT dst)
		case wxEQUIV:      // (NOT src) XOR dst
		case wxOR_INVERT:  // (NOT src) OR dst
		case wxNAND:       // (NOT src) OR (NOT dst)
		case wxOR:         // src OR dst
		case wxSET:        // 1
//		case wxSRC_OR:     // source _bitmap_ OR destination
//		case wxSRC_AND:     // source _bitmap_ AND destination
			break ;
	}
	::PenMode( mode ) ;
	m_macPenInstalled = true ;
	m_macBrushInstalled = false ;
	m_macFontInstalled = false ;
}
void wxDC::MacSetupBackgroundForCurrentPort(const wxBrush& background )
{
    Pattern whiteColor ;
    switch( background.MacGetBrushKind() )
    {
      case kwxMacBrushTheme :
        {
	        ::SetThemeBackground( background.GetMacTheme() , wxDisplayDepth() , true ) ;
          break ;
        }
      case kwxMacBrushThemeBackground :
        {
	        Rect extent ;
	        ThemeBackgroundKind bg = background.GetMacThemeBackground( &extent ) ;
	        ::ApplyThemeBackground( bg , &extent ,kThemeStateActive , wxDisplayDepth() , true ) ;
          break ;
        }
      case kwxMacBrushColour :
        {
       	    ::RGBBackColor( &MAC_WXCOLORREF( background.GetColour().GetPixel()) );
        	int brushStyle = background.GetStyle();
        	if (brushStyle == wxSOLID)
        		::BackPat(GetQDGlobalsWhite(&whiteColor));
        	else if (IS_HATCH(brushStyle))
        	{
        		Pattern pat ;
        		wxMacGetHatchPattern(brushStyle, &pat);
        		::BackPat(&pat);
        	}
        	else
        	{
        		::BackPat(GetQDGlobalsWhite(&whiteColor));
        	}
          break ;
        }
    }
}
void wxDC::MacInstallBrush() const
{
    wxCHECK_RET(Ok(), wxT("Invalid DC"));
	Pattern	 blackColor ;
    if ( m_macBrushInstalled )
        return ;
        
	// foreground
	bool backgroundTransparent = (GetBackgroundMode() == wxTRANSPARENT) ;
	::RGBForeColor( &MAC_WXCOLORREF( m_brush.GetColour().GetPixel()) );
	::RGBBackColor( &MAC_WXCOLORREF( m_backgroundBrush.GetColour().GetPixel()) );
	int brushStyle = m_brush.GetStyle();
	if (brushStyle == wxSOLID)
	{
		::PenPat(GetQDGlobalsBlack(&blackColor));
    }
	else if (IS_HATCH(brushStyle))
	{
		Pattern pat ;
		wxMacGetHatchPattern(brushStyle, &pat);
		::PenPat(&pat);
	}
	else if ( m_brush.GetStyle() == wxSTIPPLE || m_brush.GetStyle() == wxSTIPPLE_MASK_OPAQUE )
	{
	    // we force this in order to be compliant with wxMSW
	    backgroundTransparent = false ;
	    // for these the text fore (and back for MASK_OPAQUE) colors are used
	    wxBitmap* bitmap = m_brush.GetStipple() ;
	    int width = bitmap->GetWidth() ;
	    int height = bitmap->GetHeight() ;
        GWorldPtr gw = NULL ;
	    if ( m_brush.GetStyle() == wxSTIPPLE )
	        gw = MAC_WXHBITMAP(bitmap->GetHBITMAP())  ;
	    else
	        gw = MAC_WXHBITMAP(bitmap->GetMask()->GetMaskBitmap()) ;
        PixMapHandle gwpixmaphandle = GetGWorldPixMap( gw ) ;
        LockPixels( gwpixmaphandle ) ;
	    bool isMonochrome = !IsPortColor( gw ) ;
        if ( !isMonochrome )
        {
            if ( (**gwpixmaphandle).pixelSize == 1 )
                isMonochrome = true ;
        }
        if ( isMonochrome && width == 8 && height == 8 )
	    {
	        ::RGBForeColor( &MAC_WXCOLORREF( m_textForegroundColour.GetPixel()) );
	        ::RGBForeColor( &MAC_WXCOLORREF( m_textBackgroundColour.GetPixel()) );
            BitMap* gwbitmap = (BitMap*) *gwpixmaphandle ; // since the color depth is 1 it is a BitMap
            UInt8 *gwbits = (UInt8*) gwbitmap->baseAddr ;
            int alignment = gwbitmap->rowBytes & 0x7FFF ;
            Pattern pat ;
            for ( int i = 0 ; i < 8 ; ++i )
            {
                pat.pat[i] = gwbits[i*alignment+0] ;
            }
            UnlockPixels( GetGWorldPixMap( gw ) ) ;
            ::PenPat( &pat ) ;
	    }
	    else
	    {
 // this will be the code to handle power of 2 patterns, we will have to arrive at a nice
 // caching scheme before putting this into production
        	Handle      image;
        	long		imageSize;
	        PixPatHandle pixpat = NewPixPat() ;
	        CopyPixMap(gwpixmaphandle, (**pixpat).patMap);
            imageSize = GetPixRowBytes((**pixpat).patMap) *
			    ((**(**pixpat).patMap).bounds.bottom -
			    (**(**pixpat).patMap).bounds.top);
            PtrToHand( (**gwpixmaphandle).baseAddr, &image, imageSize );
            (**pixpat).patData = image;
            if ( isMonochrome )
            {
	            CTabHandle ctable = ((**((**pixpat).patMap)).pmTable) ;
	            ColorSpecPtr ctspec = (ColorSpecPtr) &(**ctable).ctTable ;
	            if ( ctspec[0].rgb.red == 0x0000 )
	            {
	                ctspec[1].rgb = MAC_WXCOLORREF( m_textBackgroundColour.GetPixel()) ;
	                ctspec[0].rgb = MAC_WXCOLORREF( m_textForegroundColour.GetPixel()) ;
	            }
	            else
	            {
	                ctspec[0].rgb = MAC_WXCOLORREF( m_textBackgroundColour.GetPixel()) ;
	                ctspec[1].rgb = MAC_WXCOLORREF( m_textForegroundColour.GetPixel()) ;
	            }
	            ::CTabChanged( ctable ) ;
            }
            ::PenPixPat(pixpat);
            m_macForegroundPixMap = pixpat ;
	    }
	    UnlockPixels( gwpixmaphandle ) ;
	}
    else
    {
	    ::PenPat(GetQDGlobalsBlack(&blackColor));
	}
	short mode = patCopy ;
	switch( m_logicalFunction )
	{
		case wxCOPY:       // src
		    if ( backgroundTransparent )
		        mode = patOr ;
		    else
			    mode = patCopy ;
			break ;
		case wxINVERT:     // NOT dst
		    if ( !backgroundTransparent )
		    {
			    ::PenPat(GetQDGlobalsBlack(&blackColor));
			}
			mode = patXor ;
			break ;
		case wxXOR:        // src XOR dst
			mode = patXor ;
			break ;
		case wxOR_REVERSE: // src OR (NOT dst)
			mode = notPatOr ;
			break ;
		case wxSRC_INVERT: // (NOT src)
			mode = notPatCopy ;
			break ;
		// unsupported TODO
		case wxCLEAR:      // 0
		case wxAND_REVERSE:// src AND (NOT dst)
		case wxAND:        // src AND dst
		case wxAND_INVERT: // (NOT src) AND dst
		case wxNO_OP:      // dst
		case wxNOR:        // (NOT src) AND (NOT dst)
		case wxEQUIV:      // (NOT src) XOR dst
		case wxOR_INVERT:  // (NOT src) OR dst
		case wxNAND:       // (NOT src) OR (NOT dst)
		case wxOR:         // src OR dst
		case wxSET:        // 1
//		case wxSRC_OR:     // source _bitmap_ OR destination
//		case wxSRC_AND:     // source _bitmap_ AND destination
			break ;
	}
	::PenMode( mode ) ;
	m_macBrushInstalled = true ;
	m_macPenInstalled = false ;
	m_macFontInstalled = false ;
}
// ---------------------------------------------------------------------------
// coordinates transformations
// ---------------------------------------------------------------------------

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
