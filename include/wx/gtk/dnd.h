///////////////////////////////////////////////////////////////////////////////
// Name:        dnd.h
// Purpose:     declaration of the wxDropTarget class
// Author:      Robert Roebling
// RCS-ID:      $Id: dnd.h,v 1.42 2002/09/07 12:28:46 GD Exp $
// Copyright:   (c) 1998 Vadim Zeitlin, Robert Roebling
// Licence:     wxWindows license
///////////////////////////////////////////////////////////////////////////////


#ifndef __GTKDNDH__
#define __GTKDNDH__

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface
#endif

#if wxUSE_DRAG_AND_DROP

#include "wx/object.h"
#include "wx/string.h"
#include "wx/dataobj.h"
#include "wx/cursor.h"
#include "wx/icon.h"
#include "wx/gdicmn.h"

//-------------------------------------------------------------------------
// classes
//-------------------------------------------------------------------------

class wxWindow;

class wxDropTarget;
class wxTextDropTarget;
class wxFileDropTarget;

class wxDropSource;

// ----------------------------------------------------------------------------
// macros
// ----------------------------------------------------------------------------

// this macro may be used instead for wxDropSource ctor arguments: it will use
// the icon 'name' from an XPM file under GTK, but will expand to something
// else under MSW. If you don't use it, you will have to use #ifdef in the
// application code.
#define wxDROP_ICON(name)   wxICON(name)

//-------------------------------------------------------------------------
// wxDropTarget
//-------------------------------------------------------------------------

class wxDropTarget: public wxDropTargetBase
{
public:
    wxDropTarget(wxDataObject *dataObject = (wxDataObject*) NULL );

    virtual wxDragResult OnDragOver(wxCoord x, wxCoord y, wxDragResult def);
    virtual bool OnDrop(wxCoord x, wxCoord y);
    virtual wxDragResult OnData(wxCoord x, wxCoord y, wxDragResult def);
    virtual bool GetData();

  // implementation

    GdkAtom GetMatchingPair();

    void RegisterWidget( GtkWidget *widget );
    void UnregisterWidget( GtkWidget *widget );

    GdkDragContext     *m_dragContext;
    GtkWidget          *m_dragWidget;
    GtkSelectionData   *m_dragData;
    guint               m_dragTime;
    bool                m_firstMotion;     // gdk has no "gdk_drag_enter" event

    void SetDragContext( GdkDragContext *dc ) { m_dragContext = dc; }
    void SetDragWidget( GtkWidget *w ) { m_dragWidget = w; }
    void SetDragData( GtkSelectionData *sd ) { m_dragData = sd; }
    void SetDragTime( guint time ) { m_dragTime = time; }
};

//-------------------------------------------------------------------------
// wxDropSource
//-------------------------------------------------------------------------

class wxDropSource: public wxDropSourceBase
{
public:
    // constructor. set data later with SetData()
    wxDropSource( wxWindow *win = (wxWindow *)NULL,
                  const wxIcon &copy = wxNullIcon,
                  const wxIcon &move = wxNullIcon,
                  const wxIcon &none = wxNullIcon);

    // constructor for setting one data object
    wxDropSource( wxDataObject& data,
                  wxWindow *win,
                  const wxIcon &copy = wxNullIcon,
                  const wxIcon &move = wxNullIcon,
                  const wxIcon &none = wxNullIcon);

    virtual ~wxDropSource();

    // start drag action
    virtual wxDragResult DoDragDrop(int flags = wxDrag_CopyOnly);

    // GTK implementation
    void RegisterWindow();
    void UnregisterWindow();

    void PrepareIcon( int action, GdkDragContext *context );

    GtkWidget       *m_widget;
    GtkWidget       *m_iconWindow;
    GdkDragContext  *m_dragContext;
    wxWindow        *m_window;

    wxDragResult     m_retValue;
    wxIcon           m_iconCopy,
                     m_iconMove,
                     m_iconNone;

    bool             m_waiting;

private:
    // common part of both ctors
    void SetIcons(const wxIcon& copy,
                  const wxIcon& move,
                  const wxIcon& none);
};

#endif // wxUSE_DRAG_AND_DROP

#endif //__GTKDNDH__

