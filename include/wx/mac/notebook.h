/////////////////////////////////////////////////////////////////////////////
// Name:        notebook.h
// Purpose:     MSW/GTK compatible notebook (a.k.a. property sheet)
// Author:      AUTHOR
// Modified by:
// RCS-ID:      $Id: notebook.h,v 1.14.2.1 2002/10/18 17:22:44 SC Exp $
// Copyright:   (c) AUTHOR
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_NOTEBOOK_H_
#define _WX_NOTEBOOK_H_

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "notebook.h"
#endif

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------
#include "wx/event.h"

// ----------------------------------------------------------------------------
// types
// ----------------------------------------------------------------------------

// fwd declarations
class WXDLLEXPORT wxImageList;
class WXDLLEXPORT wxWindow;

// ----------------------------------------------------------------------------
// wxNotebook
// ----------------------------------------------------------------------------

// @@@ this class should really derive from wxTabCtrl, but the interface is not
//     exactly the same, so I can't do it right now and instead we reimplement
//     part of wxTabCtrl here
class wxNotebook : public wxNotebookBase
{
public:
  // ctors
  // -----
    // default for dynamic class
  wxNotebook();
    // the same arguments as for wxControl (@@@ any special styles?)
  wxNotebook(wxWindow *parent,
             wxWindowID id, 
             const wxPoint& pos = wxDefaultPosition,
             const wxSize& size = wxDefaultSize,
             long style = 0,
             const wxString& name = "notebook");
    // Create() function
  bool Create(wxWindow *parent,
              wxWindowID id, 
              const wxPoint& pos = wxDefaultPosition,
              const wxSize& size = wxDefaultSize,
              long style = 0,
              const wxString& name = "notebook");
    // dtor
  ~wxNotebook();

  // accessors
  // ---------
    // set the currently selected page, return the index of the previously
    // selected one (or -1 on error)
    // NB: this function will _not_ generate wxEVT_NOTEBOOK_PAGE_xxx events
  int SetSelection(int nPage);
    // get the currently selected page
  int GetSelection() const { return m_nSelection; }

    // set/get the title of a page
  bool SetPageText(int nPage, const wxString& strText);
  wxString GetPageText(int nPage) const;

    // sets/returns item's image index in the current image list
  int  GetPageImage(int nPage) const;
  bool SetPageImage(int nPage, int nImage);

  // control the appearance of the notebook pages
    // set the size (the same for all pages)
  virtual void SetPageSize(const wxSize& size);
    // set the padding between tabs (in pixels)
  virtual void SetPadding(const wxSize& padding);
    // sets the size of the tabs (assumes all tabs are the same size)
  virtual void SetTabSize(const wxSize& sz);
   
    // calculate size for wxNotebookSizer
  wxSize CalcSizeFromPage(const wxSize& sizePage);

/*
    // get number of pages in the dialog
  int GetPageCount() const;

    // cycle thru the tabs
  void AdvanceSelection(bool bForward = TRUE);


    // currently it's always 1 because wxGTK doesn't support multi-row
    // tab controls
  int GetRowCount() const;
*/
  // operations
  // ----------
    // remove all pages
  bool DeleteAllPages();
    // the same as AddPage(), but adds it at the specified position
  bool InsertPage(int nPage,
                  wxNotebookPage *pPage,
                  const wxString& strText,
                  bool bSelect = FALSE,
                  int imageId = -1);
/*
    // get the panel which represents the given page
  wxNotebookPage *GetPage(int nPage) { return m_aPages[nPage]; }
*/
  // callbacks
  // ---------
  void OnSize(wxSizeEvent& event);
  void OnSelChange(wxNotebookEvent& event);
  void OnSetFocus(wxFocusEvent& event);
  void OnNavigationKey(wxNavigationKeyEvent& event);
  void OnMouse(wxMouseEvent &event);  

    // implementation
    // --------------

#if wxUSE_CONSTRAINTS
  virtual void SetConstraintSizes(bool recurse = TRUE);
  virtual bool DoPhase(int nPhase);

#endif

  // base class virtuals
  // -------------------
  virtual void Command(wxCommandEvent& event);
protected:
    virtual wxNotebookPage *DoRemovePage(int page) ;
	virtual void MacHandleControlClick( WXWidget control , wxInt16 controlpart ) ;
  // common part of all ctors
  void Init();

  // helper functions
  void ChangePage(int nOldSel, int nSel); // change pages
  void MacSetupTabs();

  // the icon indices
  wxArrayInt m_images;

  int m_nSelection;           // the current selection (-1 if none)

  DECLARE_DYNAMIC_CLASS(wxNotebook)
  DECLARE_EVENT_TABLE()
};


#endif // _WX_NOTEBOOK_H_
