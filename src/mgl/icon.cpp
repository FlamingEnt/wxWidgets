/////////////////////////////////////////////////////////////////////////////
// Name:        icon.cpp
// Author:      Vaclav Slavik
// Id:          $Id: icon.cpp,v 1.4 2002/02/15 01:07:51 VS Exp $
// Copyright:   (c) 2001-2002 SciTech Software, Inc. (www.scitechsoft.com)
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "icon.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/icon.h"

//-----------------------------------------------------------------------------
// wxIcon
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxIcon, wxBitmap)

wxIcon::wxIcon(const char **bits, int WXUNUSED(width), int WXUNUSED(height)) :
    wxBitmap(bits)
{
}

wxIcon::wxIcon(char **bits, int WXUNUSED(width), int WXUNUSED(height)) :
    wxBitmap(bits)
{
}

wxIcon::wxIcon(const wxIcon& icon) : wxBitmap()
{
    Ref(icon);
}

wxIcon& wxIcon::operator = (const wxIcon& icon)
{
    if (*this == icon) 
        return (*this);
    Ref(icon);
    return *this;
}

void wxIcon::CopyFromBitmap(const wxBitmap& bmp)
{
    wxIcon *icon = (wxIcon*)(&bmp);
    *this = *icon;
}

