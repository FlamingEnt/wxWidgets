/////////////////////////////////////////////////////////////////////////////
// Name:        dc.h
// Purpose:     wxDC class
// Author:      AUTHOR
// Modified by:
// Created:     ??/??/98
// RCS-ID:      $Id: dc.h,v 1.24 2002/08/31 11:29:13 GD Exp $
// Copyright:   (c) AUTHOR
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DC_H_
#define _WX_DC_H_

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "dc.h"
#endif

#include "wx/pen.h"
#include "wx/brush.h"
#include "wx/icon.h"
#include "wx/font.h"
#include "wx/gdicmn.h"

//-----------------------------------------------------------------------------
// constants
//-----------------------------------------------------------------------------

#ifndef MM_TEXT
#define MM_TEXT			0
#define MM_ISOTROPIC	1
#define MM_ANISOTROPIC	2
#define MM_LOMETRIC		3
#define MM_HIMETRIC		4
#define MM_TWIPS		5
#define MM_POINTS		6
#define MM_METRIC		7
#endif

//-----------------------------------------------------------------------------
// global variables
//-----------------------------------------------------------------------------

extern int wxPageNumber;

class wxMacPortStateHelper ;
//-----------------------------------------------------------------------------
// wxDC
//-----------------------------------------------------------------------------

class WXDLLEXPORT wxDC: public wxDCBase
{
    DECLARE_DYNAMIC_CLASS(wxDC)
    DECLARE_NO_COPY_CLASS(wxDC)

  public:

    wxDC();
    ~wxDC();
    

    // implement base class pure virtuals
    // ----------------------------------

    virtual void Clear();

    virtual bool StartDoc( const wxString& WXUNUSED(message) ) { return TRUE; }
    virtual void EndDoc(void) {};
    
    virtual void StartPage(void) {};
    virtual void EndPage(void) {};

    virtual void SetFont(const wxFont& font);
    virtual void SetPen(const wxPen& pen);
    virtual void SetBrush(const wxBrush& brush);
    virtual void SetBackground(const wxBrush& brush);
    virtual void SetBackgroundMode(int mode);
    virtual void SetPalette(const wxPalette& palette);

    virtual void DestroyClippingRegion();

    virtual wxCoord GetCharHeight() const;
    virtual wxCoord GetCharWidth() const;
    virtual void DoGetTextExtent(const wxString& string,
                                 wxCoord *x, wxCoord *y,
                                 wxCoord *descent = NULL,
                                 wxCoord *externalLeading = NULL,
                                 wxFont *theFont = NULL) const;

    virtual bool CanDrawBitmap() const;
    virtual bool CanGetTextExtent() const;
    virtual int GetDepth() const;
    virtual wxSize GetPPI() const;

    virtual void SetMapMode(int mode);
    virtual void SetUserScale(double x, double y);

    virtual void SetLogicalScale(double x, double y);
    virtual void SetLogicalOrigin(wxCoord x, wxCoord y);
    virtual void SetDeviceOrigin(wxCoord x, wxCoord y);
    virtual void SetAxisOrientation(bool xLeftRight, bool yBottomUp);
    virtual void SetLogicalFunction(int function);

    virtual void SetTextForeground(const wxColour& colour) ;
    virtual void SetTextBackground(const wxColour& colour) ;

    void ComputeScaleAndOrigin(void);
  public:
  
    
    wxCoord XDEV2LOG(wxCoord x) const
	{
	  long new_x = x - m_deviceOriginX ;
	  if (new_x > 0) 
	    return (wxCoord)((double)(new_x) / m_scaleX + 0.5) * m_signX + m_logicalOriginX;
	  else
	    return (wxCoord)((double)(new_x) / m_scaleX - 0.5) * m_signX + m_logicalOriginX;
	}
    wxCoord XDEV2LOGREL(wxCoord x) const
	{ 
	  if (x > 0) 
	    return (wxCoord)((double)(x) / m_scaleX + 0.5);
	  else
	    return (wxCoord)((double)(x) / m_scaleX - 0.5);
	}
    wxCoord YDEV2LOG(wxCoord y) const
	{
	  long new_y = y - m_deviceOriginY ;
	  if (new_y > 0)
	    return (wxCoord)((double)(new_y) / m_scaleY + 0.5) * m_signY + m_logicalOriginY;
	  else
	    return (wxCoord)((double)(new_y) / m_scaleY - 0.5) * m_signY + m_logicalOriginY;
	}
    wxCoord YDEV2LOGREL(wxCoord y) const
	{ 
	  if (y > 0)
	    return (wxCoord)((double)(y) / m_scaleY + 0.5);
	  else
	    return (wxCoord)((double)(y) / m_scaleY - 0.5);
	}
    wxCoord XLOG2DEV(wxCoord x) const
	{ 
	  long new_x = x - m_logicalOriginX;
	  if (new_x > 0)
	    return (wxCoord)((double)(new_x) * m_scaleX + 0.5) * m_signX + m_deviceOriginX  ;
	  else
	    return (wxCoord)((double)(new_x) * m_scaleX - 0.5) * m_signX + m_deviceOriginX ;
	}
    wxCoord XLOG2DEVREL(wxCoord x) const
	{ 
	  if (x > 0)
	    return (wxCoord)((double)(x) * m_scaleX + 0.5);
	  else
	    return (wxCoord)((double)(x) * m_scaleX - 0.5);
	}
    wxCoord YLOG2DEV(wxCoord y) const
	{
	  long new_y = y - m_logicalOriginY ;
	  if (new_y > 0)
	    return (wxCoord)((double)(new_y) * m_scaleY + 0.5) * m_signY + m_deviceOriginY ;
	  else
	    return (wxCoord)((double)(new_y) * m_scaleY - 0.5) * m_signY + m_deviceOriginY ;
	}
    wxCoord YLOG2DEVREL(wxCoord y) const
	{ 
	  if (y > 0)
	    return (wxCoord)((double)(y) * m_scaleY + 0.5);
	  else
	    return (wxCoord)((double)(y) * m_scaleY - 0.5);
	}
    wxCoord XLOG2DEVMAC(wxCoord x) const
	{ 
	  long new_x = x - m_logicalOriginX;
	  if (new_x > 0)
	    return (wxCoord)((double)(new_x) * m_scaleX + 0.5) * m_signX + m_deviceOriginX + m_macLocalOrigin.x ;
	  else
	    return (wxCoord)((double)(new_x) * m_scaleX - 0.5) * m_signX + m_deviceOriginX + m_macLocalOrigin.x ;
	}
    wxCoord YLOG2DEVMAC(wxCoord y) const
	{
	  long new_y = y - m_logicalOriginY ;
	  if (new_y > 0)
	    return (wxCoord)((double)(new_y) * m_scaleY + 0.5) * m_signY + m_deviceOriginY + m_macLocalOrigin.y ;
	  else
	    return (wxCoord)((double)(new_y) * m_scaleY - 0.5) * m_signY + m_deviceOriginY + m_macLocalOrigin.y ;
	}
  
    WXHRGN MacGetCurrentClipRgn() { return m_macCurrentClipRgn ; }
    static void MacSetupBackgroundForCurrentPort(const wxBrush& background ) ;
//

protected:
    virtual bool DoFloodFill(wxCoord x, wxCoord y, const wxColour& col,
                             int style = wxFLOOD_SURFACE);

    virtual bool DoGetPixel(wxCoord x, wxCoord y, wxColour *col) const;

    virtual void DoDrawPoint(wxCoord x, wxCoord y);
    virtual void DoDrawLine(wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2);

    virtual void DoDrawArc(wxCoord x1, wxCoord y1,
                           wxCoord x2, wxCoord y2,
                           wxCoord xc, wxCoord yc);
    
    virtual void DoDrawEllipticArc(wxCoord x, wxCoord y, wxCoord w, wxCoord h,
                                   double sa, double ea);

    virtual void DoDrawRectangle(wxCoord x, wxCoord y, wxCoord width, wxCoord height);
    virtual void DoDrawRoundedRectangle(wxCoord x, wxCoord y,
                                        wxCoord width, wxCoord height,
                                        double radius);
    virtual void DoDrawEllipse(wxCoord x, wxCoord y, wxCoord width, wxCoord height);

    virtual void DoCrossHair(wxCoord x, wxCoord y);

    virtual void DoDrawIcon(const wxIcon& icon, wxCoord x, wxCoord y);
    virtual void DoDrawBitmap(const wxBitmap &bmp, wxCoord x, wxCoord y,
                              bool useMask = FALSE);

    virtual void DoDrawText(const wxString& text, wxCoord x, wxCoord y);
    virtual void DoDrawRotatedText(const wxString& text, wxCoord x, wxCoord y,
                                   double angle);

    virtual bool DoBlit(wxCoord xdest, wxCoord ydest, wxCoord width, wxCoord height,
                        wxDC *source, wxCoord xsrc, wxCoord ysrc,
                        int rop = wxCOPY, bool useMask = FALSE, wxCoord xsrcMask = -1, wxCoord ysrcMask = -1);

    // this is gnarly - we can't even call this function DoSetClippingRegion()
    // because of virtual function hiding
    virtual void DoSetClippingRegionAsRegion(const wxRegion& region);
    virtual void DoSetClippingRegion(wxCoord x, wxCoord y,
                                     wxCoord width, wxCoord height);
    virtual void DoGetClippingRegion(wxCoord *x, wxCoord *y,
                                     wxCoord *width, wxCoord *height)
    {
        GetClippingBox(x, y, width, height);
    }

    virtual void DoGetSize(int *width, int *height) const;
    virtual void DoGetSizeMM(int* width, int* height) const;

    virtual void DoDrawLines(int n, wxPoint points[],
                             wxCoord xoffset, wxCoord yoffset);
    virtual void DoDrawPolygon(int n, wxPoint points[],
                               wxCoord xoffset, wxCoord yoffset,
                               int fillStyle = wxODDEVEN_RULE);

  protected:
    //begin wxmac
    // Variables used for scaling
    double       m_mm_to_pix_x,m_mm_to_pix_y; 
    // not yet used
    bool         m_needComputeScaleX,m_needComputeScaleY;         
    // If un-scrolled is non-zero or d.o. changes with scrolling.
	// Set using SetInternalDeviceOrigin().
	long         m_internalDeviceOriginX,m_internalDeviceOriginY;  
	 // To be set by external classes such as wxScrolledWindow
	 // using SetDeviceOrigin()
    long         m_externalDeviceOriginX,m_externalDeviceOriginY;  
                                                                    
    // Begin implementation for Mac
    public:
								    
	WXHDC				m_macPort ;
	WXHBITMAP			m_macMask ;

	// in order to preserve the const inheritance of the virtual functions, we have to 
	// use mutable variables starting from CWPro 5

	void					MacInstallFont() const ;
	void					MacInstallPen() const ;
	void					MacInstallBrush() const ;
	
	mutable bool	m_macFontInstalled ;
	mutable bool	m_macPenInstalled ;
	mutable bool	m_macBrushInstalled ;
	
	WXHRGN				    m_macBoundaryClipRgn ;
	WXHRGN                  m_macCurrentClipRgn ;
	wxPoint					m_macLocalOrigin ;
	void					MacSetupPort( wxMacPortStateHelper* ph ) const ;
	void                    MacCleanupPort( wxMacPortStateHelper* ph ) const ;
	mutable void*                   m_macATSUIStyle ;
	mutable wxMacPortStateHelper*   m_macCurrentPortStateHelper ;
	mutable bool                    m_macFormerAliasState ;
	mutable short                   m_macFormerAliasSize ;
	mutable bool                    m_macAliasWasEnabled ;
	mutable void*                   m_macForegroundPixMap ;
	mutable void*                   m_macBackgroundPixMap ;
};

#endif
    // _WX_DC_H_
