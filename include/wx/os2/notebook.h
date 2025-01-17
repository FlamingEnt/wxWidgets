/////////////////////////////////////////////////////////////////////////////
// Name:        notebook.h
// Purpose:     MSW/GTK compatible notebook (a.k.a. property sheet)
// Author:      David Webster
// Modified by:
// RCS-ID:      $Id: NOTEBOOK.H,v 1.8.2.1 2002/11/27 05:48:39 DW Exp $
// Copyright:   (c) David Webster
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _NOTEBOOK_H
#define _NOTEBOOK_H

#if wxUSE_NOTEBOOK

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "wx/control.h"

// ----------------------------------------------------------------------------
// wxNotebook
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxNotebook : public wxNotebookBase
{
public:
    //
    // Ctors
    // -----
    // Default for dynamic class
    //
    wxNotebook();

    //
    // the same arguments as for wxControl
    //
    wxNotebook( wxWindow*       pParent
               ,wxWindowID      vId
               ,const wxPoint&  rPos = wxDefaultPosition
               ,const wxSize&   rSize = wxDefaultSize
               ,long            lStyle = 0
               ,const wxString& rsName = "notebook"
              );

    bool Create( wxWindow*       pParent
                ,wxWindowID      vId
                ,const wxPoint&  rPos = wxDefaultPosition
                ,const wxSize&   rSize = wxDefaultSize
                ,long            lStyle = 0
                ,const wxString& rsName = "notebook"
               );

    //
    // Accessors
    // ---------
    // Get number of pages in the dialog
    //
           int      GetPageCount(void) const;

    //
    // Set the currently selected page, return the index of the previously
    // selected one (or -1 on error)
    // NB: this function will _not_ generate wxEVT_NOTEBOOK_PAGE_xxx events
    //
           int      SetSelection(int nPage);

    //
    // Get the currently selected page
    //
    inline int      GetSelection(void) const { return m_nSelection; }

    //
    // Set/Get the title of a page
    //
           bool     SetPageText( int             nPage
                                ,const wxString& sStrText
                               );
           wxString GetPageText(int nPage) const;

    //
    // Image list stuff: each page may have an image associated with it. All
    // the images belong to an image list, so you have to
    // 1) create an image list
    // 2) associate it with the notebook
    // 3) set for each page it's image
    // associate image list with a control
    //
           void     SetImageList(wxImageList* pImageList);

    //
    // Sets/returns item's image index in the current image list
    //
           int      GetPageImage(int nPage) const;
           bool     SetPageImage( int nPage
                                 ,int nImage
                                );

    //
    // Currently it's always 1 because wxGTK doesn't support multi-row
    // tab controls
    //
           int      GetRowCount(void) const;

    //
    // control the appearance of the notebook pages
    // set the size (the same for all pages)
    //
           void     SetPageSize(const wxSize& rSize);

    //
    // Set the padding between tabs (in pixels)
    //
           void     SetPadding(const wxSize& rPadding);

    //
    // Operations
    // ----------
    // Remove all pages
    //
           bool     DeleteAllPages(void);

    //
    // Adds a new page to the notebook (it will be deleted ny the notebook,
    // don't delete it yourself). If bSelect, this page becomes active.
    //
           bool     AddPage( wxNotebookPage* pPage
                            ,const wxString& rsStrText
                            ,bool            bSelect = FALSE
                            ,int             nImageId = -1
                           );

    //
    // The same as AddPage(), but adds it at the specified position
    //
           bool     InsertPage( int nPage
                               ,wxNotebookPage* pPage
                               ,const wxString& rsStrText
                               ,bool            bSelect = FALSE
                               ,int             nImageId = -1
                              );

    //
    // Windows-only at present. Also, you must use the wxNB_FIXEDWIDTH
    // style.
    //
           void     SetTabSize(const wxSize& rSize);

    //
    // Callbacks
    // ---------
    //
    void OnSize(wxSizeEvent& rEvent);
    void OnSelChange(wxNotebookEvent& rEvent);
    void OnSetFocus(wxFocusEvent& rEvent);
    void OnNavigationKey(wxNavigationKeyEvent& rEvent);

    //
    // Base class virtuals
    // -------------------
    //
    virtual bool OS2OnScroll( int    nOrientation
                             ,WXWORD wSBCode
                             ,WXWORD wPos
                             ,WXHWND hControl
                            );
    virtual void SetConstraintSizes(bool bRecurse = TRUE);
    virtual bool DoPhase(int nPhase);

protected:
    //
    // Common part of all ctors
    //
    void                    Init(void);

    //
    // Translate wxWin styles to the PM ones
    //
    virtual WXDWORD         OS2GetStyle( long     lFlags
                                        ,WXDWORD* pwExstyle = NULL
                                       ) const;

    //
    // Remove one page from the notebook, without deleting
    //
    virtual wxNotebookPage* DoRemovePage(int nPage);

    //
    // Helper functions
    //

    int                     m_nSelection; // The current selection (-1 if none)

private:
    wxArrayLong                     m_alPageId;
    int                             m_nTabSize; // holds the largest tab size

    DECLARE_DYNAMIC_CLASS(wxNotebook)
    DECLARE_EVENT_TABLE()
}; // end of CLASS wxNotebook

#endif // wxUSE_NOTEBOOK

#endif // _NOTEBOOK_H

