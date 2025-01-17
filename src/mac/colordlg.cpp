/////////////////////////////////////////////////////////////////////////////
// Name:        colordlg.cpp
// Purpose:     wxColourDialog class. NOTE: you can use the generic class
//              if you wish, instead of implementing this.
// Author:      AUTHOR
// Modified by:
// Created:     ??/??/98
// RCS-ID:      $Id: colordlg.cpp,v 1.6 2002/01/11 14:07:01 SC Exp $
// Copyright:   (c) AUTHOR
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "colordlg.h"
#endif

#include "wx/mac/colordlg.h"

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxColourDialog, wxDialog)
#endif

#include "wx/mac/private.h"
#ifndef __DARWIN__
#include <ColorPicker.h>
#endif

/*
 * wxColourDialog
 */

wxColourDialog::wxColourDialog()
{
    m_dialogParent = NULL;
}

wxColourDialog::wxColourDialog(wxWindow *parent, wxColourData *data)
{
    Create(parent, data);
}

bool wxColourDialog::Create(wxWindow *parent, wxColourData *data)
{
    m_dialogParent = parent;
  
    if (data)
        m_colourData = *data;
    return TRUE;
}

int wxColourDialog::ShowModal()
{
	Point where ;
	RGBColor currentColor = *((RGBColor*)m_colourData.dataColour.GetPixel()) , newColor ;
	
	where.h = where.v = -1;

	if (GetColor( where, "\pSelect a new palette color.", &currentColor, &newColor ))
	{
		m_colourData.dataColour.Set( (WXCOLORREF*) &newColor ) ;
    	return wxID_OK;
	}
	else
	{
    	return wxID_CANCEL;
	}

    return wxID_CANCEL;
}

