/////////////////////////////////////////////////////////////////////////////
// Name:        pnghand.h
// Purpose:     PNG bitmap handler
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id: PNGHAND.H,v 1.3 1999/12/22 18:41:00 DW Exp $
// Copyright:   (c) Microsoft, Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma interface "pnghand.h"
#endif

#ifndef _WX_PNGHAND_H_
#define _WX_PNGHAND_H_

class WXDLLEXPORT wxPNGFileHandler: public wxBitmapHandler
{
    DECLARE_DYNAMIC_CLASS(wxPNGFileHandler)
public:
    inline wxPNGFileHandler(void)
    {
        m_sName = "PNG bitmap file";
        m_sExtension = "bmp";
        m_lType = wxBITMAP_TYPE_PNG;
    };

    virtual bool LoadFile( wxBitmap*       pBitmap
                          ,const wxString& rName
                          ,HPS             hPs
                          ,long            lFlags
                          ,int             nDesiredWidth
                          ,int             nDesiredHeight
                         );
    virtual bool SaveFile( wxBitmap*        pBitmap
                          ,const wxString&  rName
                          ,int              nType
                          ,const wxPalette* pPalette = NULL
                         );
};

#endif
  // _WX_PNGHAND_H_

