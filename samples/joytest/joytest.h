/////////////////////////////////////////////////////////////////////////////
// Name:        joytest.cpp
// Purpose:     Joystick sample
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id: joytest.h,v 1.5 2002/08/01 19:12:24 MBN Exp $
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// Define a new application
class MyApp: public wxApp
{
public:
    bool OnInit();

    // Joystick max values
    int     m_maxX;
    int     m_maxY;

#if wxUSE_WAVE
    wxWave  m_fire;
#endif // wxUSE_WAVE
};

DECLARE_APP(MyApp)

class MyCanvas: public wxScrolledWindow
{
public:
    MyCanvas(wxWindow *parent, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize);
    ~MyCanvas();
    void OnJoystickEvent(wxJoystickEvent& event);

    DECLARE_EVENT_TABLE()
};

class MyFrame: public wxFrame
{
public:
    MyCanvas *canvas;
    MyFrame(wxFrame *parent, const wxString& title,
        const wxPoint& pos, const wxSize& size, const long style);
    ~MyFrame();
    void OnActivate(wxActivateEvent& event);
    void OnQuit(wxCommandEvent& event);

DECLARE_EVENT_TABLE()
};

#define JOYTEST_QUIT        1
