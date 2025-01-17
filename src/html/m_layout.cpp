/////////////////////////////////////////////////////////////////////////////
// Name:        m_layout.cpp
// Purpose:     wxHtml module for basic paragraphs/layout handling
// Author:      Vaclav Slavik
// RCS-ID:      $Id: m_layout.cpp,v 1.16.2.7 2003/06/09 17:06:40 VS Exp $
// Copyright:   (c) 1999 Vaclav Slavik
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////
#ifdef __GNUG__
#pragma implementation
#endif

#include "wx/wxprec.h"


#include "wx/defs.h"
#if wxUSE_HTML && wxUSE_STREAMS
#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WXPRECOMP
#endif


#include "wx/html/forcelnk.h"
#include "wx/html/m_templ.h"

#include "wx/html/htmlwin.h"

FORCE_LINK_ME(m_layout)


#include <stdlib.h> // bsearch()

//-----------------------------------------------------------------------------
// wxHtmlPageBreakCell
//-----------------------------------------------------------------------------

// Since html isn't a page-layout language, it doesn't support page
// page breaks directly--that requires CSS2 support. But a page-break
// facility is handy, and has been requested more than once on the
// mailing lists. This wxHtml tag handler implements just enough of
// CSS2 to support a page break by recognizing only
//   <div style="page-break-before:always">
//
// wxHtml maintains page breaks in wxHtmlPrintout::m_PageBreaks. The
// tag handler below adds appropriate offsets to that array member.
// wxHtmlDCRenderer::Render() accesses that array and makes a new page
// begin after each page-break tag.

// The page-break handler does all its work in AdjustPagebreak(). For
// all tag handlers, that function adjusts the page-break position.
// For other tags, it determines whether the html element can fit on
// the remainder of the page; if it cannot fit, but must not be split,
// then the function moves the page break provided in the argument up,
// and returns 'true' to inform the caller that the argument was
// modified.
//
// Due to its special purpose, the page-break facility differs from
// other tags. It takes up no space, but it behaves as though there is
// never enough room to fit it on the remainder of the page--it always
// forces a page break. Therefore, unlike other elements that trigger
// a page break, it would never 'fit' on the following page either.
// Therefore it's necessary to compare each pagebreak candidate to the
// array wxHtmlPrintout::m_PageBreaks of pagebreaks already set, and
// set a new one only if it's not in that array.

class WXDLLEXPORT wxHtmlPageBreakCell : public wxHtmlCell
{
  public:
    wxHtmlPageBreakCell() {}

// wx 2.5 will use this signature:
//    bool AdjustPagebreak(int* pagebreak, int* known_pagebreaks = NULL, int number_of_pages = 0) const;
    bool AdjustPagebreak(int* pagebreak) const;

  private:
    DECLARE_NO_COPY_CLASS(wxHtmlPageBreakCell)
};

// Comparison routine for bsearch into an int* array of pagebreaks.
extern "C" int wxCMPFUNC_CONV wxInteger_compare(void const* i0, void const* i1)
{
    return *(int*)i0 - *(int*)i1;
}

// wx 2.5 will use this signature:
//   bool wxHtmlContainerCell::AdjustPagebreak(int *pagebreak, int* known_pagebreaks, int number_of_pages) const
//
// Workaround to backport html pagebreaks to 2.4.0:
// Actually, we're passing a pointer to struct wxHtmlKludge, casting
// that pointer to an int* . We don't need to do anything special
// here because that struct's first element is an int* to 'pagebreak'.
// Other struct members are addressed by casting that int* back to
// wxHtmlKludge*; they don't get modified, so we don't have to pass
// them back to the caller.
bool wxHtmlPageBreakCell::AdjustPagebreak(int* pagebreak) const
{
    // Workaround to backport html pagebreaks to 2.4.0:
    wxHtmlKludge* kludge = (wxHtmlKludge*)pagebreak;
    int* known_pagebreaks = kludge->known_pagebreaks;
    int number_of_pages = kludge->number_of_pages;

    // When we are counting pages, 'known_pagebreaks' is non-NULL.
    // That's the only time we change 'pagebreak'. Otherwise, pages
    // were already counted, 'known_pagebreaks' is NULL, and we don't
    // do anything except return FALSE.
    //
    // We also simply return FALSE if the 'pagebreak' argument is
    // less than (vertically above) or the same as the current
    // vertical position. Otherwise we'd be setting a pagebreak above
    // the current cell, which is incorrect, or duplicating a
    // pagebreak that has already been set.
    if(NULL == known_pagebreaks || *pagebreak <= m_PosY)
        {
        return FALSE;
        }

    // m_PosY is only the vertical offset from the parent. The pagebreak
    // required here is the total page offset, so m_PosY must be added
    // to the parent's offset and height.
    int total_height = m_PosY + GetParent()->GetPosY() + GetParent()->GetHeight();

    // Search the array of pagebreaks to see whether we've already set
    // a pagebreak here. The standard bsearch() function is appropriate
    // because the array of pagebreaks through known_pagebreaks[number_of_pages]
    // is known to be sorted in strictly increasing order. '1 + number_of_pages'
    // is used as a bsearch() argument because the array contains a leading
    // zero plus one element for each page.
    int* where = (int*) bsearch(&total_height, known_pagebreaks,
                                1 + number_of_pages, sizeof(int),
                                wxInteger_compare);
    // Add a pagebreak only if there isn't one already set here.
    if(NULL != where)
        {
        return FALSE;
        }
    else
        {
        *pagebreak = m_PosY;
        return TRUE;
        }
}

TAG_HANDLER_BEGIN(P, "P")

    TAG_HANDLER_PROC(tag)
    {
        if (m_WParser->GetContainer()->GetFirstCell() != NULL)
        {
            m_WParser->CloseContainer();
            m_WParser->OpenContainer();
        }
        m_WParser->GetContainer()->SetIndent(m_WParser->GetCharHeight(), wxHTML_INDENT_TOP);
        m_WParser->GetContainer()->SetAlign(tag);
        return FALSE;
    }

TAG_HANDLER_END(P)



TAG_HANDLER_BEGIN(BR, "BR")

    TAG_HANDLER_PROC(tag)
    {
        int al = m_WParser->GetContainer()->GetAlignHor();
        wxHtmlContainerCell *c;

        m_WParser->CloseContainer();
        c = m_WParser->OpenContainer();
        c->SetAlignHor(al);
        c->SetAlign(tag);
        c->SetMinHeight(m_WParser->GetCharHeight());
        return FALSE;
    }

TAG_HANDLER_END(BR)



TAG_HANDLER_BEGIN(CENTER, "CENTER")

    TAG_HANDLER_PROC(tag)
    {
        int old = m_WParser->GetAlign();
        wxHtmlContainerCell *c = m_WParser->GetContainer();

        m_WParser->SetAlign(wxHTML_ALIGN_CENTER);
        if (c->GetFirstCell() != NULL)
        {
            m_WParser->CloseContainer();
            m_WParser->OpenContainer();
        }
        else
            c->SetAlignHor(wxHTML_ALIGN_CENTER);

        if (tag.HasEnding())
        {
            ParseInner(tag);

            m_WParser->SetAlign(old);
            if (c->GetFirstCell() != NULL)
            {
                m_WParser->CloseContainer();
                m_WParser->OpenContainer();
            }
            else
                c->SetAlignHor(old);

            return TRUE;
        }
        else return FALSE;
    }

TAG_HANDLER_END(CENTER)



TAG_HANDLER_BEGIN(DIV, "DIV")

    TAG_HANDLER_PROC(tag)
    {
        if (tag.HasParam(wxT("STYLE")))
        {
            if (tag.GetParam(wxT("STYLE")).IsSameAs(wxString(wxT("PAGE-BREAK-BEFORE:ALWAYS")), FALSE))
            {
                m_WParser->CloseContainer();
                m_WParser->OpenContainer()->InsertCell(new wxHtmlPageBreakCell);
                m_WParser->CloseContainer();
                m_WParser->OpenContainer();
                return FALSE;
            }
            else
            {
                // Treat other STYLE parameters here when they're supported.
                return FALSE;
            }
        }
        else if (tag.HasParam(wxT("ALIGN")))
        {
            int old = m_WParser->GetAlign();
            wxHtmlContainerCell *c = m_WParser->GetContainer();
            if (c->GetFirstCell() != NULL)
            {
                m_WParser->CloseContainer();
                m_WParser->OpenContainer();
                c = m_WParser->GetContainer();
                c->SetAlign(tag);
                m_WParser->SetAlign(c->GetAlignHor());
            }
            else
            {
                c->SetAlign(tag);
                m_WParser->SetAlign(c->GetAlignHor());
            }

            ParseInner(tag);

            m_WParser->SetAlign(old);
            if (c->GetFirstCell() != NULL)
            {
                m_WParser->CloseContainer();
                m_WParser->OpenContainer();
            }
            else
                c->SetAlignHor(old);

            return TRUE;
        }
        else
        {
            return FALSE;
        }
    }

TAG_HANDLER_END(DIV)




TAG_HANDLER_BEGIN(TITLE, "TITLE")

    TAG_HANDLER_PROC(tag)
    {
        if (m_WParser->GetWindow())
        {
            wxHtmlWindow *wfr = (wxHtmlWindow*)(m_WParser->GetWindow());
            if (wfr)
            {
                const wxString& src = *m_WParser->GetSource();
                wfr->OnSetTitle(src.Mid(tag.GetBeginPos(),
                                        tag.GetEndPos1()-tag.GetBeginPos()));
            }
        }
        return TRUE;
    }

TAG_HANDLER_END(TITLE)




TAG_HANDLER_BEGIN(BODY, "BODY")

    TAG_HANDLER_PROC(tag)
    {
        wxColour clr;

        if (tag.GetParamAsColour(wxT("TEXT"), &clr))
        {
            m_WParser->SetActualColor(clr);
            m_WParser->GetContainer()->InsertCell(new wxHtmlColourCell(clr));
        }

        if (tag.GetParamAsColour(wxT("LINK"), &clr))
            m_WParser->SetLinkColor(clr);

        if (tag.GetParamAsColour(wxT("BGCOLOR"), &clr))
        {
            m_WParser->GetContainer()->InsertCell(
                new wxHtmlColourCell(clr, wxHTML_CLR_BACKGROUND));
            if (m_WParser->GetWindow() != NULL)
                m_WParser->GetWindow()->SetBackgroundColour(clr);
        }
        return FALSE;
    }

TAG_HANDLER_END(BODY)



TAG_HANDLER_BEGIN(BLOCKQUOTE, "BLOCKQUOTE")

    TAG_HANDLER_PROC(tag)
    {
        wxHtmlContainerCell *c;

        m_WParser->CloseContainer();
        c = m_WParser->OpenContainer();

        if (c->GetAlignHor() == wxHTML_ALIGN_RIGHT)
            c->SetIndent(5 * m_WParser->GetCharWidth(), wxHTML_INDENT_RIGHT);
        else
            c->SetIndent(5 * m_WParser->GetCharWidth(), wxHTML_INDENT_LEFT);

        c->SetIndent(m_WParser->GetCharHeight(), wxHTML_INDENT_TOP);
        m_WParser->OpenContainer();
        ParseInner(tag);
        c = m_WParser->CloseContainer();
        c->SetIndent(m_WParser->GetCharHeight(), wxHTML_INDENT_BOTTOM);
        m_WParser->CloseContainer();
        m_WParser->OpenContainer();
        return TRUE;
    }

TAG_HANDLER_END(BLOCKQUOTE)



// Tag handler for tags that we have to ignore, otherwise non-text data
// would show up as text:
TAG_HANDLER_BEGIN(DoNothing, "SCRIPT")
    TAG_HANDLER_PROC(tag)
    {
        return true;
    }
TAG_HANDLER_END(DoNothing)



TAGS_MODULE_BEGIN(Layout)

    TAGS_MODULE_ADD(P)
    TAGS_MODULE_ADD(BR)
    TAGS_MODULE_ADD(CENTER)
    TAGS_MODULE_ADD(DIV)
    TAGS_MODULE_ADD(TITLE)
    TAGS_MODULE_ADD(BODY)
    TAGS_MODULE_ADD(BLOCKQUOTE)
    TAGS_MODULE_ADD(DoNothing)

TAGS_MODULE_END(Layout)

#endif
