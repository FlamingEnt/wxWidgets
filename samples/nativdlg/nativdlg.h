/////////////////////////////////////////////////////////////////////////////
// Name:        nativdlg.h
// Purpose:     Native Windows dialog sample
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id: nativdlg.h,v 1.3 2002/03/17 14:15:46 VZ Exp $
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma interface
#endif

// Define a new application
class MyApp: public wxApp
{
  public:
    MyApp(void) ;
    bool OnInit(void);
};

class MyFrame: public wxFrame
{
  public:
    wxWindow *panel;
    MyFrame(wxWindow *parent, const wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size);
    void OnQuit(wxCommandEvent& event);
    void OnTest1(wxCommandEvent& event);

 DECLARE_EVENT_TABLE()
};

class MyDialog : public wxDialog
{
  public:
    void OnOk(wxCommandEvent& event);
    void OnCancel(wxCommandEvent& event);

 DECLARE_EVENT_TABLE()
};

#define RESOURCE_QUIT       4
#define RESOURCE_TEST1      2

