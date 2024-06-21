/////////////////////////////////////////////////////////////////////////////
// Name:        gtk/clipbrd.cpp
// Purpose:
// Author:      Robert Roebling
// Id:          $Id: clipbrd.cpp,v 1.4 2001/12/08 00:26:05 VS Exp $
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "clipbrd.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/clipbrd.h"

#if wxUSE_CLIPBOARD

// FIXME_MGL

IMPLEMENT_DYNAMIC_CLASS(wxClipboard,wxObject)

#endif
  // wxUSE_CLIPBOARD

