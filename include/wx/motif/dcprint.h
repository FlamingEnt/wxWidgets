/////////////////////////////////////////////////////////////////////////////
// Name:        dcprint.h
// Purpose:     wxPrinterDC class
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id: dcprint.h,v 1.2 2002/02/05 16:34:30 JS Exp $
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DCPRINT_H_
#define _WX_DCPRINT_H_

#ifdef __GNUG__
#pragma interface "dcprint.h"
#endif

#include "wx/dc.h"

class WXDLLEXPORT wxPrinterDC: public wxDC
{
public:
    DECLARE_CLASS(wxPrinterDC)
        
    // Create a printer DC
    wxPrinterDC(const wxString& driver, const wxString& device, const wxString& output, bool interactive = TRUE, int orientation = wxPORTRAIT);
    
    ~wxPrinterDC();
};

#endif
// _WX_DCPRINT_H_

