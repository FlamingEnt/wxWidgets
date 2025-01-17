/////////////////////////////////////////////////////////////////////////////
// Name:        brush.cpp
// Purpose:     wxBrush
// Author:      AUTHOR
// Modified by:
// Created:     ??/??/98
// RCS-ID:      $Id: brush.cpp,v 1.10 2002/05/07 21:58:26 GD Exp $
// Copyright:   (c) AUTHOR
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "brush.h"
#endif

#include "wx/setup.h"
#include "wx/utils.h"
#include "wx/brush.h"

#include "wx/mac/private.h"

#if !USE_SHARED_LIBRARIES
IMPLEMENT_DYNAMIC_CLASS(wxBrush, wxGDIObject)
#endif

class WXDLLEXPORT wxBrushRefData: public wxGDIRefData
{
    friend class WXDLLEXPORT wxBrush;
public:
    wxBrushRefData();
    wxBrushRefData(const wxBrushRefData& data);
    ~wxBrushRefData();

protected:
    wxMacBrushKind m_macBrushKind ;
    int           m_style;
    wxBitmap      m_stipple ;
    wxColour      m_colour;
    
    ThemeBrush    m_macThemeBrush ;
    
    ThemeBackgroundKind m_macThemeBackground ;
    Rect         m_macThemeBackgroundExtent ;
};

#define M_BRUSHDATA ((wxBrushRefData *)m_refData)

wxBrushRefData::wxBrushRefData()
    : m_style(wxSOLID)
{
    m_macBrushKind = kwxMacBrushColour ;
}

wxBrushRefData::wxBrushRefData(const wxBrushRefData& data)
    : wxGDIRefData()
    , m_style(data.m_style)
{
  m_stipple = data.m_stipple;
  m_colour = data.m_colour;
  m_macBrushKind = data.m_macBrushKind ;
  m_macThemeBrush = data.m_macThemeBrush ;
  m_macThemeBackground = data.m_macThemeBackground ;
  m_macThemeBackgroundExtent = data.m_macThemeBackgroundExtent ;
}

wxBrushRefData::~wxBrushRefData()
{
}

// Brushes
wxBrush::wxBrush()
{
}

wxBrush::~wxBrush()
{
}

wxBrush::wxBrush(const wxColour& col, int Style)
{
    m_refData = new wxBrushRefData;

    M_BRUSHDATA->m_colour = col;
    M_BRUSHDATA->m_style = Style;

    RealizeResource();
}

wxBrush::wxBrush(const wxBitmap& stipple)
{
    m_refData = new wxBrushRefData;

    M_BRUSHDATA->m_colour = *wxBLACK;
    M_BRUSHDATA->m_stipple = stipple;
    
    if (M_BRUSHDATA->m_stipple.GetMask())
        M_BRUSHDATA->m_style = wxSTIPPLE_MASK_OPAQUE;
    else
        M_BRUSHDATA->m_style = wxSTIPPLE;

    RealizeResource();
}

wxBrush::wxBrush(ThemeBrush macThemeBrush ) 
{
    m_refData = new wxBrushRefData;

    M_BRUSHDATA->m_macBrushKind = kwxMacBrushTheme;
    M_BRUSHDATA->m_macThemeBrush = macThemeBrush;

    RealizeResource();
}
void wxBrush::Unshare()
{
	// Don't change shared data
	if (!m_refData)
    {
		m_refData = new wxBrushRefData();
	}
    else
    {
		wxBrushRefData* ref = new wxBrushRefData(*(wxBrushRefData*)m_refData);
		UnRef();
		m_refData = ref;
	}
}

void wxBrush::SetColour(const wxColour& col)
{
    Unshare();
    M_BRUSHDATA->m_macBrushKind = kwxMacBrushColour;
    M_BRUSHDATA->m_colour = col;

    RealizeResource();
}

void wxBrush::SetColour(unsigned char r, unsigned char g, unsigned char b)
{
    Unshare();

    M_BRUSHDATA->m_macBrushKind = kwxMacBrushColour;
    M_BRUSHDATA->m_colour.Set(r, g, b);

    RealizeResource();
}

void wxBrush::SetStyle(int Style)
{
    Unshare();

    M_BRUSHDATA->m_macBrushKind = kwxMacBrushColour;
    M_BRUSHDATA->m_style = Style;

    RealizeResource();
}

void wxBrush::SetStipple(const wxBitmap& Stipple)
{
    Unshare();

    M_BRUSHDATA->m_macBrushKind = kwxMacBrushColour;
    M_BRUSHDATA->m_stipple = Stipple;

    RealizeResource();
}

void wxBrush::SetMacTheme(ThemeBrush macThemeBrush)
{
    Unshare();

    M_BRUSHDATA->m_macBrushKind = kwxMacBrushTheme;
    M_BRUSHDATA->m_macThemeBrush = macThemeBrush;

    RealizeResource();
}

void wxBrush::SetMacThemeBackground(unsigned long macThemeBackground, const WXRECTPTR extent)
{
    Unshare();

    M_BRUSHDATA->m_macBrushKind = kwxMacBrushThemeBackground;
    M_BRUSHDATA->m_macThemeBackground = macThemeBackground;
    M_BRUSHDATA->m_macThemeBackgroundExtent = *(Rect*)extent ;
    RealizeResource();
}

bool wxBrush::RealizeResource()
{
    return TRUE;
}

unsigned long wxBrush::GetMacThemeBackground( WXRECTPTR extent)  const 
{
  if ( M_BRUSHDATA && M_BRUSHDATA->m_macBrushKind == kwxMacBrushThemeBackground )
  {
    if ( extent )
      *(Rect*)extent = M_BRUSHDATA->m_macThemeBackgroundExtent ;
    return M_BRUSHDATA->m_macThemeBackground ;
  }
  else
  {
    return 0 ;  
  }
}

short wxBrush::GetMacTheme()  const 
{ 
  return (M_BRUSHDATA ? ( M_BRUSHDATA->m_macBrushKind == kwxMacBrushTheme ? M_BRUSHDATA->m_macThemeBrush : kThemeBrushBlack) : kThemeBrushBlack); 
};

wxColour& wxBrush::GetColour() const 
{ return (M_BRUSHDATA ? M_BRUSHDATA->m_colour : wxNullColour); };

int wxBrush::GetStyle() const 
{ 
  return (M_BRUSHDATA ? M_BRUSHDATA->m_style : 0); 
};

wxBitmap *wxBrush::GetStipple() const 
{ 
  return (M_BRUSHDATA ? & M_BRUSHDATA->m_stipple : 0); 
};

wxMacBrushKind wxBrush::MacGetBrushKind()  const 
{ 
  return (M_BRUSHDATA ? M_BRUSHDATA->m_macBrushKind : kwxMacBrushColour); 
};
