///////////////////////////////////////////////////////////////////////////////
// Name:        notebook.cpp
// Purpose:     implementation of wxNotebook
// Author:      David Webster
// Modified by:
// Created:     10/12/99
// RCS-ID:      $Id: NOTEBOOK.CPP,v 1.12.2.2 2002/12/02 14:05:20 DW Exp $
// Copyright:   (c) David Webster
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_NOTEBOOK

// wxWindows
#ifndef WX_PRECOMP
  #include "wx/app.h"
  #include "wx/string.h"
  #include "wx/settings.h"
#endif  // WX_PRECOMP

#include  "wx/log.h"
#include  "wx/imaglist.h"
#include  "wx/event.h"
#include  "wx/control.h"
#include  "wx/notebook.h"

#include  "wx/os2/private.h"

// ----------------------------------------------------------------------------
// macros
// ----------------------------------------------------------------------------

// check that the page index is valid
#define IS_VALID_PAGE(nPage) (((nPage) >= 0) && ((nPage) < GetPageCount()))

// hide the ugly cast
#define m_hWnd    (HWND)GetHWND()

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// event table
// ----------------------------------------------------------------------------

DEFINE_EVENT_TYPE(wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGING)

BEGIN_EVENT_TABLE(wxNotebook, wxControl)
    EVT_NOTEBOOK_PAGE_CHANGED(-1, wxNotebook::OnSelChange)
    EVT_SIZE(wxNotebook::OnSize)
    EVT_SET_FOCUS(wxNotebook::OnSetFocus)
    EVT_NAVIGATION_KEY(wxNotebook::OnNavigationKey)
END_EVENT_TABLE()

IMPLEMENT_DYNAMIC_CLASS(wxNotebook, wxControl)
IMPLEMENT_DYNAMIC_CLASS(wxNotebookEvent, wxNotifyEvent)

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxNotebook construction
// ----------------------------------------------------------------------------

//
// Common part of all ctors
//
void wxNotebook::Init()
{
    m_imageList  = NULL;
    m_nSelection = -1;
    m_nTabSize   = 0;
} // end of wxNotebook::Init

//
// Default for dynamic class
//
wxNotebook::wxNotebook()
{
    Init();
} // end of wxNotebook::wxNotebook

//
// The same arguments as for wxControl
//
wxNotebook::wxNotebook(
  wxWindow*                         pParent
, wxWindowID                        vId
, const wxPoint&                    rPos
, const wxSize&                     rSize
, long                              lStyle
, const wxString&                   rsName
)
{
    Init();
    Create( pParent
           ,vId
           ,rPos
           ,rSize
           ,lStyle
           ,rsName
          );
} // end of wxNotebook::wxNotebook

//
// Create() function
//
bool wxNotebook::Create(
  wxWindow*                         pParent
, wxWindowID                        vId
, const wxPoint&                    rPos
, const wxSize&                     rSize
, long                              lStyle
, const wxString&                   rsName
)
{
    //
    // Base init
    //
    if (!CreateControl( pParent
                       ,vId
                       ,rPos
                       ,rSize
                       ,lStyle
                       ,wxDefaultValidator
                       ,rsName
                      ))
        return FALSE;

    //
    // Notebook, so explicitly specify 0 as last parameter
    //
    if (!OS2CreateControl( "NOTEBOOK"
                          ,_T("")
                          ,rPos
                          ,rSize
                          ,lStyle | wxTAB_TRAVERSAL
                         ))
        return FALSE;

    SetBackgroundColour(wxColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE)));
    return TRUE;
} // end of wxNotebook::Create

WXDWORD wxNotebook::OS2GetStyle (
  long                              lStyle
, WXDWORD*                          pdwExstyle
) const
{
    WXDWORD                         dwTabStyle = wxControl::OS2GetStyle( lStyle
                                                                        ,pdwExstyle
                                                                       );

    dwTabStyle |= WS_TABSTOP | BKS_SOLIDBIND | BKS_ROUNDEDTABS | BKS_TABTEXTCENTER;

    if (lStyle & wxNB_BOTTOM)
        dwTabStyle |= BKS_MAJORTABBOTTOM | BKS_BACKPAGESBL;
    else if (lStyle & wxNB_RIGHT)
        dwTabStyle |= BKS_MAJORTABRIGHT | BKS_BACKPAGESBR;
    else if (lStyle & wxNB_LEFT)
        dwTabStyle |= BKS_MAJORTABLEFT | BKS_BACKPAGESTL;
    else // default to top
        dwTabStyle |= BKS_MAJORTABTOP | BKS_BACKPAGESTR;

    //
    // Ex style
    //
    if (pdwExstyle )
    {
        //
        // Note that we never want to have the default WS_EX_CLIENTEDGE style
        // as it looks too ugly for the notebooks
        //
        *pdwExstyle = 0;
    }
    return dwTabStyle;
} // end of wxNotebook::OS2GetStyle

// ----------------------------------------------------------------------------
// wxNotebook accessors
// ----------------------------------------------------------------------------

int wxNotebook::GetPageCount() const
{
    int                             nPageInternal = m_pages.Count();
    int                             nPageAPI = (int)::WinSendMsg(GetHWND(), BKM_QUERYPAGECOUNT, (MPARAM)0, (MPARAM)BKA_END);

    //
    // Consistency check
    //
    wxASSERT((int)m_pages.Count() == (int)::WinSendMsg(GetHWND(), BKM_QUERYPAGECOUNT, (MPARAM)0, (MPARAM)BKA_END));
    return m_pages.Count();
} // end of wxNotebook::GetPageCount

int wxNotebook::GetRowCount() const
{
    return (int)::WinSendMsg( GetHWND()
                             ,BKM_QUERYPAGECOUNT
                             ,(MPARAM)0
                             ,(MPARAM)BKA_MAJOR
                            );
} // end of wxNotebook::GetRowCount

int wxNotebook::SetSelection(
  int                               nPage
)
{
    wxCHECK_MSG( IS_VALID_PAGE(nPage), -1, wxT("notebook page out of range") );

    if (nPage != m_nSelection)
    {
        wxNotebookEvent             vEvent( wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGING
                                           ,m_windowId
                                          );

        vEvent.SetSelection(nPage);
        vEvent.SetOldSelection(m_nSelection);
        vEvent.SetEventObject(this);
        if (!GetEventHandler()->ProcessEvent(vEvent) || vEvent.IsAllowed())
        {

            //
            // Program allows the page change
            //
            vEvent.SetEventType(wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED);
            GetEventHandler()->ProcessEvent(vEvent);

            ULONG                   ulPageId = (ULONG)m_alPageId[nPage];

            ::WinSendMsg( GetHWND()
                         ,BKM_TURNTOPAGE
                         ,MPFROMLONG((ULONG)m_alPageId[nPage])
                         ,(MPARAM)0
                        );
        }
    }
    m_nSelection = nPage;
    return nPage;
} // end of wxNotebook::SetSelection

bool wxNotebook::SetPageText(
  int                               nPage
, const wxString&                   rsStrText
)
{
    wxCHECK_MSG( IS_VALID_PAGE(nPage), FALSE, wxT("notebook page out of range") );


    ULONG                           ulPageId = (ULONG)m_alPageId[nPage];

    return (bool)::WinSendMsg( m_hWnd
                              ,BKM_SETTABTEXT
                              ,MPFROMLONG((ULONG)m_alPageId[nPage])
                              ,MPFROMP((PSZ)rsStrText.c_str())
                             );
} // end of wxNotebook::SetPageText

wxString wxNotebook::GetPageText (
  int                               nPage
) const
{
    BOOKTEXT                        vBookText;
    wxChar                          zBuf[256];
    wxString                        sStr;
    ULONG                           ulRc;

    wxCHECK_MSG( IS_VALID_PAGE(nPage), wxT(""), wxT("notebook page out of range") );


    ULONG                           ulPageId = (ULONG)m_alPageId[nPage];

    memset(&vBookText, '\0', sizeof(BOOKTEXT));
    vBookText.textLen = 0; // This will get the length
    ulRc = LONGFROMMR(::WinSendMsg( m_hWnd
                                   ,BKM_QUERYTABTEXT
                                   ,MPFROMLONG((ULONG)m_alPageId[nPage])
                                   ,MPFROMP(&vBookText)
                                  ));
    if (ulRc == BOOKERR_INVALID_PARAMETERS || ulRc == 0L)
    {
        if (ulRc == BOOKERR_INVALID_PARAMETERS)
        {
            wxLogError(wxT("Invalid Page Id for page text querry."));
        }
        return wxEmptyString;
    }
    vBookText.textLen = ulRc + 1; // To get the null terminator
    vBookText.pString = zBuf;

    //
    // Now get the actual text
    //
    ulRc = LONGFROMMR(::WinSendMsg( m_hWnd
                                   ,BKM_QUERYTABTEXT
                                   ,MPFROMLONG((ULONG)m_alPageId[nPage])
                                   ,MPFROMP(&vBookText)
                                  ));
    if (ulRc == BOOKERR_INVALID_PARAMETERS || ulRc == 0L)
    {
        return wxEmptyString;
    }
    if (ulRc > 255L)
        ulRc = 255L;

    vBookText.pString[ulRc] = '\0';
    sStr = vBookText.pString;
    return sStr;
} // end of wxNotebook::GetPageText

int wxNotebook::GetPageImage (
  int                               nPage
) const
{
    wxCHECK_MSG( IS_VALID_PAGE(nPage), -1, wxT("notebook page out of range") );

    //
    // For OS/2 just return the page
    //
    return nPage;
} // end of wxNotebook::GetPageImage

bool wxNotebook::SetPageImage (
  int                               nPage
, int                               nImage
)
{
    wxBitmap*                       pBitmap = (wxBitmap*)m_imageList->GetBitmap(nImage);


    ULONG                           ulPageId = (ULONG)m_alPageId[nPage];

    return (bool)::WinSendMsg( GetHWND()
                              ,BKM_SETTABBITMAP
                              ,MPFROMLONG((ULONG)m_alPageId[nPage])
                              ,(MPARAM)pBitmap->GetHBITMAP()
                             );
} // end of wxNotebook::SetPageImage

void wxNotebook::SetImageList (
  wxImageList*                      WXUNUSED(pImageList)
)
{
    //
    // Does nothing under OS/2
    //
} // end of wxNotebook::SetImageList

// ----------------------------------------------------------------------------
// wxNotebook size settings
// ----------------------------------------------------------------------------
void wxNotebook::SetPageSize (
  const wxSize&                     rSize
)
{
    RECTL                           vRect;

    //
    // Transform the page size into the notebook size
    //
    vRect.xLeft   = vRect.yTop = 0;
    vRect.xRight  = rSize.x;
    vRect.yBottom = rSize.y;


    //
    // And now set it
    //
    SetSize( vRect.xRight - vRect.xLeft
            ,vRect.yBottom - vRect.yTop
           );
} // end of wxNotebook::SetPageSize

void wxNotebook::SetPadding (
  const wxSize&                     WXUNUSED(rPadding)
)
{
    //
    // No padding in OS/2
    //
} // end of wxNotebook::SetPadding

void wxNotebook::SetTabSize (
  const wxSize&                     rSize
)
{
    ::WinSendMsg( GetHWND()
                 ,BKM_SETDIMENSIONS
                 ,MPFROM2SHORT( (USHORT)rSize.x
                               ,(USHORT)rSize.y
                              )
                 ,(MPARAM)BKA_MAJORTAB
                );
} // end of wxNotebook::SetTabSize

// ----------------------------------------------------------------------------
// wxNotebook operations
// ----------------------------------------------------------------------------

//
// Remove one page from the notebook, without deleting
//
wxNotebookPage* wxNotebook::DoRemovePage (
  int                               nPage
)
{
    wxNotebookPage*                 pPageRemoved = wxNotebookBase::DoRemovePage(nPage);

    if (!pPageRemoved)
        return NULL;


    ULONG                           ulPageId = (ULONG)m_alPageId[nPage];

    ::WinSendMsg( GetHWND()
                 ,BKM_DELETEPAGE
                 ,MPFROMLONG((ULONG)m_alPageId[nPage])
                 ,(MPARAM)BKA_TAB
                );
    if (m_pages.IsEmpty())
    {
        //
        // No selection any more, the notebook becamse empty
        //
        m_nSelection = -1;
    }
    else // notebook still not empty
    {
        //
        // Change the selected page if it was deleted or became invalid
        //
        int                         nSelNew;

        if (m_nSelection == GetPageCount())
        {
            //
            // Last page deleted, make the new last page the new selection
            //
            nSelNew = m_nSelection - 1;
        }
        else if (nPage <= m_nSelection)
        {
            //
            // We must show another page, even if it has the same index
            //
            nSelNew = m_nSelection;
        }
        else // nothing changes for the currently selected page
        {
            nSelNew = -1;

            //
            // We still must refresh the current page: this needs to be done
            // for some unknown reason if the tab control shows the up-down
            // control (i.e. when there are too many pages) -- otherwise after
            // deleting a page nothing at all is shown
            //
            m_pages[m_nSelection]->Refresh();
        }

        if (nSelNew != -1)
        {
            //
            // m_nSelection must be always valid so reset it before calling
            // SetSelection()
            //
            m_nSelection = -1;
            SetSelection(nSelNew);
        }
    }
    return pPageRemoved;
} // end of wxNotebook::DoRemovePage

//
// Remove all pages
//
bool wxNotebook::DeleteAllPages()
{
    int                             nPageCount = GetPageCount();
    int                             nPage;

    for (nPage = 0; nPage < nPageCount; nPage++)
        delete m_pages[nPage];
    m_pages.Clear();
    ::WinSendMsg( GetHWND()
                 ,BKM_DELETEPAGE
                 ,(MPARAM)0
                 ,(MPARAM)BKA_ALL
                );
    m_nSelection = -1;
    return TRUE;
} // end of wxNotebook::DeleteAllPages

//
// Add a page to the notebook
//
bool wxNotebook::AddPage (
  wxNotebookPage*                   pPage
, const wxString&                   rStrText
, bool                              bSelect
, int                               nImageId
)
{
    return InsertPage( GetPageCount()
                      ,pPage
                      ,rStrText
                      ,bSelect
                      ,nImageId
                     );
} // end of wxNotebook::AddPage

//
// Same as AddPage() but does it at given position
//
bool wxNotebook::InsertPage (
  int                               nPage
, wxNotebookPage*                   pPage
, const wxString&                   rsStrText
, bool                              bSelect
, int                               nImageId
)
{
    ULONG                           ulApiPage;

    wxASSERT( pPage != NULL );
    wxCHECK( IS_VALID_PAGE(nPage) || nPage == GetPageCount(), FALSE );

    //
    // Under OS/2 we can only insert FIRST, LAST, NEXT or PREV.  Requires
    // two different calls to the API.  Page 1 uses the BKA_FIRST.  Subsequent
    // pages use the previous page ID coupled with a BKA_NEXT call.  Unlike
    // Windows, OS/2 uses an internal Page ID to ID the pages.
    //
    // OS/2 also has a nice auto-size feature that automatically sizes the
    // the attached window so we don't have to worry about the size of the
    // window on the page.
    //
    if (nPage == 0)
    {
        ulApiPage = LONGFROMMR(::WinSendMsg( GetHWND()
                                            ,BKM_INSERTPAGE
                                            ,(MPARAM)0
                                            ,MPFROM2SHORT(BKA_AUTOPAGESIZE | BKA_MAJOR, BKA_FIRST)
                                           ));
        if (ulApiPage == 0L)
        {
            ERRORID                 vError;
            wxString                sError;

            vError = ::WinGetLastError(vHabmain);
            sError = wxPMErrorToStr(vError);
            return FALSE;
        }
        m_alPageId.Insert((long)ulApiPage, nPage);
    }
    else
    {
        ulApiPage = LONGFROMMR(::WinSendMsg( GetHWND()
                                            ,BKM_INSERTPAGE
                                            ,MPFROMLONG((ULONG)m_alPageId[nPage - 1])
                                            ,MPFROM2SHORT(BKA_AUTOPAGESIZE | BKA_MAJOR, BKA_NEXT)
                                           ));
        if (ulApiPage == 0L)
        {
            ERRORID                     vError;
            wxString                    sError;

            vError = ::WinGetLastError(vHabmain);
            sError = wxPMErrorToStr(vError);
            return FALSE;
        }
        m_alPageId.Insert((long)ulApiPage, nPage);
    }

    //
    // Associate a window handle with the page
    //
    if (pPage)
    {
        if (!::WinSendMsg( GetHWND()
                          ,BKM_SETPAGEWINDOWHWND
                          ,MPFROMLONG((ULONG)m_alPageId[nPage])
                          ,MPFROMHWND(pPage->GetHWND())
                         ))
            return FALSE;
    }
    //
    // If the inserted page is before the selected one, we must update the
    // index of the selected page
    //
    if (nPage <= m_nSelection)
    {
        //
        // One extra page added
        //
        m_nSelection++;
    }

    if (pPage)
    {
        //
        // Save the pointer to the page
        //
        m_pages.Insert( pPage
                       ,nPage
                      );
    }

    //
    // Now set TAB dimenstions
    //

    wxWindowDC                      vDC(this);
    wxCoord                         nTextX;
    wxCoord                         nTextY;

    vDC.GetTextExtent(rsStrText, &nTextX, &nTextY);
    nTextY *= 2;
    nTextX *= 1.3;
    if (nTextX > m_nTabSize)
    {
        m_nTabSize = nTextX;
        ::WinSendMsg( GetHWND()
                     ,BKM_SETDIMENSIONS
                     ,MPFROM2SHORT((USHORT)m_nTabSize, (USHORT)nTextY)
                     ,(MPARAM)BKA_MAJORTAB
                    );
    }
    //
    // Now set any TAB text
    //
    if (!rsStrText.IsEmpty())
    {
        if (!SetPageText( nPage
                         ,rsStrText
                        ))
            return FALSE;
    }

    //
    // Now set any TAB bitmap image
    //
    if (nImageId != -1)
    {
        if (!SetPageImage( nPage
                          ,nImageId
                         ))
            return FALSE;
    }

    if (pPage)
    {
        //
        // Don't show pages by default (we'll need to adjust their size first)
        //
        HWND                        hWnd = GetWinHwnd(pPage);

        WinSetWindowULong( hWnd
                          ,QWL_STYLE
                          ,WinQueryWindowULong( hWnd
                                               ,QWL_STYLE
                                              ) & ~WS_VISIBLE
                         );

        //
        // This updates internal flag too - otherwise it will get out of sync
        //
        pPage->Show(FALSE);
    }

    //
    // Some page should be selected: either this one or the first one if there is
    // still no selection
    //
    int                             nSelNew = -1;

    if (bSelect)
        nSelNew = nPage;
    else if ( m_nSelection == -1 )
        nSelNew = 0;

    if (nSelNew != -1)
        SetSelection(nSelNew);
    return TRUE;
} // end of wxNotebook::InsertPage

// ----------------------------------------------------------------------------
// wxNotebook callbacks
// ----------------------------------------------------------------------------
void wxNotebook::OnSize(
  wxSizeEvent&                      rEvent
)
{
    int                             nPage;
    int                             nCount = (int)m_pages.Count();

    for (nPage = 0; nPage < nCount; nPage++)
    {
        if (m_nSelection == nPage)
            m_pages[nPage]->Refresh();
        else
            ::WinSetWindowPos(m_pages[nPage]->GetHWND()
                              ,NULLHANDLE
                              ,0,0,0,0
                              ,SWP_HIDE
                             );
    }
    rEvent.Skip();
} // end of wxNotebook::OnSize

void wxNotebook::OnSelChange (
  wxNotebookEvent&                  rEvent
)
{
    //
    // Is it our tab control?
    //
    if (rEvent.GetEventObject() == this)
    {
        int                         nPageCount = GetPageCount();
        int                         nSel;
        ULONG                       ulOS2Sel = (ULONG)rEvent.GetOldSelection();
        bool                        bFound = FALSE;

        for (nSel = 0; nSel < nPageCount; nSel++)
        {
            if (ulOS2Sel == m_alPageId[nSel])
            {
                bFound = TRUE;
                break;
            }
        }

        if (!bFound)
            return;

        m_pages[nSel]->Show(FALSE);

        ulOS2Sel = (ULONG)rEvent.GetSelection();

        bFound = FALSE;

        for (nSel = 0; nSel < nPageCount; nSel++)
        {
            if (ulOS2Sel == m_alPageId[nSel])
            {
                bFound = TRUE;
                break;
            }
        }

        if (!bFound)
            return;

        wxNotebookPage*         pPage = m_pages[nSel];

        pPage->Show(TRUE);
        m_nSelection = nSel;
    }

    //
    // We want to give others a chance to process this message as well
    //
    rEvent.Skip();
} // end of wxNotebook::OnSelChange

void wxNotebook::OnSetFocus (
  wxFocusEvent&                     rEvent
)
{
    //
    // This function is only called when the focus is explicitly set (i.e. from
    // the program) to the notebook - in this case we don't need the
    // complicated OnNavigationKey() logic because the programmer knows better
    // what [s]he wants
    //
    // set focus to the currently selected page if any
    //
    if (m_nSelection != -1)
        m_pages[m_nSelection]->SetFocus();
    rEvent.Skip();
} // end of wxNotebook::OnSetFocus

void wxNotebook::OnNavigationKey (
  wxNavigationKeyEvent&             rEvent
)
{
    if (rEvent.IsWindowChange())
    {
        //
        // Change pages
        //
        AdvanceSelection(rEvent.GetDirection());
    }
    else
    {
        //
        // We get this event in 2 cases
        //
        // a) one of our pages might have generated it because the user TABbed
        // out from it in which case we should propagate the event upwards and
        // our parent will take care of setting the focus to prev/next sibling
        //
        // or
        //
        // b) the parent panel wants to give the focus to us so that we
        // forward it to our selected page. We can't deal with this in
        // OnSetFocus() because we don't know which direction the focus came
        // from in this case and so can't choose between setting the focus to
        // first or last panel child
        //
        wxWindow*                   pParent = GetParent();

        if (rEvent.GetEventObject() == pParent)
        {
            //
            // No, it doesn't come from child, case (b): forward to a page
            //
            if (m_nSelection != -1)
            {
                //
                // So that the page knows that the event comes from it's parent
                // and is being propagated downwards
                //
                rEvent.SetEventObject(this);

                wxWindow*           pPage = m_pages[m_nSelection];

                if (!pPage->GetEventHandler()->ProcessEvent(rEvent))
                {
                    pPage->SetFocus();
                }
                //else: page manages focus inside it itself
            }
            else
            {
                //
                // We have no pages - still have to give focus to _something_
                //
                SetFocus();
            }
        }
        else
        {
            //
            // It comes from our child, case (a), pass to the parent
            //
            if (pParent)
            {
                rEvent.SetCurrentFocus(this);
                pParent->GetEventHandler()->ProcessEvent(rEvent);
            }
        }
    }
} // end of wxNotebook::OnNavigationKey

// ----------------------------------------------------------------------------
// wxNotebook base class virtuals
// ----------------------------------------------------------------------------

//
// Override these 2 functions to do nothing: everything is done in OnSize
//
void wxNotebook::SetConstraintSizes(
  bool                              WXUNUSED(bRecurse)
)
{
    //
    // Don't set the sizes of the pages - their correct size is not yet known
    //
    wxControl::SetConstraintSizes(FALSE);
} // end of wxNotebook::SetConstraintSizes

bool wxNotebook::DoPhase (
  int                               WXUNUSED(nPhase)
)
{
    return TRUE;
} // end of wxNotebook::DoPhase

// ----------------------------------------------------------------------------
// wxNotebook Windows message handlers
// ----------------------------------------------------------------------------
bool wxNotebook::OS2OnScroll (
  int                               nOrientation
, WXWORD                            wSBCode
, WXWORD                            wPos
, WXHWND                            wControl
)
{
    //
    // Don't generate EVT_SCROLLWIN events for the WM_SCROLLs coming from the
    // up-down control
    //
    if (wControl)
        return FALSE;
    return wxNotebookBase::OS2OnScroll( nOrientation
                                       ,wSBCode
                                       ,wPos
                                       ,wControl
                                      );
} // end of wxNotebook::OS2OnScroll

#endif // wxUSE_NOTEBOOK

