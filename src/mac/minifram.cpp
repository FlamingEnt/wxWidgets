/////////////////////////////////////////////////////////////////////////////
// Name:        minifram.cpp
// Purpose:     wxMiniFrame. Optional; identical to wxFrame if not supported.
// Author:      AUTHOR
// Modified by:
// Created:     ??/??/98
// RCS-ID:      $Id: minifram.cpp,v 1.4 2000/08/09 10:10:14 SC Exp $
// Copyright:   (c) AUTHOR
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "minifram.h"
#endif

#include "wx/minifram.h"

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxMiniFrame, wxFrame)
#endif


