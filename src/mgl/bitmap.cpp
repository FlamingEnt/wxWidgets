/////////////////////////////////////////////////////////////////////////////
// Name:        bitmap.cpp
// Author:      Vaclav Slavik
// RCS-ID:      $Id: bitmap.cpp,v 1.12 2002/08/10 14:51:34 VS Exp $
// Copyright:   (c) 2001-2002 SciTech Software, Inc. (www.scitechsoft.com)
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "bitmap.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/bitmap.h"
#include "wx/icon.h"
#include "wx/filefn.h"
#include "wx/image.h"
#include "wx/dcmemory.h"
#include "wx/utils.h"
#include "wx/log.h"
#include "wx/intl.h"
#include "wx/image.h"
#include "wx/xpmdecod.h"

#include "wx/mgl/private.h"

#include <mgraph.hpp>

//-----------------------------------------------------------------------------
// MGL pixel formats:
//-----------------------------------------------------------------------------

static pixel_format_t gs_pixel_format_15 =
	{0x1F,0x0A,3, 0x1F,0x05,3, 0x1F,0x00,3, 0x01,0x0F,7}; // 555 15bpp
    
static pixel_format_t gs_pixel_format_16 =
	{0x1F,0x0B,3, 0x3F,0x05,2, 0x1F,0x00,3, 0x00,0x00,0}; // 565 16bpp

static pixel_format_t gs_pixel_format_24 =
	{0xFF,0x10,0, 0xFF,0x08,0, 0xFF,0x00,0, 0x00,0x00,0}; // RGB 24bpp

static pixel_format_t gs_pixel_format_32 =
	{0xFF,0x18,0, 0xFF,0x10,0, 0xFF,0x08,0, 0xFF,0x00,0}; // RGBA 32bpp

static pixel_format_t gs_pixel_format_wxImage =
	{0xFF,0x00,0, 0xFF,0x08,0, 0xFF,0x10,0, 0x00,0x00,0}; // RGB 24bpp for wxImage

//-----------------------------------------------------------------------------
// helpers
//-----------------------------------------------------------------------------

// Convert wxColour into it's quantized value in lower-precision
// pixel format (needed for masking by colour).
static wxColour wxQuantizeColour(const wxColour& clr, const wxBitmap& bmp)
{
    pixel_format_t *pf = bmp.GetMGLbitmap_t()->pf;
    
    if ( pf->redAdjust == 0 && pf->greenAdjust == 0 && pf->blueAdjust == 0 )
        return clr;
    else
        return wxColour((clr.Red() >> pf->redAdjust) << pf->redAdjust,
                        (clr.Green() >> pf->greenAdjust) << pf->greenAdjust,
                        (clr.Blue() >> pf->blueAdjust) << pf->blueAdjust);
}



//-----------------------------------------------------------------------------
// wxMask
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxMask,wxObject)

wxMask::wxMask()
{
    m_bitmap = NULL;
}

wxMask::wxMask(const wxBitmap& bitmap, const wxColour& colour)
{
    m_bitmap = NULL;
    Create(bitmap, colour);
}

wxMask::wxMask(const wxBitmap& bitmap, int paletteIndex)
{
    m_bitmap = NULL;
    Create(bitmap, paletteIndex);
}

wxMask::wxMask(const wxBitmap& bitmap)
{
    m_bitmap = NULL;
    Create(bitmap);
}

wxMask::~wxMask()
{
    delete m_bitmap;
}

bool wxMask::Create(const wxBitmap& bitmap, const wxColour& colour)
{
    delete m_bitmap;
    m_bitmap = NULL;
    
    wxColour clr(wxQuantizeColour(colour, bitmap));

    wxImage imgSrc(bitmap.ConvertToImage());
    imgSrc.SetMask(FALSE);
    wxImage image(imgSrc.ConvertToMono(clr.Red(), clr.Green(), clr.Blue()));
    if ( !image.Ok() ) return FALSE;

    m_bitmap = new wxBitmap(image, 1);

    return m_bitmap->Ok();
}

bool wxMask::Create(const wxBitmap& bitmap, int paletteIndex)
{
    unsigned char r,g,b;
    wxPalette *pal = bitmap.GetPalette();

    wxCHECK_MSG( pal, FALSE, wxT("Cannot create mask from bitmap without palette") );
    
    pal->GetRGB(paletteIndex, &r, &g, &b);

    return Create(bitmap, wxColour(r, g, b));
}

bool wxMask::Create(const wxBitmap& bitmap)
{
    delete m_bitmap;
    m_bitmap = NULL;

    wxCHECK_MSG( bitmap.Ok(), FALSE, wxT("Invalid bitmap") );
    wxCHECK_MSG( bitmap.GetDepth() == 1, FALSE, wxT("Cannot create mask from colour bitmap") );

    m_bitmap = new wxBitmap(bitmap);
    return TRUE;
}


//-----------------------------------------------------------------------------
// wxBitmap
//-----------------------------------------------------------------------------

class wxBitmapRefData: public wxObjectRefData
{
public:
    wxBitmapRefData();
    ~wxBitmapRefData();

    int             m_width;
    int             m_height;
    int             m_bpp;
    wxPalette      *m_palette;
    wxMask         *m_mask;
    bitmap_t       *m_bitmap;
};

wxBitmapRefData::wxBitmapRefData()
{
    m_mask = NULL;
    m_width = 0;
    m_height = 0;
    m_bpp = 0;
    m_palette = NULL;
    m_bitmap = NULL;
}

wxBitmapRefData::~wxBitmapRefData()
{
    if ( m_bitmap )
        MGL_unloadBitmap(m_bitmap);
    delete m_mask;
    delete m_palette;
}

//-----------------------------------------------------------------------------

#define M_BMPDATA ((wxBitmapRefData *)m_refData)


IMPLEMENT_ABSTRACT_CLASS(wxBitmapHandler,wxObject)
IMPLEMENT_DYNAMIC_CLASS(wxBitmap,wxBitmapBase)

wxBitmap::wxBitmap(int width, int height, int depth)
{
    Create(width, height, depth);
}


static bitmap_t *MyMGL_createBitmap(int width, int height, 
                                    int bpp, pixel_format_t *pf)
{
    MGLMemoryDC mdc(width, height, bpp, pf);
    return MGL_getBitmapFromDC(mdc.getDC(), 0, 0, width, height, TRUE);
}

bool wxBitmap::Create(int width, int height, int depth)
{
    UnRef();

    wxCHECK_MSG( (width > 0) && (height > 0), FALSE, wxT("invalid bitmap size") )
    
    pixel_format_t pf_dummy, *pf;
    int mglDepth = depth;

    switch ( depth )
    {
        case -1:
            wxASSERT_MSG( g_displayDC, wxT("MGL display DC not created yet.") );

            g_displayDC->getPixelFormat(pf_dummy);
            mglDepth = g_displayDC->getBitsPerPixel();                
            pf = &pf_dummy;
            break;
        case 1:
        case 8:
            pf = NULL;
            break;
        case 15:
            pf = &gs_pixel_format_15;
            break;
        case 16:
            pf = &gs_pixel_format_16;
            break;
        case 24:
            pf = &gs_pixel_format_24;
            break;
        case 32:
            pf = &gs_pixel_format_32;
            break;
        default:
            wxASSERT_MSG( 0, wxT("invalid bitmap depth") );
            return FALSE;
            break;
    }

    m_refData = new wxBitmapRefData();
    M_BMPDATA->m_mask = (wxMask *) NULL;
    M_BMPDATA->m_palette = (wxPalette *) NULL;
    M_BMPDATA->m_width = width;
    M_BMPDATA->m_height = height;
    M_BMPDATA->m_bpp = mglDepth;

    if ( mglDepth != 1 )
    {
        M_BMPDATA->m_bitmap = MyMGL_createBitmap(width, height, mglDepth, pf);
    }
    else
    {
        // MGL does not support mono DCs, so we have to emulate them with
        // 8bpp ones. We do that by using a special palette with color 0
        // set to black and all other colors set to white. 

        M_BMPDATA->m_bitmap = MyMGL_createBitmap(width, height, 8, pf);
        SetMonoPalette(wxColour(255, 255, 255), wxColour(0, 0, 0));
    }

    return Ok();
}

bool wxBitmap::CreateFromXpm(const char **bits)
{
    wxCHECK_MSG( bits != NULL, FALSE, wxT("invalid bitmap data") )
    
    wxXPMDecoder decoder;
    wxImage img = decoder.ReadData(bits);
    wxCHECK_MSG( img.Ok(), FALSE, wxT("invalid bitmap data") )
    
    *this = wxBitmap(img);
    
    return TRUE;
}

wxBitmap::wxBitmap(const wxImage& image, int depth)
{
    long width, height;

    wxCHECK_RET( image.Ok(), wxT("invalid image") )
    
    width = image.GetWidth();
    height = image.GetHeight();

    if ( !Create(width, height, depth) ) return;
    
    MGLMemoryDC idc(width, height, 24, &gs_pixel_format_wxImage,
                    width * 3, (void*)image.GetData(), NULL);
    wxASSERT_MSG( idc.isValid(), wxT("cannot create custom MGLDC") );

    MGLDevCtx *bdc = CreateTmpDC();

    if ( GetDepth() <= 8 && image.HasPalette() )
        SetPalette(image.GetPalette());

    bdc->bitBlt(idc, 0, 0, width, height, 0, 0, MGL_REPLACE_MODE);
    delete bdc;
                               
    if ( image.HasMask() )
    {
        wxImage mask_image = image.ConvertToMono(image.GetMaskRed(),
                                                 image.GetMaskGreen(),
                                                 image.GetMaskBlue());
        mask_image.SetMask(FALSE);
        wxBitmap mask_bmp(mask_image, 1);
        SetMask(new wxMask(mask_bmp));
    }
}

wxImage wxBitmap::ConvertToImage() const
{
    wxCHECK_MSG( Ok(), FALSE, wxT("invalid bitmap") );

    int width, height;
    width = GetWidth();
    height = GetHeight();
    
    wxImage image(width, height);
    wxASSERT_MSG( image.Ok(), wxT("cannot create image") );
    
    MGLMemoryDC idc(width, height, 24, &gs_pixel_format_wxImage,
                    width * 3, (void*)image.GetData(), NULL);
    wxASSERT_MSG( idc.isValid(), wxT("cannot create custom MGLDC") );

    if ( M_BMPDATA->m_palette )
        image.SetPalette(*(M_BMPDATA->m_palette));
        
    if ( GetMask() )
    {
        // in consistency with other ports, we convert parts covered
        // by the mask to <16,16,16> colour and set that colour to image's
        // mask. We do that by OR-blitting the mask over image with
        // bg colour set to black and fg colour to <16,16,16>

        image.SetMaskColour(16, 16, 16);
        image.SetMask(TRUE);      

        wxDC tmpDC;
        tmpDC.SetMGLDC(&idc, FALSE);
        tmpDC.SetBackground(wxBrush(wxColour(16,16,16), wxSOLID));
        tmpDC.Clear();
        tmpDC.DrawBitmap(*this, 0, 0, TRUE);
    }
    else
    {
        image.SetMask(FALSE);
        idc.putBitmap(0, 0, M_BMPDATA->m_bitmap, MGL_REPLACE_MODE);
    }

    return image;
}

wxBitmap::wxBitmap(const wxBitmap& bmp)
{
    Ref(bmp);
}

wxBitmap::wxBitmap(const wxString &filename, wxBitmapType type)
{
    LoadFile(filename, type);
}

wxBitmap::wxBitmap(const char bits[], int width, int height, int depth)
{
    wxCHECK_RET( depth == 1, wxT("can only create mono bitmap from XBM data") );

    if ( !Create(width, height, 1) ) return;
    MGLDevCtx *bdc = CreateTmpDC();
    wxCurrentDCSwitcher curDC(bdc);
    bdc->setColor(1);
    bdc->setBackColor(0);
    bdc->clearDevice();
    bdc->putMonoImage(0, 0, width, (width + 7) / 8, height, (void*)bits);
    delete bdc;
}

wxBitmap& wxBitmap::operator = (const wxBitmap& bmp)
{
    if ( *this == bmp ) return (*this);
    Ref(bmp);
    return *this;
}

bool wxBitmap::operator == (const wxBitmap& bmp) const
{
    return (m_refData == bmp.m_refData);
}

bool wxBitmap::operator != (const wxBitmap& bmp) const
{
    return (m_refData != bmp.m_refData);
}

bool wxBitmap::Ok() const
{
    return (m_refData != NULL && M_BMPDATA->m_bitmap != NULL);
}

int wxBitmap::GetHeight() const
{
    wxCHECK_MSG( Ok(), -1, wxT("invalid bitmap") );

    return M_BMPDATA->m_height;
}

int wxBitmap::GetWidth() const
{
    wxCHECK_MSG( Ok(), -1, wxT("invalid bitmap") );

    return M_BMPDATA->m_width;
}

int wxBitmap::GetDepth() const
{
    wxCHECK_MSG( Ok(), -1, wxT("invalid bitmap") );

    return M_BMPDATA->m_bpp;
}

wxMask *wxBitmap::GetMask() const
{
    wxCHECK_MSG( Ok(), (wxMask *) NULL, wxT("invalid bitmap") );

    return M_BMPDATA->m_mask;
}

void wxBitmap::SetMask(wxMask *mask)
{
    wxCHECK_RET( Ok(), wxT("invalid bitmap") );

    delete M_BMPDATA->m_mask;
    M_BMPDATA->m_mask = mask;
}

bool wxBitmap::CopyFromIcon(const wxIcon& icon)
{
    wxBitmap *bmp = (wxBitmap*)(&icon);
    *this = *bmp;
    return TRUE;
}

wxBitmap wxBitmap::GetSubBitmap(const wxRect& rect) const
{
    wxCHECK_MSG( Ok() &&
                 (rect.x >= 0) && (rect.y >= 0) &&
                 (rect.x+rect.width <= M_BMPDATA->m_width) && (rect.y+rect.height <= M_BMPDATA->m_height),
                 wxNullBitmap, wxT("invalid bitmap or bitmap region") );

    wxBitmap ret( rect.width, rect.height, M_BMPDATA->m_bpp );
    wxASSERT_MSG( ret.Ok(), wxT("GetSubBitmap error") );

    if ( GetPalette() )
        ret.SetPalette(*GetPalette());

    MGLDevCtx *tdc = ret.CreateTmpDC();
    tdc->putBitmapSection(rect.x, rect.y, 
                          rect.x + rect.width, rect.y + rect.height,
                          0, 0, M_BMPDATA->m_bitmap, MGL_REPLACE_MODE);
    delete tdc;

    if ( GetMask() )
    {
        wxBitmap submask = GetMask()->GetBitmap()->GetSubBitmap(rect);
        ret.SetMask(new wxMask(submask));
    }

    return ret;
}

void wxBitmap::SetMonoPalette(const wxColour& fg, const wxColour& bg)
{
    wxCHECK_RET( Ok(), wxT("invalid bitmap") );

    palette_t *mono = M_BMPDATA->m_bitmap->pal;

    wxCHECK_RET( M_BMPDATA->m_bpp == 1, wxT("bitmap is not 1bpp") );
    wxCHECK_RET( mono != NULL, wxT("bitmap w/o palette") );

    mono[0].red = bg.Red();
    mono[0].green = bg.Green();
    mono[0].blue = bg.Blue();
    mono[0].alpha = 0;
    for (size_t i = 1; i < 256; i++)
    {
        mono[i].red = fg.Red();
        mono[i].green = fg.Green();
        mono[i].blue = fg.Blue();
        mono[i].alpha = 0;
    }
}

MGLDevCtx *wxBitmap::CreateTmpDC() const
{
    wxCHECK_MSG( Ok(), NULL, wxT("invalid bitmap") );

    MGLDevCtx *tdc = new MGLMemoryDC(GetWidth(), GetHeight(),
                                     M_BMPDATA->m_bitmap->bitsPerPixel,
                                     M_BMPDATA->m_bitmap->pf,
                                     M_BMPDATA->m_bitmap->bytesPerLine,
                                     M_BMPDATA->m_bitmap->surface, 
                                     NULL);
    wxCHECK_MSG( tdc->isValid(), NULL, wxT("cannot create temporary MGLDC") );

    if ( M_BMPDATA->m_bitmap->pal != NULL )
    {
        int cnt;
        
        switch (M_BMPDATA->m_bitmap->bitsPerPixel)
        {
            case  2: cnt = 2;   break;
            case  4: cnt = 16;  break;
            case  8: cnt = 256; break;
            default:
                cnt = 0;
                wxFAIL_MSG( wxT("bitmap with this depth cannot have palette") );
                break;
        }
        
        tdc->setPalette(M_BMPDATA->m_bitmap->pal, cnt, 0);
        tdc->realizePalette(cnt, 0, FALSE);
    }
    
    return tdc;
}

bool wxBitmap::LoadFile(const wxString &name, wxBitmapType type)
{
    UnRef();
    
    if ( type == wxBITMAP_TYPE_BMP || type == wxBITMAP_TYPE_PNG || 
         type == wxBITMAP_TYPE_PCX || type == wxBITMAP_TYPE_JPEG )
    {
        // prevent accidental loading of bitmap from $MGL_ROOT:
        if ( !wxFileExists(name) )
        {
            wxLogError(_("File %s does not exist."), name.c_str());
            return FALSE;
        }
    }
        
    wxBitmapHandler *handler = FindHandler(type);

    if ( handler == NULL ) 
    {
        wxImage image;
        if ( !image.LoadFile(name, type) || !image.Ok() )
        {
            wxLogError("no bitmap handler for type %d defined.", type);            
            return FALSE;
        }
        else
        {
            *this = wxBitmap(image);
            return TRUE;
        }
    }

    m_refData = new wxBitmapRefData();

    return handler->LoadFile(this, name, type, -1, -1);
}

bool wxBitmap::SaveFile(const wxString& filename, wxBitmapType type, const wxPalette *palette) const
{
    wxCHECK_MSG( Ok(), FALSE, wxT("invalid bitmap") );

    wxBitmapHandler *handler = FindHandler(type);

    if ( handler == NULL ) 
    {
        wxImage image = ConvertToImage();
        if ( palette )
            image.SetPalette(*palette);

        if ( image.Ok() )
            return image.SaveFile(filename, type);
        else
        {
            wxLogError("no bitmap handler for type %d defined.", type);
            return FALSE;
        }
    }

    return handler->SaveFile(this, filename, type, palette);
}

wxPalette *wxBitmap::GetPalette() const
{
    wxCHECK_MSG( Ok(), NULL, wxT("invalid bitmap") );

    return M_BMPDATA->m_palette;
}

void wxBitmap::SetPalette(const wxPalette& palette)
{
    wxCHECK_RET( Ok(), wxT("invalid bitmap") );
    wxCHECK_RET( GetDepth() > 1 && GetDepth() <= 8, wxT("cannot set palette for bitmap of this depth") );

    delete M_BMPDATA->m_palette;
    M_BMPDATA->m_palette = NULL;

    if ( !palette.Ok() ) return;
   
    M_BMPDATA->m_palette = new wxPalette(palette);

    int cnt = palette.GetColoursCount();
    palette_t *pal = palette.GetMGLpalette_t();
    memcpy(M_BMPDATA->m_bitmap->pal, pal, cnt * sizeof(palette_t));
}

void wxBitmap::SetHeight(int height)
{
    if (!m_refData) m_refData = new wxBitmapRefData();

    M_BMPDATA->m_height = height;
}

void wxBitmap::SetWidth(int width)
{
    if (!m_refData) m_refData = new wxBitmapRefData();

    M_BMPDATA->m_width = width;
}

void wxBitmap::SetDepth(int depth)
{
    if (!m_refData) m_refData = new wxBitmapRefData();

    M_BMPDATA->m_bpp = depth;
}

bitmap_t *wxBitmap::GetMGLbitmap_t() const
{
    return M_BMPDATA->m_bitmap;
}



//-----------------------------------------------------------------------------
// wxBitmap I/O handlers
//-----------------------------------------------------------------------------

class wxMGLBitmapHandler: public wxBitmapHandler
{
public:
    wxMGLBitmapHandler(wxBitmapType type,
                       const wxString& extension, const wxString& name);

    virtual bool Create(wxBitmap *bitmap, void *data, long flags, 
                          int width, int height, int depth = 1)
        { return FALSE; }

    virtual bool LoadFile(wxBitmap *bitmap, const wxString& name, long flags,
                          int desiredWidth, int desiredHeight);
    virtual bool SaveFile(const wxBitmap *bitmap, const wxString& name, 
                          int type, const wxPalette *palette = NULL);
};

wxMGLBitmapHandler::wxMGLBitmapHandler(wxBitmapType type, 
                                       const wxString& extension,
                                       const wxString& name)
   : wxBitmapHandler()
{
    SetType(type);
    SetName(name);
    SetExtension(extension);
}

bool wxMGLBitmapHandler::LoadFile(wxBitmap *bitmap, const wxString& name, 
                                  long flags, 
                                  int WXUNUSED(desiredWidth), 
                                  int WXUNUSED(desiredHeight))
{
    int width, height, bpp;
    pixel_format_t pf;
    wxString fullname;
    wxMemoryDC dc;
    
    switch (flags)
    {
        case wxBITMAP_TYPE_BMP_RESOURCE:
        case wxBITMAP_TYPE_JPEG_RESOURCE:
        case wxBITMAP_TYPE_PNG_RESOURCE:
        case wxBITMAP_TYPE_PCX_RESOURCE:
            fullname = name + wxT(".bmp");
            break;
        default:
            fullname= name;
            break;
    }   

    switch (flags)
    {
        case wxBITMAP_TYPE_BMP:
        case wxBITMAP_TYPE_BMP_RESOURCE:
            if ( !MGL_getBitmapSize(fullname.mb_str(), &width, &height, &bpp, &pf) )
                return FALSE;
            bitmap->Create(width, height, -1);
            if ( !bitmap->Ok() ) return FALSE;
            dc.SelectObject(*bitmap);
            if ( !dc.GetMGLDC()->loadBitmapIntoDC(fullname.mb_str(), 0, 0, TRUE) )
                return FALSE;
            break;

        case wxBITMAP_TYPE_JPEG:
        case wxBITMAP_TYPE_JPEG_RESOURCE:
            if ( !MGL_getJPEGSize(fullname.mb_str(), &width, &height, &bpp, &pf) )
                return FALSE;
            bitmap->Create(width, height, -1);
            if ( !bitmap->Ok() ) return FALSE;
            dc.SelectObject(*bitmap);
            if ( !dc.GetMGLDC()->loadJPEGIntoDC(fullname.mb_str(), 0, 0, TRUE) )
                return FALSE;
            break;

        case wxBITMAP_TYPE_PNG:
        case wxBITMAP_TYPE_PNG_RESOURCE:
            if ( !MGL_getPNGSize(fullname.mb_str(), &width, &height, &bpp, &pf) )
                return FALSE;
            bitmap->Create(width, height, -1);
            if ( !bitmap->Ok() ) return FALSE;
            dc.SelectObject(*bitmap);
            if ( !dc.GetMGLDC()->loadPNGIntoDC(fullname.mb_str(), 0, 0, TRUE) )
                return FALSE;
            break;

        case wxBITMAP_TYPE_PCX:
        case wxBITMAP_TYPE_PCX_RESOURCE:
            if ( !MGL_getPCXSize(fullname.mb_str(), &width, &height, &bpp) )
                return FALSE;
            bitmap->Create(width, height, -1);
            if ( !bitmap->Ok() ) return FALSE;
            dc.SelectObject(*bitmap);
            if ( !dc.GetMGLDC()->loadPCXIntoDC(fullname.mb_str(), 0, 0, TRUE) )
                return FALSE;
            break;

        default:
            wxFAIL_MSG(wxT("Unsupported image format."));
            break;
    }

    return TRUE;
}

bool wxMGLBitmapHandler::SaveFile(const wxBitmap *bitmap, const wxString& name, 
                                  int type, const wxPalette * WXUNUSED(palette))
{
    wxMemoryDC mem;
    MGLDevCtx *tdc;
    int w = bitmap->GetWidth(),
        h = bitmap->GetHeight();

    mem.SelectObject(*bitmap);
    tdc = mem.GetMGLDC();

    switch (type)
    {
        case wxBITMAP_TYPE_BMP:
            return tdc->saveBitmapFromDC(name.mb_str(), 0, 0, w, h);
            break;
        case wxBITMAP_TYPE_JPEG:
            return tdc->saveJPEGFromDC(name.mb_str(), 0, 0, w, h, 75);
            break;
        case wxBITMAP_TYPE_PNG:
            return tdc->savePNGFromDC(name.mb_str(), 0, 0, w, h);
            break;
        case wxBITMAP_TYPE_PCX:
            return tdc->savePCXFromDC(name.mb_str(), 0, 0, w, h);
            break;
        default:
            return FALSE;
            break;
    }
}



// let's handle PNGs in special way because they have alpha channel 
// which we can access via bitmap_t most easily
class wxPNGBitmapHandler: public wxMGLBitmapHandler
{
public:
    wxPNGBitmapHandler(wxBitmapType type,
                       const wxString& extension, const wxString& name)
        : wxMGLBitmapHandler(type, extension, name) {}

    virtual bool LoadFile(wxBitmap *bitmap, const wxString& name, long flags,
                          int desiredWidth, int desiredHeight);
};

bool wxPNGBitmapHandler::LoadFile(wxBitmap *bitmap, const wxString& name, 
                                  long flags, 
                                  int desiredWidth, int desiredHeight)
{
    int width, height, bpp;
    pixel_format_t pf;
    wxString fullname;

    if ( flags == wxBITMAP_TYPE_PNG_RESOURCE )
        fullname = name + wxT(".png");
    else
        fullname = name;

    if ( !MGL_getPNGSize(fullname.mb_str(), &width, &height, &bpp, &pf) )
        return FALSE;

    if ( bpp != 32 )
    {
        // We can load ordinary PNGs faster with 'normal' MGL handler.
        // Only RGBA PNGs need to be processed in special way because
        // we have to convert alpha channel to mask
        return wxMGLBitmapHandler::LoadFile(bitmap, name, flags, 
                                            desiredWidth, desiredHeight);
    }
        
    bitmap_t *bmp = MGL_loadPNG(fullname.mb_str(), TRUE);
    
    if ( bmp == NULL ) return FALSE;

    bitmap->Create(bmp->width, bmp->height, -1);
    if ( !bitmap->Ok() ) return FALSE;
   
    // convert bmp to display's depth and write it to *bitmap:
    wxMemoryDC dc;
    dc.SelectObject(*bitmap);
    dc.GetMGLDC()->putBitmap(0, 0, bmp, MGL_REPLACE_MODE);
    dc.SelectObject(wxNullBitmap);
    
    // create mask, if bmp contains alpha channel (ARGB format):
    if ( bmp->bitsPerPixel == 32 )
    {
        int x, y;
        wxUint32 *s = (wxUint32*)bmp->surface;
        for (y = 0; y < bmp->height; y++)
        {
            s = ((wxUint32*)bmp->surface) + y * bmp->bytesPerLine/4;
            for (x = 0; x < bmp->width; x++, s ++)
            {
                if ( ((((*s) >> bmp->pf->alphaPos) & bmp->pf->alphaMask) 
                        << bmp->pf->alphaAdjust) < 128 )
                    *s = 0;
                else
                    *s = 0x00FFFFFF; // white
            }
        }
        wxBitmap mask(bmp->width, bmp->height, 1);
        dc.SelectObject(mask);
        dc.GetMGLDC()->putBitmap(0, 0, bmp, MGL_REPLACE_MODE);
        dc.SelectObject(wxNullBitmap);
        bitmap->SetMask(new wxMask(mask));
    }
    
    MGL_unloadBitmap(bmp);
    
    return TRUE;
}




class wxICOBitmapHandler: public wxBitmapHandler
{
    public:
        wxICOBitmapHandler(wxBitmapType type,
                           const wxString& extension, const wxString& name);
        
        virtual bool Create(wxBitmap *bitmap, void *data, long flags, 
                              int width, int height, int depth = 1)
            { return FALSE; }
    
        virtual bool LoadFile(wxBitmap *bitmap, const wxString& name, long flags,
                              int desiredWidth, int desiredHeight);
        virtual bool SaveFile(const wxBitmap *bitmap, const wxString& name, 
                              int type, const wxPalette *palette = NULL);
};

wxICOBitmapHandler::wxICOBitmapHandler(wxBitmapType type, 
                                       const wxString& extension,
                                       const wxString& name)
   : wxBitmapHandler()
{
    SetType(type);
    SetName(name);
    SetExtension(extension);
}

bool wxICOBitmapHandler::LoadFile(wxBitmap *bitmap, const wxString& name, 
                                  long flags, 
                                  int WXUNUSED(desiredWidth), 
                                  int WXUNUSED(desiredHeight))
{
    icon_t *icon = NULL;
    MGLDevCtx *dc;

    if ( flags == wxBITMAP_TYPE_ICO_RESOURCE )
        icon = MGL_loadIcon(wxString(name + wxT(".ico")).mb_str(), TRUE);
    else   
        icon = MGL_loadIcon(name.mb_str(), TRUE);

    if ( icon == NULL ) return FALSE;

    bitmap->Create(icon->xorMask.width, icon->xorMask.height);

    wxMemoryDC mem;
    mem.SelectObject(*bitmap);
    dc = mem.GetMGLDC();
    dc->putBitmap(0, 0, &(icon->xorMask), MGL_REPLACE_MODE);
    mem.SelectObject(wxNullBitmap);

    wxBitmap mask(icon->xorMask.width, icon->xorMask.height, 1);
    mem.SelectObject(mask);
    dc = mem.GetMGLDC();

    wxCurrentDCSwitcher curDC(dc);
    dc->setColor(0);
    dc->setBackColor(1);
    dc->clearDevice();
    dc->putMonoImage(0, 0, icon->xorMask.width, icon->byteWidth,
                           icon->xorMask.height, (void*)icon->andMask);
    
    bitmap->SetMask(new wxMask(mask));

    MGL_unloadIcon(icon);
    
    return TRUE;
}

bool wxICOBitmapHandler::SaveFile(const wxBitmap *bitmap, const wxString& name, 
                                  int type, const wxPalette * WXUNUSED(palette))
{
    return FALSE;
}




/*static*/ void wxBitmap::InitStandardHandlers()
{
    AddHandler(new wxMGLBitmapHandler(wxBITMAP_TYPE_BMP, wxT("bmp"), wxT("Windows bitmap")));
    AddHandler(new wxMGLBitmapHandler(wxBITMAP_TYPE_BMP_RESOURCE, wxEmptyString, wxT("Windows bitmap resource")));
    AddHandler(new wxMGLBitmapHandler(wxBITMAP_TYPE_JPEG, wxT("jpg"), wxT("JPEG image")));
    AddHandler(new wxMGLBitmapHandler(wxBITMAP_TYPE_JPEG_RESOURCE, wxEmptyString, wxT("JPEG resource")));
    AddHandler(new wxMGLBitmapHandler(wxBITMAP_TYPE_PCX, wxT("pcx"), wxT("PCX image")));
    AddHandler(new wxMGLBitmapHandler(wxBITMAP_TYPE_PCX_RESOURCE, wxEmptyString, wxT("PCX resource")));

    AddHandler(new wxPNGBitmapHandler(wxBITMAP_TYPE_PNG, wxT("png"), wxT("PNG image")));
    AddHandler(new wxPNGBitmapHandler(wxBITMAP_TYPE_PNG_RESOURCE, wxEmptyString, wxT("PNG resource")));

    AddHandler(new wxICOBitmapHandler(wxBITMAP_TYPE_ICO, wxT("ico"), wxT("Icon resource")));
    AddHandler(new wxICOBitmapHandler(wxBITMAP_TYPE_ICO_RESOURCE, wxEmptyString, wxT("Icon resource")));
}
