/////////////////////////////////////////////////////////////////////////////
// Name:        dcscreen.cpp
// Author:      Vaclav Slavik
// Id:          $Id: dcscreen.cpp,v 1.5 2002/02/15 01:07:51 VS Exp $
// Copyright:   (c) 2001-2002 SciTech Software, Inc. (www.scitechsoft.com)
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "dcscreen.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/dcscreen.h"
#include "wx/mgl/private.h"

IMPLEMENT_DYNAMIC_CLASS(wxScreenDC, wxDC)

wxScreenDC::wxScreenDC() : wxDC()
{
    SetMGLDC(g_displayDC, FALSE /* no ownership */);
    
    // VS: we have to hide the mouse, otherwise rendering artifacts may occur
    MS_obscure();
}

wxScreenDC::~wxScreenDC()
{
    MS_show();
}
