/////////////////////////////////////////////////////////////////////////////
// Name:        dirdlg.cpp
// Purpose:     wxDirDialog
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id: dirdlg.cpp,v 1.2 1999/12/14 23:46:39 VS Exp $
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "dirdlg.h"
#endif

#include "wx/defs.h"
#include "wx/utils.h"
#include "wx/dialog.h"
#include "wx/dirdlg.h"

#include "wx/cmndata.h"

IMPLEMENT_CLASS(wxDirDialog, wxDialog)

wxDirDialog::wxDirDialog(wxWindow *parent, const wxString& message,
        const wxString& defaultPath,
        long style, const wxPoint& pos)
{
    m_message = message;
    m_dialogStyle = style;
    m_parent = parent;
	m_path = defaultPath;
}

int wxDirDialog::ShowModal()
{
    // TODO
	return wxID_CANCEL;
}

