///////////////////////////////////////////////////////////////////////////////
// Name:        wx/motif/dataobj.h
// Purpose:     declaration of the wxDataObject class for Motif
// Author:      Julian Smart
// RCS-ID:      $Id: dataobj.h,v 1.7 2002/07/10 14:17:58 VZ Exp $
// Copyright:   (c) 1998 Julian Smart
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MOTIF_DATAOBJ_H_
#define _WX_MOTIF_DATAOBJ_H_

#ifdef __GNUG__
#pragma interface "dataobj.h"
#endif

// ----------------------------------------------------------------------------
// wxDataObject is the same as wxDataObjectBase under wxMotif
// ----------------------------------------------------------------------------

class wxDataObject : public wxDataObjectBase
{
public:
    virtual ~wxDataObject();
};

#endif //_WX_MOTIF_DATAOBJ_H_

