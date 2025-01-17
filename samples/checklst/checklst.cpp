///////////////////////////////////////////////////////////////////////////////
// Name:        checklst.cpp
// Purpose:     wxCheckListBox sample
// Author:      Vadim Zeitlin
// Modified by:
// Created:     13.11.97
// RCS-ID:      $Id: checklst.cpp,v 1.19.2.2 2002/12/13 21:38:46 MBN Exp $
// Copyright:   (c) 1998 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
    //#pragma implementation
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#ifdef __WXMSW__
    #include  "wx/ownerdrw.h"
#endif

#include  "wx/log.h"

#include  "wx/sizer.h"
#include  "wx/menuitem.h"
#include  "wx/checklst.h"

// Define a new application type
class CheckListBoxApp: public wxApp
{
public:
    bool OnInit();
};

// Define a new frame type
class CheckListBoxFrame : public wxFrame
{
public:
    // ctor & dtor
    CheckListBoxFrame(wxFrame *frame, const wxChar *title,
                      int x, int y, int w, int h);
    virtual ~CheckListBoxFrame();

    // notifications
    void OnQuit           (wxCommandEvent& event);
    void OnAbout          (wxCommandEvent& event);
    void OnToggleSelection(wxCommandEvent& event);
    void OnListboxSelect  (wxCommandEvent& event);
    void OnCheckboxToggle (wxCommandEvent& event);
    void OnListboxDblClick(wxCommandEvent& event);
    void OnButtonUp       (wxCommandEvent& event);
    void OnButtonDown     (wxCommandEvent& event);

private:
    void CreateCheckListbox(long flags = 0);

    void OnButtonMove(bool up);

    void AdjustColour(size_t index);

    wxPanel *m_panel;

    wxCheckListBox *m_pListBox;

    DECLARE_EVENT_TABLE()
};

enum
{
    Menu_About = 100,
    Menu_Quit,
    Menu_Selection,

    Control_First = 1000,
    Control_Listbox,
    Btn_Up,
    Btn_Down
};

BEGIN_EVENT_TABLE(CheckListBoxFrame, wxFrame)
    EVT_MENU(Menu_About, CheckListBoxFrame::OnAbout)
    EVT_MENU(Menu_Quit, CheckListBoxFrame::OnQuit)

    EVT_MENU(Menu_Selection, CheckListBoxFrame::OnToggleSelection)

    EVT_LISTBOX(Control_Listbox, CheckListBoxFrame::OnListboxSelect)
    EVT_CHECKLISTBOX(Control_Listbox, CheckListBoxFrame::OnCheckboxToggle)
    EVT_LISTBOX_DCLICK(Control_Listbox, CheckListBoxFrame::OnListboxDblClick)

    EVT_BUTTON(Btn_Up, CheckListBoxFrame::OnButtonUp)
    EVT_BUTTON(Btn_Down, CheckListBoxFrame::OnButtonDown)
END_EVENT_TABLE()

IMPLEMENT_APP(CheckListBoxApp);

// init our app: create windows
bool CheckListBoxApp::OnInit(void)
{
    CheckListBoxFrame *pFrame = new CheckListBoxFrame
                                    (
                                     NULL,
                                     _T("wxWindows Checklistbox Sample"),
                                     50, 50, 480, 320
                                    );
    SetTopWindow(pFrame);

    return TRUE;
}

// main frame constructor
CheckListBoxFrame::CheckListBoxFrame(wxFrame *frame,
                                     const wxChar *title,
                                     int x, int y, int w, int h)
                 : wxFrame(frame, -1, title, wxPoint(x, y), wxSize(w, h))
{
    // create the status line
    const int widths[] = { -1, 60 };
    CreateStatusBar(2);
    SetStatusWidths(2, widths);
    wxLogStatus(this, _T("no selection"));

    // Make a menubar
    // --------------

    // file submenu
    wxMenu *menuFile = new wxMenu;
    menuFile->Append(Menu_About, _T("&About...\tF1"));
    menuFile->AppendSeparator();
    menuFile->Append(Menu_Quit, _T("E&xit\tAlt-X"));

    // listbox submenu
    wxMenu *menuList = new wxMenu;
    menuList->AppendCheckItem(Menu_Selection, _T("Multiple selection\tCtrl-M"));

    // put it all together
    wxMenuBar *menu_bar = new wxMenuBar;
    menu_bar->Append(menuFile, _T("&File"));
    menu_bar->Append(menuList, _T("&List"));
    SetMenuBar(menu_bar);

    // make a panel with some controls
    m_panel = new wxPanel(this, -1, wxPoint(0, 0),
                          wxSize(400, 200), wxTAB_TRAVERSAL);

    CreateCheckListbox();

    // create buttons for moving the items around
    wxButton *button1 = new wxButton(m_panel, Btn_Up, _T("   &Up  "), wxPoint(420, 90));
    wxButton *button2 = new wxButton(m_panel, Btn_Down, _T("&Down"), wxPoint(420, 120));


    wxBoxSizer *mainsizer = new wxBoxSizer( wxVERTICAL );

    mainsizer->Add( m_pListBox, 1, wxGROW|wxALL, 10 );

    wxBoxSizer *bottomsizer = new wxBoxSizer( wxHORIZONTAL );

    bottomsizer->Add( button1, 0, wxALL, 10 );
    bottomsizer->Add( button2, 0, wxALL, 10 );

    mainsizer->Add( bottomsizer, 0, wxCENTER );

    // tell frame to make use of sizer (or constraints, if any)
    m_panel->SetAutoLayout( TRUE );
    m_panel->SetSizer( mainsizer );

    // don't allow frame to get smaller than what the sizers tell ye
    mainsizer->SetSizeHints( this );

    Show(TRUE);
}

void CheckListBoxFrame::CreateCheckListbox(long flags)
{
    // check list box
    static const wxChar *aszChoices[] =
    {
        _T("Zeroth"),
        _T("First"), _T("Second"), _T("Third"),
        _T("Fourth"), _T("Fifth"), _T("Sixth"),
        _T("Seventh"), _T("Eighth"), _T("Nineth")
    };

    wxString *astrChoices = new wxString[WXSIZEOF(aszChoices)];
    unsigned int ui;
    for ( ui = 0; ui < WXSIZEOF(aszChoices); ui++ )
        astrChoices[ui] = aszChoices[ui];

    m_pListBox = new wxCheckListBox
        (
         m_panel,               // parent
         Control_Listbox,       // control id
         wxPoint(10, 10),       // listbox poistion
         wxSize(400, 100),      // listbox size
         WXSIZEOF(aszChoices),  // number of strings
         astrChoices,           // array of strings
         flags
        );

    //m_pListBox->SetBackgroundColour(*wxGREEN);

    delete [] astrChoices;

    // set grey background for every second entry
    for ( ui = 0; ui < WXSIZEOF(aszChoices); ui += 2 ) {
        AdjustColour(ui);
    }

    m_pListBox->Check(2);
    m_pListBox->Select(3);
}

CheckListBoxFrame::~CheckListBoxFrame()
{
}

void CheckListBoxFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    Close(TRUE);
}

void CheckListBoxFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxMessageBox(wxT("Demo of wxCheckListBox control\n� Vadim Zeitlin 1998-2002"),
                 wxT("About wxCheckListBox"),
                 wxICON_INFORMATION, this);
}

void CheckListBoxFrame::OnToggleSelection(wxCommandEvent& event)
{
    wxSizer *sizer = m_panel->GetSizer();

    sizer->Remove(m_pListBox);
    delete m_pListBox;

    CreateCheckListbox(event.IsChecked() ? wxLB_EXTENDED : 0);

    sizer->Insert(0, m_pListBox, 1, wxGROW | wxALL, 10);

    m_panel->Layout();
}

void CheckListBoxFrame::OnListboxSelect(wxCommandEvent& event)
{
    int nSel = event.GetSelection();
    wxLogStatus(this, wxT("Item %d selected (%schecked)"), nSel,
                      m_pListBox->IsChecked(nSel) ? _T("") : wxT("not "));
}

void CheckListBoxFrame::OnListboxDblClick(wxCommandEvent& WXUNUSED(event))
{
    wxString strSelection;
    strSelection.sprintf(wxT("Item %d double clicked"), m_pListBox->GetSelection());
    wxMessageDialog dialog(this, strSelection, wxT("wxCheckListBox message"), wxICON_INFORMATION);
    dialog.ShowModal();
}

void CheckListBoxFrame::OnCheckboxToggle(wxCommandEvent& event)
{
    unsigned int nItem = event.GetInt();

    wxLogStatus(this, wxT("item %d was %schecked"), nItem,
                      m_pListBox->IsChecked(nItem) ? wxT("") : wxT("un"));
}

void CheckListBoxFrame::OnButtonUp(wxCommandEvent& WXUNUSED(event))
{
    OnButtonMove(TRUE);
}

void CheckListBoxFrame::OnButtonDown(wxCommandEvent& WXUNUSED(event))
{
    OnButtonMove(FALSE);
}

void CheckListBoxFrame::OnButtonMove(bool up)
{
    int selection = m_pListBox->GetSelection();
    if ( selection != -1 )
    {
        wxString label = m_pListBox->GetString(selection);

        int positionNew = up ? selection - 1 : selection + 2;
        if ( positionNew < 0 || positionNew > m_pListBox->GetCount() )
        {
            wxLogStatus(this, wxT("Can't move this item %s"), up ? wxT("up") : wxT("down"));
        }
        else
        {
            bool wasChecked = m_pListBox->IsChecked(selection);

            int positionOld = up ? selection + 1 : selection;

            // insert the item
            m_pListBox->InsertItems(1, &label, positionNew);

            // and delete the old one
            m_pListBox->Delete(positionOld);

            int selectionNew = up ? positionNew : positionNew - 1;
            m_pListBox->Check(selectionNew, wasChecked);
            m_pListBox->SetSelection(selectionNew);

            AdjustColour(selection);
            AdjustColour(selectionNew);

            wxLogStatus(this, wxT("Item moved %s"), up ? wxT("up") : wxT("down"));
        }
    }
    else
    {
        wxLogStatus(this, wxT("Please select an item"));
    }
}

void CheckListBoxFrame::AdjustColour(size_t index)
{
    // not implemented in ports other than (native) MSW yet
#if defined(__WXMSW__) && !defined(__WXUNIVERSAL__)
    // even items have grey backround, odd ones - white
    unsigned char c = index % 2 ? 255 : 200;
    m_pListBox->GetItem(index)->SetBackgroundColour(wxColor(c, c, c));
#endif // wxMSW
}
