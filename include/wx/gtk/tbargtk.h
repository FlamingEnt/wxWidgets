/////////////////////////////////////////////////////////////////////////////
// Name:        tbargtk.h
// Purpose:     GTK toolbar
// Author:      Robert Roebling
// RCS-ID:      $Id: tbargtk.h,v 1.28 2002/09/07 12:28:46 GD Exp $
// Copyright:   (c) Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GTK_TBARGTK_H_
#define _WX_GTK_TBARGTK_H_

#if defined(__GNUG__) && !defined(__APPLE__)
    #pragma interface "tbargtk.h"
#endif

#if wxUSE_TOOLBAR

// ----------------------------------------------------------------------------
// wxToolBar
// ----------------------------------------------------------------------------

class wxToolBar : public wxToolBarBase
{
public:
    // construction/destruction
    wxToolBar() { Init(); }
    wxToolBar( wxWindow *parent,
               wxWindowID id,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = 0,
               const wxString& name = wxToolBarNameStr )
    {
        Init();

        Create(parent, id, pos, size, style, name);
    }

    bool Create( wxWindow *parent,
                 wxWindowID id,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 long style = 0,
                 const wxString& name = wxToolBarNameStr );

    virtual ~wxToolBar();

    // override base class virtuals
    virtual void SetMargins(int x, int y);
    virtual void SetToolSeparation(int separation);

    virtual wxToolBarToolBase *FindToolForPosition(wxCoord x, wxCoord y) const;

    virtual void SetToolShortHelp(int id, const wxString& helpString);

    virtual void SetWindowStyleFlag( long style );

    // implementation from now on
    // --------------------------

    GtkToolbar   *m_toolbar;

    GdkColor     *m_fg;
    GdkColor     *m_bg;

    bool          m_blockEvent;

    void OnInternalIdle();

protected:
    // common part of all ctors
    void Init();

    // set the GTK toolbar style and orientation
    void GtkSetStyle();

    // implement base class pure virtuals
    virtual bool DoInsertTool(size_t pos, wxToolBarToolBase *tool);
    virtual bool DoDeleteTool(size_t pos, wxToolBarToolBase *tool);

    virtual void DoEnableTool(wxToolBarToolBase *tool, bool enable);
    virtual void DoToggleTool(wxToolBarToolBase *tool, bool toggle);
    virtual void DoSetToggle(wxToolBarToolBase *tool, bool toggle);

    virtual wxToolBarToolBase *CreateTool(int id,
                                          const wxString& label,
                                          const wxBitmap& bitmap1,
                                          const wxBitmap& bitmap2,
                                          wxItemKind kind,
                                          wxObject *clientData,
                                          const wxString& shortHelpString,
                                          const wxString& longHelpString);
    virtual wxToolBarToolBase *CreateTool(wxControl *control);

private:
    DECLARE_DYNAMIC_CLASS(wxToolBar)
};

#endif // wxUSE_TOOLBAR

#endif
    // _WX_GTK_TBARGTK_H_
