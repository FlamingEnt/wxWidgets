/////////////////////////////////////////////////////////////////////////////
// Name:        statbar.cpp
// Purpose:     wxStatusBar sample
// Author:      Vadim Zeitlin
// Modified by:
// Created:     04.02.00
// RCS-ID:      $Id: statbar.cpp,v 1.16.2.1 2002/12/14 18:19:54 MBN Exp $
// Copyright:   (c) Vadim Zeitlin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if !wxUSE_STATUSBAR
    #error "You need to set wxUSE_STATUSBAR to 1 to compile this sample"
#endif // wxUSE_STATUSBAR

// for all others, include the necessary headers
#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/log.h"
    #include "wx/frame.h"
    #include "wx/statusbr.h"
    #include "wx/timer.h"
    #include "wx/checkbox.h"
    #include "wx/statbmp.h"
    #include "wx/menu.h"
    #include "wx/msgdlg.h"
    #include "wx/textdlg.h"
    #include "wx/sizer.h"
    #include "wx/stattext.h"
    #include "wx/bmpbuttn.h"
    #include "wx/dcmemory.h"
#endif

#include "wx/datetime.h"

// define this for the platforms which don't support wxBitmapButton (such as
// Motif), else a wxBitmapButton will be used
#ifdef __WXMOTIF__
//#define USE_MDI_PARENT_FRAME 1

#ifdef USE_MDI_PARENT_FRAME
    #include "wx/mdi.h"
#endif // USE_MDI_PARENT_FRAME
    #define USE_STATIC_BITMAP
#endif

// ----------------------------------------------------------------------------
// resources
// ----------------------------------------------------------------------------

#ifdef USE_STATIC_BITMAP
    #include "green.xpm"
    #include "red.xpm"
#endif // USE_STATIC_BITMAP

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

// Define a new application type, each program should derive a class from wxApp
class MyApp : public wxApp
{
public:
    // override base class virtuals
    // ----------------------------

    // this one is called on application startup and is a good place for the app
    // initialization (doing it here and not in the ctor allows to have an error
    // return: if OnInit() returns false, the application terminates)
    virtual bool OnInit();
};

// A custom status bar which contains controls, icons &c
class MyStatusBar : public wxStatusBar
{
public:
    MyStatusBar(wxWindow *parent);
    virtual ~MyStatusBar();

    void UpdateClock();

    // event handlers
    void OnTimer(wxTimerEvent& event) { UpdateClock(); }
    void OnSize(wxSizeEvent& event);
    void OnToggleClock(wxCommandEvent& event);
    void OnButton(wxCommandEvent& event);

private:
    // toggle the state of the status bar controls
    void DoToggle();

    wxBitmap CreateBitmapForButton(bool on = FALSE);

    enum
    {
        Field_Text,
        Field_Checkbox,
        Field_Bitmap,
        Field_Clock,
        Field_Max
    };

    wxTimer m_timer;

    wxCheckBox *m_checkbox;
#ifdef USE_STATIC_BITMAP
    wxStaticBitmap *m_statbmp;
#else
    wxBitmapButton *m_statbmp;
#endif

    DECLARE_EVENT_TABLE()
};

// Define a new frame type: this is going to be our main frame
class MyFrame : public wxFrame
{
public:
    // ctor(s)
    MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
#ifdef USE_MDI_PARENT_FRAME
class MyFrame : public wxMDIParentFrame
#else
    virtual ~MyFrame();
#endif

    // event handlers (these functions should _not_ be virtual)
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);

    void OnSetStatusFields(wxCommandEvent& event);
    void OnRecreateStatusBar(wxCommandEvent& event);

private:
    enum StatBarKind
    {
        StatBar_Default,
        StatBar_Custom,
        StatBar_Max
    } m_statbarKind;
    void OnUpdateSetStatusFields(wxUpdateUIEvent& event);
    void OnUpdateStatusBarToggle(wxUpdateUIEvent& event);
    void OnStatusBarToggle(wxCommandEvent& event);
    void DoCreateStatusBar(StatBarKind kind);

    wxStatusBar *m_statbarDefault;
    MyStatusBar *m_statbarCustom;

    // any class wishing to process wxWindows events must use this macro
    DECLARE_EVENT_TABLE()
};

// Our about dialog ith its status bar
class MyAboutDialog : public wxDialog
{
public:
    MyAboutDialog(wxWindow *parent);
};

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// IDs for the controls and the menu commands
enum
{
    // menu items
    StatusBar_Quit = 1,
    StatusBar_SetFields,
    StatusBar_Recreate,
    StatusBar_About,
    StatusBar_Toggle,
    StatusBar_Checkbox = 1000
};

static const int BITMAP_SIZE_X = 32;
static const int BITMAP_SIZE_Y = 15;

// ----------------------------------------------------------------------------
// event tables and other macros for wxWindows
// ----------------------------------------------------------------------------

// the event tables connect the wxWindows events with the functions (event
// handlers) which process them. It can be also done at run-time, but for the
// simple menu events like this the static method is much simpler.
#ifdef USE_MDI_PARENT_FRAME
BEGIN_EVENT_TABLE(MyFrame, wxMDIParentFrame)
#else
BEGIN_EVENT_TABLE(MyFrame, wxFrame)
#endif
    EVT_MENU(StatusBar_Quit,  MyFrame::OnQuit)
    EVT_MENU(StatusBar_SetFields, MyFrame::OnSetStatusFields)
    EVT_MENU(StatusBar_Recreate, MyFrame::OnRecreateStatusBar)
    EVT_MENU(StatusBar_About, MyFrame::OnAbout)
    EVT_MENU(StatusBar_Toggle, MyFrame::OnStatusBarToggle)
    EVT_UPDATE_UI(StatusBar_Toggle, MyFrame::OnUpdateStatusBarToggle)
    EVT_UPDATE_UI(StatusBar_SetFields, MyFrame::OnUpdateSetStatusFields)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(MyStatusBar, wxStatusBar)
    EVT_SIZE(MyStatusBar::OnSize)
    EVT_CHECKBOX(StatusBar_Checkbox, MyStatusBar::OnToggleClock)
    EVT_BUTTON(-1, MyStatusBar::OnButton)
    EVT_TIMER(-1, MyStatusBar::OnTimer)
END_EVENT_TABLE()

// Create a new application object: this macro will allow wxWindows to create
// the application object during program execution (it's better than using a
// static object for many reasons) and also declares the accessor function
// wxGetApp() which will return the reference of the right type (i.e. MyApp and
// not wxApp)
IMPLEMENT_APP(MyApp)

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// the application class
// ----------------------------------------------------------------------------

// `Main program' equivalent: the program execution "starts" here
bool MyApp::OnInit()
{
    // create the main application window
    MyFrame *frame = new MyFrame(_T("wxStatusBar sample"),
                                 wxPoint(50, 50), wxSize(450, 340));

    // and show it (the frames, unlike simple controls, are not shown when
    // created initially)
    frame->Show(TRUE);

    // success: wxApp::OnRun() will be called which will enter the main message
    // loop and the application will run. If we returned FALSE here, the
    // application would exit immediately.
    return TRUE;
}

// ----------------------------------------------------------------------------
// main frame
// ----------------------------------------------------------------------------

// frame constructor
MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
#ifdef USE_MDI_PARENT_FRAME
       : wxMDIParentFrame((wxWindow *)NULL, -1, title, pos, size)
#else
       : wxFrame((wxWindow *)NULL, -1, title, pos, size)
#endif
{
    m_statbarDefault = NULL;
    m_statbarCustom = NULL;

#ifdef __WXMAC__
    // we need this in order to allow the about menu relocation, since ABOUT is
    // not the default id of the about menu
    wxApp::s_macAboutMenuItemId = StatusBar_About;
#endif

    // create a menu bar
    wxMenu *menuFile = new wxMenu;
    menuFile->Append(StatusBar_Quit, _T("E&xit\tAlt-X"), _T("Quit this program"));

    wxMenu *statbarMenu = new wxMenu;
    statbarMenu->Append(StatusBar_SetFields, _T("&Set field count\tCtrl-C"),
                        _T("Set the number of status bar fields"));
    statbarMenu->Append(StatusBar_Toggle, _T("&Toggle Status Bar"),
                        _T("Toggle the status bar display"), true);
    statbarMenu->Append(StatusBar_Recreate, _T("&Recreate\tCtrl-R"),
                        _T("Toggle status bar format"));

    wxMenu *helpMenu = new wxMenu;
    helpMenu->Append(StatusBar_About, _T("&About...\tCtrl-A"), _T("Show about dialog"));

    // now append the freshly created menu to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(menuFile, _T("&File"));
    menuBar->Append(statbarMenu, _T("&Status bar"));
    menuBar->Append(helpMenu, _T("&Help"));

    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);

    // create default status bar to start with
    CreateStatusBar(2);
    m_statbarKind = StatBar_Default;
    SetStatusText(_T("Welcome to wxWindows!"));

    m_statbarDefault = GetStatusBar();
}

MyFrame::~MyFrame()
{
    SetStatusBar(NULL);

    delete m_statbarDefault;
    delete m_statbarCustom;
}

void MyFrame::DoCreateStatusBar(MyFrame::StatBarKind kind)
{
    wxStatusBar *statbarOld = GetStatusBar();
    if ( statbarOld )
    {
        statbarOld->Hide();
    }

    switch ( kind )
    {
        case StatBar_Default:
            SetStatusBar(m_statbarDefault);
            break;

        case StatBar_Custom:
            if ( !m_statbarCustom )
            {
                m_statbarCustom = new MyStatusBar(this);
            }
            SetStatusBar(m_statbarCustom);
            break;

        default:
            wxFAIL_MSG(wxT("unknown stat bar kind"));
    }

    GetStatusBar()->Show();
    PositionStatusBar();

    m_statbarKind = kind;
}

void MyFrame::OnUpdateSetStatusFields(wxUpdateUIEvent& event)
{
    // only allow the setting of the number of status fields for the default
    // status bar
    wxStatusBar *sb = GetStatusBar();
    event.Enable(sb == m_statbarDefault);
}


// event handlers
void MyFrame::OnSetStatusFields(wxCommandEvent& WXUNUSED(event))
{
    wxStatusBar *sb = GetStatusBar();

    long nFields = wxGetNumberFromUser
                   (
                    _T("Select the number of fields in the status bar"),
                    _T("Fields:"),
                    _T("wxWindows statusbar sample"),
                    sb->GetFieldsCount(),
                    1, 5,
                    this
                   );

    // we don't check if the number changed at all on purpose: calling
    // SetFieldsCount() with the same number of fields should be ok
    if ( nFields != -1 )
    {
        static const int widthsFor2Fields[] = { 200, -1 };
        static const int widthsFor3Fields[] = { -1, -2, -1 };
        static const int widthsFor4Fields[] = { 100, -1, 100, -2, 100 };

        static const int *widthsAll[] =
        {
            NULL,               // 1 field: default
            widthsFor2Fields,   // 2 fields: 1 fixed, 1 var
            widthsFor3Fields,   // 3 fields: 3 var
            widthsFor4Fields,   // 4 fields: 3 fixed, 2 vars
            NULL                // 5 fields: default (all have same width)
        };

        const int * const widths = widthsAll[nFields - 1];
        sb->SetFieldsCount(nFields, widths);

        wxString s;
        for ( long n = 0; n < nFields; n++ )
        {
            if ( widths )
            {
                if ( widths[n] > 0 )
                    s.Printf(_T("fixed (%d)"), widths[n]);
                else
                    s.Printf(_T("variable (*%d)"), -widths[n]);
            }
            else
            {
                s = _T("default");
            }

            SetStatusText(s, n);
        }
    }
    else
    {
        wxLogStatus(this, wxT("Cancelled"));
    }
}

void MyFrame::OnUpdateStatusBarToggle(wxUpdateUIEvent& event)
{
    event.Check(GetStatusBar() != 0);
}

void MyFrame::OnStatusBarToggle(wxCommandEvent& WXUNUSED(event))
{
    wxStatusBar *statbarOld = GetStatusBar();
    if ( statbarOld )
    {
        statbarOld->Hide();
        SetStatusBar(0);
    }
    else
    {
        DoCreateStatusBar(m_statbarKind);
    }
#ifdef __WXMSW__
    // The following is a kludge suggested by Vadim Zeitlin (one of the wxWindows
    // authors) while we look for a proper fix..
//    SendSizeEvent();
#endif
}

void MyFrame::OnRecreateStatusBar(wxCommandEvent& WXUNUSED(event))
{
    DoCreateStatusBar(m_statbarKind == StatBar_Custom ? StatBar_Default
                                                      : StatBar_Custom);
}

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    // TRUE is to force the frame to close
    Close(TRUE);
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    MyAboutDialog dlg(this);
    dlg.ShowModal();
}

// ----------------------------------------------------------------------------
// MyAboutDialog
// ----------------------------------------------------------------------------

MyAboutDialog::MyAboutDialog(wxWindow *parent)
             : wxDialog(parent, -1, wxString(_T("About statbar")),
                        wxDefaultPosition, wxDefaultSize,
                        wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
{
    wxStaticText *text = new wxStaticText(this, -1,
                                          _T("wxStatusBar sample\n")
                                          _T("(c) 2000 Vadim Zeitlin"));

    wxButton *btn = new wxButton(this, wxID_OK, _T("&Close"));

    // create the top status bar without the size grip (default style),
    // otherwise it looks weird
    wxStatusBar *statbarTop = new wxStatusBar(this, -1, 0);
    statbarTop->SetFieldsCount(3);
    statbarTop->SetStatusText(_T("This is a top status bar"), 0);
    statbarTop->SetStatusText(_T("in a dialog"), 1);
    statbarTop->SetStatusText(_T("Great, isn't it?"), 2);

    wxStatusBar *statbarBottom = new wxStatusBar(this, -1);
    statbarBottom->SetFieldsCount(2);
    statbarBottom->SetStatusText(_T("This is a bottom status bar"), 0);
    statbarBottom->SetStatusText(_T("in a dialog"), 1);

    wxBoxSizer *sizerTop = new wxBoxSizer(wxVERTICAL);
    sizerTop->Add(statbarTop, 0, wxGROW);
    sizerTop->Add(-1, 10, 1, wxGROW);
    sizerTop->Add(text, 0, wxCENTRE | wxRIGHT | wxLEFT, 20);
    sizerTop->Add(-1, 10, 1, wxGROW);
    sizerTop->Add(btn, 0, wxCENTRE | wxRIGHT | wxLEFT, 20);
    sizerTop->Add(-1, 10, 1, wxGROW);
    sizerTop->Add(statbarBottom, 0, wxGROW);

    SetAutoLayout(TRUE);
    SetSizer(sizerTop);

    sizerTop->Fit(this);
    sizerTop->SetSizeHints(this);
}

// ----------------------------------------------------------------------------
// MyStatusBar
// ----------------------------------------------------------------------------

#ifdef __VISUALC__
    // 'this' : used in base member initializer list -- so what??
    #pragma warning(disable: 4355)
#endif

MyStatusBar::MyStatusBar(wxWindow *parent)
           : wxStatusBar(parent, -1), m_timer(this), m_checkbox(NULL)
{
    static const int widths[Field_Max] = { -1, 150, BITMAP_SIZE_X, 100 };

    SetFieldsCount(Field_Max);
    SetStatusWidths(Field_Max, widths);

    m_checkbox = new wxCheckBox(this, StatusBar_Checkbox, _T("&Toggle clock"));
    m_checkbox->SetValue(TRUE);

#ifdef USE_STATIC_BITMAP
    m_statbmp = new wxStaticBitmap(this, -1, wxIcon(green_xpm));
#else
    m_statbmp = new wxBitmapButton(this, -1, CreateBitmapForButton(),
                                   wxDefaultPosition, wxDefaultSize,
                                   wxBU_EXACTFIT);
#endif

    m_timer.Start(1000);

    SetMinHeight(BITMAP_SIZE_Y);

    UpdateClock();
}

#ifdef __VISUALC__
    #pragma warning(default: 4355)
#endif

MyStatusBar::~MyStatusBar()
{
    if ( m_timer.IsRunning() )
    {
        m_timer.Stop();
    }
}

wxBitmap MyStatusBar::CreateBitmapForButton(bool on)
{
    static const int BMP_BUTTON_SIZE_X = 10;
    static const int BMP_BUTTON_SIZE_Y = 9;

    wxBitmap bitmap(BMP_BUTTON_SIZE_X, BMP_BUTTON_SIZE_Y);
    wxMemoryDC dc;
    dc.SelectObject(bitmap);
    dc.SetBrush(on ? *wxGREEN_BRUSH : *wxRED_BRUSH);
    dc.SetBackground(*wxLIGHT_GREY_BRUSH);
    dc.Clear();
    dc.DrawEllipse(0, 0, BMP_BUTTON_SIZE_X, BMP_BUTTON_SIZE_Y);
    dc.SelectObject(wxNullBitmap);

    return bitmap;
}

void MyStatusBar::OnSize(wxSizeEvent& event)
{
    if ( !m_checkbox )
        return;

    wxRect rect;
    GetFieldRect(Field_Checkbox, rect);

    m_checkbox->SetSize(rect.x + 2, rect.y + 2, rect.width - 4, rect.height - 4);

    GetFieldRect(Field_Bitmap, rect);
    wxSize size = m_statbmp->GetSize();

    m_statbmp->Move(rect.x + (rect.width - size.x) / 2,
                    rect.y + (rect.height - size.y) / 2);

    event.Skip();
}

void MyStatusBar::OnButton(wxCommandEvent& WXUNUSED(event))
{
    m_checkbox->SetValue(!m_checkbox->GetValue());

    DoToggle();
}

void MyStatusBar::OnToggleClock(wxCommandEvent& WXUNUSED(event))
{
    DoToggle();
}

void MyStatusBar::DoToggle()
{
    if ( m_checkbox->GetValue() )
    {
        m_timer.Start(1000);

#ifdef USE_STATIC_BITMAP
        m_statbmp->SetIcon(wxIcon(green_xpm));
#else
        m_statbmp->SetBitmapLabel(CreateBitmapForButton(FALSE));
        m_statbmp->Refresh();
#endif

        UpdateClock();
    }
    else // don't show clock
    {
        m_timer.Stop();

#ifdef USE_STATIC_BITMAP
        m_statbmp->SetIcon(wxIcon(red_xpm));
#else
        m_statbmp->SetBitmapLabel(CreateBitmapForButton(TRUE));
        m_statbmp->Refresh();
#endif

        SetStatusText(_T(""), Field_Clock);
    }
}

void MyStatusBar::UpdateClock()
{
    SetStatusText(wxDateTime::Now().FormatTime(), Field_Clock);
}
