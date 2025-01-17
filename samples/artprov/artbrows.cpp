/////////////////////////////////////////////////////////////////////////////
// Name:        artbrows.cpp
// Purpose:     wxArtProvider demo - art browser dialog
// Author:      Vaclav Slavik
// Modified by:
// Created:     2002/04/05
// RCS-ID:      $Id: artbrows.cpp,v 1.3.2.1 2002/12/13 21:38:40 MBN Exp $
// Copyright:   (c) Vaclav Slavik
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "artbrows.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
    #include "wx/choice.h"
#endif

#include "wx/listctrl.h"
#include "wx/sizer.h"
#include "wx/imaglist.h"
#include "wx/listctrl.h"

#include "artbrows.h"

#define ART_CLIENT(id) \
    choice->Append(_T(#id), (void*)id);
#define ART_ICON(id) \
    { \
        int ind; \
        wxIcon icon = wxArtProvider::GetIcon(id, client, size); \
        if ( icon.Ok() ) \
            ind = images->Add(icon); \
        else \
            ind = 0; \
        list->InsertItem(index, _T(#id), ind); \
        list->SetItemData(index, (long)id); \
        index++; \
    }

// ----------------------------------------------------------------------------
// Functions to fill-in all supported art IDs
// ----------------------------------------------------------------------------

static void FillClients(wxChoice *choice)
{
    ART_CLIENT(wxART_OTHER)
    ART_CLIENT(wxART_TOOLBAR)
    ART_CLIENT(wxART_MENU)
    ART_CLIENT(wxART_FRAME_ICON)
    ART_CLIENT(wxART_CMN_DIALOG)
    ART_CLIENT(wxART_HELP_BROWSER)
    ART_CLIENT(wxART_MESSAGE_BOX)
}

static void FillBitmaps(wxImageList *images, wxListCtrl *list,
                        int& index,
                        const wxArtClient& client, const wxSize& size)
{
    ART_ICON(wxART_ERROR)
    ART_ICON(wxART_QUESTION)
    ART_ICON(wxART_WARNING)
    ART_ICON(wxART_INFORMATION)
    ART_ICON(wxART_ADD_BOOKMARK)
    ART_ICON(wxART_DEL_BOOKMARK)
    ART_ICON(wxART_HELP_SIDE_PANEL)
    ART_ICON(wxART_HELP_SETTINGS)
    ART_ICON(wxART_HELP_BOOK)
    ART_ICON(wxART_HELP_FOLDER)
    ART_ICON(wxART_HELP_PAGE)
    ART_ICON(wxART_GO_BACK)
    ART_ICON(wxART_GO_FORWARD)
    ART_ICON(wxART_GO_UP)
    ART_ICON(wxART_GO_DOWN)
    ART_ICON(wxART_GO_TO_PARENT)
    ART_ICON(wxART_GO_HOME)
    ART_ICON(wxART_FILE_OPEN)
    ART_ICON(wxART_PRINT)
    ART_ICON(wxART_HELP)
    ART_ICON(wxART_TIP)
    ART_ICON(wxART_REPORT_VIEW)
    ART_ICON(wxART_LIST_VIEW)
    ART_ICON(wxART_NEW_DIR)
    ART_ICON(wxART_FOLDER)
    ART_ICON(wxART_GO_DIR_UP)
    ART_ICON(wxART_EXECUTABLE_FILE)
    ART_ICON(wxART_NORMAL_FILE)
    ART_ICON(wxART_TICK_MARK)
    ART_ICON(wxART_CROSS_MARK)
}


// ----------------------------------------------------------------------------
// Browser implementation
// ----------------------------------------------------------------------------

#include "null.xpm"

BEGIN_EVENT_TABLE(wxArtBrowserDialog, wxDialog)
    EVT_LIST_ITEM_SELECTED(-1, wxArtBrowserDialog::OnSelectItem)
    EVT_CHOICE(-1, wxArtBrowserDialog::OnChooseClient)
END_EVENT_TABLE()

wxArtBrowserDialog::wxArtBrowserDialog(wxWindow *parent)
    : wxDialog(parent, -1, _T("Art resources browser"), 
               wxDefaultPosition, wxDefaultSize, 
               wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER)
{
    wxSizer *sizer = new wxBoxSizer(wxVERTICAL);
    wxSizer *subsizer;
    
    wxChoice *choice = new wxChoice(this, -1);
    FillClients(choice);
    
    subsizer = new wxBoxSizer(wxHORIZONTAL);
    subsizer->Add(new wxStaticText(this, -1, _T("Client:")), 0, wxALIGN_CENTER_VERTICAL);
    subsizer->Add(choice, 1, wxLEFT, 5);
    sizer->Add(subsizer, 0, wxALL | wxEXPAND, 10);

    subsizer = new wxBoxSizer(wxHORIZONTAL);
    
    m_list = new wxListCtrl(this, -1, wxDefaultPosition, wxSize(250, 300),
                            wxLC_REPORT | wxSUNKEN_BORDER);
    m_list->InsertColumn(0, _T("wxArtID"));
    subsizer->Add(m_list, 1, wxEXPAND | wxRIGHT, 10);
    
    wxSizer *subsub = new wxBoxSizer(wxVERTICAL);
    m_canvas = new wxStaticBitmap(this, -1, wxBitmap(null_xpm));
    subsub->Add(m_canvas);
    subsub->Add(100, 100);
    subsizer->Add(subsub);

    sizer->Add(subsizer, 1, wxEXPAND | wxLEFT|wxRIGHT, 10);
    
    wxButton *ok = new wxButton(this, wxID_OK, _T("Close"));
    ok->SetDefault();
    sizer->Add(ok, 0, wxALIGN_RIGHT | wxALL, 10);
    
    SetSizer(sizer);
    SetAutoLayout(TRUE);
    sizer->Fit(this);

    choice->SetSelection(6/*wxART_MESSAGE_BOX*/);
    SetArtClient(wxART_MESSAGE_BOX);
}


void wxArtBrowserDialog::SetArtClient(const wxArtClient& client)
{
    wxBusyCursor bcur;
    
    wxImageList *img = new wxImageList(16, 16);
    img->Add(wxIcon(null_xpm));
    int index = 0;

    m_list->DeleteAllItems();
    FillBitmaps(img, m_list, index, client, wxSize(16, 16));
    m_list->AssignImageList(img, wxIMAGE_LIST_SMALL);
    m_list->SetColumnWidth(0, wxLIST_AUTOSIZE);

    m_client = client;
}

void wxArtBrowserDialog::OnSelectItem(wxListEvent &event)
{
    const wxChar *data = (const wxChar*)event.GetData();
    wxBitmap bmp = wxArtProvider::GetBitmap(data, m_client);
    m_canvas->SetBitmap(bmp);
    m_canvas->SetSize(bmp.GetWidth(), bmp.GetHeight());
}

void wxArtBrowserDialog::OnChooseClient(wxCommandEvent &event)
{
    const wxChar *data = (const wxChar*)event.GetClientData();
    SetArtClient(data);
}
