///////////////////////////////////////////////////////////////////////////////
// Name:        wx/motif/dataobj.h
// Purpose:     declaration of the wxDataObject class for Motif
// Author:      Julian Smart
// RCS-ID:      $Id: dataobj.h,v 1.2 2002/02/23 21:26:15 RR Exp $
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_X11_DATAOBJ_H_
#define _WX_X11_DATAOBJ_H_

#ifdef __GNUG__
#pragma interface "dataobj.h"
#endif

// ----------------------------------------------------------------------------
// wxDataObject is the same as wxDataObjectBase under wxMotif
// ----------------------------------------------------------------------------

class wxDataObject : public wxDataObjectBase
{
public:
    wxDataObject();
    
#ifdef __DARWIN__
    ~wxDataObject() { }
#endif

    virtual bool IsSupportedFormat( const wxDataFormat& format, Direction dir = Get ) const;
};

#endif //_WX_X11_DATAOBJ_H_

