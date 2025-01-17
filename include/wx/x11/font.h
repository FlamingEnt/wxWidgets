/////////////////////////////////////////////////////////////////////////////
// Name:        font.h
// Purpose:     wxFont class
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id: font.h,v 1.2.2.2 2002/11/11 09:48:33 JS Exp $
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_FONT_H_
#define _WX_FONT_H_

#ifdef __GNUG__
#pragma interface "font.h"
#endif

class wxXFont;

// Font
class wxFont : public wxFontBase
{
public:
    // ctors and such
    wxFont() { Init(); }
    wxFont(const wxFont& font) { Init(); Ref(font); }
    
    wxFont(int size,
        int family,
        int style,
        int weight,
        bool underlined = FALSE,
        const wxString& face = wxEmptyString,
        wxFontEncoding encoding = wxFONTENCODING_DEFAULT)
    {
        Init();
        
        (void)Create(size, family, style, weight, underlined, face, encoding);
    }
    
    wxFont(const wxNativeFontInfo& info);
    
    bool Create(int size,
        int family,
        int style,
        int weight,
        bool underlined = FALSE,
        const wxString& face = wxEmptyString,
        wxFontEncoding encoding = wxFONTENCODING_DEFAULT);

    // FIXME: I added the ! to make it compile;
    // is this right? - JACS
#if !wxUSE_UNICODE    
    bool Create(const wxString& fontname,
        wxFontEncoding fontenc = wxFONTENCODING_DEFAULT);
#endif
    // DELETEME: no longer seems to be implemented.
    // bool Create(const wxNativeFontInfo& fontinfo);
    
    virtual ~wxFont();
    
    // assignment
    wxFont& operator=(const wxFont& font);
    
    // implement base class pure virtuals
    virtual int GetPointSize() const;
    virtual int GetFamily() const;
    virtual int GetStyle() const;
    virtual int GetWeight() const;
    virtual bool GetUnderlined() const;
    virtual wxString GetFaceName() const;
    virtual wxFontEncoding GetEncoding() const;
    virtual wxNativeFontInfo *GetNativeFontInfo() const;
    
    virtual bool IsFixedWidth() const;
    
    virtual void SetPointSize(int pointSize);
    virtual void SetFamily(int family);
    virtual void SetStyle(int style);
    virtual void SetWeight(int weight);
    virtual void SetFaceName(const wxString& faceName);
    virtual void SetUnderlined(bool underlined);
    virtual void SetEncoding(wxFontEncoding encoding);
    virtual void SetNativeFontInfo( const wxNativeFontInfo& info );
    
    virtual void SetNoAntiAliasing( bool no = TRUE );
    virtual bool GetNoAntiAliasing();
    
    // Implementation

#if wxUSE_PANGO
#else
    // Find an existing, or create a new, XFontStruct
    // based on this wxFont and the given scale. Append the
    // font to list in the private data for future reference.
    
    // TODO This is a fairly basic implementation, that doesn't
    // allow for different facenames, and also doesn't do a mapping
    // between 'standard' facenames (e.g. Arial, Helvetica, Times Roman etc.)
    // and the fonts that are available on a particular system.
    // Maybe we need to scan the user's machine to build up a profile
    // of the fonts and a mapping file.
    
    // Return font struct, and optionally the Motif font list
    wxXFont *GetInternalFont(double scale = 1.0,
        WXDisplay* display = NULL) const;
    
    // Helper function for convenient access of the above.
    WXFontStructPtr GetFontStruct(double scale = 1.0,
        WXDisplay* display = NULL) const;
#endif
    
protected:
    // common part of all ctors
    void Init();
    
    void Unshare();
    
private:
    DECLARE_DYNAMIC_CLASS(wxFont)
};

#endif
// _WX_FONT_H_
