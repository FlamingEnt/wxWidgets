/////////////////////////////////////////////////////////////////////////////
// Name:        src/unix/fontenum.cpp
// Purpose:     wxFontEnumerator class for X11/GDK
// Author:      Vadim Zeitlin
// Modified by:
// Created:     01.10.99
// RCS-ID:      $Id: fontenum.cpp,v 1.17.2.3 2003/02/13 16:45:52 RR Exp $
// Copyright:   (c) Vadim Zeitlin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "fontenum.h"
#endif

#include "wx/defs.h"
#include "wx/dynarray.h"
#include "wx/string.h"
#include "wx/regex.h"
#include "wx/utils.h"
#include "wx/app.h"
#include "wx/fontmap.h"
#include "wx/fontenum.h"
#include "wx/fontutil.h"

// ----------------------------------------------------------------------------
// Pango
// ----------------------------------------------------------------------------

#if wxUSE_PANGO

#include "pango/pango.h"

#ifdef __WXGTK20__
#include "gtk/gtk.h"
extern GtkWidget *wxGetRootWindow();
#endif

static int
cmp_families (const void *a, const void *b)
{
    const char *a_name = pango_font_family_get_name (*(PangoFontFamily **)a);
    const char *b_name = pango_font_family_get_name (*(PangoFontFamily **)b);
  
    return g_utf8_collate (a_name, b_name);
}

static PangoLanguage *font_test_language;

static gboolean font_is_latin( PangoContext *context, PangoFontDescription *desc )
{
    PangoFont *font = pango_context_load_font( context, desc );
    if (!font) return FALSE;
    
    font_test_language = pango_language_from_string( "eng" );
    
    PangoCoverage *coverage =pango_font_get_coverage( font, font_test_language );
    if (!coverage) return FALSE;
    
    if ((pango_coverage_get(coverage, 'i') == PANGO_COVERAGE_EXACT) &&
        (pango_coverage_get(coverage, 'W') == PANGO_COVERAGE_EXACT))
    {
        return TRUE;
    }
    
    return FALSE;
}

bool wxFontEnumerator::EnumerateFacenames(wxFontEncoding encoding,
                                          bool fixedWidthOnly)
{
    PangoContext *context =
#ifdef __WXGTK20__
            gtk_widget_get_pango_context( wxGetRootWindow() );
#else
            wxTheApp->GetPangoContext();
#endif
    PangoFontFamily **families = NULL;
    gint n_families = 0;
    pango_context_list_families( context,  &families, &n_families );
    qsort (families, n_families, sizeof (PangoFontFamily *), cmp_families);

    for (int i=0; i<n_families; i++)
    {
        const gchar *name = pango_font_family_get_name( families[i] );
            
        if (fixedWidthOnly)
        {
            if (
                 (strstr( name, "mono") == NULL) &&
                 (strstr( name, "Mono") == NULL) &&
                 (strstr( name, "MONO") == NULL) &&
                 (strstr( name, "fixed") == NULL) &&
                 (strstr( name, "Fixed") == NULL) &&
                 (strstr( name, "FIXED") == NULL) &&
                 (strstr( name, "terminal") == NULL) &&
                 (strstr( name, "Terminal") == NULL) &&
                 (strstr( name, "TERMINAL") == NULL) &&
                 (strstr( name, "cour") == NULL) &&
                 (strstr( name, "Cour") == NULL) &&
                 (strstr( name, "COUR") == NULL) 
               )
            {
                continue;
            }
        
            PangoFontDescription *desc = pango_font_description_new();
            pango_font_description_set_family( desc, name );
            pango_font_description_set_size( desc, 12 * PANGO_SCALE );
                
            if (!font_is_latin( context, desc))
            {
                pango_font_description_free( desc );
                continue;
            }
                
            PangoLayout *layout = pango_layout_new(context);
            pango_layout_set_font_description(layout, desc);
                
            pango_layout_set_text(layout, "i", 1 );
            int i_width = 0;
            pango_layout_get_size(layout, &i_width, NULL);
                
            pango_layout_set_text(layout, "W", 1 );
            int W_width = 0;
            pango_layout_get_size(layout, &W_width, NULL);
               
            g_object_unref( G_OBJECT( layout ) );
            pango_font_description_free( desc );
               
            if (W_width != i_width) continue;
            if (i_width == 0) continue;
        }
        
        wxString tmp( name, wxConvUTF8 );
        OnFacename( tmp );
    }
    
    return TRUE;
    }

bool wxFontEnumerator::EnumerateEncodings(const wxString& family)
{
    return FALSE;
}


#else
  // Pango

#ifdef __VMS__ // Xlib.h for VMS is not (yet) compatible with C++
               // The resulting warnings are switched off here
#pragma message disable nosimpint
#endif
#include <X11/Xlib.h>
#ifdef __VMS__
#pragma message enable nosimpint
#endif

// ----------------------------------------------------------------------------
// private functions
// ----------------------------------------------------------------------------

// create the list of all fonts with the given spacing and encoding
static char **CreateFontList(wxChar spacing, wxFontEncoding encoding,
                             int *nFonts);

// extract all font families from the given font list and call our
// OnFacename() for each of them
static bool ProcessFamiliesFromFontList(wxFontEnumerator *This,
                                        char **fonts,
                                        int nFonts);


// ----------------------------------------------------------------------------
// private types
// ----------------------------------------------------------------------------

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// helpers
// ----------------------------------------------------------------------------

#if !wxUSE_NANOX
static char **CreateFontList(wxChar spacing,
                             wxFontEncoding encoding,
                             int *nFonts)
{
    wxNativeEncodingInfo info;
    wxGetNativeFontEncoding(encoding, &info);

#if wxUSE_FONTMAP
    if ( !wxTestFontEncoding(info) )
    {
        // ask font mapper for a replacement
        (void)wxFontMapper::Get()->GetAltForEncoding(encoding, &info);
    }
#endif // wxUSE_FONTMAP

    wxString pattern;
    pattern.Printf(wxT("-*-*-*-*-*-*-*-*-*-*-%c-*-%s-%s"),
                   spacing,
                   info.xregistry.c_str(),
                   info.xencoding.c_str());

    // get the list of all fonts
    return XListFonts((Display *)wxGetDisplay(), pattern.mb_str(), 32767, nFonts);
}

static bool ProcessFamiliesFromFontList(wxFontEnumerator *This,
                                        char **fonts,
                                        int nFonts)
{
#if wxUSE_REGEX
    wxRegEx re(wxT("^(-[^-]*){14}$"), wxRE_NOSUB);
#endif // wxUSE_REGEX

    // extract the list of (unique) font families
    wxSortedArrayString families;
    for ( int n = 0; n < nFonts; n++ )
    {
        char *font = fonts[n];
#if wxUSE_REGEX
        if ( !re.Matches(font) )
#else // !wxUSE_REGEX
        if ( !wxString(font).Matches(wxT("-*-*-*-*-*-*-*-*-*-*-*-*-*-*")) )
#endif // wxUSE_REGEX/!wxUSE_REGEX
        {
            // it's not a full font name (probably an alias)
            continue;
        }

        char *dash = strchr(font + 1, '-');
        char *family = dash + 1;
        dash = strchr(family, '-');
        *dash = '\0'; // !NULL because Matches() above succeeded
        wxString fam(family);

        if ( families.Index(fam) == wxNOT_FOUND )
        {
            if ( !This->OnFacename(fam) )
            {
                // stop enumerating
                return FALSE;
            }

            families.Add(fam);
        }
        //else: already seen
    }

    return TRUE;
}
#endif
  // wxUSE_NANOX

// ----------------------------------------------------------------------------
// wxFontEnumerator
// ----------------------------------------------------------------------------

bool wxFontEnumerator::EnumerateFacenames(wxFontEncoding encoding,
                                          bool fixedWidthOnly)
{
#if wxUSE_NANOX
    return FALSE;
#else
    int nFonts;
    char **fonts;

    if ( fixedWidthOnly )
    {
        bool cont = TRUE;
        fonts = CreateFontList(wxT('m'), encoding, &nFonts);
        if ( fonts )
        {
            cont = ProcessFamiliesFromFontList(this, fonts, nFonts);

            XFreeFontNames(fonts);
        }

        if ( !cont )
        {
            return TRUE;
        }

        fonts = CreateFontList(wxT('c'), encoding, &nFonts);
        if ( !fonts )
        {
            return TRUE;
        }
    }
    else
    {
        fonts = CreateFontList(wxT('*'), encoding, &nFonts);

        if ( !fonts )
        {
            // it's ok if there are no fonts in given encoding - but it's not
            // ok if there are no fonts at all
            wxASSERT_MSG(encoding != wxFONTENCODING_SYSTEM,
                         wxT("No fonts at all on this system?"));

            return FALSE;
        }
    }

    (void)ProcessFamiliesFromFontList(this, fonts, nFonts);

    XFreeFontNames(fonts);
    return TRUE;
#endif
    // wxUSE_NANOX
}

bool wxFontEnumerator::EnumerateEncodings(const wxString& family)
{
#if wxUSE_NANOX
    return FALSE;
#else
    wxString pattern;
    pattern.Printf(wxT("-*-%s-*-*-*-*-*-*-*-*-*-*-*-*"),
                   family.IsEmpty() ? wxT("*") : family.c_str());

    // get the list of all fonts
    int nFonts;
    char **fonts = XListFonts((Display *)wxGetDisplay(), pattern.mb_str(),
                              32767, &nFonts);

    if ( !fonts )
    {
        // unknown family?
        return FALSE;
    }

    // extract the list of (unique) encodings
    wxSortedArrayString encodings;
    for ( int n = 0; n < nFonts; n++ )
    {
        char *font = fonts[n];
        if ( !wxString(font).Matches(wxT("-*-*-*-*-*-*-*-*-*-*-*-*-*-*")) )
        {
            // it's not a full font name (probably an alias)
            continue;
        }

        // extract the family
        char *dash = strchr(font + 1, '-');
        char *familyFont = dash + 1;
        dash = strchr(familyFont, '-');
        *dash = '\0'; // !NULL because Matches() above succeeded

        if ( !family.IsEmpty() && (family != familyFont) )
        {
            // family doesn't match
            continue;
        }

        // now extract the registry/encoding
        char *p = dash + 1; // just after the dash after family
        dash = strrchr(p, '-');

        wxString registry(dash + 1);
        *dash = '\0';

        dash = strrchr(p, '-');
        wxString encoding(dash + 1);

        encoding << wxT('-') << registry;
        if ( encodings.Index(encoding) == wxNOT_FOUND )
        {
            if ( !OnFontEncoding(familyFont, encoding) )
            {
                break;
            }

            encodings.Add(encoding);
        }
        //else: already had this one
    }

    XFreeFontNames(fonts);

    return TRUE;
#endif
    // wxUSE_NANOX
}

#endif
   // __WXGTK20__
