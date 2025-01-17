/////////////////////////////////////////////////////////////////////////////
// Name:        oleauto.cpp
// Purpose:     OLE Automation wxWindows sample
// Author:      Julian Smart
// Modified by:
// Created:     08/12/98
// RCS-ID:      $Id: oleauto.cpp,v 1.5.2.1 2002/12/15 17:25:17 MBN Exp $
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------
#ifdef __GNUG__
    #pragma implementation "oleauto.cpp"
    #pragma interface "oleauto.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWindows headers
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include <wx/msw/ole/automtn.h>

#ifndef __WXMSW__
#error "Sorry, this sample works under Windows only."
#endif

#ifdef __WATCOMC__
#error "Sorry, Watcom C++ does not support wxAutomationObject."
#endif

// ----------------------------------------------------------------------------
// ressources
// ----------------------------------------------------------------------------
// the application icon
#if defined(__WXGTK__) || defined(__WXX11__) || defined(__WXMOTIF__)
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
};

// Define a new frame type: this is going to be our main frame
class MyFrame : public wxFrame
{
public:
    // ctor(s)
    MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size);

    // event handlers (these functions should _not_ be virtual)
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnTest(wxCommandEvent& event);

private:
    // any class wishing to process wxWindows events must use this macro
    DECLARE_EVENT_TABLE()
};

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// IDs for the controls and the menu commands
enum
{
    // menu items
    OleAuto_Quit = 1,
    OleAuto_About,
    OleAuto_Test,

    // controls start here (the numbers are, of course, arbitrary)
    OleAuto_Text = 1000,
};

// ----------------------------------------------------------------------------
// event tables and other macros for wxWindows
// ----------------------------------------------------------------------------

// the event tables connect the wxWindows events with the functions (event
// handlers) which process them. It can be also done at run-time, but for the
// simple menu events like this the static method is much simpler.
BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(OleAuto_Quit,  MyFrame::OnQuit)
    EVT_MENU(OleAuto_About, MyFrame::OnAbout)
    EVT_MENU(OleAuto_Test, MyFrame::OnTest)
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
    // Create the main application window
    MyFrame *frame = new MyFrame(_T("OleAuto wxWindows App"),
                                 wxPoint(50, 50), wxSize(450, 340));

    // Show it and tell the application that it's our main window
    // @@@ what does it do exactly, in fact? is it necessary here?
    frame->Show(TRUE);
    SetTopWindow(frame);

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
       : wxFrame((wxFrame *)NULL, -1, title, pos, size)
{
    // set the frame icon
    SetIcon(wxICON(mondrian));

    // create a menu bar
    wxMenu *menuFile = new wxMenu;

    menuFile->Append(OleAuto_Test, _T("&Test Excel Automation..."));
    menuFile->Append(OleAuto_About, _T("&About..."));
    menuFile->AppendSeparator();
    menuFile->Append(OleAuto_Quit, _T("E&xit"));

    // now append the freshly created menu to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append(menuFile, _T("&File"));

    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);

    // create a status bar just for fun (by default with 1 pane only)
    CreateStatusBar(2);
    SetStatusText(_T("Welcome to wxWindows!"));
}


// event handlers

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    // TRUE is to force the frame to close
    Close(TRUE);
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxMessageBox(_T("This is an OLE Automation sample"),
                 _T("About OleAuto"), wxOK | wxICON_INFORMATION, this);
}

/* Tests OLE automation by making the active Excel cell bold,
 * and changing the text.
 */
void MyFrame::OnTest(wxCommandEvent& WXUNUSED(event))
{
    wxMessageBox(_T("Please ensure Excel is running, then press OK.\nThe active cell should then say 'wxWindows automation test!' in bold."));

    wxAutomationObject excelObject, rangeObject;
    if (!excelObject.GetInstance(_T("Excel.Application")))
    {
        if (!excelObject.CreateInstance(_T("Excel.Application")))
        {
            wxMessageBox(_T("Could not create Excel object."));
            return;
        }
    }
    if (!excelObject.PutProperty(_T("ActiveCell.Value"), _T("wxWindows automation test!")))
    {
        wxMessageBox(_T("Could not set active cell value."));
        return;
    }
#ifdef HAVE_BOOL
    if (!excelObject.PutProperty(_T("ActiveCell.Font.Bold"), wxVariant((bool) TRUE)) )
    {
        wxMessageBox(_T("Could not put Bold property to active cell."));
        return;
    }
#endif
}

