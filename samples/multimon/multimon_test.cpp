/////////////////////////////////////////////////////////////////////////////
// Name:        multimon_test.cpp
// Purpose:     tests wxDisplay class
// Author:      Royce Mitchell III
// Modified by:
// Created:     06/21/02
// RCS-ID:      $Id: multimon_test.cpp,v 1.1.2.1 2003/09/01 12:54:58 CE Exp $
// Copyright:   (c) wxWindows team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include <wx/wx.h>
#if ! wxUSE_DISPLAY 
#error "wxUSE_DISPLAY must be set to 1 in $WXWIN\lib\_sc\wx\setup.h"
#endif
#include <wx/display.h>

class TestApp : public wxApp
{
	bool OnInit();
};

DECLARE_APP(TestApp)
IMPLEMENT_APP(TestApp)

bool TestApp::OnInit()
{
	size_t count = wxDisplay::GetCount();
	wxLogDebug ( "I detected %i display(s) on your system", count );
	size_t i = 0;
	while ( i < count )
	{
		wxDisplay display ( i );
		wxRect r = display.GetGeometry();
		wxLogDebug ( "Display #%i \"%s\" = ( %i, %i, %i, %i ) @ %i bits",
			i, display.GetName().c_str(), r.GetLeft(), r.GetTop(), r.GetWidth(), r.GetHeight(),
			display.GetDepth() );
		i++;
	}
	return FALSE;
}
