/////////////////////////////////////////////////////////////////////////////
// Name:        clipbrd.h
// Purpose:     Clipboard functionality.
// Author:      Robert Roebling
// Created:     17/09/98
// RCS-ID:      $Id: clipbrd.h,v 1.2 2002/02/23 21:26:15 RR Exp $
// Copyright:   (c) Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __X11CLIPBOARDH__
#define __X11CLIPBOARDH__

#ifdef __GNUG__
#pragma interface
#endif

#if wxUSE_CLIPBOARD

#include "wx/object.h"
#include "wx/list.h"
#include "wx/dataobj.h"
#include "wx/control.h"
#include "wx/module.h"

// ----------------------------------------------------------------------------
// wxClipboard
// ----------------------------------------------------------------------------

class wxClipboard : public wxClipboardBase
{
public:
    wxClipboard();
    ~wxClipboard();

    // open the clipboard before SetData() and GetData()
    virtual bool Open();

    // close the clipboard after SetData() and GetData()
    virtual void Close();

    // query whether the clipboard is opened
    virtual bool IsOpened() const;

    // set the clipboard data. all other formats will be deleted.
    virtual bool SetData( wxDataObject *data );

    // add to the clipboard data.
    virtual bool AddData( wxDataObject *data );

    // ask if data in correct format is available
    virtual bool IsSupported( const wxDataFormat& format );

    // fill data with data on the clipboard (if available)
    virtual bool GetData( wxDataObject& data );

    // clears wxTheClipboard and the system's clipboard if possible
    virtual void Clear();

    // If primary == TRUE, use primary selection in all further ops,
    // primary == FALSE resets it.
    virtual void UsePrimarySelection(bool primary = TRUE)
        { m_usePrimary = primary; }
    
    // implementation from now on
    bool              m_open;
    bool              m_ownsClipboard;
    bool              m_ownsPrimarySelection;
    wxDataObject     *m_data;

    WXWindow          m_clipboardWidget;  /* for getting and offering data */
    WXWindow          m_targetsWidget;    /* for getting list of supported formats */
    bool              m_waiting;          /* querying data or formats is asynchronous */

    bool              m_formatSupported;
    Atom              m_targetRequested;
    bool              m_usePrimary;
    wxDataObject     *m_receivedData;

private:
    DECLARE_DYNAMIC_CLASS(wxClipboard)
};

#endif
   // wxUSE_CLIPBOARD

#endif
    // __X11CLIPBOARDH__
