///////////////////////////////////////////////////////////////////////////////
// Name:        dnd.h
// Purpose:     declaration of wxDropTarget, wxDropSource classes
// Author:      Julian Smart
// RCS-ID:      $Id: dnd.h,v 1.2 2002/04/28 14:30:53 VZ Exp $
// Copyright:   (c) 1998 Vadim Zeitlin, Robert Roebling, Julian Smart
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////


#ifndef _WX_DND_H_
#define _WX_DND_H_

#ifdef __GNUG__
#pragma interface
#endif

#include "wx/defs.h"

#if wxUSE_DRAG_AND_DROP

#include "wx/object.h"
#include "wx/string.h"
#include "wx/dataobj.h"
#include "wx/cursor.h"

//-------------------------------------------------------------------------
// classes
//-------------------------------------------------------------------------

class WXDLLEXPORT wxWindow;

class WXDLLEXPORT wxDropTarget;
class WXDLLEXPORT wxTextDropTarget;
class WXDLLEXPORT wxFileDropTarget;
class WXDLLEXPORT wxPrivateDropTarget;

class WXDLLEXPORT wxDropSource;

//-------------------------------------------------------------------------
// wxDropTarget
//-------------------------------------------------------------------------

class WXDLLEXPORT wxDropTarget: public wxObject
{
public:
    
    wxDropTarget();
    ~wxDropTarget();
    
    virtual void OnEnter() { }
    virtual void OnLeave() { }
    virtual bool OnDrop( long x, long y, const void *data, size_t size ) = 0;
    
    // Override these to indicate what kind of data you support: 
    
    virtual size_t GetFormatCount() const = 0;
    virtual wxDataFormat GetFormat(size_t n) const = 0;
    
    // implementation
};

//-------------------------------------------------------------------------
// wxTextDropTarget
//-------------------------------------------------------------------------

class WXDLLEXPORT wxTextDropTarget: public wxDropTarget
{
public:
    
    wxTextDropTarget() {};
    virtual bool OnDrop( long x, long y, const void *data, size_t size );
    virtual bool OnDropText( long x, long y, const char *psz );
    
protected:
    
    virtual size_t GetFormatCount() const;
    virtual wxDataFormat GetFormat(size_t n) const;
};

//-------------------------------------------------------------------------
// wxPrivateDropTarget
//-------------------------------------------------------------------------

class WXDLLEXPORT wxPrivateDropTarget: public wxDropTarget
{
public:
    
    wxPrivateDropTarget();
    
    // you have to override OnDrop to get at the data
    
    // the string ID identifies the format of clipboard or DnD data. a word
    // processor would e.g. add a wxTextDataObject and a wxPrivateDataObject
    // to the clipboard - the latter with the Id "WXWORD_FORMAT".
    
    void SetId( const wxString& id )
    { m_id = id; }
    
    wxString GetId()
    { return m_id; }
    
private:
    
    virtual size_t GetFormatCount() const;
    virtual wxDataFormat GetFormat(size_t n) const;
    
    wxString   m_id;
};

// ----------------------------------------------------------------------------
// A drop target which accepts files (dragged from File Manager or Explorer)
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxFileDropTarget: public wxDropTarget
{
public:
    
    wxFileDropTarget() {};
    
    virtual bool OnDrop( long x, long y, const void *data, size_t size );
    virtual bool OnDropFiles( long x, long y, 
        size_t nFiles, const char * const aszFiles[] );
    
protected:
    
    virtual size_t GetFormatCount() const;
    virtual wxDataFormat GetFormat(size_t n) const;
};

//-------------------------------------------------------------------------
// wxDropSource
//-------------------------------------------------------------------------

enum wxDragResult
{
    wxDragError,    // error prevented the d&d operation from completing
        wxDragNone,     // drag target didn't accept the data
        wxDragCopy,     // the data was successfully copied
        wxDragMove,     // the data was successfully moved
        wxDragCancel    // the operation was cancelled by user (not an error)
};

class WXDLLEXPORT wxDropSource: public wxObject
{
public:
    
    wxDropSource( wxWindow *win );
    wxDropSource( wxDataObject &data, wxWindow *win );
    
    ~wxDropSource(void);
    
    void SetData( wxDataObject &data  );
    wxDragResult DoDragDrop(int flags = wxDrag_CopyOnly);
    
    virtual bool GiveFeedback( wxDragResult WXUNUSED(effect), bool WXUNUSED(bScrolling) ) { return TRUE; };
    
    // implementation
#if 0
    void RegisterWindow(void);
    void UnregisterWindow(void);
    
    wxWindow      *m_window;
    wxDragResult   m_retValue;
    wxDataObject  *m_data;
    
    wxCursor      m_defaultCursor;
    wxCursor      m_goaheadCursor;
#endif
};

#endif

// wxUSE_DRAG_AND_DROP

#endif  
//_WX_DND_H_

