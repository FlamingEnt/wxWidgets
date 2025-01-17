/////////////////////////////////////////////////////////////////////////////
// Name:        toolbar.h
// Purpose:     wxToolBar class
// Author:      AUTHOR
// Modified by:
// Created:     ??/??/98
// RCS-ID:      $Id: toolbar.h,v 1.9 2002/08/31 11:29:13 GD Exp $
// Copyright:   (c) AUTHOR
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_TOOLBAR_H_
#define _WX_TOOLBAR_H_

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "toolbar.h"
#endif

#if wxUSE_TOOLBAR

#include "wx/tbarbase.h"
#include "wx/dynarray.h"

WXDLLEXPORT_DATA(extern const char*) wxToolBarNameStr;

class WXDLLEXPORT wxToolBar: public wxToolBarBase
{
  DECLARE_DYNAMIC_CLASS(wxToolBar)
 public:
  /*
   * Public interface
   */

   wxToolBar() : m_macToolHandles() { Init(); }


  inline wxToolBar(wxWindow *parent, wxWindowID id,
                   const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
                   long style = wxNO_BORDER|wxTB_HORIZONTAL,
                   const wxString& name = wxToolBarNameStr)
      : m_macToolHandles()
  {
    Init();
    Create(parent, id, pos, size, style, name);
  }
  ~wxToolBar();

  bool Create(wxWindow *parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
            long style = wxNO_BORDER|wxTB_HORIZONTAL,
            const wxString& name = wxToolBarNameStr);

    // override/implement base class virtuals
    virtual wxToolBarToolBase *FindToolForPosition(wxCoord x, wxCoord y) const;

    virtual bool Realize();

    virtual void SetToolBitmapSize(const wxSize& size);
    virtual wxSize GetToolSize() const;

    virtual void SetRows(int nRows);

  // Add all the buttons

	virtual void MacHandleControlClick( WXWidget control , short controlpart ) ;
	virtual wxString MacGetToolTipString( wxPoint &where ) ;
	void OnPaint(wxPaintEvent& event) ;
	void OnMouse(wxMouseEvent& event) ;
	virtual void MacSuperChangedPosition() ;
protected:
    // common part of all ctors
    void Init();

    // implement base class pure virtuals
    virtual bool DoInsertTool(size_t pos, wxToolBarToolBase *tool);
    virtual bool DoDeleteTool(size_t pos, wxToolBarToolBase *tool);

    virtual void DoEnableTool(wxToolBarToolBase *tool, bool enable);
    virtual void DoToggleTool(wxToolBarToolBase *tool, bool toggle);
    virtual void DoSetToggle(wxToolBarToolBase *tool, bool toggle);

    virtual wxToolBarToolBase *CreateTool(int id,
                                          const wxString& label,
                                          const wxBitmap& bmpNormal,
                                          const wxBitmap& bmpDisabled,
                                          wxItemKind kind,
                                          wxObject *clientData,
                                          const wxString& shortHelp,
                                          const wxString& longHelp);
    virtual wxToolBarToolBase *CreateTool(wxControl *control);

	wxArrayPtrVoid	m_macToolHandles ;

	DECLARE_EVENT_TABLE()
};

#endif // wxUSE_TOOLBAR

#endif
    // _WX_TOOLBAR_H_
