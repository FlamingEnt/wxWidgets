/////////////////////////////////////////////////////////////////////////////
// Name:        listctrl.cpp
// Purpose:     wxListCtrl sample
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id: listtest.cpp,v 1.60.2.4 2002/12/15 10:22:31 MBN Exp $
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation
#pragma interface
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#ifndef __WXMSW__
    #include "mondrian.xpm"

    #include "bitmaps/toolbrai.xpm"
    #include "bitmaps/toolchar.xpm"
    #include "bitmaps/tooldata.xpm"
    #include "bitmaps/toolnote.xpm"
    #include "bitmaps/tooltodo.xpm"
    #include "bitmaps/toolchec.xpm"
    #include "bitmaps/toolgame.xpm"
    #include "bitmaps/tooltime.xpm"
    #include "bitmaps/toolword.xpm"
    #include "bitmaps/small1.xpm"
#endif

#include "wx/imaglist.h"
#include "wx/listctrl.h"
#include "wx/timer.h"           // for wxStopWatch
#include "wx/colordlg.h"        // for wxGetColourFromUser

#include "listtest.h"

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_SIZE(MyFrame::OnSize)

    EVT_MENU(LIST_QUIT, MyFrame::OnQuit)
    EVT_MENU(LIST_ABOUT, MyFrame::OnAbout)
    EVT_MENU(LIST_LIST_VIEW, MyFrame::OnListView)
    EVT_MENU(LIST_REPORT_VIEW, MyFrame::OnReportView)
    EVT_MENU(LIST_ICON_VIEW, MyFrame::OnIconView)
    EVT_MENU(LIST_ICON_TEXT_VIEW, MyFrame::OnIconTextView)
    EVT_MENU(LIST_SMALL_ICON_VIEW, MyFrame::OnSmallIconView)
    EVT_MENU(LIST_SMALL_ICON_TEXT_VIEW, MyFrame::OnSmallIconTextView)
    EVT_MENU(LIST_VIRTUAL_VIEW, MyFrame::OnVirtualView)

    EVT_MENU(LIST_FOCUS_LAST, MyFrame::OnFocusLast)
    EVT_MENU(LIST_TOGGLE_FIRST, MyFrame::OnToggleFirstSel)
    EVT_MENU(LIST_DESELECT_ALL, MyFrame::OnDeselectAll)
    EVT_MENU(LIST_SELECT_ALL, MyFrame::OnSelectAll)
    EVT_MENU(LIST_DELETE, MyFrame::OnDelete)
    EVT_MENU(LIST_ADD, MyFrame::OnAdd)
    EVT_MENU(LIST_EDIT, MyFrame::OnEdit)
    EVT_MENU(LIST_DELETE_ALL, MyFrame::OnDeleteAll)
    EVT_MENU(LIST_SORT, MyFrame::OnSort)
    EVT_MENU(LIST_SET_FG_COL, MyFrame::OnSetFgColour)
    EVT_MENU(LIST_SET_BG_COL, MyFrame::OnSetBgColour)
    EVT_MENU(LIST_TOGGLE_MULTI_SEL, MyFrame::OnToggleMultiSel)
    EVT_MENU(LIST_SHOW_COL_INFO, MyFrame::OnShowColInfo)
    EVT_MENU(LIST_SHOW_SEL_INFO, MyFrame::OnShowSelInfo)
    EVT_MENU(LIST_FREEZE, MyFrame::OnFreeze)
    EVT_MENU(LIST_THAW, MyFrame::OnThaw)

    EVT_UPDATE_UI(LIST_SHOW_COL_INFO, MyFrame::OnUpdateShowColInfo)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(MyListCtrl, wxListCtrl)
    EVT_LIST_BEGIN_DRAG(LIST_CTRL, MyListCtrl::OnBeginDrag)
    EVT_LIST_BEGIN_RDRAG(LIST_CTRL, MyListCtrl::OnBeginRDrag)
    EVT_LIST_BEGIN_LABEL_EDIT(LIST_CTRL, MyListCtrl::OnBeginLabelEdit)
    EVT_LIST_END_LABEL_EDIT(LIST_CTRL, MyListCtrl::OnEndLabelEdit)
    EVT_LIST_DELETE_ITEM(LIST_CTRL, MyListCtrl::OnDeleteItem)
    EVT_LIST_DELETE_ALL_ITEMS(LIST_CTRL, MyListCtrl::OnDeleteAllItems)
    EVT_LIST_GET_INFO(LIST_CTRL, MyListCtrl::OnGetInfo)
    EVT_LIST_SET_INFO(LIST_CTRL, MyListCtrl::OnSetInfo)
    EVT_LIST_ITEM_SELECTED(LIST_CTRL, MyListCtrl::OnSelected)
    EVT_LIST_ITEM_DESELECTED(LIST_CTRL, MyListCtrl::OnDeselected)
    EVT_LIST_KEY_DOWN(LIST_CTRL, MyListCtrl::OnListKeyDown)
    EVT_LIST_ITEM_ACTIVATED(LIST_CTRL, MyListCtrl::OnActivated)
    EVT_LIST_ITEM_FOCUSED(LIST_CTRL, MyListCtrl::OnFocused)

    EVT_LIST_COL_CLICK(LIST_CTRL, MyListCtrl::OnColClick)
    EVT_LIST_COL_RIGHT_CLICK(LIST_CTRL, MyListCtrl::OnColRightClick)
    EVT_LIST_COL_BEGIN_DRAG(LIST_CTRL, MyListCtrl::OnColBeginDrag)
    EVT_LIST_COL_DRAGGING(LIST_CTRL, MyListCtrl::OnColDragging)
    EVT_LIST_COL_END_DRAG(LIST_CTRL, MyListCtrl::OnColEndDrag)

    EVT_LIST_CACHE_HINT(LIST_CTRL, MyListCtrl::OnCacheHint)

    EVT_CHAR(MyListCtrl::OnChar)
END_EVENT_TABLE()

IMPLEMENT_APP(MyApp)

// number of items in list/report view
static const int NUM_ITEMS = 30;

// number of items in icon/small icon view
static const int NUM_ICONS = 9;

int wxCALLBACK MyCompareFunction(long item1, long item2, long sortData)
{
    // inverse the order
    if (item1 < item2) return -1;
    if (item1 > item2) return 1;
    return 0;
}

// `Main program' equivalent, creating windows and returning main app frame
bool MyApp::OnInit()
{
  // Create the main frame window
  MyFrame *frame = new MyFrame(wxT("wxListCtrl Test"), 50, 50, 450, 340);

  // Show the frame
  frame->Show(TRUE);

  SetTopWindow(frame);

  return TRUE;
}

// My frame constructor
MyFrame::MyFrame(const wxChar *title, int x, int y, int w, int h)
       : wxFrame((wxFrame *)NULL, -1, title, wxPoint(x, y), wxSize(w, h))
{
    m_listCtrl = (MyListCtrl *) NULL;
    m_logWindow = (wxTextCtrl *) NULL;

    // Give it an icon
    SetIcon( wxICON(mondrian) );

    // Make an image list containing large icons
    m_imageListNormal = new wxImageList(32, 32, TRUE);
    m_imageListSmall = new wxImageList(16, 16, TRUE);

#ifdef __WXMSW__
    m_imageListNormal->Add( wxIcon(_T("icon1"), wxBITMAP_TYPE_ICO_RESOURCE) );
    m_imageListNormal->Add( wxIcon(_T("icon2"), wxBITMAP_TYPE_ICO_RESOURCE) );
    m_imageListNormal->Add( wxIcon(_T("icon3"), wxBITMAP_TYPE_ICO_RESOURCE) );
    m_imageListNormal->Add( wxIcon(_T("icon4"), wxBITMAP_TYPE_ICO_RESOURCE) );
    m_imageListNormal->Add( wxIcon(_T("icon5"), wxBITMAP_TYPE_ICO_RESOURCE) );
    m_imageListNormal->Add( wxIcon(_T("icon6"), wxBITMAP_TYPE_ICO_RESOURCE) );
    m_imageListNormal->Add( wxIcon(_T("icon7"), wxBITMAP_TYPE_ICO_RESOURCE) );
    m_imageListNormal->Add( wxIcon(_T("icon8"), wxBITMAP_TYPE_ICO_RESOURCE) );
    m_imageListNormal->Add( wxIcon(_T("icon9"), wxBITMAP_TYPE_ICO_RESOURCE) );

    m_imageListSmall->Add( wxIcon(_T("iconsmall"), wxBITMAP_TYPE_ICO_RESOURCE) );

#else
    m_imageListNormal->Add( wxIcon( toolbrai_xpm ) );
    m_imageListNormal->Add( wxIcon( toolchar_xpm ) );
    m_imageListNormal->Add( wxIcon( tooldata_xpm ) );
    m_imageListNormal->Add( wxIcon( toolnote_xpm ) );
    m_imageListNormal->Add( wxIcon( tooltodo_xpm ) );
    m_imageListNormal->Add( wxIcon( toolchec_xpm ) );
    m_imageListNormal->Add( wxIcon( toolgame_xpm ) );
    m_imageListNormal->Add( wxIcon( tooltime_xpm ) );
    m_imageListNormal->Add( wxIcon( toolword_xpm ) );

    m_imageListSmall->Add( wxIcon( small1_xpm) );
#endif

    // Make a menubar
    wxMenu *menuFile = new wxMenu;
    menuFile->Append(LIST_ABOUT, _T("&About"));
    menuFile->AppendSeparator();
    menuFile->Append(LIST_QUIT, _T("E&xit\tAlt-X"));

    wxMenu *menuView = new wxMenu;
    menuView->Append(LIST_LIST_VIEW, _T("&List view\tF1"));
    menuView->Append(LIST_REPORT_VIEW, _T("&Report view\tF2"));
    menuView->Append(LIST_ICON_VIEW, _T("&Icon view\tF3"));
    menuView->Append(LIST_ICON_TEXT_VIEW, _T("Icon view with &text\tF4"));
    menuView->Append(LIST_SMALL_ICON_VIEW, _T("&Small icon view\tF5"));
    menuView->Append(LIST_SMALL_ICON_TEXT_VIEW, _T("Small icon &view with text\tF6"));
    menuView->Append(LIST_VIRTUAL_VIEW, _T("Virtual view\tF7"));

    wxMenu *menuList = new wxMenu;
    menuList->Append(LIST_FOCUS_LAST, _T("&Make last item current\tCtrl-L"));
    menuList->Append(LIST_TOGGLE_FIRST, _T("To&ggle first item\tCtrl-G"));
    menuList->Append(LIST_DESELECT_ALL, _T("&Deselect All\tCtrl-D"));
    menuList->Append(LIST_SELECT_ALL, _T("S&elect All\tCtrl-A"));
    menuList->AppendSeparator();
    menuList->Append(LIST_SHOW_COL_INFO, _T("Show &column info\tCtrl-C"));
    menuList->Append(LIST_SHOW_SEL_INFO, _T("Show &selected items\tCtrl-S"));
    menuList->AppendSeparator();
    menuList->Append(LIST_SORT, _T("&Sort\tCtrl-S"));
    menuList->AppendSeparator();
    menuList->Append(LIST_ADD, _T("&Append an item\tCtrl-P"));
    menuList->Append(LIST_EDIT, _T("&Edit the item\tCtrl-E"));
    menuList->Append(LIST_DELETE, _T("&Delete first item\tCtrl-X"));
    menuList->Append(LIST_DELETE_ALL, _T("Delete &all items"));
    menuList->AppendSeparator();
    menuList->Append(LIST_FREEZE, _T("Free&ze\tCtrl-Z"));
    menuList->Append(LIST_THAW, _T("Tha&w\tCtrl-W"));
    menuList->AppendSeparator();
    menuList->Append(LIST_TOGGLE_MULTI_SEL, _T("&Multiple selection\tCtrl-M"),
            _T("Toggle multiple selection"), TRUE);

    wxMenu *menuCol = new wxMenu;
    menuCol->Append(LIST_SET_FG_COL, _T("&Foreground colour..."));
    menuCol->Append(LIST_SET_BG_COL, _T("&Background colour..."));

    wxMenuBar *menubar = new wxMenuBar;
    menubar->Append(menuFile, _T("&File"));
    menubar->Append(menuView, _T("&View"));
    menubar->Append(menuList, _T("&List"));
    menubar->Append(menuCol, _T("&Colour"));
    SetMenuBar(menubar);

    m_panel = new wxPanel(this, -1);
    m_logWindow = new wxTextCtrl(m_panel, -1, wxEmptyString,
                                 wxDefaultPosition, wxDefaultSize,
                                 wxTE_MULTILINE | wxSUNKEN_BORDER);

    m_logOld = wxLog::SetActiveTarget(new wxLogTextCtrl(m_logWindow));

    RecreateList(wxLC_REPORT | wxLC_SINGLE_SEL);

    CreateStatusBar(3);
}

MyFrame::~MyFrame()
{
    delete wxLog::SetActiveTarget(m_logOld);

    delete m_imageListNormal;
    delete m_imageListSmall;
}

void MyFrame::OnSize(wxSizeEvent& event)
{
    if ( !m_logWindow )
        return;

    wxSize size = GetClientSize();
    wxCoord y = (2*size.y)/3;
    m_listCtrl->SetSize(0, 0, size.x, y);
    m_logWindow->SetSize(0, y + 1, size.x, size.y - y);

    event.Skip();
}

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    Close(TRUE);
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxMessageDialog dialog(this, _T("List test sample\nJulian Smart (c) 1997"),
            _T("About list test"), wxOK|wxCANCEL);

    dialog.ShowModal();
}

void MyFrame::OnFreeze(wxCommandEvent& event)
{
    wxLogMessage(_T("Freezing the control"));

    m_listCtrl->Freeze();
}

void MyFrame::OnThaw(wxCommandEvent& event)
{
    wxLogMessage(_T("Thawing the control"));

    m_listCtrl->Thaw();
}

void MyFrame::OnFocusLast(wxCommandEvent& WXUNUSED(event))
{
    long index = m_listCtrl->GetItemCount() - 1;
    if ( index == -1 )
    {
        return;
    }

    m_listCtrl->SetItemState(index, wxLIST_STATE_FOCUSED, wxLIST_STATE_FOCUSED);
    m_listCtrl->EnsureVisible(index);
}

void MyFrame::OnToggleFirstSel(wxCommandEvent& WXUNUSED(event))
{
    m_listCtrl->SetItemState(0, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
}

void MyFrame::OnDeselectAll(wxCommandEvent& WXUNUSED(event))
{
    int n = m_listCtrl->GetItemCount();
    for (int i = 0; i < n; i++)
        m_listCtrl->SetItemState(i,0,wxLIST_STATE_SELECTED);
}

void MyFrame::OnSelectAll(wxCommandEvent& WXUNUSED(event))
{
    int n = m_listCtrl->GetItemCount();
    for (int i = 0; i < n; i++)
        m_listCtrl->SetItemState(i,wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
}

// ----------------------------------------------------------------------------
// changing listctrl modes
// ----------------------------------------------------------------------------

void MyFrame::RecreateList(long flags, bool withText)
{
    // we could avoid recreating it if we don't set/clear the wxLC_VIRTUAL
    // style, but it is more trouble to do it than not
#if 0
    if ( !m_listCtrl || ((flags & wxLC_VIRTUAL) !=
            (m_listCtrl->GetWindowStyleFlag() & wxLC_VIRTUAL)) )
#endif
    {
        delete m_listCtrl;

        m_listCtrl = new MyListCtrl(m_panel, LIST_CTRL,
                                    wxDefaultPosition, wxDefaultSize,
                                    flags |
                                    wxSUNKEN_BORDER | wxLC_EDIT_LABELS);

        switch ( flags & wxLC_MASK_TYPE )
        {
            case wxLC_LIST:
                InitWithListItems();
                break;

            case wxLC_ICON:
                InitWithIconItems(withText);
                break;

            case wxLC_SMALL_ICON:
                InitWithIconItems(withText, TRUE);
                break;

            case wxLC_REPORT:
                if ( flags & wxLC_VIRTUAL )
                    InitWithVirtualItems();
                else
                    InitWithReportItems();
                break;

            default:
                wxFAIL_MSG( _T("unknown listctrl mode") );
        }
    }

#ifdef __WXMSW__
        SendSizeEvent();
#endif

    m_logWindow->Clear();
}

void MyFrame::OnListView(wxCommandEvent& WXUNUSED(event))
{
    RecreateList(wxLC_LIST);
}

void MyFrame::InitWithListItems()
{
    for ( int i = 0; i < NUM_ITEMS; i++ )
    {
        m_listCtrl->InsertItem(i, wxString::Format(_T("Item %d"), i));
    }
}

void MyFrame::OnReportView(wxCommandEvent& WXUNUSED(event))
{
    RecreateList(wxLC_REPORT);
}

void MyFrame::InitWithReportItems()
{
    m_listCtrl->SetImageList(m_imageListSmall, wxIMAGE_LIST_SMALL);

    // under MSW for SetColumnWidth() to work we need to create the items with
    // images initially
#if 1
    wxListItem itemCol;
    itemCol.m_mask = wxLIST_MASK_TEXT | wxLIST_MASK_IMAGE;
    itemCol.m_text = _T("Column 1");
    itemCol.m_image = -1;
    m_listCtrl->InsertColumn(0, itemCol);
    itemCol.m_text = _T("Column 2");
    m_listCtrl->InsertColumn(1, itemCol);
    itemCol.m_text = _T("Column 3");
    m_listCtrl->InsertColumn(2, itemCol);
#else
    m_listCtrl->InsertColumn(0, _T("Column 1")); // , wxLIST_FORMAT_LEFT, 140);
    m_listCtrl->InsertColumn(1, _T("Column 2")); // , wxLIST_FORMAT_LEFT, 140);
    m_listCtrl->InsertColumn(2, _T("One More Column (2)")); // , wxLIST_FORMAT_LEFT, 140);
#endif

    // to speed up inserting we hide the control temporarily
    m_listCtrl->Hide();

    wxStopWatch sw;

    for ( int i = 0; i < NUM_ITEMS; i++ )
    {
        m_listCtrl->InsertItemInReportView(i);
    }

    m_logWindow->WriteText(wxString::Format(_T("%d items inserted in %ldms\n"),
                                            NUM_ITEMS, sw.Time()));
    m_listCtrl->Show();

    // we leave all mask fields to 0 and only change the colour
    wxListItem item;
    item.m_itemId = 0;
    item.SetTextColour(*wxRED);
    m_listCtrl->SetItem( item );

    item.m_itemId = 2;
    item.SetTextColour(*wxGREEN);
    m_listCtrl->SetItem( item );
    item.m_itemId = 4;
    item.SetTextColour(*wxLIGHT_GREY);
    item.SetFont(*wxITALIC_FONT);
    item.SetBackgroundColour(*wxRED);
    m_listCtrl->SetItem( item );

    m_listCtrl->SetTextColour(*wxBLUE);
    m_listCtrl->SetBackgroundColour(*wxLIGHT_GREY);

    m_listCtrl->SetColumnWidth( 0, wxLIST_AUTOSIZE );
    m_listCtrl->SetColumnWidth( 1, wxLIST_AUTOSIZE );
    m_listCtrl->SetColumnWidth( 2, wxLIST_AUTOSIZE );
}

void MyFrame::InitWithIconItems(bool withText, bool sameIcon)
{
    m_listCtrl->SetImageList(m_imageListNormal, wxIMAGE_LIST_NORMAL);
    m_listCtrl->SetImageList(m_imageListSmall, wxIMAGE_LIST_SMALL);

    for ( int i = 0; i < NUM_ICONS; i++ )
    {
        int image = sameIcon ? 0 : i;

        if ( withText )
        {
            m_listCtrl->InsertItem(i, wxString::Format(_T("Label %d"), i),
                                   image);
        }
        else
        {
            m_listCtrl->InsertItem(i, image);
        }
    }
}

void MyFrame::OnIconView(wxCommandEvent& WXUNUSED(event))
{
    RecreateList(wxLC_ICON, FALSE);
}

void MyFrame::OnIconTextView(wxCommandEvent& WXUNUSED(event))
{
    RecreateList(wxLC_ICON);
}

void MyFrame::OnSmallIconView(wxCommandEvent& WXUNUSED(event))
{
    RecreateList(wxLC_SMALL_ICON, FALSE);
}

void MyFrame::OnSmallIconTextView(wxCommandEvent& WXUNUSED(event))
{
    RecreateList(wxLC_SMALL_ICON);
}

void MyFrame::OnVirtualView(wxCommandEvent& WXUNUSED(event))
{
    RecreateList(wxLC_REPORT | wxLC_VIRTUAL);
}

void MyFrame::InitWithVirtualItems()
{
    m_listCtrl->SetImageList(m_imageListSmall, wxIMAGE_LIST_SMALL);

    m_listCtrl->InsertColumn(0, _T("First Column"));
    m_listCtrl->InsertColumn(1, _T("Second Column"));
    m_listCtrl->SetColumnWidth(0, 150);
    m_listCtrl->SetColumnWidth(1, 150);

    m_listCtrl->SetItemCount(1000000);
}

void MyFrame::OnSort(wxCommandEvent& WXUNUSED(event))
{
    wxStopWatch sw;

    m_listCtrl->SortItems(MyCompareFunction, 0);

    m_logWindow->WriteText(wxString::Format(_T("Sorting %d items took %ld ms\n"),
                                            m_listCtrl->GetItemCount(),
                                            sw.Time()));
}

void MyFrame::OnShowSelInfo(wxCommandEvent& event)
{
    int selCount = m_listCtrl->GetSelectedItemCount();
    wxLogMessage(_T("%d items selected:"), selCount);

    // don't show too many items
    size_t shownCount = 0;

    long item = m_listCtrl->GetNextItem(-1, wxLIST_NEXT_ALL,
                                        wxLIST_STATE_SELECTED);
    while ( item != -1 )
    {
        wxLogMessage(_T("\t%ld (%s)"),
                     item, m_listCtrl->GetItemText(item).c_str());

        if ( ++shownCount > 10 )
        {
            wxLogMessage(_T("\t... more selected items snipped..."));
            break;
        }

        item = m_listCtrl->GetNextItem(item, wxLIST_NEXT_ALL,
                                       wxLIST_STATE_SELECTED);
    }
}

void MyFrame::OnShowColInfo(wxCommandEvent& event)
{
    int count = m_listCtrl->GetColumnCount();
    wxLogMessage(wxT("%d columns:"), count);
    for ( int c = 0; c < count; c++ )
    {
        wxLogMessage(wxT("\tcolumn %d has width %d"), c,
                     m_listCtrl->GetColumnWidth(c));
    }
}

void MyFrame::OnUpdateShowColInfo(wxUpdateUIEvent& event)
{
    event.Enable( (m_listCtrl->GetWindowStyleFlag() & wxLC_REPORT) != 0 );
}

void MyFrame::OnToggleMultiSel(wxCommandEvent& WXUNUSED(event))
{
    long flags = m_listCtrl->GetWindowStyleFlag();
    if ( flags & wxLC_SINGLE_SEL )
        flags &= ~wxLC_SINGLE_SEL;
    else
        flags |= wxLC_SINGLE_SEL;

    m_logWindow->WriteText(wxString::Format(wxT("Current selection mode: %sle\n"),
                           (flags & wxLC_SINGLE_SEL) ? _T("sing") : _T("multip")));

    RecreateList(flags);
}

void MyFrame::OnSetFgColour(wxCommandEvent& WXUNUSED(event))
{
    m_listCtrl->SetForegroundColour(wxGetColourFromUser(this));
    m_listCtrl->Refresh();
}

void MyFrame::OnSetBgColour(wxCommandEvent& WXUNUSED(event))
{
    m_listCtrl->SetBackgroundColour(wxGetColourFromUser(this));
    m_listCtrl->Refresh();
}

void MyFrame::OnAdd(wxCommandEvent& WXUNUSED(event))
{
    m_listCtrl->InsertItem(m_listCtrl->GetItemCount(), _T("Appended item"));
}

void MyFrame::OnEdit(wxCommandEvent& WXUNUSED(event))
{
    long itemCur = m_listCtrl->GetNextItem(-1, wxLIST_NEXT_ALL,
                                           wxLIST_STATE_FOCUSED);

    if ( itemCur != -1 )
    {
        m_listCtrl->EditLabel(itemCur);
    }
    else
    {
        m_logWindow->WriteText(_T("No item to edit"));
    }
}

void MyFrame::OnDelete(wxCommandEvent& WXUNUSED(event))
{
    if ( m_listCtrl->GetItemCount() )
    {
        m_listCtrl->DeleteItem(0);
    }
    else
    {
        m_logWindow->WriteText(_T("Nothing to delete"));
    }
}

void MyFrame::OnDeleteAll(wxCommandEvent& WXUNUSED(event))
{
    wxStopWatch sw;

    m_listCtrl->DeleteAllItems();

    m_logWindow->WriteText(wxString::Format(_T("Deleting %d items took %ld ms\n"),
                                            m_listCtrl->GetItemCount(),
                                            sw.Time()));
}

// MyListCtrl

void MyListCtrl::OnCacheHint(wxListEvent& event)
{
    wxLogMessage( wxT("OnCacheHint: cache items %ld..%ld"),
                  event.GetCacheFrom(), event.GetCacheTo() );
}

void MyListCtrl::SetColumnImage(int col, int image)
{
    wxListItem item;
    item.SetMask(wxLIST_MASK_IMAGE);
    item.SetImage(image);
    SetColumn(col, item);
}

void MyListCtrl::OnColClick(wxListEvent& event)
{
    int col = event.GetColumn();
    SetColumnImage(col, 0);

    wxLogMessage( wxT("OnColumnClick at %d."), col );
}

void MyListCtrl::OnColRightClick(wxListEvent& event)
{
    int col = event.GetColumn();
    if ( col != -1 )
    {
        SetColumnImage(col, -1);
    }

    wxLogMessage( wxT("OnColumnRightClick at %d."), event.GetColumn() );
}

void MyListCtrl::LogColEvent(const wxListEvent& event, const wxChar *name)
{
    const int col = event.GetColumn();

    wxLogMessage(wxT("%s: column %d (width = %d or %d)."),
                 name,
                 col,
                 event.GetItem().GetWidth(),
                 GetColumnWidth(col));
}

void MyListCtrl::OnColBeginDrag(wxListEvent& event)
{
    LogColEvent( event, wxT("OnColBeginDrag") );
}

void MyListCtrl::OnColDragging(wxListEvent& event)
{
    LogColEvent( event, wxT("OnColDragging") );
}

void MyListCtrl::OnColEndDrag(wxListEvent& event)
{
    LogColEvent( event, wxT("OnColEndDrag") );
}

void MyListCtrl::OnBeginDrag(wxListEvent& event)
{
    const wxPoint& pt = event.m_pointDrag;

    int flags;
    wxLogMessage( wxT("OnBeginDrag at (%d, %d), item %ld."),
                  pt.x, pt.y, HitTest(pt, flags) );
}

void MyListCtrl::OnBeginRDrag(wxListEvent& event)
{
    wxLogMessage( wxT("OnBeginRDrag at %d,%d."),
                  event.m_pointDrag.x, event.m_pointDrag.y );
}

void MyListCtrl::OnBeginLabelEdit(wxListEvent& event)
{
    wxLogMessage( wxT("OnBeginLabelEdit: %s"), event.m_item.m_text.c_str());
}

void MyListCtrl::OnEndLabelEdit(wxListEvent& event)
{
    wxLogMessage( wxT("OnEndLabelEdit: %s"), event.m_item.m_text.c_str());
}

void MyListCtrl::OnDeleteItem(wxListEvent& event)
{
    LogEvent(event, _T("OnDeleteItem"));
}

void MyListCtrl::OnDeleteAllItems(wxListEvent& event)
{
    LogEvent(event, _T("OnDeleteAllItems"));
}

void MyListCtrl::OnGetInfo(wxListEvent& event)
{
    wxString msg;

    msg << _T("OnGetInfo (") << event.m_item.m_itemId << _T(", ") << event.m_item.m_col << _T(")");
    if ( event.m_item.m_mask & wxLIST_MASK_STATE )
        msg << _T(" wxLIST_MASK_STATE");
    if ( event.m_item.m_mask & wxLIST_MASK_TEXT )
        msg << _T(" wxLIST_MASK_TEXT");
    if ( event.m_item.m_mask & wxLIST_MASK_IMAGE )
        msg << _T(" wxLIST_MASK_IMAGE");
    if ( event.m_item.m_mask & wxLIST_MASK_DATA )
        msg << _T(" wxLIST_MASK_DATA");
    if ( event.m_item.m_mask & wxLIST_SET_ITEM )
        msg << _T(" wxLIST_SET_ITEM");
    if ( event.m_item.m_mask & wxLIST_MASK_WIDTH )
        msg << _T(" wxLIST_MASK_WIDTH");
    if ( event.m_item.m_mask & wxLIST_MASK_FORMAT )
        msg << _T(" wxLIST_MASK_WIDTH");

    if ( event.m_item.m_mask & wxLIST_MASK_TEXT )
    {
        event.m_item.m_text = _T("My callback text");
    }

    wxLogMessage(msg);
}

void MyListCtrl::OnSetInfo(wxListEvent& event)
{
    LogEvent(event, _T("OnSetInfo"));
}

void MyListCtrl::OnSelected(wxListEvent& event)
{
    LogEvent(event, _T("OnSelected"));

    if ( GetWindowStyle() & wxLC_REPORT )
    {
        wxListItem info;
        info.m_itemId = event.m_itemIndex;
        info.m_col = 1;
        info.m_mask = wxLIST_MASK_TEXT;
        if ( GetItem(info) )
        {
            wxLogMessage(wxT("Value of the 2nd field of the selected item: %s"),
                         info.m_text.c_str());
        }
        else
        {
            wxFAIL_MSG(wxT("wxListCtrl::GetItem() failed"));
        }
    }
}

void MyListCtrl::OnDeselected(wxListEvent& event)
{
    LogEvent(event, _T("OnDeselected"));
}

void MyListCtrl::OnActivated(wxListEvent& event)
{
    LogEvent(event, _T("OnActivated"));
}

void MyListCtrl::OnFocused(wxListEvent& event)
{
    LogEvent(event, _T("OnFocused"));

    event.Skip();
}

void MyListCtrl::OnListKeyDown(wxListEvent& event)
{
    switch ( event.GetKeyCode() )
    {
        case 'c': // colorize
        case 'C':
            {
                wxListItem info;
                info.m_itemId = event.GetIndex();
                GetItem(info);

                wxListItemAttr *attr = info.GetAttributes();
                if ( !attr || !attr->HasTextColour() )
                {
                    info.SetTextColour(*wxCYAN);

                    SetItem(info);

                    RefreshItem(info.m_itemId);
                }
            }
            break;

        case 'n': // next
        case 'N':
            {
                long item = GetNextItem(-1,
                                        wxLIST_NEXT_ALL, wxLIST_STATE_FOCUSED);
                if ( item++ == GetItemCount() - 1 )
                {
                    item = 0;
                }

                wxLogMessage(_T("Focusing item %ld"), item);

                SetItemState(item, wxLIST_STATE_FOCUSED, wxLIST_STATE_FOCUSED);
                EnsureVisible(item);
            }
            break;

        case WXK_DELETE:
            {
                long item = GetNextItem(-1,
                                        wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
                while ( item != -1 )
                {
                    DeleteItem(item);

                    wxLogMessage(_T("Item %ld deleted"), item);

                    // -1 because the indices were shifted by DeleteItem()
                    item = GetNextItem(item - 1,
                                       wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
                }
            }
            break;

        case WXK_INSERT:
            if ( GetWindowStyle() & wxLC_REPORT )
            {
                if ( GetWindowStyle() & wxLC_VIRTUAL )
                {
                    SetItemCount(GetItemCount() + 1);
                }
                else // !virtual
                {
                    InsertItemInReportView(event.GetIndex());
                }
            }
            //else: fall through

        default:
            LogEvent(event, _T("OnListKeyDown"));

            event.Skip();
    }
}

void MyListCtrl::OnChar(wxKeyEvent& event)
{
    wxLogMessage(_T("Got char event."));

    switch ( event.GetKeyCode() )
    {
        case 'n':
        case 'N':
        case 'c':
        case 'C':
            // these are the keys we process ourselves
            break;

        default:
            event.Skip();
    }
}

void MyListCtrl::LogEvent(const wxListEvent& event, const wxChar *eventName)
{
    wxLogMessage(_T("Item %ld: %s (item text = %s, data = %ld)"),
                 event.GetIndex(), eventName,
                 event.GetText().c_str(), event.GetData());
}

wxString MyListCtrl::OnGetItemText(long item, long column) const
{
    return wxString::Format(_T("Column %ld of item %ld"), column, item);
}

int MyListCtrl::OnGetItemImage(long item) const
{
    return 0;
}

wxListItemAttr *MyListCtrl::OnGetItemAttr(long item) const
{
    return item % 2 ? NULL : (wxListItemAttr *)&m_attr;
}

void MyListCtrl::InsertItemInReportView(int i)
{
    wxString buf;
    buf.Printf(_T("This is item %d"), i);
    long tmp = InsertItem(i, buf, 0);
    SetItemData(tmp, i);

    buf.Printf(_T("Col 1, item %d"), i);
    SetItem(i, 1, buf);

    buf.Printf(_T("Item %d in column 2"), i);
    SetItem(i, 2, buf);
}

