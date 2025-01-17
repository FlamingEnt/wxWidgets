/////////////////////////////////////////////////////////////////////////////
// Name:        pen.cpp
// Purpose:
// Author:      Robert Roebling
// Id:          $Id: pen.cpp,v 1.21 2002/05/07 21:58:26 GD Exp $
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifdef __GNUG__
#pragma implementation "pen.h"
#endif

#include "wx/pen.h"

#include <gdk/gdk.h>

//-----------------------------------------------------------------------------
// wxPen
//-----------------------------------------------------------------------------

class wxPenRefData: public wxObjectRefData
{
public:
    wxPenRefData()
    {
        m_width = 1;
        m_style = wxSOLID;
        m_joinStyle = wxJOIN_ROUND;
        m_capStyle = wxCAP_ROUND;
        m_dash = (wxGTKDash*) NULL;
        m_countDashes = 0;
    }
    
    wxPenRefData( const wxPenRefData& data )
        : wxObjectRefData()
    {
        m_style = data.m_style;
        m_width = data.m_width;
        m_joinStyle = data.m_joinStyle;
        m_capStyle = data.m_capStyle;
        m_colour = data.m_colour;
        m_countDashes = data.m_countDashes;
/*
        if (data.m_dash)  TODO
            m_dash = new
*/
        m_dash = data.m_dash;
    }

    bool operator == (const wxPenRefData& data) const
    {
        return (m_style == data.m_style &&
                m_width == data.m_width &&
                m_joinStyle == data.m_joinStyle &&
                m_capStyle == data.m_capStyle &&
                m_colour == data.m_colour);
    }
            
    int        m_width;
    int        m_style;
    int        m_joinStyle;
    int        m_capStyle;
    wxColour   m_colour;
    int        m_countDashes;
    wxGTKDash *m_dash;
};

//-----------------------------------------------------------------------------

#define M_PENDATA ((wxPenRefData *)m_refData)

IMPLEMENT_DYNAMIC_CLASS(wxPen,wxGDIObject)

wxPen::wxPen( const wxColour &colour, int width, int style )
{
    m_refData = new wxPenRefData();
    M_PENDATA->m_width = width;
    M_PENDATA->m_style = style;
    M_PENDATA->m_colour = colour;
}

wxPen::~wxPen()
{
    // m_refData unrefed in ~wxObject
}

wxObjectRefData *wxPen::CreateRefData() const
{
    return new wxPenRefData;
}

wxObjectRefData *wxPen::CloneRefData(const wxObjectRefData *data) const
{
    return new wxPenRefData(*(wxPenRefData *)data);
}

bool wxPen::operator == ( const wxPen& pen ) const
{
    if (m_refData == pen.m_refData) return TRUE;
    
    if (!m_refData || !pen.m_refData) return FALSE;
    
    return ( *(wxPenRefData*)m_refData == *(wxPenRefData*)pen.m_refData );
}

void wxPen::SetColour( const wxColour &colour )
{
    AllocExclusive();
    
    M_PENDATA->m_colour = colour;
}

void wxPen::SetDashes( int number_of_dashes, const wxDash *dash )
{
    AllocExclusive();
    
    M_PENDATA->m_countDashes = number_of_dashes;
    M_PENDATA->m_dash = (wxGTKDash *)dash; /* TODO */
}

void wxPen::SetColour( int red, int green, int blue )
{
    AllocExclusive();
    
    M_PENDATA->m_colour.Set( red, green, blue );
}

void wxPen::SetCap( int capStyle )
{
    AllocExclusive();
    
    M_PENDATA->m_capStyle = capStyle;
}

void wxPen::SetJoin( int joinStyle )
{
    AllocExclusive();
    
    M_PENDATA->m_joinStyle = joinStyle;
}

void wxPen::SetStyle( int style )
{
    AllocExclusive();
    
    M_PENDATA->m_style = style;
}

void wxPen::SetWidth( int width )
{
    AllocExclusive();
    
    M_PENDATA->m_width = width;
}

int wxPen::GetDashes( wxDash **ptr ) const
{
     *ptr = (M_PENDATA ? (wxDash*)M_PENDATA->m_dash : (wxDash*) NULL);
     return (M_PENDATA ? M_PENDATA->m_countDashes : 0);
}

int wxPen::GetDashCount() const
{
    return (M_PENDATA->m_countDashes);
}

wxDash* wxPen::GetDash() const
{
    return (wxDash*)M_PENDATA->m_dash;
}

int wxPen::GetCap() const
{
    wxCHECK_MSG( Ok(), -1, wxT("invalid pen") );

    return M_PENDATA->m_capStyle;
}

int wxPen::GetJoin() const
{
    wxCHECK_MSG( Ok(), -1, wxT("invalid pen") );

    return M_PENDATA->m_joinStyle;
}

int wxPen::GetStyle() const
{
    wxCHECK_MSG( Ok(), -1, wxT("invalid pen") );

    return M_PENDATA->m_style;
}

int wxPen::GetWidth() const
{
    wxCHECK_MSG( Ok(), -1, wxT("invalid pen") );

    return M_PENDATA->m_width;
}

wxColour &wxPen::GetColour() const
{
    wxCHECK_MSG( Ok(), wxNullColour, wxT("invalid pen") );

    return M_PENDATA->m_colour;
}

