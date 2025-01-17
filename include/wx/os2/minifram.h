/////////////////////////////////////////////////////////////////////////////
// Name:        minifram.h
// Purpose:     wxMiniFrame class. A small frame for e.g. floating toolbars.
//              If there is no equivalent on your platform, just make it a
//              normal frame.
// Author:      David Webster
// Modified by:
// Created:     10/10/99
// RCS-ID:      $Id: MINIFRAM.H,v 1.3 1999/10/11 02:49:06 DW Exp $
// Copyright:   (c) David Webster
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MINIFRAM_H_
#define _WX_MINIFRAM_H_

#include "wx/frame.h"

class WXDLLEXPORT wxMiniFrame: public wxFrame {

  DECLARE_DYNAMIC_CLASS(wxMiniFrame)

public:
  inline wxMiniFrame(void) {}
  inline wxMiniFrame(wxWindow *parent,
           wxWindowID id,
           const wxString& title,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize,
           long style = wxDEFAULT_FRAME_STYLE,
           const wxString& name = wxFrameNameStr)
  {
      Create(parent, id, title, pos, size, style | wxFRAME_TOOL_WINDOW | wxFRAME_FLOAT_ON_PARENT, name);
  }

protected:
};

#endif
    // _WX_MINIFRAM_H_

