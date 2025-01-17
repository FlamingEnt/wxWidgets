///////////////////////////////////////////////////////////////////////////////
// Name:        dnd.cpp
// Purpose:     wxDropTarget class
// Author:      Robert Roebling
// Id:          $Id: dnd.cpp,v 1.86 2002/08/30 13:48:41 CE Exp $
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
    #pragma implementation "dnd.h"
#endif

#include "wx/dnd.h"
#include "wx/log.h"
									       
									        
#if wxUSE_DRAG_AND_DROP

#include "wx/window.h"
#include "wx/app.h"
#include "wx/gdicmn.h"
#include "wx/intl.h"
#include "wx/utils.h"

#include "wx/gtk/private.h"

#include <gdk/gdkprivate.h>

#include <gtk/gtkdnd.h>
#include <gtk/gtkselection.h>

//-----------------------------------------------------------------------------
// idle system
//-----------------------------------------------------------------------------

extern void wxapp_install_idle_handler();
extern bool g_isIdle;

//-----------------------------------------------------------------------------
// thread system
//-----------------------------------------------------------------------------

#if wxUSE_THREADS
#endif

//----------------------------------------------------------------------------
// global data
//----------------------------------------------------------------------------

extern bool g_blockEventsOnDrag;

// the flags used for the last DoDragDrop()
static long gs_flagsForDrag = 0;

// the trace mask we use with wxLogTrace() - call
// wxLog::AddTraceMask(TRACE_DND) to enable the trace messages from here
// (there are quite a few of them, so don't enable this by default)
static const wxChar *TRACE_DND = _T("dnd");

//----------------------------------------------------------------------------
// standard icons
//----------------------------------------------------------------------------

/* XPM */
static const char * page_xpm[] = {
/* width height ncolors chars_per_pixel */
"32 32 5 1",
/* colors */
"         s None        c None",
".        c black",
"X        c wheat",
"o        c tan",
"O        c #6699FF",
/* pixels */
"    ...................         ",
"    .XXXXXXXXXXXXXXXXX..        ",
"    .XXXXXXXXXXXXXXXXX.o.       ",
"    .XXXXXXXXXXXXXXXXX.oo.      ",
"    .XXXXXXXXXXXXXXXXX.ooo.     ",
"    .XXXXXXXXXXXXXXXXX.oooo.    ",
"    .XXXXXXXXXXXXXXXXX.......   ",
"    .XXXXXOOOOOOOOOOXXXooooo.   ",
"    .XXXXXXXXXXXXXXXXXXooooo.   ",
"    .XXXXXOOOOOOOOOOXXXXXXXX.   ",
"    .XXXXXXXXXXXXXXXXXXXXXXX.   ",
"    .XXXXXXXOOOOOOOOOXXXXXXX.   ",
"    .XXXXXXXXXXXXXXXXXXXXXXX.   ",
"    .XXXXXXOOOOOOOOOOXXXXXXX.   ",
"    .XXXXXXXXXXXXXXXXXXXXXXX.   ",
"    .XXXXXOOOOOOOOOOXXXXXXXX.   ",
"    .XXXXXXXXXXXXXXXXXXXXXXX.   ",
"    .XXXXXXXOOOOOOOOOXXXXXXX.   ",
"    .XXXXXXXXXXXXXXXXXXXXXXX.   ",
"    .XXXXXXOOOOOOOOOOXXXXXXX.   ",
"    .XXXXXXXXXXXXXXXXXXXXXXX.   ",
"    .XXXXXOOOOOOOOOOXXXXXXXX.   ",
"    .XXXXXXXXXXXXXXXXXXXXXXX.   ",
"    .XXXXXXOOOOOOOOOOXXXXXXX.   ",
"    .XXXXXXXXXXXXXXXXXXXXXXX.   ",
"    .XXXXXOOOOOOOXXXXXXXXXXX.   ",
"    .XXXXXXXXXXXXXXXXXXXXXXX.   ",
"    .XXXXXXXXXXXXXXXXXXXXXXX.   ",
"    .XXXXXXXXXXXXXXXXXXXXXXX.   ",
"    .XXXXXXXXXXXXXXXXXXXXXXX.   ",
"    .XXXXXXXXXXXXXXXXXXXXXXX.   ",
"    .........................   "};


// ============================================================================
// private functions
// ============================================================================

// ----------------------------------------------------------------------------
// convert between GTK+ and wxWindows DND constants
// ----------------------------------------------------------------------------

static wxDragResult ConvertFromGTK(long action)
{
    switch ( action )
    {
        case GDK_ACTION_COPY:
            return wxDragCopy;

        case GDK_ACTION_LINK:
            return wxDragLink;

        case GDK_ACTION_MOVE:
            return wxDragMove;
    }

    return wxDragNone;
}

// ----------------------------------------------------------------------------
// "drag_leave"
// ----------------------------------------------------------------------------

static void target_drag_leave( GtkWidget *WXUNUSED(widget),
                               GdkDragContext *context,
                               guint WXUNUSED(time),
                               wxDropTarget *drop_target )
{
    if (g_isIdle) wxapp_install_idle_handler();

    /* inform the wxDropTarget about the current GdkDragContext.
       this is only valid for the duration of this call */
    drop_target->SetDragContext( context );

    /* we don't need return values. this event is just for
       information */
    drop_target->OnLeave();

    /* this has to be done because GDK has no "drag_enter" event */
    drop_target->m_firstMotion = TRUE;

    /* after this, invalidate the drop_target's GdkDragContext */
    drop_target->SetDragContext( (GdkDragContext*) NULL );
}

// ----------------------------------------------------------------------------
// "drag_motion"
// ----------------------------------------------------------------------------

static gboolean target_drag_motion( GtkWidget *WXUNUSED(widget),
                                    GdkDragContext *context,
                                    gint x,
                                    gint y,
                                    guint time,
                                    wxDropTarget *drop_target )
{
    if (g_isIdle) wxapp_install_idle_handler();

    /* Owen Taylor: "if the coordinates not in a drop zone,
       return FALSE, otherwise call gtk_drag_status() and
       return TRUE" */

    /* inform the wxDropTarget about the current GdkDragContext.
       this is only valid for the duration of this call */
    drop_target->SetDragContext( context );

    // GTK+ always supposes that we want to copy the data by default while we
    // might want to move it, so examine not only suggested_action - which is
    // only good if we don't have our own preferences - but also the actions
    // field
    wxDragResult result;
    if ( (gs_flagsForDrag & wxDrag_DefaultMove) == wxDrag_DefaultMove &&
            (context->actions & GDK_ACTION_MOVE ) )
    {
        // move is requested by the program and allowed by GTK+ - do it, even
        // though suggested_action may be currently wxDragCopy
        result = wxDragMove;
    }
    else // use whatever GTK+ says we should
    {
        result = ConvertFromGTK(context->suggested_action);

        if ( (result == wxDragMove) && !(gs_flagsForDrag & wxDrag_AllowMove) )
        {
            // we're requested to move but we can't
            result = wxDragCopy;
        }
    }

    if (drop_target->m_firstMotion)
    {
        /* the first "drag_motion" event substitutes a "drag_enter" event */
        result = drop_target->OnEnter( x, y, result );
    }
    else
    {
        /* give program a chance to react (i.e. to say no by returning FALSE) */
        result = drop_target->OnDragOver( x, y, result );
    }

    bool ret = wxIsDragResultOk( result );
    if (ret)
    {
        GdkDragAction action;
        if (result == wxDragCopy)
            action = GDK_ACTION_COPY;
        else if (result == wxDragLink)
            action = GDK_ACTION_LINK;
        else
            action = GDK_ACTION_MOVE;

        gdk_drag_status( context, action, time );
    }

    /* after this, invalidate the drop_target's GdkDragContext */
    drop_target->SetDragContext( (GdkDragContext*) NULL );

    /* this has to be done because GDK has no "drag_enter" event */
    drop_target->m_firstMotion = FALSE;

    return ret;
}

// ----------------------------------------------------------------------------
// "drag_drop"
// ----------------------------------------------------------------------------

static gboolean target_drag_drop( GtkWidget *widget,
                                  GdkDragContext *context,
                                  gint x,
                                  gint y,
                                  guint time,
                                  wxDropTarget *drop_target )
{
    if (g_isIdle) wxapp_install_idle_handler();

    /* Owen Taylor: "if the drop is not in a drop zone,
       return FALSE, otherwise, if you aren't accepting
       the drop, call gtk_drag_finish() with success == FALSE
       otherwise call gtk_drag_data_get()" */

//    printf( "drop.\n" );

    /* this seems to make a difference between not accepting
       due to wrong target area and due to wrong format. let
       us hope that this is not required.. */

    /* inform the wxDropTarget about the current GdkDragContext.
       this is only valid for the duration of this call */
    drop_target->SetDragContext( context );

    /* inform the wxDropTarget about the current drag widget.
       this is only valid for the duration of this call */
    drop_target->SetDragWidget( widget );

    /* inform the wxDropTarget about the current drag time.
       this is only valid for the duration of this call */
    drop_target->SetDragTime( time );

/*
    wxDragResult result = wxDragMove;
    if (context->suggested_action == GDK_ACTION_COPY) result = wxDragCopy;
*/

    /* reset the block here as someone might very well
       show a dialog as a reaction to a drop and this
       wouldn't work without events */
    g_blockEventsOnDrag = FALSE;

    bool ret = drop_target->OnDrop( x, y );

    if (!ret)
    {
        wxLogTrace(TRACE_DND, wxT( "Drop target: OnDrop returned FALSE") );

        /* cancel the whole thing */
        gtk_drag_finish( context,
                          FALSE,        /* no success */
                          FALSE,        /* don't delete data on dropping side */
                          time );
    }
    else
    {
        wxLogTrace(TRACE_DND, wxT( "Drop target: OnDrop returned TRUE") );

#if wxUSE_THREADS
        /* disable GUI threads */
#endif

        GdkAtom format = drop_target->GetMatchingPair();

        // this does happen somehow, see bug 555111
        wxCHECK_MSG( format, FALSE, _T("no matching GdkAtom for format?") )

/*
        GdkDragAction action = GDK_ACTION_MOVE;
        if (result == wxDragCopy) action == GDK_ACTION_COPY;
        context->action = action;
*/
        /* this should trigger an "drag_data_received" event */
        gtk_drag_get_data( widget,
                           context,
                           format,
                           time );

#if wxUSE_THREADS
        /* re-enable GUI threads */
#endif
    }

    /* after this, invalidate the drop_target's GdkDragContext */
    drop_target->SetDragContext( (GdkDragContext*) NULL );

    /* after this, invalidate the drop_target's drag widget */
    drop_target->SetDragWidget( (GtkWidget*) NULL );

    /* this has to be done because GDK has no "drag_enter" event */
    drop_target->m_firstMotion = TRUE;

    return ret;
}

// ----------------------------------------------------------------------------
// "drag_data_received"
// ----------------------------------------------------------------------------

static void target_drag_data_received( GtkWidget *WXUNUSED(widget),
                                       GdkDragContext *context,
                                       gint x,
                                       gint y,
                                       GtkSelectionData *data,
                                       guint WXUNUSED(info),
                                       guint time,
                                       wxDropTarget *drop_target )
{
    if (g_isIdle) wxapp_install_idle_handler();

    /* Owen Taylor: "call gtk_drag_finish() with
       success == TRUE" */

    if ((data->length <= 0) || (data->format != 8))
    {
        /* negative data length and non 8-bit data format
           qualifies for junk */
        gtk_drag_finish (context, FALSE, FALSE, time);

        return;
    }

    wxLogTrace(TRACE_DND, wxT( "Drop target: data received event") );

    /* inform the wxDropTarget about the current GtkSelectionData.
       this is only valid for the duration of this call */
    drop_target->SetDragData( data );

    wxDragResult result = ConvertFromGTK(context->suggested_action);

    if ( wxIsDragResultOk( drop_target->OnData( x, y, result ) ) )
    {
        wxLogTrace(TRACE_DND, wxT( "Drop target: OnData returned TRUE") );

        /* tell GTK that data transfer was successfull */
        gtk_drag_finish( context, TRUE, FALSE, time );
    }
    else
    {
        wxLogTrace(TRACE_DND, wxT( "Drop target: OnData returned FALSE") );

        /* tell GTK that data transfer was not successfull */
        gtk_drag_finish( context, FALSE, FALSE, time );
    }

    /* after this, invalidate the drop_target's drag data */
    drop_target->SetDragData( (GtkSelectionData*) NULL );
}

//----------------------------------------------------------------------------
// wxDropTarget
//----------------------------------------------------------------------------

wxDropTarget::wxDropTarget( wxDataObject *data )
            : wxDropTargetBase( data )
{
    m_firstMotion = TRUE;
    m_dragContext = (GdkDragContext*) NULL;
    m_dragWidget = (GtkWidget*) NULL;
    m_dragData = (GtkSelectionData*) NULL;
    m_dragTime = 0;
}

wxDragResult wxDropTarget::OnDragOver( wxCoord WXUNUSED(x),
                                       wxCoord WXUNUSED(y),
                                       wxDragResult def )
{
    // GetMatchingPair() checks for m_dataObject too, no need to do it here

    // disable the debug message from GetMatchingPair() - there are too many
    // of them otherwise
#ifdef __WXDEBUG__
    wxLogNull noLog;
#endif // Debug

    return (GetMatchingPair() != (GdkAtom) 0) ? def : wxDragNone;
}

bool wxDropTarget::OnDrop( wxCoord WXUNUSED(x), wxCoord WXUNUSED(y) )
{
    if (!m_dataObject)
        return FALSE;

    return (GetMatchingPair() != (GdkAtom) 0);
}

wxDragResult wxDropTarget::OnData( wxCoord WXUNUSED(x), wxCoord WXUNUSED(y),
                                   wxDragResult def )
{
    if (!m_dataObject)
        return wxDragNone;

    if (GetMatchingPair() == (GdkAtom) 0)
        return wxDragNone;

    return GetData() ? def : wxDragNone;
}

GdkAtom wxDropTarget::GetMatchingPair()
{
    if (!m_dataObject)
        return (GdkAtom) 0;

    if (!m_dragContext)
        return (GdkAtom) 0;

    GList *child = m_dragContext->targets;
    while (child)
    {
        GdkAtom formatAtom = (GdkAtom) GPOINTER_TO_INT(child->data);
        wxDataFormat format( formatAtom );

#ifdef __WXDEBUG__
        wxLogTrace(TRACE_DND, wxT("Drop target: drag has format: %s"),
                   format.GetId().c_str());
#endif // Debug

        if (m_dataObject->IsSupportedFormat( format ))
            return formatAtom;

        child = child->next;
    }

    return (GdkAtom) 0;
}

bool wxDropTarget::GetData()
{
    if (!m_dragData)
        return FALSE;

    if (!m_dataObject)
        return FALSE;

    wxDataFormat dragFormat( m_dragData->target );

    if (!m_dataObject->IsSupportedFormat( dragFormat ))
        return FALSE;

    m_dataObject->SetData( dragFormat, (size_t)m_dragData->length, (const void*)m_dragData->data );

    return TRUE;
}

void wxDropTarget::UnregisterWidget( GtkWidget *widget )
{
    wxCHECK_RET( widget != NULL, wxT("unregister widget is NULL") );

    gtk_drag_dest_unset( widget );

    gtk_signal_disconnect_by_func( GTK_OBJECT(widget),
                      GTK_SIGNAL_FUNC(target_drag_leave), (gpointer) this );

    gtk_signal_disconnect_by_func( GTK_OBJECT(widget),
                      GTK_SIGNAL_FUNC(target_drag_motion), (gpointer) this );

    gtk_signal_disconnect_by_func( GTK_OBJECT(widget),
                      GTK_SIGNAL_FUNC(target_drag_drop), (gpointer) this );

    gtk_signal_disconnect_by_func( GTK_OBJECT(widget),
                      GTK_SIGNAL_FUNC(target_drag_data_received), (gpointer) this );
}

void wxDropTarget::RegisterWidget( GtkWidget *widget )
{
    wxCHECK_RET( widget != NULL, wxT("register widget is NULL") );

    /* gtk_drag_dest_set() determines what default behaviour we'd like
       GTK to supply. we don't want to specify out targets (=formats)
       or actions in advance (i.e. not GTK_DEST_DEFAULT_MOTION and
       not GTK_DEST_DEFAULT_DROP). instead we react individually to
       "drag_motion" and "drag_drop" events. this makes it possible
       to allow dropping on only a small area. we should set
       GTK_DEST_DEFAULT_HIGHLIGHT as this will switch on the nice
       highlighting if dragging over standard controls, but this
       seems to be broken without the other two. */

    gtk_drag_dest_set( widget,
                       (GtkDestDefaults) 0,         /* no default behaviour */
                       (GtkTargetEntry*) NULL,      /* we don't supply any formats here */
                       0,                           /* number of targets = 0 */
                       (GdkDragAction) 0 );         /* we don't supply any actions here */

    gtk_signal_connect( GTK_OBJECT(widget), "drag_leave",
                      GTK_SIGNAL_FUNC(target_drag_leave), (gpointer) this );

    gtk_signal_connect( GTK_OBJECT(widget), "drag_motion",
                      GTK_SIGNAL_FUNC(target_drag_motion), (gpointer) this );

    gtk_signal_connect( GTK_OBJECT(widget), "drag_drop",
                      GTK_SIGNAL_FUNC(target_drag_drop), (gpointer) this );

    gtk_signal_connect( GTK_OBJECT(widget), "drag_data_received",
                      GTK_SIGNAL_FUNC(target_drag_data_received), (gpointer) this );
}

//----------------------------------------------------------------------------
// "drag_data_get"
//----------------------------------------------------------------------------

static void
source_drag_data_get  (GtkWidget          *WXUNUSED(widget),
                       GdkDragContext     *WXUNUSED(context),
                       GtkSelectionData   *selection_data,
                       guint               WXUNUSED(info),
                       guint               WXUNUSED(time),
                       wxDropSource       *drop_source )
{
    if (g_isIdle) wxapp_install_idle_handler();

    wxDataFormat format( selection_data->target );

    wxLogTrace(TRACE_DND, wxT("Drop source: format requested: %s"),
               format.GetId().c_str());

    drop_source->m_retValue = wxDragCancel;

    wxDataObject *data = drop_source->GetDataObject();

    if (!data)
    {
        wxLogTrace(TRACE_DND, wxT("Drop source: no data object") );
        return;
    }

    if (!data->IsSupportedFormat(format))
    {
        wxLogTrace(TRACE_DND, wxT("Drop source: unsupported format") );
        return;
    }

    if (data->GetDataSize(format) == 0)
    {
        wxLogTrace(TRACE_DND, wxT("Drop source: empty data") );
        return;
    }

    size_t size = data->GetDataSize(format);

//  printf( "data size: %d.\n", (int)data_size );

    guchar *d = new guchar[size];

    if (!data->GetDataHere( format, (void*)d ))
    {
        delete[] d;
        return;
    }

#if wxUSE_THREADS
    /* disable GUI threads */
#endif

                gtk_selection_data_set( selection_data,
                                        selection_data->target,
                                        8,   // 8-bit
                                        d,
                                        size );

#if wxUSE_THREADS
    /* enable GUI threads */
#endif

    delete[] d;
}

//----------------------------------------------------------------------------
// "drag_data_delete"
//----------------------------------------------------------------------------

static void source_drag_data_delete( GtkWidget *WXUNUSED(widget),
                                     GdkDragContext *context,
                                     wxDropSource *WXUNUSED(drop_source) )
{
    if (g_isIdle)
        wxapp_install_idle_handler();

    // printf( "Drag source: drag_data_delete\n" );
}

//----------------------------------------------------------------------------
// "drag_begin"
//----------------------------------------------------------------------------

static void source_drag_begin( GtkWidget          *WXUNUSED(widget),
                               GdkDragContext     *WXUNUSED(context),
                               wxDropSource       *WXUNUSED(drop_source) )
{
    if (g_isIdle)
        wxapp_install_idle_handler();

    // printf( "Drag source: drag_begin.\n" );
}

//----------------------------------------------------------------------------
// "drag_end"
//----------------------------------------------------------------------------

static void source_drag_end( GtkWidget          *WXUNUSED(widget),
                             GdkDragContext     *WXUNUSED(context),
                             wxDropSource       *drop_source )
{
    if (g_isIdle) wxapp_install_idle_handler();

    // printf( "Drag source: drag_end.\n" );

    drop_source->m_waiting = FALSE;
}

//-----------------------------------------------------------------------------
// "configure_event" from m_iconWindow
//-----------------------------------------------------------------------------

static gint
gtk_dnd_window_configure_callback( GtkWidget *WXUNUSED(widget), GdkEventConfigure *WXUNUSED(event), wxDropSource *source )
{
    if (g_isIdle)
        wxapp_install_idle_handler();

    source->GiveFeedback( ConvertFromGTK(source->m_dragContext->action) );

    return 0;
}

//---------------------------------------------------------------------------
// wxDropSource
//---------------------------------------------------------------------------

wxDropSource::wxDropSource(wxWindow *win,
                           const wxIcon &iconCopy,
                           const wxIcon &iconMove,
                           const wxIcon &iconNone)
{
    m_waiting = TRUE;

    m_iconWindow = (GtkWidget*) NULL;

    m_window = win;
    m_widget = win->m_widget;
    if (win->m_wxwindow) m_widget = win->m_wxwindow;

    m_retValue = wxDragCancel;

    SetIcons(iconCopy, iconMove, iconNone);
}

wxDropSource::wxDropSource(wxDataObject& data,
                           wxWindow *win,
                           const wxIcon &iconCopy,
                           const wxIcon &iconMove,
                           const wxIcon &iconNone)
{
    m_waiting = TRUE;

    SetData( data );

    m_iconWindow = (GtkWidget*) NULL;

    m_window = win;
    m_widget = win->m_widget;
    if (win->m_wxwindow) m_widget = win->m_wxwindow;

    m_retValue = wxDragCancel;

    SetIcons(iconCopy, iconMove, iconNone);
}

void wxDropSource::SetIcons(const wxIcon &iconCopy,
                            const wxIcon &iconMove,
                            const wxIcon &iconNone)
{
    m_iconCopy = iconCopy;
    m_iconMove = iconMove;
    m_iconNone = iconNone;

    if ( !m_iconCopy.Ok() )
        m_iconCopy = wxIcon(page_xpm);
    if ( !m_iconMove.Ok() )
        m_iconMove = m_iconCopy;
    if ( !m_iconNone.Ok() )
        m_iconNone = m_iconCopy;
}

wxDropSource::~wxDropSource()
{
}

void wxDropSource::PrepareIcon( int action, GdkDragContext *context )
{
    // get the right icon to display
    wxIcon *icon = NULL;
    if ( action & GDK_ACTION_MOVE )
        icon = &m_iconMove;
    else if ( action & GDK_ACTION_COPY )
        icon = &m_iconCopy;
    else
        icon = &m_iconNone;

    GdkBitmap *mask;
    if ( icon->GetMask() )
        mask = icon->GetMask()->GetBitmap();
    else
        mask = (GdkBitmap *)NULL;

    GdkPixmap *pixmap = icon->GetPixmap();

    gint width,height;
    gdk_window_get_size (pixmap, &width, &height);

    GdkColormap *colormap = gtk_widget_get_colormap( m_widget );
#ifndef __WXGTK20__
    gtk_widget_push_visual (gdk_colormap_get_visual (colormap));
#endif
    gtk_widget_push_colormap (colormap);

    m_iconWindow = gtk_window_new (GTK_WINDOW_POPUP);
    gtk_widget_set_events (m_iconWindow, GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK);
    gtk_widget_set_app_paintable (GTK_WIDGET (m_iconWindow), TRUE);

#ifndef __WXGTK20__
    gtk_widget_pop_visual ();
#endif
    gtk_widget_pop_colormap ();

    gtk_widget_set_usize (m_iconWindow, width, height);
    gtk_widget_realize (m_iconWindow);

    gtk_signal_connect( GTK_OBJECT(m_iconWindow), "configure_event",
        GTK_SIGNAL_FUNC(gtk_dnd_window_configure_callback), (gpointer)this );

    gdk_window_set_back_pixmap (m_iconWindow->window, pixmap, FALSE);

    if (mask)
        gtk_widget_shape_combine_mask (m_iconWindow, mask, 0, 0);

    gtk_drag_set_icon_widget( context, m_iconWindow, 0, 0 );
}

wxDragResult wxDropSource::DoDragDrop(int flags)
{
    wxCHECK_MSG( m_data && m_data->GetFormatCount(), wxDragNone,
                 wxT("Drop source: no data") );

    // still in drag
    if (g_blockEventsOnDrag)
        return wxDragNone;

    // disabled for now
    g_blockEventsOnDrag = TRUE;

    RegisterWindow();

    m_waiting = TRUE;

    GtkTargetList *target_list = gtk_target_list_new( (GtkTargetEntry*) NULL, 0 );

    wxDataFormat *array = new wxDataFormat[ m_data->GetFormatCount() ];
    m_data->GetAllFormats( array );
    size_t count = m_data->GetFormatCount();
    for (size_t i = 0; i < count; i++)
    {
        GdkAtom atom = array[i];
        wxLogTrace(TRACE_DND, wxT("Drop source: Supported atom %s"), gdk_atom_name( atom ));
        gtk_target_list_add( target_list, atom, 0, 0 );
    }
    delete[] array;

    GdkEventMotion event;
    event.window = m_widget->window;
    int x = 0;
    int y = 0;
    GdkModifierType state;
    gdk_window_get_pointer( event.window, &x, &y, &state );
    event.x = x;
    event.y = y;
    event.state = state;
    event.time = (guint32)GDK_CURRENT_TIME;

    /* GTK wants to know which button was pressed which caused the dragging */
    int button_number = 0;
    if (event.state & GDK_BUTTON1_MASK) button_number = 1;
    else if (event.state & GDK_BUTTON2_MASK) button_number = 2;
    else if (event.state & GDK_BUTTON3_MASK) button_number = 3;

#if wxUSE_THREADS
    /* disable GUI threads */
#endif

    /* don't start dragging if no button is down */
    if (button_number)
    {
        int action = GDK_ACTION_COPY;
        if ( flags & wxDrag_AllowMove )
            action |= GDK_ACTION_MOVE;

        // VZ: as we already use g_blockEventsOnDrag it shouldn't be that bad
        //     to use a global to pass the flags to the drop target but I'd
        //     surely prefer a better way to do it
        gs_flagsForDrag = flags;

        GdkDragContext *context = gtk_drag_begin( m_widget,
                target_list,
                (GdkDragAction)action,
                button_number,  /* number of mouse button which started drag */
                (GdkEvent*) &event );

        m_dragContext = context;

        PrepareIcon( action, context );

        while (m_waiting)
            gtk_main_iteration();

        m_retValue = ConvertFromGTK(context->action);
        if ( m_retValue == wxDragNone )
            m_retValue = wxDragCancel;
    }

#if wxUSE_THREADS
    /* re-enable GUI threads */
#endif

    g_blockEventsOnDrag = FALSE;

    UnregisterWindow();

    return m_retValue;
}

void wxDropSource::RegisterWindow()
{
    if (!m_widget) return;

    gtk_signal_connect( GTK_OBJECT(m_widget), "drag_data_get",
                      GTK_SIGNAL_FUNC (source_drag_data_get), (gpointer) this);
    gtk_signal_connect (GTK_OBJECT(m_widget), "drag_data_delete",
                      GTK_SIGNAL_FUNC (source_drag_data_delete),  (gpointer) this );
    gtk_signal_connect (GTK_OBJECT(m_widget), "drag_begin",
                      GTK_SIGNAL_FUNC (source_drag_begin),  (gpointer) this );
    gtk_signal_connect (GTK_OBJECT(m_widget), "drag_end",
                      GTK_SIGNAL_FUNC (source_drag_end),  (gpointer) this );

}

void wxDropSource::UnregisterWindow()
{
    if (!m_widget) return;

    gtk_signal_disconnect_by_func( GTK_OBJECT(m_widget),
                      GTK_SIGNAL_FUNC(source_drag_data_get), (gpointer) this );
    gtk_signal_disconnect_by_func( GTK_OBJECT(m_widget),
                      GTK_SIGNAL_FUNC(source_drag_data_delete), (gpointer) this );
    gtk_signal_disconnect_by_func( GTK_OBJECT(m_widget),
                      GTK_SIGNAL_FUNC(source_drag_begin), (gpointer) this );
    gtk_signal_disconnect_by_func( GTK_OBJECT(m_widget),
                      GTK_SIGNAL_FUNC(source_drag_end), (gpointer) this );
}

#endif
      // wxUSE_DRAG_AND_DROP
