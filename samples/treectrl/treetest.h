/////////////////////////////////////////////////////////////////////////////
// Name:        treectrl.h
// Purpose:     wxTreeCtrl sample
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id: treetest.h,v 1.31.2.3 2003/05/07 09:48:22 JS Exp $
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#define USE_GENERIC_TREECTRL 0

#if USE_GENERIC_TREECTRL
#include "wx/generic/treectlg.h"
#ifndef wxTreeCtrl
#define wxTreeCtrl wxGenericTreeCtrl
#define sm_classwxTreeCtrl sm_classwxGenericTreeCtrl
#endif
#endif

// Define a new application type
class MyApp : public wxApp
{
public:
    MyApp() { m_showImages = TRUE; m_showButtons = FALSE; }

    bool OnInit();

    void SetShowImages(bool show) { m_showImages = show; }
    bool ShowImages() const { return m_showImages; }

    void SetShowButtons(bool show) { m_showButtons = show; }
    bool ShowButtons() const { return m_showButtons; }

private:
    bool m_showImages, m_showButtons;
};

class MyTreeItemData : public wxTreeItemData
{
public:
    MyTreeItemData(const wxString& desc) : m_desc(desc) { }

    void ShowInfo(wxTreeCtrl *tree);
    const wxChar *GetDesc() const { return m_desc.c_str(); }

private:
    wxString m_desc;
};

class MyTreeCtrl : public wxTreeCtrl
{
public:
    enum
    {
        TreeCtrlIcon_File,
        TreeCtrlIcon_FileSelected,
        TreeCtrlIcon_Folder,
        TreeCtrlIcon_FolderSelected,
        TreeCtrlIcon_FolderOpened
    };

    MyTreeCtrl() { }
    MyTreeCtrl(wxWindow *parent, const wxWindowID id,
               const wxPoint& pos, const wxSize& size,
               long style);
    virtual ~MyTreeCtrl();

    void OnBeginDrag(wxTreeEvent& event);
    void OnBeginRDrag(wxTreeEvent& event);
    void OnEndDrag(wxTreeEvent& event);
    void OnBeginLabelEdit(wxTreeEvent& event);
    void OnEndLabelEdit(wxTreeEvent& event);
    void OnDeleteItem(wxTreeEvent& event);
    void OnRMouseUp(wxMouseEvent& event);
    void OnGetInfo(wxTreeEvent& event);
    void OnTreeRMouseClick(wxTreeEvent& event);
    void OnItemRightClick(wxTreeEvent& event);
    void OnSetInfo(wxTreeEvent& event);
    void OnItemExpanded(wxTreeEvent& event);
    void OnItemExpanding(wxTreeEvent& event);
    void OnItemCollapsed(wxTreeEvent& event);
    void OnItemCollapsing(wxTreeEvent& event);
    void OnSelChanged(wxTreeEvent& event);
    void OnSelChanging(wxTreeEvent& event);
    void OnTreeKeyDown(wxTreeEvent& event);
    void OnItemActivated(wxTreeEvent& event);
    void OnRMouseDClick(wxMouseEvent& event);

    void GetItemsRecursively(const wxTreeItemId& idParent, long cookie);

    void CreateImageList(int size = 16);
    void CreateButtonsImageList(int size = 11);

    void AddTestItemsToTree(size_t numChildren, size_t depth);

    void DoSortChildren(const wxTreeItemId& item, bool reverse = FALSE)
        { m_reverseSort = reverse; wxTreeCtrl::SortChildren(item); }
    void DoEnsureVisible() { EnsureVisible(m_lastItem); }

    void DoToggleIcon(const wxTreeItemId& item);

    void ShowMenu(wxTreeItemId id, const wxPoint& pt);

    int ImageSize(void) const { return m_imageSize; }

protected:
    virtual int OnCompareItems(const wxTreeItemId& i1, const wxTreeItemId& i2);

    // is this the test item which we use in several event handlers?
    bool IsTestItem(const wxTreeItemId& item)
    {
        // the test item is the first child folder
        return GetItemParent(item) == GetRootItem() && !GetPrevSibling(item);
    }

private:
    void AddItemsRecursively(const wxTreeItemId& idParent,
                             size_t nChildren,
                             size_t depth,
                             size_t folder);

    int          m_imageSize;               // current size of images
    bool         m_reverseSort;             // flag for OnCompareItems
    wxTreeItemId m_lastItem,                // for OnEnsureVisible()
                 m_draggedItem;             // item being dragged right now

    // NB: due to an ugly wxMSW hack you _must_ use DECLARE_DYNAMIC_CLASS()
    //     if you want your overloaded OnCompareItems() to be called.
    //     OTOH, if you don't want it you may omit the next line - this will
    //     make default (alphabetical) sorting much faster under wxMSW.
    DECLARE_DYNAMIC_CLASS(MyTreeCtrl)
    DECLARE_EVENT_TABLE()
};

// Define a new frame type
class MyFrame: public wxFrame
{
public:
    // ctor and dtor
    MyFrame(const wxString& title, int x, int y, int w, int h);
    virtual ~MyFrame();

    // menu callbacks
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);

    void OnTogButtons(wxCommandEvent& event)
        { TogStyle(event.GetId(), wxTR_HAS_BUTTONS); }
    void OnTogTwist(wxCommandEvent& event)
        { TogStyle(event.GetId(), wxTR_TWIST_BUTTONS); }
    void OnTogLines(wxCommandEvent& event)
        { TogStyle(event.GetId(), wxTR_NO_LINES); }
    void OnTogEdit(wxCommandEvent& event)
        { TogStyle(event.GetId(), wxTR_EDIT_LABELS); }
    void OnTogHideRoot(wxCommandEvent& event)
        { TogStyle(event.GetId(), wxTR_HIDE_ROOT); }
    void OnTogRootLines(wxCommandEvent& event)
        { TogStyle(event.GetId(), wxTR_LINES_AT_ROOT); }
    void OnTogBorder(wxCommandEvent& event)
        { TogStyle(event.GetId(), wxTR_ROW_LINES); }
    void OnTogFullHighlight(wxCommandEvent& event)
        { TogStyle(event.GetId(), wxTR_FULL_ROW_HIGHLIGHT); }

    void OnResetStyle(wxCommandEvent& WXUNUSED(event))
        { CreateTreeWithDefStyle(); }

    void OnSetFgColour(wxCommandEvent& event);
    void OnSetBgColour(wxCommandEvent& event);

    void OnDump(wxCommandEvent& event);
#ifndef NO_MULTIPLE_SELECTION
    void OnDumpSelected(wxCommandEvent& event);
    void OnSelect(wxCommandEvent& event);
    void OnUnselect(wxCommandEvent& event);
    void OnToggleSel(wxCommandEvent& event);
#endif // NO_MULTIPLE_SELECTION
    void OnDelete(wxCommandEvent& event);
    void OnDeleteChildren(wxCommandEvent& event);
    void OnDeleteAll(wxCommandEvent& event);

    void OnRecreate(wxCommandEvent& event);
    void OnToggleButtons(wxCommandEvent& event);
    void OnToggleImages(wxCommandEvent& event);
    void OnSetImageSize(wxCommandEvent& event);
    void OnCollapseAndReset(wxCommandEvent& event);

    void OnSetBold(wxCommandEvent& WXUNUSED(event)) { DoSetBold(TRUE); }
    void OnClearBold(wxCommandEvent& WXUNUSED(event)) { DoSetBold(FALSE); }

    void OnEnsureVisible(wxCommandEvent& event);

    void OnCount(wxCommandEvent& event);
    void OnCountRec(wxCommandEvent& event);

    void OnRename(wxCommandEvent& event);
    void OnSort(wxCommandEvent& event) { DoSort(); }
    void OnSortRev(wxCommandEvent& event) { DoSort(TRUE); }

    void OnAddItem(wxCommandEvent& event);
    void OnInsertItem(wxCommandEvent& event);

    void OnIncIndent(wxCommandEvent& event);
    void OnDecIndent(wxCommandEvent& event);

    void OnIncSpacing(wxCommandEvent& event);
    void OnDecSpacing(wxCommandEvent& event);

    void OnToggleIcon(wxCommandEvent& event);

    void OnSize(wxSizeEvent& event);

private:
    void TogStyle(int id, long flag);

    void DoSort(bool reverse = FALSE);

    void Resize();

    void CreateTreeWithDefStyle();
    void CreateTree(long style);

    MyTreeCtrl *m_treeCtrl;
    wxTextCtrl *m_textCtrl;

    void DoSetBold(bool bold = TRUE);

    DECLARE_EVENT_TABLE()
};

// menu and control ids
enum
{
    TreeTest_Quit,
    TreeTest_About,
    TreeTest_TogButtons,
    TreeTest_TogTwist,
    TreeTest_TogLines,
    TreeTest_TogEdit,
    TreeTest_TogHideRoot,
    TreeTest_TogRootLines,
    TreeTest_TogBorder,
    TreeTest_TogFullHighlight,
    TreeTest_SetFgColour,
    TreeTest_SetBgColour,
    TreeTest_ResetStyle,
    TreeTest_Dump,
    TreeTest_DumpSelected,
    TreeTest_Count,
    TreeTest_CountRec,
    TreeTest_Sort,
    TreeTest_SortRev,
    TreeTest_SetBold,
    TreeTest_ClearBold,
    TreeTest_Rename,
    TreeTest_Delete,
    TreeTest_DeleteChildren,
    TreeTest_DeleteAll,
    TreeTest_Recreate,
    TreeTest_ToggleImages,
    TreeTest_ToggleButtons,
    TreeTest_SetImageSize,
    TreeTest_ToggleSel,
    TreeTest_CollapseAndReset,
    TreeTest_EnsureVisible,
    TreeTest_AddItem,
    TreeTest_InsertItem,
    TreeTest_IncIndent,
    TreeTest_DecIndent,
    TreeTest_IncSpacing,
    TreeTest_DecSpacing,
    TreeTest_ToggleIcon,
    TreeTest_Select,
    TreeTest_Unselect,
    TreeTest_Ctrl = 1000
};
