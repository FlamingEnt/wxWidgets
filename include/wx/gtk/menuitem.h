///////////////////////////////////////////////////////////////////////////////
// Name:        menuitem.h
// Purpose:     wxMenuItem class
// Author:      Robert Roebling
// RCS-ID:      $Id: menuitem.h,v 1.19 2002/09/07 12:28:46 GD Exp $
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

#ifndef __GTKMENUITEMH__
#define __GTKMENUITEMH__

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface
#endif

#include "wx/bitmap.h"

//-----------------------------------------------------------------------------
// wxMenuItem
//-----------------------------------------------------------------------------

class wxMenuItem : public wxMenuItemBase
{
public:
    wxMenuItem(wxMenu *parentMenu = (wxMenu *)NULL,
               int id = wxID_SEPARATOR,
               const wxString& text = wxEmptyString,
               const wxString& help = wxEmptyString,
               wxItemKind kind = wxITEM_NORMAL,
               wxMenu *subMenu = (wxMenu *)NULL);
    ~wxMenuItem();

    // implement base class virtuals
    virtual void SetText( const wxString& str );
    virtual void Enable( bool enable = TRUE );
    virtual void Check( bool check = TRUE );
    virtual bool IsChecked() const;
    virtual void SetBitmap(const wxBitmap& bitmap) { m_bitmap = bitmap; }
    virtual const wxBitmap& GetBitmap() const { return m_bitmap; }

#if wxUSE_ACCEL
    virtual wxAcceleratorEntry *GetAccel() const;
#endif // wxUSE_ACCEL

    // implementation
    void SetMenuItem(GtkWidget *menuItem) { m_menuItem = menuItem; }
    GtkWidget *GetMenuItem() const { return m_menuItem; }
    GtkWidget *GetLabelWidget() const { return m_labelWidget; }
    void SetLabelWidget(GtkWidget *labelWidget) { m_labelWidget = labelWidget; }
    wxString GetFactoryPath() const;

    wxString GetHotKey() const { return m_hotKey; }

    // compatibility only, don't use in new code
    wxMenuItem(wxMenu *parentMenu,
               int id,
               const wxString& text,
               const wxString& help,
               bool isCheckable,
               wxMenu *subMenu = (wxMenu *)NULL);

private:
    // common part of all ctors
    void Init(const wxString& text);

    // DoSetText() transforms the accel mnemonics in our label from MSW/wxWin
    // style to GTK+ and is called from ctor and SetText()
    void DoSetText(const wxString& text);

    wxString  m_hotKey;
    wxBitmap  m_bitmap; // Bitmap for menuitem, if any

    GtkWidget *m_menuItem;  // GtkMenuItem
    GtkWidget* m_labelWidget; // Label widget

    DECLARE_DYNAMIC_CLASS(wxMenuItem)
};


#endif
        //__GTKMENUITEMH__
