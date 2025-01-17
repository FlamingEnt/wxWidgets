/////////////////////////////////////////////////////////////////////////////
// Name:        bitmap.h
// Purpose:     wxBitmap class
// Author:      AUTHOR
// Modified by:
// Created:     ??/??/98
// RCS-ID:      $Id: bitmap.h,v 1.18.2.1 2003/02/02 13:18:21 SC Exp $
// Copyright:   (c) AUTHOR
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_BITMAP_H_
#define _WX_BITMAP_H_

#if defined(__GNUG__) && !defined(__APPLE__)
  #pragma interface "bitmap.h"
#endif

#include "wx/palette.h"

// Bitmap
class WXDLLEXPORT wxDC;
class WXDLLEXPORT wxControl;
class WXDLLEXPORT wxBitmap;
class WXDLLEXPORT wxBitmapHandler;
class WXDLLEXPORT wxIcon;
class WXDLLEXPORT wxCursor;
class WXDLLEXPORT wxImage;

// A mask is a mono bitmap used for drawing bitmaps
// transparently.
class WXDLLEXPORT wxMask: public wxObject
{
    DECLARE_DYNAMIC_CLASS(wxMask)
    DECLARE_NO_COPY_CLASS(wxMask)

public:
  wxMask();

  // Construct a mask from a bitmap and a colour indicating
  // the transparent area
  wxMask(const wxBitmap& bitmap, const wxColour& colour);

  // Construct a mask from a bitmap and a palette index indicating
  // the transparent area
  wxMask(const wxBitmap& bitmap, int paletteIndex);

  // Construct a mask from a mono bitmap (copies the bitmap).
  wxMask(const wxBitmap& bitmap);

  ~wxMask();

  bool Create(const wxBitmap& bitmap, const wxColour& colour);
  bool Create(const wxBitmap& bitmap, int paletteIndex);
  bool Create(const wxBitmap& bitmap);

  // Implementation
  bool PointMasked(int x, int y);
  inline WXHBITMAP GetMaskBitmap() const { return m_maskBitmap; }
  inline void SetMaskBitmap(WXHBITMAP bmp) { m_maskBitmap = bmp; }
protected:
  WXHBITMAP m_maskBitmap;
};

enum { kMacBitmapTypeUnknownType , kMacBitmapTypeGrafWorld, kMacBitmapTypePict , kMacBitmapTypeIcon } ;

class WXDLLEXPORT wxBitmapRefData: public wxGDIRefData
{
    DECLARE_NO_COPY_CLASS(wxBitmapRefData)
        
    friend class WXDLLEXPORT wxBitmap;
    friend class WXDLLEXPORT wxIcon;
    friend class WXDLLEXPORT wxCursor;
public:
    wxBitmapRefData();
    ~wxBitmapRefData();

public:
  int           m_width;
  int           m_height;
  int           m_depth;
  bool          m_ok;
  int           m_numColors;
  wxPalette     m_bitmapPalette;
  int           m_quality;

  int	        m_bitmapType ;
  WXHMETAFILE	m_hPict ;
  WXHBITMAP     m_hBitmap;
  WXHICON       m_hIcon ;
  wxMask *      m_bitmapMask; // Optional mask
};

#define M_BITMAPDATA ((wxBitmapRefData *)m_refData)

class WXDLLEXPORT wxBitmapHandler: public wxBitmapHandlerBase
{
  DECLARE_DYNAMIC_CLASS(wxBitmapHandler)
public:
  wxBitmapHandler() : m_name(), m_extension(), m_type(0) { }
  virtual ~wxBitmapHandler();

  virtual bool Create(wxBitmap *bitmap, void *data, long flags, int width, int height, int depth = 1);
  virtual bool LoadFile(wxBitmap *bitmap, const wxString& name, long flags,
      int desiredWidth, int desiredHeight);
  virtual bool SaveFile(const wxBitmap *bitmap, const wxString& name, int type, const wxPalette *palette = NULL);

  void SetName(const wxString& name) { m_name = name; }
  void SetExtension(const wxString& ext) { m_extension = ext; }
  void SetType(long type) { m_type = type; }
  wxString GetName() const { return m_name; }
  wxString GetExtension() const { return m_extension; }
  long GetType() const { return m_type; }

protected:
  wxString  m_name;
  wxString  m_extension;
  long      m_type;
};

#define M_BITMAPHANDLERDATA ((wxBitmapRefData *)bitmap->GetRefData())

class WXDLLEXPORT wxBitmap: public wxBitmapBase
{
  DECLARE_DYNAMIC_CLASS(wxBitmap)

  friend class WXDLLEXPORT wxBitmapHandler;

public:
  wxBitmap(); // Platform-specific

  // Copy constructors
  wxBitmap(const wxBitmap& bitmap)
      : wxBitmapBase()
  { Ref(bitmap); }

  // Initialize with raw data.
  wxBitmap(const char bits[], int width, int height, int depth = 1);

  // Initialize with XPM data
  bool CreateFromXpm(const char **bits);
  wxBitmap(const char **bits);
  wxBitmap(char **bits);

  // Load a file or resource
  wxBitmap(const wxString& name, wxBitmapType type = wxBITMAP_TYPE_PICT_RESOURCE);

  // Constructor for generalised creation from data
  wxBitmap(void *data, wxBitmapType type, int width, int height, int depth = 1);

  // If depth is omitted, will create a bitmap compatible with the display
  wxBitmap(int width, int height, int depth = -1);
  
  // Convert from wxImage:
  wxBitmap(const wxImage& image, int depth = -1);
  
  ~wxBitmap();
  
  wxImage ConvertToImage() const;

  // get the given part of bitmap
  wxBitmap GetSubBitmap( const wxRect& rect ) const;

  virtual bool Create(int width, int height, int depth = -1);
  virtual bool Create(void *data, wxBitmapType type, int width, int height, int depth = 1);
  virtual bool LoadFile(const wxString& name, wxBitmapType type = wxBITMAP_TYPE_BMP_RESOURCE);
  virtual bool SaveFile(const wxString& name, wxBitmapType type, const wxPalette *cmap = NULL) const;

  // copies the contents and mask of the given (colour) icon to the bitmap
  virtual bool CopyFromIcon(const wxIcon& icon);

  bool Ok() const;
  int GetWidth() const;
  int GetHeight() const;
  int GetDepth() const;
  int GetQuality() const;
  void SetWidth(int w);
  void SetHeight(int h);
  void SetDepth(int d);
  void SetQuality(int q);
  void SetOk(bool isOk);

  wxPalette* GetPalette() const;
  void SetPalette(const wxPalette& palette);

  wxMask *GetMask() const;
  void SetMask(wxMask *mask) ;

  int GetBitmapType() const;
  
  inline wxBitmap& operator = (const wxBitmap& bitmap) { if (*this == bitmap) return (*this); Ref(bitmap); return *this; }
  inline bool operator == (const wxBitmap& bitmap) const { return m_refData == bitmap.m_refData; }
  inline bool operator != (const wxBitmap& bitmap) const { return m_refData != bitmap.m_refData; }

  static void InitStandardHandlers();
protected:

  // TODO: Implementation
public:
  void SetHBITMAP(WXHBITMAP bmp);
  WXHBITMAP GetHBITMAP() const;
  void SetHICON(WXHICON ico);
  inline WXHICON GetHICON() const { return (M_BITMAPDATA ? M_BITMAPDATA->m_hIcon : 0); }
  
  WXHMETAFILE GetPict() const;
  void SetPict(WXHMETAFILE pict) ;
  bool FreeResource(bool force = FALSE);
};
#endif
  // _WX_BITMAP_H_
