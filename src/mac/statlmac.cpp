/////////////////////////////////////////////////////////////////////////////
// Name:        generic/statline.cpp
// Purpose:     a generic wxStaticLine class
// Author:      Vadim Zeitlin
// Created:     28.06.99
// Version:     $Id: statlmac.cpp,v 1.7 2002/01/04 17:26:01 SC Exp $
// Copyright:   (c) 1998 Vadim Zeitlin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "statline.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/statline.h"
#include "wx/statbox.h"

#include "wx/mac/uma.h"

// ============================================================================
// implementation
// ============================================================================

IMPLEMENT_DYNAMIC_CLASS(wxStaticLine, wxControl)

// ----------------------------------------------------------------------------
// wxStaticLine
// ----------------------------------------------------------------------------

bool wxStaticLine::Create( wxWindow *parent,
                           wxWindowID id,
                           const wxPoint &pos,
                           const wxSize &size,
                           long style,
                           const wxString &name)
{
	Rect bounds ;
	Str255 title ;
	
	MacPreControlCreate( parent , id ,  "" , pos , size ,style, wxDefaultValidator , name , &bounds , title ) ;

	m_macControl = ::NewControl( MAC_WXHWND(parent->MacGetRootWindow()) , &bounds , title , false , 0 , 0 , 1, 
	  	kControlSeparatorLineProc , (long) this ) ;
	
	MacPostControlCreate() ;

    return TRUE;
}
