/////////////////////////////////////////////////////////////////////////////
// Name:        x11/dcclient.cpp
// Purpose:     wxClientDC class
// Author:      Julian Smart, Robert Roebling
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id: dcclient.cpp,v 1.28.2.6 2002/11/09 17:35:39 RR Exp $
// Copyright:   (c) Julian Smart, Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
    #pragma implementation "dcclient.h"
#endif

#include "wx/dcclient.h"
#include "wx/dcmemory.h"
#include "wx/window.h"
#include "wx/app.h"
#include "wx/image.h"
#include "wx/module.h"
#include "wx/fontutil.h"

#include "wx/x11/private.h"

#include <math.h>

#if wxUSE_UNICODE
#include "glib.h"
#include "pango/pangox.h"
#include "pango/pangoxft.h"

#include "pango_x.cpp"
#endif

//-----------------------------------------------------------------------------
// local defines
//-----------------------------------------------------------------------------

#define USE_PAINT_REGION 1

//-----------------------------------------------------------------------------
// local data
//-----------------------------------------------------------------------------

#include "bdiag.xbm"
#include "fdiag.xbm"
#include "cdiag.xbm"
#include "horiz.xbm"
#include "verti.xbm"
#include "cross.xbm"
#define  num_hatches 6

#define IS_15_PIX_HATCH(s) ((s)==wxCROSSDIAG_HATCH || (s)==wxHORIZONTAL_HATCH || (s)==wxVERTICAL_HATCH)
#define IS_16_PIX_HATCH(s) ((s)!=wxCROSSDIAG_HATCH && (s)!=wxHORIZONTAL_HATCH && (s)!=wxVERTICAL_HATCH)

static Pixmap  hatches[num_hatches];
static Pixmap *hatch_bitmap = (Pixmap *) NULL;

//-----------------------------------------------------------------------------
// constants
//-----------------------------------------------------------------------------

const double RAD2DEG  = 180.0 / M_PI;

// ----------------------------------------------------------------------------
// private functions
// ----------------------------------------------------------------------------

static inline double dmax(double a, double b) { return a > b ? a : b; }
static inline double dmin(double a, double b) { return a < b ? a : b; }

static inline double DegToRad(double deg) { return (deg * M_PI) / 180.0; }

//-----------------------------------------------------------------------------
// Implement Pool of Graphic contexts. Creating them takes too much time.
//-----------------------------------------------------------------------------

#define GC_POOL_SIZE 200

enum wxPoolGCType
{
   wxGC_ERROR = 0,
   wxTEXT_MONO,
   wxBG_MONO,
   wxPEN_MONO,
   wxBRUSH_MONO,
   wxTEXT_COLOUR,
   wxBG_COLOUR,
   wxPEN_COLOUR,
   wxBRUSH_COLOUR,
   wxTEXT_SCREEN,
   wxBG_SCREEN,
   wxPEN_SCREEN,
   wxBRUSH_SCREEN
};

struct wxGC
{
    GC            m_gc;
    wxPoolGCType  m_type;
    bool          m_used;
};

static wxGC wxGCPool[GC_POOL_SIZE];

static void wxInitGCPool()
{
    memset( wxGCPool, 0, GC_POOL_SIZE*sizeof(wxGC) );
}

static void wxCleanUpGCPool()
{
    for (int i = 0; i < GC_POOL_SIZE; i++)
    {
        if (wxGCPool[i].m_gc)
            XFreeGC( wxGlobalDisplay(), wxGCPool[i].m_gc );
    }
}

static GC wxGetPoolGC( Window window, wxPoolGCType type )
{
    for (int i = 0; i < GC_POOL_SIZE; i++)
    {
        if (!wxGCPool[i].m_gc)
        {
            wxGCPool[i].m_gc = XCreateGC( wxGlobalDisplay(), window, 0, NULL );
            XSetGraphicsExposures( wxGlobalDisplay(), wxGCPool[i].m_gc, FALSE );
            wxGCPool[i].m_type = type;
            wxGCPool[i].m_used = FALSE;
        }
        if ((!wxGCPool[i].m_used) && (wxGCPool[i].m_type == type))
        {
            wxGCPool[i].m_used = TRUE;
            return wxGCPool[i].m_gc;
        }
    }

    wxFAIL_MSG( wxT("No GC available") );

    return (GC) NULL;
}

static void wxFreePoolGC( GC gc )
{
    for (int i = 0; i < GC_POOL_SIZE; i++)
    {
        if (wxGCPool[i].m_gc == gc)
        {
            wxGCPool[i].m_used = FALSE;
            return;
        }
    }

    wxFAIL_MSG( wxT("Wrong GC") );
}

// ----------------------------------------------------------------------------
// wxWindowDC
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxWindowDC, wxDC)

wxWindowDC::wxWindowDC()
{
    m_display = (WXDisplay *) NULL;
    m_penGC = (WXGC *) NULL;
    m_brushGC = (WXGC *) NULL;
    m_textGC = (WXGC *) NULL;
    m_bgGC = (WXGC *) NULL;
    m_cmap = (WXColormap *) NULL;
    m_isMemDC = FALSE;
    m_isScreenDC = FALSE;
    m_owner = (wxWindow *)NULL;
    
#if wxUSE_UNICODE
    m_context = (PangoContext *)NULL;
    m_fontdesc = (PangoFontDescription *)NULL;
#endif
}

wxWindowDC::wxWindowDC( wxWindow *window )
{
    wxASSERT_MSG( window, wxT("DC needs a window") );

    m_display = (WXDisplay *) NULL;
    m_penGC = (WXGC *) NULL;
    m_brushGC = (WXGC *) NULL;
    m_textGC = (WXGC *) NULL;
    m_bgGC = (WXGC *) NULL;
    m_cmap = (WXColormap *) NULL;
    m_owner = (wxWindow *)NULL;
    m_isMemDC = FALSE;
    m_isScreenDC = FALSE;
    m_font = window->GetFont();

    m_window = (WXWindow*) window->GetMainWindow();

    // not realized ?
    if (!m_window)
    {
         // don't report problems
         m_ok = TRUE;

         return;
    }

    m_display = (WXDisplay *) wxGlobalDisplay();
    
#if wxUSE_UNICODE
    m_context = wxTheApp->GetPangoContext();
    m_fontdesc = window->GetFont().GetNativeFontInfo()->description;
#endif

    int screen = DefaultScreen( (Display*) m_display );
    m_cmap = (WXColormap) DefaultColormap( (Display*) m_display, screen );

    SetUpDC();

    /* this must be done after SetUpDC, bacause SetUpDC calls the
       repective SetBrush, SetPen, SetBackground etc functions
       to set up the DC. SetBackground call m_owner->SetBackground
       and this might not be desired as the standard dc background
       is white whereas a window might assume gray to be the
       standard (as e.g. wxStatusBar) */

    m_owner = window;
}

wxWindowDC::~wxWindowDC()
{
    Destroy();
}

void wxWindowDC::SetUpDC()
{
    m_ok = TRUE;

    wxASSERT_MSG( !m_penGC, wxT("GCs already created") );

    if (m_isScreenDC)
    {
        m_penGC = (WXGC*) wxGetPoolGC( (Window) m_window, wxPEN_SCREEN );
        m_brushGC = (WXGC*) wxGetPoolGC( (Window) m_window, wxBRUSH_SCREEN );
        m_textGC = (WXGC*) wxGetPoolGC( (Window) m_window, wxTEXT_SCREEN );
        m_bgGC = (WXGC*) wxGetPoolGC( (Window) m_window, wxBG_SCREEN );
    }
    else
    if (m_isMemDC && (((wxMemoryDC*)this)->m_selected.GetDepth() == 1))
    {
        m_penGC = (WXGC*) wxGetPoolGC( (Window) m_window, wxPEN_MONO );
        m_brushGC = (WXGC*) wxGetPoolGC( (Window) m_window, wxBRUSH_MONO );
        m_textGC = (WXGC*) wxGetPoolGC( (Window) m_window, wxTEXT_MONO );
        m_bgGC = (WXGC*) wxGetPoolGC( (Window) m_window, wxBG_MONO );
    }
    else
    {
        m_penGC = (WXGC*) wxGetPoolGC( (Window) m_window, wxPEN_COLOUR );
        m_brushGC = (WXGC*) wxGetPoolGC( (Window) m_window, wxBRUSH_COLOUR );
        m_textGC = (WXGC*) wxGetPoolGC( (Window) m_window, wxTEXT_COLOUR );
        m_bgGC = (WXGC*) wxGetPoolGC( (Window) m_window, wxBG_COLOUR );
    }

    /* background colour */
    m_backgroundBrush = *wxWHITE_BRUSH;
    m_backgroundBrush.GetColour().CalcPixel( m_cmap );
    unsigned long bg_col = m_backgroundBrush.GetColour().GetPixel();

    /* m_textGC */
    m_textForegroundColour.CalcPixel( m_cmap );
    XSetForeground( (Display*) m_display, (GC) m_textGC, m_textForegroundColour.GetPixel() );

    m_textBackgroundColour.CalcPixel( m_cmap );
    XSetBackground( (Display*) m_display, (GC) m_textGC, m_textBackgroundColour.GetPixel() );

    XSetFillStyle( (Display*) m_display, (GC) m_textGC, FillSolid );

#if wxUSE_NANOX
    // By default, draw transparently
    GrSetGCUseBackground((GC) m_textGC, FALSE);
#endif

    /* m_penGC */
    m_pen.GetColour().CalcPixel( m_cmap );
    XSetForeground( (Display*) m_display, (GC) m_penGC, m_pen.GetColour().GetPixel() );
    XSetBackground( (Display*) m_display, (GC) m_penGC, bg_col );

    XSetLineAttributes( (Display*) m_display, (GC) m_penGC, 0, LineSolid, CapNotLast, JoinRound );

    /* m_brushGC */
    m_brush.GetColour().CalcPixel( m_cmap );
    XSetForeground( (Display*) m_display, (GC) m_brushGC, m_brush.GetColour().GetPixel() );
    XSetBackground( (Display*) m_display, (GC) m_brushGC, bg_col );

    XSetFillStyle( (Display*) m_display, (GC) m_brushGC, FillSolid );

    /* m_bgGC */
    XSetForeground( (Display*) m_display, (GC) m_bgGC, bg_col );
    XSetBackground( (Display*) m_display, (GC) m_bgGC, bg_col );
    
    XSetFillStyle( (Display*) m_display, (GC) m_bgGC, FillSolid );

    /* ROPs */
    XSetFunction( (Display*) m_display, (GC) m_textGC, GXcopy );
    XSetFunction( (Display*) m_display, (GC) m_brushGC, GXcopy );
    XSetFunction( (Display*) m_display, (GC)m_penGC, GXcopy );

    /* clipping */
    XSetClipMask( (Display*) m_display, (GC) m_penGC, None );
    XSetClipMask( (Display*) m_display, (GC) m_brushGC, None );
    XSetClipMask( (Display*) m_display, (GC) m_textGC, None );
    XSetClipMask( (Display*) m_display, (GC) m_bgGC, None );

    if (!hatch_bitmap)
    {
        int xscreen = DefaultScreen( (Display*) m_display );
        Window xroot = RootWindow( (Display*) m_display, xscreen );
    
        hatch_bitmap    = hatches;
        hatch_bitmap[0] = XCreateBitmapFromData( (Display*) m_display, xroot, bdiag_bits, bdiag_width, bdiag_height );
        hatch_bitmap[1] = XCreateBitmapFromData( (Display*) m_display, xroot, cdiag_bits, cdiag_width, cdiag_height );
        hatch_bitmap[2] = XCreateBitmapFromData( (Display*) m_display, xroot, fdiag_bits, fdiag_width, fdiag_height );
        hatch_bitmap[3] = XCreateBitmapFromData( (Display*) m_display, xroot, cross_bits, cross_width, cross_height );
        hatch_bitmap[4] = XCreateBitmapFromData( (Display*) m_display, xroot, horiz_bits, horiz_width, horiz_height );
        hatch_bitmap[5] = XCreateBitmapFromData( (Display*) m_display, xroot, verti_bits, verti_width, verti_height );
    }
}

void wxWindowDC::DoGetSize( int* width, int* height ) const
{
    wxCHECK_RET( m_owner, _T("GetSize() doesn't work without window") );

    m_owner->GetSize(width, height);
}

extern bool wxDoFloodFill(wxDC *dc, wxCoord x, wxCoord y, 
                          const wxColour & col, int style);

bool wxWindowDC::DoFloodFill(wxCoord x, wxCoord y,
                             const wxColour& col, int style)
{
    return wxDoFloodFill(this, x, y, col, style);
}

bool wxWindowDC::DoGetPixel( wxCoord x1, wxCoord y1, wxColour *col ) const
{
    // Generic (and therefore rather inefficient) method.
    // Could be improved.
    wxMemoryDC memdc;
    wxBitmap bitmap(1, 1);
    memdc.SelectObject(bitmap);
    memdc.Blit(0, 0, 1, 1, (wxDC*) this, x1, y1);
    memdc.SelectObject(wxNullBitmap);
    wxImage image(bitmap.ConvertToImage());
    col->Set(image.GetRed(0, 0), image.GetGreen(0, 0), image.GetBlue(0, 0));
    return TRUE;
}

void wxWindowDC::DoDrawLine( wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2 )
{
    wxCHECK_RET( Ok(), wxT("invalid window dc") );

    if (m_pen.GetStyle() != wxTRANSPARENT)
    {
        if (m_window)
        {
            // This hack is for the iPaq: XDrawLine draws
            // nothing, whereas XDrawLines works...
            wxPoint points[2];
            points[0].x = x1;
            points[0].y = y1;
            points[1].x = x2;
            points[1].y = y2;
            DrawLines( 2, points, 0, 0 );
            
            // XDrawLine( (Display*) m_display, (Window) m_window, 
            //    (GC) m_penGC, XLOG2DEV(x1), YLOG2DEV(y1), XLOG2DEV(x2), YLOG2DEV(y2) );
        }

        CalcBoundingBox(x1, y1);
        CalcBoundingBox(x2, y2);
    }
}

void wxWindowDC::DoCrossHair( wxCoord x, wxCoord y )
{
    wxCHECK_RET( Ok(), wxT("invalid window dc") );

    if (m_pen.GetStyle() != wxTRANSPARENT)
    {
        int w = 0;
        int h = 0;
        GetSize( &w, &h );
        wxCoord xx = XLOG2DEV(x);
        wxCoord yy = YLOG2DEV(y);
        if (m_window)
        {
            XDrawLine( (Display*) m_display, (Window) m_window,
                (GC) m_penGC, 0, yy, XLOG2DEVREL(w), yy );
            XDrawLine( (Display*) m_display, (Window) m_window,
                (GC) m_penGC, xx, 0, xx, YLOG2DEVREL(h) );
        }
    }
}

void wxWindowDC::DoDrawArc( wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2, wxCoord xc, wxCoord yc )
{
    wxCHECK_RET( Ok(), wxT("invalid window dc") );

    wxCoord xx1 = XLOG2DEV(x1);
    wxCoord yy1 = YLOG2DEV(y1);
    wxCoord xx2 = XLOG2DEV(x2);
    wxCoord yy2 = YLOG2DEV(y2);
    wxCoord xxc = XLOG2DEV(xc);
    wxCoord yyc = YLOG2DEV(yc);
    double dx = xx1 - xxc;
    double dy = yy1 - yyc;
    double radius = sqrt((double)(dx*dx+dy*dy));
    wxCoord   r      = (wxCoord)radius;
    double radius1, radius2;

    if (xx1 == xx2 && yy1 == yy2)
    {
        radius1 = 0.0;
        radius2 = 360.0;
    }
    else
    if (radius == 0.0)
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
    wxCoord alpha1 = wxCoord(radius1 * 64.0);
    wxCoord alpha2 = wxCoord((radius2 - radius1) * 64.0);
    while (alpha2 <= 0) alpha2 += 360*64;
    while (alpha1 > 360*64) alpha1 -= 360*64;

    if (m_window)
    {
        if (m_brush.GetStyle() != wxTRANSPARENT)
        {
            if ((m_brush.GetStyle() == wxSTIPPLE_MASK_OPAQUE) && (m_brush.GetStipple()->GetMask()))
            {
                XSetTSOrigin( (Display*) m_display, (GC) m_textGC,
                                      m_deviceOriginX % m_brush.GetStipple()->GetWidth(),
                                      m_deviceOriginY % m_brush.GetStipple()->GetHeight() );
                                      
                XFillArc( (Display*) m_display, (Window) m_window,
                    (GC) m_brushGC, xxc-r, yyc-r, 2*r,2*r, alpha1, alpha2 );
                    
                XSetTSOrigin( (Display*) m_display, (GC) m_textGC, 0, 0 );
            } else
            if (IS_15_PIX_HATCH(m_brush.GetStyle()))
            {
                XSetTSOrigin( (Display*) m_display, (GC) m_brushGC,
                                      m_deviceOriginX % 15, m_deviceOriginY % 15 );
                                      
                XFillArc( (Display*) m_display, (Window) m_window,
                    (GC) m_brushGC, xxc-r, yyc-r, 2*r,2*r, alpha1, alpha2 );
                    
                XSetTSOrigin( (Display*) m_display, (GC) m_brushGC, 0, 0 );
            } else
            if (IS_16_PIX_HATCH(m_brush.GetStyle()))
            {
                XSetTSOrigin( (Display*) m_display, (GC) m_brushGC,
                                      m_deviceOriginX % 16, m_deviceOriginY % 16 );
                
                XFillArc( (Display*) m_display, (Window) m_window,
                    (GC) m_brushGC, xxc-r, yyc-r, 2*r,2*r, alpha1, alpha2 );
                
                XSetTSOrigin( (Display*) m_display, (GC) m_brushGC, 0, 0 );
            } else
            if (m_brush.GetStyle() == wxSTIPPLE)
            {
                XSetTSOrigin( (Display*) m_display, (GC) m_brushGC,
                                      m_deviceOriginX % m_brush.GetStipple()->GetWidth(),
                                      m_deviceOriginY % m_brush.GetStipple()->GetHeight() );
                                      
                XFillArc( (Display*) m_display, (Window) m_window,
                    (GC) m_brushGC, xxc-r, yyc-r, 2*r,2*r, alpha1, alpha2 );
                    
                XSetTSOrigin( (Display*) m_display, (GC) m_brushGC, 0, 0 );
            }
            else
            {
                XFillArc( (Display*) m_display, (Window) m_window,
                    (GC) m_brushGC, xxc-r, yyc-r, 2*r,2*r, alpha1, alpha2 );
            }
        }

        if (m_pen.GetStyle() != wxTRANSPARENT)
        {
            XDrawArc( (Display*) m_display, (Window) m_window,
               (GC) m_penGC, xxc-r, yyc-r, 2*r,2*r, alpha1, alpha2 );
               
            XDrawLine( (Display*) m_display, (Window) m_window,
               (GC) m_penGC, xx1, yy1, xxc, yyc );
               
            XDrawLine( (Display*) m_display, (Window) m_window,
               (GC) m_penGC, xxc, yyc, xx2, yy2 );
        }
    }

    CalcBoundingBox (x1, y1);
    CalcBoundingBox (x2, y2);
}

void wxWindowDC::DoDrawEllipticArc( wxCoord x, wxCoord y, wxCoord width, wxCoord height, double sa, double ea )
{
    wxCHECK_RET( Ok(), wxT("invalid window dc") );

    wxCoord xx = XLOG2DEV(x);
    wxCoord yy = YLOG2DEV(y);
    wxCoord ww = m_signX * XLOG2DEVREL(width);
    wxCoord hh = m_signY * YLOG2DEVREL(height);

    // CMB: handle -ve width and/or height
    if (ww < 0) { ww = -ww; xx = xx - ww; }
    if (hh < 0) { hh = -hh; yy = yy - hh; }

    if (m_window)
    {
        wxCoord start = wxCoord(sa * 64.0);
        wxCoord end = wxCoord((ea-sa) * 64.0);

        if (m_brush.GetStyle() != wxTRANSPARENT)
        {
            if ((m_brush.GetStyle() == wxSTIPPLE_MASK_OPAQUE) && (m_brush.GetStipple()->GetMask()))
            {
                XSetTSOrigin( (Display*) m_display, (GC) m_textGC,
                                      m_deviceOriginX % m_brush.GetStipple()->GetWidth(),
                                      m_deviceOriginY % m_brush.GetStipple()->GetHeight() );
                
                XFillArc( (Display*) m_display, (Window) m_window,
                    (GC) m_textGC, xx, yy, ww, hh, start, end );
                
                XSetTSOrigin( (Display*) m_display, (GC) m_textGC, 0, 0 );
            } else
            if (IS_15_PIX_HATCH(m_brush.GetStyle()))
            {
                XSetTSOrigin( (Display*) m_display, (GC) m_brushGC,
                                      m_deviceOriginX % 15, m_deviceOriginY % 15 );
                
                XFillArc( (Display*) m_display, (Window) m_window,
                    (GC) m_brushGC, xx, yy, ww, hh, start, end );
                
                XSetTSOrigin( (Display*) m_display, (GC) m_brushGC, 0, 0 );
            } else
            if (IS_16_PIX_HATCH(m_brush.GetStyle()))
            {
                XSetTSOrigin( (Display*) m_display, (GC) m_brushGC,
                                      m_deviceOriginX % 16, m_deviceOriginY % 16 );
                
                XFillArc( (Display*) m_display, (Window) m_window,
                    (GC) m_brushGC, xx, yy, ww, hh, start, end );
                
                XSetTSOrigin( (Display*) m_display, (GC) m_brushGC, 0, 0 );
            } else
            if (m_brush.GetStyle() == wxSTIPPLE)
            {
                XSetTSOrigin( (Display*) m_display, (GC) m_brushGC,
                                      m_deviceOriginX % m_brush.GetStipple()->GetWidth(),
                                      m_deviceOriginY % m_brush.GetStipple()->GetHeight() );
                
                XFillArc( (Display*) m_display, (Window) m_window,
                    (GC) m_brushGC, xx, yy, ww, hh, start, end );
                
                XSetTSOrigin( (Display*) m_display, (GC) m_brushGC, 0, 0 );
            }
            else
            {
                XFillArc( (Display*) m_display, (Window) m_window,
                    (GC) m_brushGC, xx, yy, ww, hh, start, end );
            }
        }

        if (m_pen.GetStyle() != wxTRANSPARENT)
        {
            XDrawArc( (Display*) m_display, (Window) m_window,
                (GC) m_penGC, xx, yy, ww, hh, start, end );
        }
    }

    CalcBoundingBox (x, y);
    CalcBoundingBox (x + width, y + height);
}

void wxWindowDC::DoDrawPoint( wxCoord x, wxCoord y )
{
    wxCHECK_RET( Ok(), wxT("invalid window dc") );

    if ((m_pen.GetStyle() != wxTRANSPARENT) && m_window)
        XDrawPoint( (Display*) m_display, (Window) m_window,
                (GC) m_penGC, XLOG2DEV(x), YLOG2DEV(y) );

    CalcBoundingBox (x, y);
}

void wxWindowDC::DoDrawLines( int n, wxPoint points[], wxCoord xoffset, wxCoord yoffset )
{
    wxCHECK_RET( Ok(), wxT("invalid window dc") );

    if (m_pen.GetStyle() == wxTRANSPARENT) return;
    if (n <= 0) return;

    XPoint *xpoints = new XPoint[n];
    for (int i = 0; i < n; i++)
    {
        xpoints[i].x = XLOG2DEV (points[i].x + xoffset);
        xpoints[i].y = YLOG2DEV (points[i].y + yoffset);
        
        CalcBoundingBox( points[i].x + xoffset, points[i].y + yoffset );
    }
    XDrawLines( (Display*) m_display, (Window) m_window, (GC) m_penGC, xpoints, n, 0 );

    delete[] xpoints;
}

void wxWindowDC::DoDrawPolygon( int n, wxPoint points[],
                                wxCoord xoffset, wxCoord yoffset, int fillStyle )
{
    wxCHECK_RET( Ok(), wxT("invalid window dc") );

    if (n <= 0) return;

    XPoint *xpoints = new XPoint[n + 1];
    int i;
    for (i = 0; i < n; i++)
    {
        xpoints[i].x = XLOG2DEV (points[i].x + xoffset);
        xpoints[i].y = YLOG2DEV (points[i].y + yoffset);
        
        CalcBoundingBox (points[i].x + xoffset, points[i].y + yoffset);
    }

    if (m_window)
    {
        if (m_brush.GetStyle() != wxTRANSPARENT)
        {
        
            if ((m_brush.GetStyle() == wxSTIPPLE_MASK_OPAQUE) && (m_brush.GetStipple()->GetMask()))
            {
                XSetTSOrigin( (Display*) m_display, (GC) m_textGC,
                                      m_deviceOriginX % m_brush.GetStipple()->GetWidth(),
                                      m_deviceOriginY % m_brush.GetStipple()->GetHeight() );
                
                XFillPolygon( (Display*) m_display, (Window) m_window,
                    (GC) m_textGC, xpoints, n, Complex, 0);
                
                XSetTSOrigin( (Display*) m_display, (GC) m_textGC, 0, 0 );
            } else
            if (IS_15_PIX_HATCH(m_brush.GetStyle()))
            {
                XSetTSOrigin( (Display*) m_display, (GC) m_brushGC,
                                      m_deviceOriginX % 15, m_deviceOriginY % 15 );
                
                XFillPolygon( (Display*) m_display, (Window) m_window,
                    (GC) m_brushGC, xpoints, n, Complex, 0);
                
                XSetTSOrigin( (Display*) m_display, (GC) m_brushGC, 0, 0 );
            } else
            if (IS_16_PIX_HATCH(m_brush.GetStyle()))
            {
                XSetTSOrigin( (Display*) m_display, (GC) m_brushGC,
                                      m_deviceOriginX % 16, m_deviceOriginY % 16 );
                
                XFillPolygon( (Display*) m_display, (Window) m_window,
                    (GC) m_brushGC, xpoints, n, Complex, 0);
                
                XSetTSOrigin( (Display*) m_display, (GC) m_brushGC, 0, 0 );
            } else
            if (m_brush.GetStyle() == wxSTIPPLE)
            {
                XSetTSOrigin( (Display*) m_display, (GC) m_brushGC,
                                      m_deviceOriginX % m_brush.GetStipple()->GetWidth(),
                                      m_deviceOriginY % m_brush.GetStipple()->GetHeight() );
                
                XFillPolygon( (Display*) m_display, (Window) m_window,
                    (GC) m_brushGC, xpoints, n, Complex, 0);
                
                XSetTSOrigin( (Display*) m_display, (GC) m_brushGC, 0, 0 );
            }
            else
            {
                XFillPolygon( (Display*) m_display, (Window) m_window,
                    (GC) m_brushGC, xpoints, n, Complex, 0);
            }
        }

        if (m_pen.GetStyle () != wxTRANSPARENT)
        {
            // Close figure for XDrawLines
            xpoints[i].x = xpoints[0].x;
            xpoints[i].y = xpoints[0].y;

            XDrawLines( (Display*) m_display, (Window) m_window, (GC) m_penGC, xpoints, n + 1, 0);
        }
    }

    delete[] xpoints;
}

void wxWindowDC::DoDrawRectangle( wxCoord x, wxCoord y, wxCoord width, wxCoord height )
{
    wxCHECK_RET( Ok(), wxT("invalid window dc") );

    wxCoord xx = XLOG2DEV(x);
    wxCoord yy = YLOG2DEV(y);
    wxCoord ww = m_signX * XLOG2DEVREL(width);
    wxCoord hh = m_signY * YLOG2DEVREL(height);

    // CMB: draw nothing if transformed w or h is 0
    if (ww == 0 || hh == 0) return;

    // CMB: handle -ve width and/or height
    if (ww < 0) { ww = -ww; xx = xx - ww; }
    if (hh < 0) { hh = -hh; yy = yy - hh; }
    
    if (m_window)
    {
        if (m_brush.GetStyle() != wxTRANSPARENT)
        {
            if ((m_brush.GetStyle() == wxSTIPPLE_MASK_OPAQUE) && (m_brush.GetStipple()->GetMask()))
            {
                XSetTSOrigin( (Display*) m_display, (GC) m_textGC,
                                      m_deviceOriginX % m_brush.GetStipple()->GetWidth(),
                                      m_deviceOriginY % m_brush.GetStipple()->GetHeight() );
                
                XFillRectangle( (Display*) m_display, (Window) m_window,
                    (GC) m_textGC, xx, yy, ww, hh );
                
                XSetTSOrigin( (Display*) m_display, (GC) m_textGC, 0, 0 );
            } else
            if (IS_15_PIX_HATCH(m_brush.GetStyle()))
            {
                XSetTSOrigin( (Display*) m_display, (GC) m_brushGC,
                                      m_deviceOriginX % 15, m_deviceOriginY % 15 );
                
                XFillRectangle( (Display*) m_display, (Window) m_window,
                    (GC) m_brushGC, xx, yy, ww, hh );
                
                XSetTSOrigin( (Display*) m_display, (GC) m_brushGC, 0, 0 );
            } else
            if (IS_16_PIX_HATCH(m_brush.GetStyle()))
            {
                XSetTSOrigin( (Display*) m_display, (GC) m_brushGC,
                                      m_deviceOriginX % 16, m_deviceOriginY % 16 );
                
                XFillRectangle( (Display*) m_display, (Window) m_window,
                    (GC) m_brushGC, xx, yy, ww, hh );
                
                XSetTSOrigin( (Display*) m_display, (GC) m_brushGC, 0, 0 );
            } else
            if (m_brush.GetStyle() == wxSTIPPLE)
            {
                XSetTSOrigin( (Display*) m_display, (GC) m_brushGC,
                                      m_deviceOriginX % m_brush.GetStipple()->GetWidth(),
                                      m_deviceOriginY % m_brush.GetStipple()->GetHeight() );
                
                XFillRectangle( (Display*) m_display, (Window) m_window,
                    (GC) m_brushGC, xx, yy, ww, hh );
                
                XSetTSOrigin( (Display*) m_display, (GC) m_brushGC, 0, 0 );
            }
            else
            {
                XFillRectangle( (Display*) m_display, (Window) m_window,
                    (GC) m_brushGC, xx, yy, ww, hh );
            }
        }

        if (m_pen.GetStyle () != wxTRANSPARENT)
        {
            XDrawRectangle( (Display*) m_display, (Window) m_window,
                (GC) m_penGC, xx, yy, ww-1, hh-1 );
        }
    }

    CalcBoundingBox( x, y );
    CalcBoundingBox( x + width, y + height );
}

void wxWindowDC::DoDrawRoundedRectangle( wxCoord x, wxCoord y, wxCoord width, wxCoord height, double radius )
{
   // later
}

void wxWindowDC::DoDrawEllipse( wxCoord x, wxCoord y, wxCoord width, wxCoord height )
{
    wxCHECK_RET( Ok(), wxT("invalid window dc") );

    wxCoord xx = XLOG2DEV(x);
    wxCoord yy = YLOG2DEV(y);
    wxCoord ww = m_signX * XLOG2DEVREL(width);
    wxCoord hh = m_signY * YLOG2DEVREL(height);

    // CMB: handle -ve width and/or height
    if (ww < 0) { ww = -ww; xx = xx - ww; }
    if (hh < 0) { hh = -hh; yy = yy - hh; }

    if (m_window)
    {
        if (m_brush.GetStyle() != wxTRANSPARENT)
        {
            if ((m_brush.GetStyle() == wxSTIPPLE_MASK_OPAQUE) && (m_brush.GetStipple()->GetMask()))
            {
                XSetTSOrigin( (Display*) m_display, (GC) m_textGC,
                                      m_deviceOriginX % m_brush.GetStipple()->GetWidth(),
                                      m_deviceOriginY % m_brush.GetStipple()->GetHeight() );
                
                XFillArc( (Display*) m_display, (Window) m_window,
                    (GC) m_textGC, xx, yy, ww, hh, 0, 360*64 );
                
                XSetTSOrigin( (Display*) m_display, (GC) m_textGC, 0, 0 );
            } else
            if (IS_15_PIX_HATCH(m_brush.GetStyle()))
            {
                XSetTSOrigin( (Display*) m_display, (GC) m_brushGC,
                                      m_deviceOriginX % 15, m_deviceOriginY % 15 );
                
                XFillArc( (Display*) m_display, (Window) m_window,
                    (GC) m_brushGC, xx, yy, ww, hh, 0, 360*64 );
                
                XSetTSOrigin( (Display*) m_display, (GC) m_brushGC, 0, 0 );
            } else
            if (IS_16_PIX_HATCH(m_brush.GetStyle()))
            {
                XSetTSOrigin( (Display*) m_display, (GC) m_brushGC,
                                      m_deviceOriginX % 16, m_deviceOriginY % 16 );
                
                XFillArc( (Display*) m_display, (Window) m_window,
                    (GC) m_brushGC, xx, yy, ww, hh, 0, 360*64 );
                
                XSetTSOrigin( (Display*) m_display, (GC) m_brushGC, 0, 0 );
            } else
            if (m_brush.GetStyle() == wxSTIPPLE)
            {
                XSetTSOrigin( (Display*) m_display, (GC) m_brushGC,
                                      m_deviceOriginX % m_brush.GetStipple()->GetWidth(),
                                      m_deviceOriginY % m_brush.GetStipple()->GetHeight() );
                
                XFillArc( (Display*) m_display, (Window) m_window,
                    (GC) m_brushGC, xx, yy, ww, hh, 0, 360*64 );
                
                XSetTSOrigin( (Display*) m_display, (GC) m_brushGC, 0, 0 );
            }
            else
            {
                XFillArc( (Display*) m_display, (Window) m_window,
                    (GC) m_brushGC, xx, yy, ww, hh, 0, 360*64 );
            }
        }

        if (m_pen.GetStyle () != wxTRANSPARENT)
        {
            XDrawArc( (Display*) m_display, (Window) m_window,
                (GC) m_penGC, xx, yy, ww, hh, 0, 360*64 );
        }
    }

    CalcBoundingBox( x, y );
    CalcBoundingBox( x + width, y + height );
}

void wxWindowDC::DoDrawIcon( const wxIcon &icon, wxCoord x, wxCoord y)
{
    DoDrawBitmap(icon, x, y, TRUE);
}

#if wxUSE_NANOX
void wxWindowDC::DoDrawBitmap( const wxBitmap &bitmap,
                               wxCoord x, wxCoord y,
                               bool useMask )
{
    wxCHECK_RET( Ok(), wxT("invalid window dc") );

    wxCHECK_RET( bitmap.Ok(), wxT("invalid bitmap") );
    
    bool is_mono = (bitmap.GetBitmap() != NULL);

    /* scale/translate size and position */
    int xx = XLOG2DEV(x);
    int yy = YLOG2DEV(y);

    int w = bitmap.GetWidth();
    int h = bitmap.GetHeight();

    CalcBoundingBox( x, y );
    CalcBoundingBox( x + w, y + h );

    if (!m_window) return;

    int ww = XLOG2DEVREL(w);
    int hh = YLOG2DEVREL(h);

    /* compare to current clipping region */
    if (!m_currentClippingRegion.IsNull())
    {
        wxRegion tmp( xx,yy,ww,hh );
        tmp.Intersect( m_currentClippingRegion );
        if (tmp.IsEmpty())
            return;
    }

    /* scale bitmap if required */
    wxBitmap use_bitmap;
    if ((w != ww) || (h != hh))
    {
        wxImage image( bitmap.ConvertToImage() );
        image.Rescale( ww, hh );
#if 0
        if (is_mono)
            use_bitmap = image.ConvertToMonoBitmap(255,255,255);
        else
#endif
            use_bitmap = image;
    }
    else
    {
        use_bitmap = bitmap;
    }

    /* apply mask if any */
    WXPixmap mask = NULL;
    if (use_bitmap.GetMask())
        mask = use_bitmap.GetMask()->GetBitmap();

    if (useMask && mask)
    {
        Pixmap pixmap = (Pixmap) use_bitmap.GetPixmap() ;
        Pixmap maskPixmap = (Pixmap) use_bitmap.GetMask()->GetBitmap() ;
        Pixmap bufPixmap = GrNewPixmap(w, h, 0);
        GC gc = GrNewGC();
        GrSetGCUseBackground(gc, FALSE);
        GrSetGCMode(gc, GR_MODE_COPY);

        // This code assumes that background and foreground
        // colours are used in ROPs, like in MSW.
        // Not sure if this is true.

        // Copy destination to buffer.
        // In DoBlit, we need this step because Blit has
        // a ROP argument. Here, we don't need it.
        // In DoBlit, we may be able to eliminate this step
        // if we check if the rop = copy
#if 0
        GrCopyArea(bufPixmap, gc, 0, 0, w, h, (Window) m_window,
                  0, 0, GR_MODE_COPY);
#endif
            
        // Copy src to buffer using selected raster op (none selected
        // in DrawBitmap, so just use Gxcopy)
        GrCopyArea(bufPixmap, gc, 0, 0, w, h, pixmap,
                   0, 0, GR_MODE_COPY);

        // Set masked area in buffer to BLACK (pixel value 0)
        GrSetGCBackground(gc, WHITE);
        GrSetGCForeground(gc, BLACK);
        GrCopyArea(bufPixmap, gc, 0, 0, w, h, maskPixmap,
                    0, 0, GR_MODE_AND);
                       
        // set unmasked area in dest to BLACK
        GrSetGCBackground(gc, BLACK);
        GrSetGCForeground(gc, WHITE);
        GrCopyArea((Window) m_window, gc, xx, yy, w, h, maskPixmap,
                   0, 0, GR_MODE_AND);

        // OR buffer to dest
        GrCopyArea((Window) m_window, gc, xx, yy, w, h, bufPixmap,
                   0, 0, GR_MODE_OR);

        GrDestroyGC(gc);
        GrDestroyWindow(bufPixmap);
    }
    else
      XCopyArea( (Display*) m_display, (Pixmap) use_bitmap.GetPixmap(), (Window) m_window,
            (GC) m_penGC, 0, 0, w, h, xx, yy );

    /* remove mask again if any */
    if (useMask && mask)
    {
        if (!m_currentClippingRegion.IsNull())
                XSetRegion( (Display*) m_display, (GC) m_penGC, (Region) m_currentClippingRegion.GetX11Region() );
    }
}

#else

// Normal X11
void wxWindowDC::DoDrawBitmap( const wxBitmap &bitmap,
                               wxCoord x, wxCoord y,
                               bool useMask )
{
    wxCHECK_RET( Ok(), wxT("invalid window dc") );

    wxCHECK_RET( bitmap.Ok(), wxT("invalid bitmap") );
    
    bool is_mono = (bitmap.GetBitmap() != NULL);

    // scale/translate size and position
    int xx = XLOG2DEV(x);
    int yy = YLOG2DEV(y);

    int w = bitmap.GetWidth();
    int h = bitmap.GetHeight();

    CalcBoundingBox( x, y );
    CalcBoundingBox( x + w, y + h );

    if (!m_window) return;

    int ww = XLOG2DEVREL(w);
    int hh = YLOG2DEVREL(h);

    // compare to current clipping region
    if (!m_currentClippingRegion.IsNull())
    {
        wxRegion tmp( xx,yy,ww,hh );
        tmp.Intersect( m_currentClippingRegion );
        if (tmp.IsEmpty())
            return;
    }

    // scale bitmap if required
    wxBitmap use_bitmap;
    if ((w != ww) || (h != hh))
    {
        wxImage image( bitmap.ConvertToImage() );
        image.Rescale( ww, hh );
#if 0
        if (is_mono)
            use_bitmap = image.ConvertToMonoBitmap(255,255,255);
        else
#endif
            use_bitmap = image;
    }
    else
    {
        use_bitmap = bitmap;
    }

    // apply mask if any
    WXPixmap mask = NULL;
    if (use_bitmap.GetMask()) mask = use_bitmap.GetMask()->GetBitmap();
    
        if (useMask && mask)
        {
            WXPixmap new_mask = NULL;
#if 0
            if (!m_currentClippingRegion.IsNull())
            {
                GdkColor col;
                new_mask = gdk_pixmap_new( wxGetRootWindow()->window, ww, hh, 1 );
                GdkGC *gc = gdk_gc_new( new_mask );
                col.pixel = 0;
                gdk_gc_set_foreground( gc, &col );
                gdk_draw_rectangle( new_mask, gc, TRUE, 0, 0, ww, hh );
                col.pixel = 0;
                gdk_gc_set_background( gc, &col );
                col.pixel = 1;
                gdk_gc_set_foreground( gc, &col );
                gdk_gc_set_clip_region( gc, m_currentClippingRegion.GetRegion() );
                gdk_gc_set_clip_origin( gc, -xx, -yy );
                gdk_gc_set_fill( gc, GDK_OPAQUE_STIPPLED );
                gdk_gc_set_stipple( gc, mask );
                gdk_draw_rectangle( new_mask, gc, TRUE, 0, 0, ww, hh );
                gdk_gc_unref( gc );
            }
#endif
            if (is_mono)
            {
                if (new_mask)
                    XSetClipMask( (Display*) m_display, (GC) m_textGC, (Pixmap) new_mask );
                else
                    XSetClipMask( (Display*) m_display, (GC) m_textGC, (Pixmap) mask );
                XSetClipOrigin( (Display*) m_display, (GC) m_textGC, xx, yy );
            }
            else
            {
                if (new_mask)
                    XSetClipMask( (Display*) m_display, (GC) m_penGC, (Pixmap) new_mask );
                else
                    XSetClipMask( (Display*) m_display, (GC) m_penGC, (Pixmap) mask );
                XSetClipOrigin( (Display*) m_display, (GC) m_penGC, xx, yy );
            }
            
            if (new_mask)
               XFreePixmap( (Display*) m_display, (Pixmap) new_mask );
        }

    // Draw XPixmap or XBitmap, depending on what the wxBitmap contains. For
    // drawing a mono-bitmap (XBitmap) we use the current text GC
    if (is_mono)
        XCopyPlane( (Display*) m_display, (Pixmap) use_bitmap.GetBitmap(), (Window) m_window,
            (GC) m_textGC, 0, 0, w, h, xx, yy, 1 );
    else
        XCopyArea( (Display*) m_display, (Pixmap) use_bitmap.GetPixmap(), (Window) m_window,
            (GC) m_penGC, 0, 0, w, h, xx, yy );

    // remove mask again if any
    if (useMask && mask)
    {
        if (is_mono)
        {
            XSetClipMask( (Display*) m_display, (GC) m_textGC, None );
            XSetClipOrigin( (Display*) m_display, (GC) m_textGC, 0, 0 );
            if (!m_currentClippingRegion.IsNull())
                XSetRegion( (Display*) m_display, (GC) m_textGC, (Region) m_currentClippingRegion.GetX11Region() );
        }
        else
        {
            XSetClipMask( (Display*) m_display, (GC) m_penGC, None );
            XSetClipOrigin( (Display*) m_display, (GC) m_penGC, 0, 0 );
            if (!m_currentClippingRegion.IsNull())
                XSetRegion( (Display*) m_display, (GC) m_penGC, (Region) m_currentClippingRegion.GetX11Region() );
        }
    }
}
#endif
  // wxUSE_NANOX/!wxUSE_NANOX

bool wxWindowDC::DoBlit( wxCoord xdest, wxCoord ydest, wxCoord width, wxCoord height,
                         wxDC *source, wxCoord xsrc, wxCoord ysrc, int logical_func, bool useMask,
                         wxCoord xsrcMask, wxCoord ysrcMask )
{
   /* this is the nth try to get this utterly useless function to
      work. it now completely ignores the scaling or translation
      of the source dc, but scales correctly on the target dc and
      knows about possible mask information in a memory dc. */

    wxCHECK_MSG( Ok(), FALSE, wxT("invalid window dc") );

    wxCHECK_MSG( source, FALSE, wxT("invalid source dc") );

    if (!m_window) return FALSE;

    // transform the source DC coords to the device ones
    xsrc = source->XLOG2DEV(xsrc);
    ysrc = source->YLOG2DEV(ysrc);

    wxClientDC *srcDC = (wxClientDC*)source;
    wxMemoryDC *memDC = (wxMemoryDC*)source;

    bool use_bitmap_method = FALSE;
    bool is_mono = FALSE;

    // TODO: use the mask origin when drawing transparently 
    if (xsrcMask == -1 && ysrcMask == -1)
    {
        xsrcMask = xsrc;
        ysrcMask = ysrc;
    }
    
    if (srcDC->m_isMemDC)
    {
        if (!memDC->m_selected.Ok()) return FALSE;

        /* we use the "XCopyArea" way to copy a memory dc into
           y different window if the memory dc BOTH
           a) doesn't have any mask or its mask isn't used
           b) it is clipped
           c) is not 1-bit */

        if (useMask && (memDC->m_selected.GetMask()))
        {
           /* we HAVE TO use the direct way for memory dcs
              that have mask since the XCopyArea doesn't know
              about masks */
            use_bitmap_method = TRUE;
        }
        else if (memDC->m_selected.GetDepth() == 1)
        {
           /* we HAVE TO use the direct way for memory dcs
              that are bitmaps because XCopyArea doesn't cope
              with different bit depths */
            is_mono = TRUE;
            use_bitmap_method = TRUE;
        }
        else if ((xsrc == 0) && (ysrc == 0) &&
                 (width == memDC->m_selected.GetWidth()) &&
                 (height == memDC->m_selected.GetHeight()))
        {
           /* we SHOULD use the direct way if all of the bitmap
              in the memory dc is copied in which case XCopyArea
              wouldn't be able able to boost performace by reducing
              the area to be scaled */
            use_bitmap_method = TRUE;
        }
        else
        {
            use_bitmap_method = FALSE;
        }
    }

    CalcBoundingBox( xdest, ydest );
    CalcBoundingBox( xdest + width, ydest + height );

    // scale/translate size and position
    wxCoord xx = XLOG2DEV(xdest);
    wxCoord yy = YLOG2DEV(ydest);

    wxCoord ww = XLOG2DEVREL(width);
    wxCoord hh = YLOG2DEVREL(height);

    // compare to current clipping region
    if (!m_currentClippingRegion.IsNull())
    {
        wxRegion tmp( xx,yy,ww,hh );
        tmp.Intersect( m_currentClippingRegion );
        if (tmp.IsEmpty())
            return TRUE;
    }

    int old_logical_func = m_logicalFunction;
    SetLogicalFunction( logical_func );

    if (use_bitmap_method)
    {
        // scale/translate bitmap size
        wxCoord bm_width = memDC->m_selected.GetWidth();
        wxCoord bm_height = memDC->m_selected.GetHeight();

        wxCoord bm_ww = XLOG2DEVREL( bm_width );
        wxCoord bm_hh = YLOG2DEVREL( bm_height );

        // scale bitmap if required
        wxBitmap use_bitmap;

        if ((bm_width != bm_ww) || (bm_height != bm_hh))
        {
            wxImage image( memDC->m_selected.ConvertToImage() );
            image = image.Scale( bm_ww, bm_hh );

#if 0
            if (is_mono)
                use_bitmap = image.ConvertToMonoBitmap(255,255,255);
            else
#endif
                use_bitmap = image;
        }
        else
        {
            use_bitmap = memDC->m_selected;
        }

        // apply mask if any
        WXPixmap mask = NULL;
        if (use_bitmap.GetMask()) mask = use_bitmap.GetMask()->GetBitmap();

        if (useMask && mask)
        {
            WXPixmap new_mask = NULL;
#if 0
            if (!m_currentClippingRegion.IsNull())
            {
                GdkColor col;
                new_mask = gdk_pixmap_new( wxGetRootWindow()->window, bm_ww, bm_hh, 1 );
                GdkGC *gc = gdk_gc_new( new_mask );
                col.pixel = 0;
                gdk_gc_set_foreground( gc, &col );
                gdk_draw_rectangle( new_mask, gc, TRUE, 0, 0, bm_ww, bm_hh );
                col.pixel = 0;
                gdk_gc_set_background( gc, &col );
                col.pixel = 1;
                gdk_gc_set_foreground( gc, &col );
                gdk_gc_set_clip_region( gc, m_currentClippingRegion.GetRegion() );
                gdk_gc_set_clip_origin( gc, -xx, -yy );
                gdk_gc_set_fill( gc, GDK_OPAQUE_STIPPLED );
                gdk_gc_set_stipple( gc, mask );
                gdk_draw_rectangle( new_mask, gc, TRUE, 0, 0, bm_ww, bm_hh );
                gdk_gc_unref( gc );
            }
#endif
            if (is_mono)
            {
                if (new_mask)
                    XSetClipMask( (Display*) m_display, (GC) m_textGC, (Pixmap) new_mask );
                else
                    XSetClipMask( (Display*) m_display, (GC) m_textGC, (Pixmap) mask );
                XSetClipOrigin( (Display*) m_display, (GC) m_textGC, xx, yy );
            }
            else
            {
                if (new_mask)
                    XSetClipMask( (Display*) m_display, (GC) m_penGC, (Pixmap) new_mask );
                else
                    XSetClipMask( (Display*) m_display, (GC) m_penGC, (Pixmap) mask );
                XSetClipOrigin( (Display*) m_display, (GC) m_penGC, xx, yy );
            }
            
            if (new_mask)
               XFreePixmap( (Display*) m_display, (Pixmap) new_mask );
        }

        // Draw XPixmap or XBitmap, depending on what the wxBitmap contains. For
        // drawing a mono-bitmap (XBitmap) we use the current text GC

        if (is_mono)
            XCopyPlane( (Display*) m_display, (Pixmap) use_bitmap.GetBitmap(), (Window) m_window,
                (GC) m_textGC, xsrc, ysrc, width, height, xx, yy, 1 );
        else
            XCopyArea( (Display*) m_display, (Pixmap) use_bitmap.GetPixmap(), (Window) m_window,
                (GC) m_penGC, xsrc, ysrc, width, height, xx, yy );

        // remove mask again if any
        if (useMask && mask)
        {
            if (is_mono)
            {
                XSetClipMask( (Display*) m_display, (GC) m_textGC, None );
                XSetClipOrigin( (Display*) m_display, (GC) m_textGC, 0, 0 );
                if (!m_currentClippingRegion.IsNull())
                    XSetRegion( (Display*) m_display, (GC) m_textGC, (Region) m_currentClippingRegion.GetX11Region() );
            }
            else
            {
                XSetClipMask( (Display*) m_display, (GC) m_penGC, None );
                XSetClipOrigin( (Display*) m_display, (GC) m_penGC, 0, 0 );
                if (!m_currentClippingRegion.IsNull())
                    XSetRegion( (Display*) m_display, (GC) m_penGC, (Region) m_currentClippingRegion.GetX11Region() );
            }
        }
    }
    else // use_bitmap_method
    {
        if ((width != ww) || (height != hh))
        {
            /* Draw source window into a bitmap as we cannot scale
               a window in contrast to a bitmap. this would actually
               work with memory dcs as well, but we'd lose the mask
               information and waste one step in this process since
               a memory already has a bitmap. all this is slightly
               inefficient as we could take an XImage directly from
               an X window, but we'd then also have to care that
               the window is not outside the screen (in which case
               we'd get a BadMatch or what not).
               Is a double XGetImage and combined XGetPixel and
               XPutPixel really faster? I'm not sure. look at wxXt
               for a different implementation of the same problem. */

            wxBitmap bitmap( width, height );

            // copy including child window contents
            XSetSubwindowMode( (Display*) m_display, (GC) m_penGC, IncludeInferiors );
            XCopyArea( (Display*) m_display, (Window) srcDC->GetWindow(), (Window) bitmap.GetPixmap(),
                       (GC) m_penGC, xsrc, ysrc, width, height, 0, 0 );
            XSetSubwindowMode( (Display*) m_display, (GC) m_penGC, ClipByChildren );

            // scale image
            wxImage image( bitmap.ConvertToImage() );
            image = image.Scale( ww, hh );

            // convert to bitmap
            bitmap = image;

            // draw scaled bitmap
            XCopyArea( (Display*) m_display, (Window) bitmap.GetPixmap(), (Window) m_window, 
                       (GC) m_penGC, 0, 0, width, height, xx, yy );
        }
        else
        {
            // No scaling and not a memory dc with a mask either

            // copy including child window contents
            XSetSubwindowMode( (Display*) m_display, (GC) m_penGC, IncludeInferiors );
            XCopyArea( (Display*) m_display, (Window) srcDC->GetWindow(), (Window) m_window, 
                       (GC) m_penGC, xsrc, ysrc, width, height, xx, yy );
            XSetSubwindowMode( (Display*) m_display, (GC) m_penGC, ClipByChildren );
        }
    }

    SetLogicalFunction( old_logical_func );
    
    return TRUE;
}

void wxWindowDC::DoDrawText( const wxString &text, wxCoord x, wxCoord y )
{
    wxCHECK_RET( Ok(), wxT("invalid window dc") );

    if (!m_window) return;

    x = XLOG2DEV(x);
    y = YLOG2DEV(y);

#if wxUSE_UNICODE
    PangoLayout *layout = pango_layout_new(m_context);
    pango_layout_set_font_description(layout, m_fontdesc);
    
    const wxCharBuffer data = wxConvUTF8.cWC2MB( text );
    pango_layout_set_text(layout, (const char*) data, strlen( (const char*) data ));
    
    // Measure layout.
    int w,h;
    pango_layout_get_pixel_size(layout, &w, &h);
    wxCoord width = w;
    wxCoord height = h;
    
    // Draw layout.
    x11_draw_layout( (Drawable) m_window, (GC) m_textGC, x, y, layout, m_textForegroundColour );
    
    g_object_unref( G_OBJECT( layout ) );
    
    CalcBoundingBox (x + width, y + height);
    CalcBoundingBox (x, y);
#else
    XFontStruct *xfont = (XFontStruct*) m_font.GetFontStruct( m_scaleY, m_display );

    wxCHECK_RET( xfont, wxT("invalid font") );
    
    // First draw a rectangle representing the text background, if a text
    // background is specified
    if (m_textBackgroundColour.Ok () && (m_backgroundMode != wxTRANSPARENT))
    {
        // Since X draws from the baseline of the text, must add the text height
        int cx = 0;
        int cy = 0;
        int ascent = 0;
        int slen;
        int direction, descent;

        slen = strlen(text);
        XCharStruct overall_return;

        (void)XTextExtents(xfont, (char*) text.c_str(), slen, &direction,
                                 &ascent, &descent, &overall_return);

        cx = overall_return.width;
        cy = ascent + descent;
        m_textBackgroundColour.CalcPixel(m_cmap);
        XSetForeground ((Display*) m_display, (GC) m_textGC, m_textBackgroundColour.GetPixel());
        XFillRectangle( (Display*) m_display, (Window) m_window,
                    (GC) m_textGC, x, y, cx, cy );
        XSetForeground ((Display*) m_display, (GC) m_textGC, m_textForegroundColour.GetPixel());

    }

    XSetFont( (Display*) m_display, (GC) m_textGC, xfont->fid );
#if !wxUSE_NANOX
    if ((xfont->min_byte1 == 0) && (xfont->max_byte1 == 0))
#endif
    {
        XDrawString( (Display*) m_display, (Window) m_window, 
            (GC) m_textGC, x, y + XFontStructGetAscent(xfont), text.c_str(), text.Len() );
    }

#if 0
    if (m_font.GetUnderlined())
    {
        wxCoord ul_y = y + XFontStructGetAscent(font);
        if (font->descent > 0) ul_y++;
        gdk_draw_line( m_window, m_textGC, x, ul_y, x + width, ul_y);
    }

    width = wxCoord(width / m_scaleX);
    height = wxCoord(height / m_scaleY);
    
    CalcBoundingBox (x + width, y + height);
    CalcBoundingBox (x, y);
#endif
#endif
}

void wxWindowDC::DoDrawRotatedText( const wxString &text, wxCoord x, wxCoord y, double angle )
{
    // later
}

void wxWindowDC::DoGetTextExtent( const wxString &string, wxCoord *width, wxCoord *height,
                                wxCoord *descent, wxCoord *externalLeading,
                                wxFont *font ) const
{
    wxCHECK_RET( Ok(), "invalid dc" );

    if (string.IsEmpty())
    {
        if (width) (*width) = 0;
        if (height) (*height) = 0;
        return;
    }
    
#if wxUSE_UNICODE
    PangoLayout *layout = pango_layout_new( m_context );

    if (font)
        pango_layout_set_font_description( layout, font->GetNativeFontInfo()->description );
    else
        pango_layout_set_font_description(layout, m_fontdesc);
        
    const wxCharBuffer data = wxConvUTF8.cWC2MB( string );
    pango_layout_set_text(layout, (const char*) data, strlen( (const char*) data ));
 
    // Measure text.
    int w,h;
    pango_layout_get_pixel_size(layout, &w, &h);
    
    if (width) (*width) = (wxCoord) w; 
    if (height) (*height) = (wxCoord) h;
    if (descent)
    {
        // Do something about metrics here. TODO.
        (*descent) = 0;
    }
    if (externalLeading) (*externalLeading) = 0;  // ??
    
    g_object_unref( G_OBJECT( layout ) );
#else
    wxFont fontToUse = m_font;
    if (font) fontToUse = *font;

    wxCHECK_RET( fontToUse.Ok(), "invalid font" );

    XFontStruct *xfont = (XFontStruct*) fontToUse.GetFontStruct( m_scaleY, m_display );

    wxCHECK_RET( xfont, wxT("invalid font") );
    
    int direction, ascent, descent2;
    XCharStruct overall;

    XTextExtents( xfont, (char*) string.c_str(), string.Len(), &direction,
        &ascent, &descent2, &overall);

    if (width)
        *width = (wxCoord)( overall.width / m_scaleX );
    if (height)
        *height = (wxCoord)((ascent + descent2) / m_scaleY );
    if (descent)
        *descent = (wxCoord)(descent2 / m_scaleY );
    if (externalLeading)
        *externalLeading = 0; // ??
#endif
}

wxCoord wxWindowDC::GetCharWidth() const
{
    wxCHECK_MSG( Ok(), 0, "invalid dc" );
    
#if wxUSE_UNICODE
    PangoLayout *layout = pango_layout_new( m_context );
    
    if (!m_fontdesc)
    {
        char *crash = NULL;
        *crash = 0;
    }
    
    pango_layout_set_font_description(layout, m_fontdesc);
    pango_layout_set_text(layout, "H", 1 );
    int w,h;
    pango_layout_get_pixel_size(layout, &w, &h);
    g_object_unref( G_OBJECT( layout ) );
    
    return w;
#else
    wxCHECK_MSG( m_font.Ok(), 0, "invalid font" );

    XFontStruct *xfont = (XFontStruct*) m_font.GetFontStruct( m_scaleY, m_display );

    wxCHECK_MSG( xfont, 0, wxT("invalid font") );

    int direction, ascent, descent;
    XCharStruct overall;
    
    XTextExtents( xfont, "H", 1, &direction, &ascent, &descent, &overall );
        
    return (wxCoord)(overall.width / m_scaleX);
#endif
}

wxCoord wxWindowDC::GetCharHeight() const
{
    wxCHECK_MSG( Ok(), 0, "invalid dc" );
    
#if wxUSE_UNICODE
    PangoLayout *layout = pango_layout_new( m_context );
    
    if (!m_fontdesc)
    {
        char *crash = NULL;
        *crash = 0;
    }
    
    pango_layout_set_font_description(layout, m_fontdesc);
    
    pango_layout_set_text(layout, "H", 1 );
    int w,h;
    pango_layout_get_pixel_size(layout, &w, &h);
    g_object_unref( G_OBJECT( layout ) );
    
    return h;
#else
    wxCHECK_MSG( m_font.Ok(), 0, "invalid font" );

    XFontStruct *xfont = (XFontStruct*) m_font.GetFontStruct( m_scaleY, m_display );

    wxCHECK_MSG( xfont, 0, wxT("invalid font") );

    int direction, ascent, descent;
    XCharStruct overall;
    
    XTextExtents( xfont, "H", 1, &direction, &ascent, &descent, &overall );
        
    return (wxCoord)((ascent+descent) / m_scaleY);
#endif
}

void wxWindowDC::Clear()
{
    wxCHECK_RET( Ok(), wxT("invalid window dc") );

    if (!m_window) return;

    /* - we either are a memory dc or have a window as the
       owner. anything else shouldn't happen.
       - we don't use gdk_window_clear() as we don't set
       the window's background colour anymore. it is too
       much pain to keep the DC's and the window's back-
       ground colour in synch. */

    if (m_owner)
    {
        int width,height;
        m_owner->GetSize( &width, &height );
        XFillRectangle( (Display*) m_display, (Window) m_window, (GC) m_bgGC, 0, 0, width, height );
        return;
    }

    if (m_isMemDC)
    {
        int width,height;
        GetSize( &width, &height );
        XFillRectangle( (Display*) m_display, (Window) m_window, (GC) m_bgGC, 0, 0, width, height );
        return;
    }
}

void wxWindowDC::SetFont( const wxFont &font )
{
    wxCHECK_RET( Ok(), "invalid dc" );

    m_font = font;
}

void wxWindowDC::SetPen( const wxPen &pen )
{
    wxCHECK_RET( Ok(), wxT("invalid window dc") );

    if (m_pen == pen) return;

    m_pen = pen;

    if (!m_pen.Ok()) return;

    if (!m_window) return;

    int width = m_pen.GetWidth();
    if (width <= 0)
    {
        // CMB: if width is non-zero scale it with the dc
        width = 1;
    }
    else
    {
        // X doesn't allow different width in x and y and so we take
        // the average
        double w = 0.5 +
                   ( fabs((double) XLOG2DEVREL(width)) +
                     fabs((double) YLOG2DEVREL(width)) ) / 2.0;
        width = (int)w;
    }

    static const wxX11Dash dotted[] = {1, 1};
    static const wxX11Dash short_dashed[] = {2, 2};
    static const wxX11Dash long_dashed[] = {2, 4};
    static const wxX11Dash dotted_dashed[] = {3, 3, 1, 3};

    // We express dash pattern in pen width unit, so we are
    // independent of zoom factor and so on...
    int req_nb_dash;
    const wxX11Dash *req_dash;

    int lineStyle = LineSolid;
    switch (m_pen.GetStyle())
    {
        case wxUSER_DASH:
        {
            lineStyle = LineOnOffDash;
            req_nb_dash = m_pen.GetDashCount();
            req_dash = (wxX11Dash*)m_pen.GetDash();
            break;
        }
        case wxDOT:
        {
            lineStyle = LineOnOffDash;
            req_nb_dash = 2;
            req_dash = dotted;
            break;
        }
        case wxLONG_DASH:
        {
            lineStyle = LineOnOffDash;
            req_nb_dash = 2;
            req_dash = long_dashed;
            break;
        }
        case wxSHORT_DASH:
        {
            lineStyle = LineOnOffDash;
            req_nb_dash = 2;
            req_dash = short_dashed;
            break;
        }
        case wxDOT_DASH:
        {
//            lineStyle = LineDoubleDash;
            lineStyle = LineOnOffDash;
            req_nb_dash = 4;
            req_dash = dotted_dashed;
            break;
        }

        case wxTRANSPARENT:
        case wxSTIPPLE_MASK_OPAQUE:
        case wxSTIPPLE:
        case wxSOLID:
        default:
        {
            lineStyle = LineSolid;
            req_dash = (wxX11Dash*)NULL;
            req_nb_dash = 0;
            break;
        }
    }

    int capStyle = CapRound;
    switch (m_pen.GetCap())
    {
        case wxCAP_PROJECTING: { capStyle = CapProjecting; break; }
        case wxCAP_BUTT:       { capStyle = CapButt;       break; }
        case wxCAP_ROUND:
        default:
        {
            if (width <= 1)
            {
                width = 0;
                capStyle = CapNotLast;
            }
            else
            {
                capStyle = CapRound;
            }
            break;
        }
    }

    int joinStyle = JoinRound;
    switch (m_pen.GetJoin())
    {
        case wxJOIN_BEVEL: { joinStyle = JoinBevel; break; }
        case wxJOIN_MITER: { joinStyle = JoinMiter; break; }
        case wxJOIN_ROUND:
        default:           { joinStyle = JoinRound; break; }
    }

    XSetLineAttributes( (Display*) m_display, (GC) m_penGC, width, lineStyle, capStyle, joinStyle );

    m_pen.GetColour().CalcPixel( m_cmap );
    XSetForeground( (Display*) m_display, (GC) m_penGC, m_pen.GetColour().GetPixel() );
}

void wxWindowDC::SetBrush( const wxBrush &brush )
{
    wxCHECK_RET( Ok(), wxT("invalid window dc") );

    if (m_brush == brush) return;

    m_brush = brush;

    if (!m_brush.Ok()) return;

    if (!m_window) return;

    m_brush.GetColour().CalcPixel( m_cmap );
    XSetForeground( (Display*) m_display, (GC) m_brushGC, m_brush.GetColour().GetPixel() );
    
    XSetFillStyle( (Display*) m_display, (GC) m_brushGC, FillSolid );

    if ((m_brush.GetStyle() == wxSTIPPLE) && (m_brush.GetStipple()->Ok()))
    {
        if (m_brush.GetStipple()->GetPixmap())
        {
            XSetFillStyle( (Display*) m_display, (GC) m_brushGC, FillTiled );
            XSetTile( (Display*) m_display, (GC) m_brushGC, (Pixmap) m_brush.GetStipple()->GetPixmap() );
        }
        else
        {
            XSetFillStyle( (Display*) m_display, (GC) m_brushGC, FillStippled );
            XSetStipple( (Display*) m_display, (GC) m_brushGC, (Pixmap) m_brush.GetStipple()->GetBitmap() );
        }
    }

    if ((m_brush.GetStyle() == wxSTIPPLE_MASK_OPAQUE) && (m_brush.GetStipple()->GetMask()))
    {
        XSetFillStyle( (Display*) m_display, (GC) m_textGC, FillOpaqueStippled );
        XSetStipple( (Display*) m_display, (GC) m_textGC, (Pixmap) m_brush.GetStipple()->GetMask()->GetBitmap() );
    }

    if (IS_HATCH(m_brush.GetStyle()))
    {
        XSetFillStyle( (Display*) m_display, (GC) m_brushGC, FillStippled );
        int num = m_brush.GetStyle() - wxBDIAGONAL_HATCH;
        XSetStipple( (Display*) m_display, (GC) m_brushGC, hatches[num] );
    }
}

void wxWindowDC::SetBackground( const wxBrush &brush )
{
   /* CMB 21/7/98: Added SetBackground. Sets background brush
    * for Clear() and bg colour for shapes filled with cross-hatch brush */

    wxCHECK_RET( Ok(), wxT("invalid window dc") );

    if (m_backgroundBrush == brush) return;

    m_backgroundBrush = brush;

    if (!m_backgroundBrush.Ok()) return;

    if (!m_window) return;

    m_backgroundBrush.GetColour().CalcPixel( m_cmap );
    XSetBackground( (Display*) m_display, (GC) m_brushGC, m_backgroundBrush.GetColour().GetPixel() );
    XSetBackground( (Display*) m_display, (GC) m_penGC, m_backgroundBrush.GetColour().GetPixel() );
    XSetBackground( (Display*) m_display, (GC) m_bgGC, m_backgroundBrush.GetColour().GetPixel() );
    XSetForeground( (Display*) m_display, (GC) m_bgGC, m_backgroundBrush.GetColour().GetPixel() );

    XSetFillStyle( (Display*) m_display, (GC) m_bgGC, FillSolid );

    if ((m_backgroundBrush.GetStyle() == wxSTIPPLE) && (m_backgroundBrush.GetStipple()->Ok()))
    {
        if (m_backgroundBrush.GetStipple()->GetPixmap())
        {
            XSetFillStyle( (Display*) m_display, (GC) m_bgGC, FillTiled );
            XSetTile( (Display*) m_display, (GC) m_bgGC, (Pixmap) m_backgroundBrush.GetStipple()->GetPixmap() );
        }
        else
        {
            XSetFillStyle( (Display*) m_display, (GC) m_bgGC, FillStippled );
            XSetStipple( (Display*) m_display, (GC) m_bgGC, (Pixmap) m_backgroundBrush.GetStipple()->GetBitmap() );
        }
    }

    if (IS_HATCH(m_backgroundBrush.GetStyle()))
    {
        XSetFillStyle( (Display*) m_display, (GC) m_bgGC, FillStippled );
        int num = m_backgroundBrush.GetStyle() - wxBDIAGONAL_HATCH;
        XSetStipple( (Display*) m_display, (GC) m_bgGC, hatches[num] );
    }
}

void wxWindowDC::SetLogicalFunction( int function )
{
    wxCHECK_RET( Ok(), "invalid dc" );

    int x_function;

    if (m_logicalFunction == function)
        return;

    // VZ: shouldn't this be a CHECK?
    if (!m_window)
        return;
        
    switch (function)
    {
    case wxCLEAR:
        x_function = GXclear;
        break;
    case wxXOR:
        x_function = GXxor;
        break;
    case wxINVERT:
        x_function = GXinvert;
        break;
    case wxOR_REVERSE:
        x_function = GXorReverse;
        break;
    case wxAND_REVERSE:
        x_function = GXandReverse;
        break;
    case wxAND:
        x_function = GXand;
        break;
    case wxOR:
        x_function = GXor;
        break;
    case wxAND_INVERT:
        x_function = GXandInverted;
        break;
    case wxNO_OP:
        x_function = GXnoop;
        break;
    case wxNOR:
        x_function = GXnor;
        break;
    case wxEQUIV:
        x_function = GXequiv;
        break;
    case wxSRC_INVERT:
        x_function = GXcopyInverted;
        break;
    case wxOR_INVERT:
        x_function = GXorInverted;
        break;
    case wxNAND:
        x_function = GXnand;
        break;
    case wxSET:
        x_function = GXset;
        break;
    case wxCOPY:
    default:
        x_function = GXcopy;
        break;
    }

    XSetFunction( (Display*) m_display, (GC) m_penGC, x_function );
    XSetFunction( (Display*) m_display, (GC) m_brushGC, x_function );

    // to stay compatible with wxMSW, we don't apply ROPs to the text
    // operations (i.e. DrawText/DrawRotatedText).
    // True, but mono-bitmaps use the m_textGC and they use ROPs as well.
    XSetFunction( (Display*) m_display, (GC) m_textGC, x_function );
    
    m_logicalFunction = function;
}

void wxWindowDC::SetTextForeground( const wxColour &col )
{
    wxCHECK_RET( Ok(), wxT("invalid window dc") );

    // don't set m_textForegroundColour to an invalid colour as we'd crash
    // later then (we use m_textForegroundColour.GetColor() without checking
    // in a few places)
    if ( !col.Ok() || (m_textForegroundColour == col) )
        return;

    m_textForegroundColour = col;

    if (m_window)
    {
        m_textForegroundColour.CalcPixel( m_cmap );
        XSetForeground( (Display*) m_display, (GC) m_textGC, m_textForegroundColour.GetPixel() );
    }
}

void wxWindowDC::SetTextBackground( const wxColour &col )
{
    wxCHECK_RET( Ok(), wxT("invalid window dc") );

    // same as above
    if ( !col.Ok() || (m_textBackgroundColour == col) )
        return;

    m_textBackgroundColour = col;

    if (m_window)
    {
        m_textBackgroundColour.CalcPixel( m_cmap );
        XSetBackground( (Display*) m_display, (GC) m_textGC, m_textBackgroundColour.GetPixel() );
    }
}

void wxWindowDC::SetBackgroundMode( int mode )
{
    wxCHECK_RET( Ok(), wxT("invalid window dc") );

    m_backgroundMode = mode;

#if wxUSE_NANOX
    GrSetGCUseBackground((GC) m_textGC, mode == wxTRANSPARENT ? FALSE : TRUE);
#endif

    if (!m_window) return;

    // CMB 21/7/98: fill style of cross-hatch brushes is affected by
    // transparent/solid background mode

    if (m_brush.GetStyle() != wxSOLID && m_brush.GetStyle() != wxTRANSPARENT)
    {
        XSetFillStyle( (Display*) m_display, (GC) m_brushGC,
          (m_backgroundMode == wxTRANSPARENT) ? FillStippled : FillOpaqueStippled );
    }
}

void wxWindowDC::SetPalette( const wxPalette& palette )
{
#if 0
    if (m_window)
    {
        if (palette.Ok())
            /* Use GetXColormap */
            XSetWindowColormap ((Display*) m_display, (Window) m_window->GetXWindow(),
            (Colormap) palette.GetXColormap());
        else
            /* Use wxGetMainColormap */
            XSetWindowColormap ((Display*) m_display, (Window) m_window->GetXWindow(),
            (Colormap) wxTheApp->GetMainColormap(m_display));
    }
#endif
}

void wxWindowDC::DoSetClippingRegion( wxCoord x, wxCoord y, wxCoord width, wxCoord height )
{
    wxCHECK_RET( Ok(), wxT("invalid window dc") );

    if (!m_window) return;

    wxRect rect;
    rect.x = XLOG2DEV(x);
    rect.y = YLOG2DEV(y);
    rect.width = XLOG2DEVREL(width);
    rect.height = YLOG2DEVREL(height);

    if (!m_currentClippingRegion.IsNull())
        m_currentClippingRegion.Intersect( rect );
    else
        m_currentClippingRegion.Union( rect );

#if USE_PAINT_REGION
    if (!m_paintClippingRegion.IsNull())
        m_currentClippingRegion.Intersect( m_paintClippingRegion );
#endif

    wxCoord xx, yy, ww, hh;
    m_currentClippingRegion.GetBox( xx, yy, ww, hh );
    wxDC::DoSetClippingRegion( xx, yy, ww, hh );

    XSetRegion( (Display*) m_display, (GC) m_penGC, (Region) m_currentClippingRegion.GetX11Region() );
    XSetRegion( (Display*) m_display, (GC) m_brushGC, (Region) m_currentClippingRegion.GetX11Region() );
    XSetRegion( (Display*) m_display, (GC) m_textGC, (Region) m_currentClippingRegion.GetX11Region() );
    XSetRegion( (Display*) m_display, (GC) m_bgGC, (Region) m_currentClippingRegion.GetX11Region() );
}

void wxWindowDC::DoSetClippingRegionAsRegion( const wxRegion& region )
{
    wxCHECK_RET( Ok(), wxT("invalid window dc") );

    if (region.Empty())
    {
        DestroyClippingRegion();
        return;
    }

    if (!m_window) return;

    if (!m_currentClippingRegion.IsNull())
        m_currentClippingRegion.Intersect( region );
    else
        m_currentClippingRegion.Union( region );

#if USE_PAINT_REGION
    if (!m_paintClippingRegion.IsNull())
        m_currentClippingRegion.Intersect( m_paintClippingRegion );
#endif

    wxCoord xx, yy, ww, hh;
    m_currentClippingRegion.GetBox( xx, yy, ww, hh );
    wxDC::DoSetClippingRegion( xx, yy, ww, hh );

    XSetRegion( (Display*) m_display, (GC) m_penGC, (Region) m_currentClippingRegion.GetX11Region() );
    XSetRegion( (Display*) m_display, (GC) m_brushGC, (Region) m_currentClippingRegion.GetX11Region() );
    XSetRegion( (Display*) m_display, (GC) m_textGC, (Region) m_currentClippingRegion.GetX11Region() );
    XSetRegion( (Display*) m_display, (GC) m_bgGC, (Region) m_currentClippingRegion.GetX11Region() );
}

void wxWindowDC::DestroyClippingRegion()
{
    wxCHECK_RET( Ok(), wxT("invalid window dc") );

    wxDC::DestroyClippingRegion();

    m_currentClippingRegion.Clear();

#if USE_PAINT_REGION
    if (!m_paintClippingRegion.IsEmpty())
        m_currentClippingRegion.Union( m_paintClippingRegion );
#endif

    if (!m_window) return;

    if (m_currentClippingRegion.IsEmpty())
    {
        XSetClipMask( (Display*) m_display, (GC) m_penGC, None );
        XSetClipMask( (Display*) m_display, (GC) m_brushGC, None );
        XSetClipMask( (Display*) m_display, (GC) m_textGC, None );
        XSetClipMask( (Display*) m_display, (GC) m_bgGC, None );
    }
    else
    {
        XSetRegion( (Display*) m_display, (GC) m_penGC, (Region) m_currentClippingRegion.GetX11Region() );
        XSetRegion( (Display*) m_display, (GC) m_brushGC, (Region) m_currentClippingRegion.GetX11Region() );
        XSetRegion( (Display*) m_display, (GC) m_textGC, (Region) m_currentClippingRegion.GetX11Region() );
        XSetRegion( (Display*) m_display, (GC) m_bgGC, (Region) m_currentClippingRegion.GetX11Region() );
    }
}

void wxWindowDC::Destroy()
{
    if (m_penGC) wxFreePoolGC( (GC) m_penGC );
    m_penGC = NULL;
    if (m_brushGC) wxFreePoolGC( (GC) m_brushGC );
    m_brushGC = NULL;
    if (m_textGC) wxFreePoolGC( (GC) m_textGC );
    m_textGC = NULL;
    if (m_bgGC) wxFreePoolGC( (GC) m_bgGC );
    m_bgGC = NULL;
}

void wxWindowDC::ComputeScaleAndOrigin()
{
    /* CMB: copy scale to see if it changes */
    double origScaleX = m_scaleX;
    double origScaleY = m_scaleY;

    wxDC::ComputeScaleAndOrigin();

    /* CMB: if scale has changed call SetPen to recalulate the line width */
    if ((m_scaleX != origScaleX || m_scaleY != origScaleY) &&
        (m_pen.Ok()))
    {
      /* this is a bit artificial, but we need to force wxDC to think
         the pen has changed */
      wxPen pen = m_pen;
      m_pen = wxNullPen;
      SetPen( pen );
  }
}

wxSize wxWindowDC::GetPPI() const
{
    return wxSize(100, 100);
}

int wxWindowDC::GetDepth() const
{
    wxFAIL_MSG(wxT("not implemented"));

    return -1;
}

//-----------------------------------------------------------------------------
// wxClientDC
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxClientDC, wxWindowDC)

wxClientDC::wxClientDC( wxWindow *window )
          : wxWindowDC( window )
{
    wxCHECK_RET( window, _T("NULL window in wxClientDC::wxClientDC") );
    
    m_window = (WXWindow*) window->GetClientAreaWindow();
    
    // Adjust the client area when the wxWindow is not using 2 X11 windows.
    if (m_window == (WXWindow*) window->GetMainWindow())
    {
        wxPoint ptOrigin = window->GetClientAreaOrigin();
        SetDeviceOrigin(ptOrigin.x, ptOrigin.y);
        wxSize size = window->GetClientSize();
        SetClippingRegion(wxPoint(0, 0), size);
    }
}

void wxClientDC::DoGetSize(int *width, int *height) const
{
    wxCHECK_RET( m_owner, _T("GetSize() doesn't work without window") );

    m_owner->GetClientSize( width, height );
}

// ----------------------------------------------------------------------------
// wxPaintDC
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxPaintDC, wxClientDC)

wxPaintDC::wxPaintDC(wxWindow* window)
  : wxClientDC(window)
{
#if USE_PAINT_REGION
    if (!window->GetClipPaintRegion())
        return;

    m_paintClippingRegion = window->GetUpdateRegion();
    Region region = (Region) m_paintClippingRegion.GetX11Region();
    if (region)
    {
            m_currentClippingRegion.Union( m_paintClippingRegion );

            XSetRegion( (Display*) m_display, (GC) m_penGC, region );
            XSetRegion( (Display*) m_display, (GC) m_brushGC, region );
            XSetRegion( (Display*) m_display, (GC) m_textGC, region );
            XSetRegion( (Display*) m_display, (GC) m_bgGC, region );
    }
#endif // USE_PAINT_REGION
}

// ----------------------------------------------------------------------------
// wxDCModule
// ----------------------------------------------------------------------------

class wxDCModule : public wxModule
{
public:
    bool OnInit();
    void OnExit();

private:
    DECLARE_DYNAMIC_CLASS(wxDCModule)
};

IMPLEMENT_DYNAMIC_CLASS(wxDCModule, wxModule)

bool wxDCModule::OnInit()
{
    wxInitGCPool();
    return TRUE;
}

void wxDCModule::OnExit()
{
    wxCleanUpGCPool();
}
