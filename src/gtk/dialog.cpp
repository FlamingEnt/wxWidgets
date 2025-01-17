/////////////////////////////////////////////////////////////////////////////
// Name:        dialog.cpp
// Purpose:
// Author:      Robert Roebling
// Id:          $Id: dialog.cpp,v 1.102.2.2 2002/10/13 21:03:45 RR Exp $
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "dialog.h"
#endif

#include "wx/dialog.h"
#include "wx/frame.h"
#include "wx/app.h"
#include "wx/cursor.h"

#include <gdk/gdk.h>
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

#include "wx/gtk/win_gtk.h"

//-----------------------------------------------------------------------------
// idle system
//-----------------------------------------------------------------------------

extern void wxapp_install_idle_handler();
extern bool g_isIdle;
extern int g_openDialogs;

//-----------------------------------------------------------------------------
// wxDialog
//-----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(wxDialog,wxDialogBase)
    EVT_BUTTON  (wxID_OK,       wxDialog::OnOK)
    EVT_BUTTON  (wxID_CANCEL,   wxDialog::OnCancel)
    EVT_BUTTON  (wxID_APPLY,    wxDialog::OnApply)
    EVT_CLOSE   (wxDialog::OnCloseWindow)
END_EVENT_TABLE()

IMPLEMENT_DYNAMIC_CLASS(wxDialog,wxTopLevelWindow)

void wxDialog::Init()
{
    m_returnCode = 0;
    m_sizeSet = FALSE;
    m_modalShowing = FALSE;
    m_themeEnabled = TRUE;
}

wxDialog::wxDialog( wxWindow *parent,
                    wxWindowID id, const wxString &title,
                    const wxPoint &pos, const wxSize &size,
                    long style, const wxString &name )
{
    Init();

    (void)Create( parent, id, title, pos, size, style, name );
}

bool wxDialog::Create( wxWindow *parent,
                       wxWindowID id, const wxString &title,
                       const wxPoint &pos, const wxSize &size,
                       long style, const wxString &name )
{
    SetExtraStyle(GetExtraStyle() | wxTOPLEVEL_EX_DIALOG);

    // all dialogs should have tab traversal enabled
    style |= wxTAB_TRAVERSAL;

    return wxTopLevelWindow::Create(parent, id, title, pos, size, style, name);
}

void wxDialog::OnApply( wxCommandEvent &WXUNUSED(event) )
{
    if (Validate())
        TransferDataFromWindow();
}

void wxDialog::OnCancel( wxCommandEvent &WXUNUSED(event) )
{
    if (IsModal())
    {
        EndModal(wxID_CANCEL);
    }
    else
    {
        SetReturnCode(wxID_CANCEL);
        Show(FALSE);
    }
}

void wxDialog::OnOK( wxCommandEvent &WXUNUSED(event) )
{
    if (Validate() && TransferDataFromWindow())
    {
        if (IsModal())
        {
            EndModal(wxID_OK);
        }
        else
        {
            SetReturnCode(wxID_OK);
            Show(FALSE);
        }
    }
}

void wxDialog::OnPaint( wxPaintEvent& WXUNUSED(event) )
{
    // yes
}

void wxDialog::OnCloseWindow(wxCloseEvent& WXUNUSED(event))
{
    // We'll send a Cancel message by default,
    // which may close the dialog.
    // Check for looping if the Cancel event handler calls Close().

    // Note that if a cancel button and handler aren't present in the dialog,
    // nothing will happen when you close the dialog via the window manager, or
    // via Close().
    // We wouldn't want to destroy the dialog by default, since the dialog may have been
    // created on the stack.
    // However, this does mean that calling dialog->Close() won't delete the dialog
    // unless the handler for wxID_CANCEL does so. So use Destroy() if you want to be
    // sure to destroy the dialog.
    // The default OnCancel (above) simply ends a modal dialog, and hides a modeless dialog.

    static wxList s_closing;

    if (s_closing.Member(this))
        return;   // no loops

    s_closing.Append(this);

    wxCommandEvent cancelEvent(wxEVT_COMMAND_BUTTON_CLICKED, wxID_CANCEL);
    cancelEvent.SetEventObject( this );
    GetEventHandler()->ProcessEvent(cancelEvent);
    s_closing.DeleteObject(this);
}

bool wxDialog::Show( bool show )
{
    if (!show && IsModal())
    {
        EndModal( wxID_CANCEL );
    }

    if (show && !m_sizeSet)
    {
        /* by calling GtkOnSize here, we don't have to call
           either after showing the frame, which would entail
           much ugly flicker nor from within the size_allocate
           handler, because GTK 1.1.X forbids that. */

        GtkOnSize( m_x, m_y, m_width, m_height );
    }

    bool ret = wxWindow::Show( show );

    if (show) InitDialog();

    return ret;
}

bool wxDialog::IsModal() const
{
    return m_modalShowing;
}

void wxDialog::SetModal( bool WXUNUSED(flag) )
{
/*
  if (flag)
    m_windowStyle |= wxDIALOG_MODAL;
  else
    if (m_windowStyle & wxDIALOG_MODAL) m_windowStyle -= wxDIALOG_MODAL;
*/
    wxFAIL_MSG( wxT("wxDialog:SetModal obsolete now") );
}

int wxDialog::ShowModal()
{
    if (IsModal())
    {
       wxFAIL_MSG( wxT("wxDialog:ShowModal called twice") );
       return GetReturnCode();
    }

    // use the apps top level window as parent if none given unless explicitly
    // forbidden
    if ( !GetParent() && !(GetWindowStyleFlag() & wxDIALOG_NO_PARENT) )
    {
        wxWindow *parent = wxTheApp->GetTopWindow();
        if ( parent &&
                parent != this &&
                    parent->IsBeingDeleted() &&
                        !(parent->GetExtraStyle() & wxWS_EX_TRANSIENT) )
        {
            m_parent = parent;
            gtk_window_set_transient_for( GTK_WINDOW(m_widget), GTK_WINDOW(parent->m_widget) );
        }
    }

    wxBusyCursorSuspender cs; // temporarily suppress the busy cursor
    
    Show( TRUE );

    SetFocus();

    m_modalShowing = TRUE;

    g_openDialogs++;

    gtk_grab_add( m_widget );
    gtk_main();
    gtk_grab_remove( m_widget );

    g_openDialogs--;

    return GetReturnCode();
}

void wxDialog::EndModal( int retCode )
{
    SetReturnCode( retCode );

    if (!IsModal())
    {
        wxFAIL_MSG( wxT("wxDialog:EndModal called twice") );
        return;
    }

    m_modalShowing = FALSE;

    gtk_main_quit();

    Show( FALSE );
}
