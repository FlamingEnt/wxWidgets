/////////////////////////////////////////////////////////////////////////////
// Name:        print.h
// Purpose:     wxPrinter, wxPrintPreview classes
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id: print.h,v 1.1 2002/02/05 16:34:32 JS Exp $
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PRINT_H_
#define _WX_PRINT_H_

#ifdef __GNUG__
#pragma interface "print.h"
#endif

#include "wx/prntbase.h"

/*
* Represents the printer: manages printing a wxPrintout object
*/

class WXDLLEXPORT wxPrinter: public wxPrinterBase
{
    DECLARE_DYNAMIC_CLASS(wxPrinter)
        
public:
    wxPrinter(wxPrintData *data = NULL);
    ~wxPrinter();
    
    virtual bool Print(wxWindow *parent, wxPrintout *printout, bool prompt = TRUE);
    virtual bool PrintDialog(wxWindow *parent);
    virtual bool Setup(wxWindow *parent);
};

/*
* wxPrintPreview
* Programmer creates an object of this class to preview a wxPrintout.
*/

class WXDLLEXPORT wxPrintPreview: public wxPrintPreviewBase
{
    DECLARE_CLASS(wxPrintPreview)
        
public:
    wxPrintPreview(wxPrintout *printout, wxPrintout *printoutForPrinting = NULL, wxPrintData *data = NULL);
    ~wxPrintPreview();
    
    virtual bool Print(bool interactive);
    virtual void DetermineScaling();
};

#endif
// _WX_PRINT_H_
