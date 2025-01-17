/////////////////////////////////////////////////////////////////////////////
// Name:        cursor.h
// Purpose:     wxCursor class
// Author:      AUTHOR
// Modified by:
// Created:     ??/??/98
// RCS-ID:      $Id: cursor.h,v 1.5.2.1 2003/03/04 12:04:55 SC Exp $
// Copyright:   (c) AUTHOR
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_CURSOR_H_
#define _WX_CURSOR_H_

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "cursor.h"
#endif

#include "wx/bitmap.h"

class WXDLLEXPORT wxCursorRefData: public wxBitmapRefData
{
    DECLARE_NO_COPY_CLASS(wxCursorRefData)
        
    friend class WXDLLEXPORT wxBitmap;
    friend class WXDLLEXPORT wxCursor;
public:
    wxCursorRefData();
    ~wxCursorRefData();

protected:
    WXHCURSOR     m_hCursor;
    bool        m_disposeHandle;
    bool        m_releaseHandle;
    bool        m_isColorCursor ;
    long        m_themeCursor ;
};

#define M_CURSORDATA ((wxCursorRefData *)m_refData)
#define M_CURSORHANDLERDATA ((wxCursorRefData *)bitmap->m_refData)

// Cursor
class WXDLLEXPORT wxCursor: public wxBitmap
{
  DECLARE_DYNAMIC_CLASS(wxCursor)

public:
  wxCursor();

  // Copy constructors
  wxCursor(const wxCursor& cursor)
      : wxBitmap()
  { Ref(cursor); }

  wxCursor(const char bits[], int width, int height, int hotSpotX = -1, int hotSpotY = -1,
    const char maskBits[] = NULL);

    wxCursor(const wxImage & image) ;
    wxCursor(const char **bits) ;
    wxCursor(char **bits) ;
  wxCursor(const wxString& name, long flags = wxBITMAP_TYPE_MACCURSOR_RESOURCE,
   int hotSpotX = 0, int hotSpotY = 0);

  wxCursor(int cursor_type);
  ~wxCursor();

	bool CreateFromXpm(const char **bits) ;
  virtual bool Ok() const { return (m_refData != NULL && ( M_CURSORDATA->m_hCursor != NULL || M_CURSORDATA->m_themeCursor != -1 ) ) ; }

  inline wxCursor& operator = (const wxCursor& cursor) { if (*this == cursor) return (*this); Ref(cursor); return *this; }
  inline bool operator == (const wxCursor& cursor) { return m_refData == cursor.m_refData; }
  inline bool operator != (const wxCursor& cursor) { return m_refData != cursor.m_refData; }

	void MacInstall() const ;

  void SetHCURSOR(WXHCURSOR cursor);
  inline WXHCURSOR GetHCURSOR() const { return (M_CURSORDATA ? M_CURSORDATA->m_hCursor : 0); }
private :
    void CreateFromImage(const wxImage & image) ;
};

extern WXDLLEXPORT void wxSetCursor(const wxCursor& cursor);

#endif
    // _WX_CURSOR_H_
