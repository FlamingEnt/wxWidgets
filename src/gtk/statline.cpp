/////////////////////////////////////////////////////////////////////////////
// Name:        statline.cpp
// Purpose:
// Author:      Robert Roebling
// Id:          $Id: statline.cpp,v 1.9 2000/08/08 06:11:45 cvsuser Exp $
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "statline.h"
#endif

#include "wx/statline.h"

#if wxUSE_STATLINE

#include "gdk/gdk.h"
#include "gtk/gtk.h"

//-----------------------------------------------------------------------------
// wxStaticLine
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxStaticLine, wxControl)

wxStaticLine::wxStaticLine()
{
}

wxStaticLine::wxStaticLine( wxWindow *parent, wxWindowID id,
                            const wxPoint &pos, const wxSize &size,
                            long style, const wxString &name )
{
    Create( parent, id, pos, size, style, name );
}

bool wxStaticLine::Create( wxWindow *parent, wxWindowID id,
                           const wxPoint &pos, const wxSize &size,
                           long style, const wxString &name )
{
    m_needParent = TRUE;

    if (!PreCreation( parent, pos, size ) ||
        !CreateBase( parent, id, pos, size, style, wxDefaultValidator, name ))
    {
        wxFAIL_MSG( wxT("wxStaticLine creation failed") );
        return FALSE;
    }

    if ( IsVertical() )
    {
        m_widget = gtk_vseparator_new();
        if (size.x == -1)
        {
            wxSize new_size( size );
            new_size.x = 4;
            SetSize( new_size );
        }
    }
    else
    {
        m_widget = gtk_hseparator_new();
        if (size.y == -1)
        {
            wxSize new_size( size );
            new_size.y = 4;
            SetSize( new_size );
        }
    }

    m_parent->DoAddChild( this );

    PostCreation();

    Show( TRUE );

    return TRUE;
}

#endif
