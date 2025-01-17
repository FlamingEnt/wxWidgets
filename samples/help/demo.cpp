/////////////////////////////////////////////////////////////////////////////
// Name:        demo.cpp
// Purpose:     wxHelpController demo
// Author:      Karsten Ballueder
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id: demo.cpp,v 1.33.2.1 2002/12/15 10:22:11 MBN Exp $
// Copyright:   (c) Karsten Ballueder, Julian Smart
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
#   pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWindows headers
#ifndef WX_PRECOMP
#   include "wx/wx.h"
#endif

#   include "wx/image.h"
#   include "wx/help.h"
#   include "wx/cshelp.h"

#if wxUSE_TOOLTIPS
#   include "wx/tooltip.h"
#endif

// define this to 1 to use HTML help even under Windows (by default, Windows
// version will use WinHelp).
// Please also see samples/html/helpview.

#define USE_HTML_HELP 1

// Define this to 0 to use the help controller as the help
// provider, or to 1 to use the 'simple help provider'
// (the one implemented with wxTipWindow).
#define USE_SIMPLE_HELP_PROVIDER 0

#if !wxUSE_HTML
#undef USE_HTML_HELP
#define USE_HTML_HELP 0
#endif

#if USE_HTML_HELP
#include "wx/filesys.h"
#include "wx/fs_zip.h"

#include "wx/html/helpctrl.h"
#endif

#if wxUSE_MS_HTML_HELP
#include "wx/msw/helpchm.h"
#endif

#if wxUSE_MS_HTML_HELP && wxUSE_WXHTML_HELP
#include "wx/msw/helpbest.h"
#endif

// ----------------------------------------------------------------------------
// ressources
// ----------------------------------------------------------------------------
// the application icon
#if defined(__WXGTK__) || defined(__WXX11__) || defined(__WXMOTIF__) || defined(__WXMAC__) || defined(__WXMGL__)
    #include "mondrian.xpm"
#endif

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

    // do some clean up here
    virtual int OnExit();
};

// Define a new frame type: this is going to be our main frame
class MyFrame : public wxFrame
{
public:
    // ctor(s)
    MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size);

    wxHelpController& GetHelpController() { return m_help; }

#if USE_HTML_HELP
    wxHtmlHelpController& GetAdvancedHtmlHelpController() { return m_advancedHtmlHelp; }
#endif
#if wxUSE_MS_HTML_HELP
    wxCHMHelpController& GetMSHtmlHelpController() { return m_msHtmlHelp; }
#endif
#if wxUSE_MS_HTML_HELP && wxUSE_WXHTML_HELP
    wxBestHelpController& GetBestHelpController() { return m_bestHelp; }
#endif

    // event handlers (these functions should _not_ be virtual)
    void OnQuit(wxCommandEvent& event);
    void OnHelp(wxCommandEvent& event);
    void OnAdvancedHtmlHelp(wxCommandEvent& event);
    void OnMSHtmlHelp(wxCommandEvent& event);
    void OnBestHelp(wxCommandEvent& event);

    void OnShowContextHelp(wxCommandEvent& event);
    void OnShowDialogContextHelp(wxCommandEvent& event);

    void ShowHelp(int commandId, wxHelpControllerBase& helpController);

private:
   wxHelpController         m_help;

#if USE_HTML_HELP
   wxHtmlHelpController     m_advancedHtmlHelp;
#endif

#if wxUSE_MS_HTML_HELP
    wxCHMHelpController     m_msHtmlHelp;
#endif

#if wxUSE_MS_HTML_HELP && wxUSE_WXHTML_HELP
    wxBestHelpController    m_bestHelp;
#endif

    // any class wishing to process wxWindows events must use this macro
   DECLARE_EVENT_TABLE()
};

// A custom modal dialog
class MyModalDialog : public wxDialog
{
public:
    MyModalDialog(wxWindow *parent);

private:

    DECLARE_EVENT_TABLE()
};

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// IDs for the controls and the menu commands
enum
{
    // menu items
    HelpDemo_Quit = 100,
    HelpDemo_Help_Index,
    HelpDemo_Help_Classes,
    HelpDemo_Help_Functions,
    HelpDemo_Help_Help,
    HelpDemo_Help_Search,
    HelpDemo_Help_ContextHelp,
    HelpDemo_Help_DialogContextHelp,

    HelpDemo_Html_Help_Index,
    HelpDemo_Html_Help_Classes,
    HelpDemo_Html_Help_Functions,
    HelpDemo_Html_Help_Help,
    HelpDemo_Html_Help_Search,

    HelpDemo_Advanced_Html_Help_Index,
    HelpDemo_Advanced_Html_Help_Classes,
    HelpDemo_Advanced_Html_Help_Functions,
    HelpDemo_Advanced_Html_Help_Help,
    HelpDemo_Advanced_Html_Help_Search,

    HelpDemo_MS_Html_Help_Index,
    HelpDemo_MS_Html_Help_Classes,
    HelpDemo_MS_Html_Help_Functions,
    HelpDemo_MS_Html_Help_Help,
    HelpDemo_MS_Html_Help_Search,

    HelpDemo_Best_Help_Index,
    HelpDemo_Best_Help_Classes,
    HelpDemo_Best_Help_Functions,
    HelpDemo_Best_Help_Help,
    HelpDemo_Best_Help_Search,

    HelpDemo_Help_KDE,
    HelpDemo_Help_GNOME,
    HelpDemo_Help_Netscape,
    // controls start here (the numbers are, of course, arbitrary)
    HelpDemo_Text = 1000,
};

// ----------------------------------------------------------------------------
// event tables and other macros for wxWindows
// ----------------------------------------------------------------------------

// the event tables connect the wxWindows events with the functions (event
// handlers) which process them. It can be also done at run-time, but for the
// simple menu events like this the static method is much simpler.
BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(HelpDemo_Quit,  MyFrame::OnQuit)
    EVT_MENU(HelpDemo_Help_Index, MyFrame::OnHelp)
    EVT_MENU(HelpDemo_Help_Classes, MyFrame::OnHelp)
    EVT_MENU(HelpDemo_Help_Functions, MyFrame::OnHelp)
    EVT_MENU(HelpDemo_Help_Help, MyFrame::OnHelp)
    EVT_MENU(HelpDemo_Help_Search, MyFrame::OnHelp)
    EVT_MENU(HelpDemo_Help_ContextHelp, MyFrame::OnShowContextHelp)
    EVT_MENU(HelpDemo_Help_DialogContextHelp, MyFrame::OnShowDialogContextHelp)

    EVT_MENU(HelpDemo_Advanced_Html_Help_Index, MyFrame::OnAdvancedHtmlHelp)
    EVT_MENU(HelpDemo_Advanced_Html_Help_Classes, MyFrame::OnAdvancedHtmlHelp)
    EVT_MENU(HelpDemo_Advanced_Html_Help_Functions, MyFrame::OnAdvancedHtmlHelp)
    EVT_MENU(HelpDemo_Advanced_Html_Help_Help, MyFrame::OnAdvancedHtmlHelp)
    EVT_MENU(HelpDemo_Advanced_Html_Help_Search, MyFrame::OnAdvancedHtmlHelp)

    EVT_MENU(HelpDemo_MS_Html_Help_Index, MyFrame::OnMSHtmlHelp)
    EVT_MENU(HelpDemo_MS_Html_Help_Classes, MyFrame::OnMSHtmlHelp)
    EVT_MENU(HelpDemo_MS_Html_Help_Functions, MyFrame::OnMSHtmlHelp)
    EVT_MENU(HelpDemo_MS_Html_Help_Help, MyFrame::OnMSHtmlHelp)
    EVT_MENU(HelpDemo_MS_Html_Help_Search, MyFrame::OnMSHtmlHelp)

    EVT_MENU(HelpDemo_Best_Help_Index, MyFrame::OnBestHelp)

    EVT_MENU(HelpDemo_Help_KDE, MyFrame::OnHelp)
    EVT_MENU(HelpDemo_Help_GNOME, MyFrame::OnHelp)
    EVT_MENU(HelpDemo_Help_Netscape, MyFrame::OnHelp)
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
    // Create a simple help provider to make SetHelpText() do something.
    // Note that this must be set before any SetHelpText() calls are made.
#if USE_SIMPLE_HELP_PROVIDER
    wxSimpleHelpProvider* provider = new wxSimpleHelpProvider;
#else
    wxHelpControllerHelpProvider* provider = new wxHelpControllerHelpProvider;
#endif
    wxHelpProvider::Set(provider);

#if wxUSE_HTML
#if wxUSE_GIF
    // Required for images in the online documentation
    wxImage::AddHandler(new wxGIFHandler);

    // Required for advanced HTML help
#if wxUSE_STREAMS && wxUSE_ZIPSTREAM && wxUSE_ZLIB
      wxFileSystem::AddHandler(new wxZipFSHandler);
#endif

#endif
#endif

    // Create the main application window
    MyFrame *frame = new MyFrame(_T("HelpDemo wxWindows App"),
                                 wxPoint(50, 50), wxSize(450, 340));

#if !USE_SIMPLE_HELP_PROVIDER
#if wxUSE_MS_HTML_HELP
    provider->SetHelpController(& frame->GetMSHtmlHelpController());
#else
    provider->SetHelpController(& frame->GetHelpController());
#endif
#endif

    frame->Show(TRUE);
    SetTopWindow(frame);

    // initialise the help system: this means that we'll use doc.hlp file under
    // Windows and that the HTML docs are in the subdirectory doc for platforms
    // using HTML help
    if ( !frame->GetHelpController().Initialize(_T("doc")) )
    {
        wxLogError(wxT("Cannot initialize the help system, aborting."));

        return FALSE;
    }

#if wxUSE_MS_HTML_HELP
    if( !frame->GetMSHtmlHelpController().Initialize(_T("doc")) )
    {
        wxLogError(wxT("Cannot initialize the MS HTML Help system."));
    }
#endif

#if wxUSE_MS_HTML_HELP && wxUSE_WXHTML_HELP
    // you need to call Initialize in order to use wxBestHelpController
    if( !frame->GetBestHelpController().Initialize(_T("doc")) )
    {
        wxLogError(wxT("Cannot initialize the best help system, aborting."));
    }
#endif

#if USE_HTML_HELP
    // initialise the advanced HTML help system: this means that the HTML docs are in .htb
    // (zipped) form
    if ( !frame->GetAdvancedHtmlHelpController().Initialize(_T("doc")) )
    {
        wxLogError(wxT("Cannot initialize the advanced HTML help system, aborting."));

        return FALSE;
    }
#endif

#if 0
    // defined(__WXMSW__) && wxUSE_MS_HTML_HELP
    wxString path(wxGetCwd());
    if ( !frame->GetMSHtmlHelpController().Initialize(path + _T("\\doc.chm")) )
    {
        wxLogError("Cannot initialize the MS HTML help system, aborting.");

        return FALSE;
    }
#endif

    return TRUE;
}

int MyApp::OnExit()
{
    // clean up
    delete wxHelpProvider::Set(NULL);

    return 0;
}

// ----------------------------------------------------------------------------
// main frame
// ----------------------------------------------------------------------------

// frame constructor
MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
       : wxFrame((wxFrame *)NULL, 300, title, pos, size)
{
    // set the frame icon
    SetIcon(wxICON(mondrian));

    // create a menu bar
    wxMenu *menuFile = new wxMenu;

    menuFile->Append(HelpDemo_Help_Index, _T("&Help Index..."));
    menuFile->Append(HelpDemo_Help_Classes, _T("&Help on Classes..."));
    menuFile->Append(HelpDemo_Help_Functions, _T("&Help on Functions..."));
    menuFile->Append(HelpDemo_Help_ContextHelp, _T("&Context Help..."));
    menuFile->Append(HelpDemo_Help_DialogContextHelp, _T("&Dialog Context Help...\tCtrl-H"));
    menuFile->Append(HelpDemo_Help_Help, _T("&About Help Demo..."));
    menuFile->Append(HelpDemo_Help_Search, _T("&Search help..."));
#if USE_HTML_HELP
    menuFile->AppendSeparator();
    menuFile->Append(HelpDemo_Advanced_Html_Help_Index, _T("Advanced HTML &Help Index..."));
    menuFile->Append(HelpDemo_Advanced_Html_Help_Classes, _T("Advanced HTML &Help on Classes..."));
    menuFile->Append(HelpDemo_Advanced_Html_Help_Functions, _T("Advanced HTML &Help on Functions..."));
    menuFile->Append(HelpDemo_Advanced_Html_Help_Help, _T("Advanced HTML &About Help Demo..."));
    menuFile->Append(HelpDemo_Advanced_Html_Help_Search, _T("Advanced HTML &Search help..."));
#endif

#if wxUSE_MS_HTML_HELP
    menuFile->AppendSeparator();
    menuFile->Append(HelpDemo_MS_Html_Help_Index, _T("MS HTML &Help Index..."));
    menuFile->Append(HelpDemo_MS_Html_Help_Classes, _T("MS HTML &Help on Classes..."));
    menuFile->Append(HelpDemo_MS_Html_Help_Functions, _T("MS HTML &Help on Functions..."));
    menuFile->Append(HelpDemo_MS_Html_Help_Help, _T("MS HTML &About Help Demo..."));
    menuFile->Append(HelpDemo_MS_Html_Help_Search, _T("MS HTML &Search help..."));
#endif

#if wxUSE_MS_HTML_HELP && wxUSE_WXHTML_HELP
    menuFile->AppendSeparator();
    menuFile->Append(HelpDemo_Best_Help_Index, _T("Best &Help Index..."));
#endif

#ifndef __WXMSW__
#if !wxUSE_HTML
    menuFile->AppendSeparator();
    menuFile->Append(HelpDemo_Help_KDE, _T("Use &KDE"));
    menuFile->Append(HelpDemo_Help_GNOME, _T("Use &GNOME"));
    menuFile->Append(HelpDemo_Help_Netscape, _T("Use &Netscape"));
#endif
#endif
    menuFile->AppendSeparator();
    menuFile->Append(HelpDemo_Quit, _T("E&xit"));

    // now append the freshly created menu to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append(menuFile, _T("&File"));

    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);

    // create a status bar just for fun (by default with 1 pane only)
    CreateStatusBar();
    SetStatusText(_T("Welcome to wxWindows!"));

    // now create some controls

    // a panel first - if there were several controls, it would allow us to
    // navigate between them from the keyboard
    wxPanel *panel = new wxPanel(this, 301, wxPoint(0, 0), wxSize(400, 200));
    panel->SetHelpText(_("This panel just holds a static text control."));
    //panel->SetHelpText(wxContextId(300));

    // and a static control whose parent is the panel
    wxStaticText* staticText = new wxStaticText(panel, 302, _T("Hello, world!"), wxPoint(10, 10));
    staticText->SetHelpText(_("This static text control isn't doing a lot right now."));
}


// event handlers

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    // TRUE is to force the frame to close
    Close(TRUE);
}

void MyFrame::OnHelp(wxCommandEvent& event)
{
    ShowHelp(event.GetId(), m_help);
}

void MyFrame::OnShowContextHelp(wxCommandEvent& event)
{
    // This starts context help mode, then the user
    // clicks on a window to send a help message
    wxContextHelp contextHelp(this);
}

void MyFrame::OnShowDialogContextHelp(wxCommandEvent& event)
{
    MyModalDialog dialog(this);
    dialog.ShowModal();
}

void MyFrame::OnAdvancedHtmlHelp(wxCommandEvent& event)
{
#if USE_HTML_HELP
    ShowHelp(event.GetId(), m_advancedHtmlHelp);
#endif
}

void MyFrame::OnMSHtmlHelp(wxCommandEvent& event)
{
#if wxUSE_MS_HTML_HELP
    ShowHelp(event.GetId(), m_msHtmlHelp);
#endif
}

void MyFrame::OnBestHelp(wxCommandEvent& event)
{
#if wxUSE_MS_HTML_HELP && wxUSE_WXHTML_HELP
    ShowHelp(event.GetId(), m_bestHelp);
#endif
}

/*
 Notes: ShowHelp uses section ids for displaying particular topics,
 but you might want to use a unique keyword to display a topic, instead.

 Section ids are specified as follows for the different formats.

 WinHelp

   The [MAP] section specifies the topic to integer id mapping, e.g.

   [MAP]
   #define intro       100
   #define functions   1
   #define classes     2
   #define about       3

   The identifier name corresponds to the label used for that topic.
   You could also put these in a .h file and #include it in both the MAP
   section and your C++ source.

   Note that Tex2RTF doesn't currently generate the MAP section automatically.

 MS HTML Help

   The [MAP] section specifies the HTML filename root to integer id mapping, e.g.

   [MAP]
   #define doc1       100
   #define doc3       1
   #define doc2       2
   #define doc4       3

   The identifier name corresponds to the HTML filename used for that topic.
   You could also put these in a .h file and #include it in both the MAP
   section and your C++ source.

   Note that Tex2RTF doesn't currently generate the MAP section automatically.

 Simple wxHTML Help and External HTML Help

   A wxhelp.map file is used, for example:

   0 wx.htm             ; wxWindows: Help index; additional keywords like overview
   1 wx204.htm          ; wxWindows Function Reference
   2 wx34.htm           ; wxWindows Class Reference

   Note that Tex2RTF doesn't currently generate the MAP section automatically.

 Advanced HTML Help

   An extension to the .hhc file format is used, specifying a new parameter
   with name="ID":

   <OBJECT type="text/sitemap">
   <param name="Local" value="doc2.htm#classes">
   <param name="Name" value="Classes">
   <param name="ID" value=2>
   </OBJECT>

   Again, this is not generated automatically by Tex2RTF, though it could
   be added quite easily.

   Unfortunately adding the ID parameters appears to interfere with MS HTML Help,
   so you should not try to compile a .chm file from a .hhc file with
   this extension, or the contents will be messed up.
 */

void MyFrame::ShowHelp(int commandId, wxHelpControllerBase& helpController)
{
   switch(commandId)
   {
       case HelpDemo_Help_Classes:
       case HelpDemo_Html_Help_Classes:
       case HelpDemo_Advanced_Html_Help_Classes:
       case HelpDemo_MS_Html_Help_Classes:
       case HelpDemo_Best_Help_Classes:
          helpController.DisplaySection(2);
          //helpController.DisplaySection("Classes"); // An alternative form for most controllers
          break;

       case HelpDemo_Help_Functions:
       case HelpDemo_Html_Help_Functions:
       case HelpDemo_Advanced_Html_Help_Functions:
       case HelpDemo_MS_Html_Help_Functions:
          helpController.DisplaySection(1);
          //helpController.DisplaySection("Functions"); // An alternative form for most controllers
          break;

       case HelpDemo_Help_Help:
       case HelpDemo_Html_Help_Help:
       case HelpDemo_Advanced_Html_Help_Help:
       case HelpDemo_MS_Html_Help_Help:
       case HelpDemo_Best_Help_Help:
          helpController.DisplaySection(3);
          //helpController.DisplaySection("About"); // An alternative form for most controllers
          break;

       case HelpDemo_Help_Search:
       case HelpDemo_Html_Help_Search:
       case HelpDemo_Advanced_Html_Help_Search:
       case HelpDemo_MS_Html_Help_Search:
       case HelpDemo_Best_Help_Search:
       {
          wxString key = wxGetTextFromUser(_T("Search for?"),
                                           _T("Search help for keyword"),
                                           _T(""),
                                           this);
          if(! key.IsEmpty())
             helpController.KeywordSearch(key);
       }
       break;

       case HelpDemo_Help_Index:
       case HelpDemo_Html_Help_Index:
       case HelpDemo_Advanced_Html_Help_Index:
       case HelpDemo_MS_Html_Help_Index:
       case HelpDemo_Best_Help_Index:
          helpController.DisplayContents();
          break;

       // These three calls are only used by wxExtHelpController

       case HelpDemo_Help_KDE:
          helpController.SetViewer(_T("kdehelp"));
          break;
       case HelpDemo_Help_GNOME:
          helpController.SetViewer(_T("gnome-help-browser"));
          break;
       case HelpDemo_Help_Netscape:
          helpController.SetViewer(_T("netscape"), wxHELP_NETSCAPE);
          break;
   }
}

// ----------------------------------------------------------------------------
// MyModalDialog
// Demonstrates context-sensitive help
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(MyModalDialog, wxDialog)
END_EVENT_TABLE()

MyModalDialog::MyModalDialog(wxWindow *parent)
             : wxDialog()
{
    // Add the context-sensitive help button on the caption for MSW
#ifdef __WXMSW__
    SetExtraStyle(wxDIALOG_EX_CONTEXTHELP);
#endif

    wxDialog::Create(parent, -1, wxString(_T("Modal dialog")));

    wxBoxSizer *sizerTop = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer *sizerRow = new wxBoxSizer(wxHORIZONTAL);

    wxButton* btnOK = new wxButton(this, wxID_OK, _T("&OK"));
    btnOK->SetHelpText(_("The OK button confirms the dialog choices."));

    wxButton* btnCancel = new wxButton(this, wxID_CANCEL, _T("&Cancel"));
    btnCancel->SetHelpText(_("The Cancel button cancels the dialog."));

    sizerRow->Add(btnOK, 0, wxALIGN_CENTER | wxALL, 5);
    sizerRow->Add(btnCancel, 0, wxALIGN_CENTER | wxALL, 5);

    // Add explicit context-sensitive help button for non-MSW
#ifndef __WXMSW__
    sizerRow->Add(new wxContextHelpButton(this), 0, wxALIGN_CENTER | wxALL, 5);
#endif

    wxTextCtrl *text = new wxTextCtrl(this, -1, wxT("A demo text control"),
                                      wxDefaultPosition, wxSize(300, 100),
                                      wxTE_MULTILINE);
    text->SetHelpText(_("Type text here if you have got nothing more interesting to do"));
    sizerTop->Add(text, 0, wxEXPAND|wxALIGN_CENTER_VERTICAL|wxALL, 5 );
    sizerTop->Add(sizerRow, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    SetAutoLayout(TRUE);
    SetSizer(sizerTop);

    sizerTop->SetSizeHints(this);
    sizerTop->Fit(this);

    btnOK->SetFocus();
    btnOK->SetDefault();
}

