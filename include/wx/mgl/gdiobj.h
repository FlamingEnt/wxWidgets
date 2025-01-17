/////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/gdiobj.h
// Purpose:     wxGDIObject class: base class for other GDI classes
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id: gdiobj.h,v 1.3 2001/10/06 22:44:47 VS Exp $
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GDIOBJ_H_
#define _WX_GDIOBJ_H_

#ifdef __GNUG__
    #pragma interface "gdiobj.h"
#endif

#include "wx/object.h"  // base class

// ----------------------------------------------------------------------------
// wxGDIRefData is the base class for wxXXXData structures which contain the
// real data for the GDI object and are shared among all wxWin objects sharing
// the same native GDI object
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxGDIRefData : public wxObjectRefData
{
    // this class is intentionally left blank
};

// ----------------------------------------------------------------------------
// wxGDIObject
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxGDIObject : public wxObject
{
public:
    wxGDIObject() { m_visible = FALSE; }

    virtual bool GetVisible() { return m_visible; }
    virtual void SetVisible( bool visible ) { m_visible = visible; }

    bool IsNull() const { return (m_refData == 0); }

protected:
    bool m_visible; /* can a pointer to this object be safely taken?
                     * - only if created within FindOrCreate... */

private:
    DECLARE_DYNAMIC_CLASS(wxGDIObject)
};

#endif // _WX_GDIOBJ_H_
