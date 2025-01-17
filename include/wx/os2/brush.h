/////////////////////////////////////////////////////////////////////////////
// Name:        brush.h
// Purpose:     wxBrush class
// Author:      David Webster
// Modified by:
// Created:     10/13/99
// RCS-ID:      $Id: BRUSH.H,v 1.5 2000/12/12 04:46:51 dwebster Exp $
// Copyright:   (c) David Webster
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_BRUSH_H_
#define _WX_BRUSH_H_

#include "wx/gdicmn.h"
#include "wx/gdiobj.h"
#include "wx/bitmap.h"

class WXDLLEXPORT wxBrush;

class WXDLLEXPORT wxBrushRefData: public wxGDIRefData
{
    friend class WXDLLEXPORT wxBrush;
public:
    wxBrushRefData();
    wxBrushRefData(const wxBrushRefData& rData);
    ~wxBrushRefData();

protected:
    int                             m_nStyle;
    wxBitmap                        m_vStipple ;
    wxColour                        m_vColour;
    WXHBRUSH                        m_hBrush; // in OS/2 GPI this will be the PS the pen is associated with
    AREABUNDLE                      m_vBundle;
};

#define M_BRUSHDATA ((wxBrushRefData *)m_refData)

// Brush
class WXDLLEXPORT wxBrush: public wxGDIObject
{
    DECLARE_DYNAMIC_CLASS(wxBrush)

public:
    wxBrush();
    wxBrush( const wxColour& rCol
            ,int             nStyle
           );
    wxBrush(const wxBitmap& rStipple);
    inline wxBrush(const wxBrush& rBrush) { Ref(rBrush); }
    ~wxBrush();

    inline wxBrush& operator = (const wxBrush& rBrush) { if (*this == rBrush) return (*this); Ref(rBrush); return *this; }
    inline bool operator == (const wxBrush& rBrush) { return m_refData == rBrush.m_refData; }
    inline bool operator != (const wxBrush& rBrush) { return m_refData != rBrush.m_refData; }

    virtual void SetColour(const wxColour& rColour);
    virtual void SetColour( unsigned char cRed
                           ,unsigned char cGreen
                           ,unsigned char cBrush
                          );
    virtual void SetPS(HPS hPS);
    virtual void SetStyle(int nStyle)  ;
    virtual void SetStipple(const wxBitmap& rStipple);

    inline wxColour& GetColour(void) const { return (M_BRUSHDATA ? M_BRUSHDATA->m_vColour : wxNullColour); };
    inline int       GetStyle(void) const { return (M_BRUSHDATA ? M_BRUSHDATA->m_nStyle : 0); };
    inline wxBitmap* GetStipple(void) const { return (M_BRUSHDATA ? & M_BRUSHDATA->m_vStipple : 0); };
    inline int       GetPS(void) const { return (M_BRUSHDATA ? M_BRUSHDATA->m_hBrush : 0); };

    inline virtual bool Ok(void) const { return (m_refData != NULL) ; }

    //
    // Implementation
    //

    //
    // Useful helper: create the brush resource
    //
    bool     RealizeResource(void);
    WXHANDLE GetResourceHandle(void) ;
    bool     FreeResource(bool bForce = FALSE);
    bool     IsFree(void) const;
    void     Unshare(void);
}; // end of CLASS wxBrush

#endif
    // _WX_BRUSH_H_
