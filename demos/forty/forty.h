/////////////////////////////////////////////////////////////////////////////
// Name:        forty.h
// Purpose:     Forty Thieves patience game
// Author:      Chris Breeze
// Modified by:
// Created:     21/07/97
// RCS-ID:      $Id: forty.h,v 1.4.2.1 2003/01/12 20:47:59 MBN Exp $
// Copyright:   (c) 1993-1998 Chris Breeze
// Licence:   	wxWindows licence
//---------------------------------------------------------------------------
// Last modified: 22nd July 1998 - ported to wxWindows 2.0
/////////////////////////////////////////////////////////////////////////////
#ifndef _FORTY_H_
#define _FORTY_H_

class FortyApp: public wxApp
{
public:
    FortyApp();
    ~FortyApp();
	bool OnInit();

	static const wxColour& BackgroundColour();
	static const wxColour& TextColour();
	static const wxBrush&  BackgroundBrush();

private:
	static wxColour* m_backgroundColour;
	static wxColour* m_textColour;
	static wxBrush*  m_backgroundBrush;
};

class FortyCanvas;
class FortyFrame: public wxFrame
{
public:
        FortyFrame(wxFrame* frame, const wxString& title, int x, int y, int w, int h,bool largecards);
	virtual ~FortyFrame();

	void OnCloseWindow(wxCloseEvent& event);

	// Menu callbacks
	void NewGame(wxCommandEvent& event);
	void Exit(wxCommandEvent& event);
	void About(wxCommandEvent& event);
	void Undo(wxCommandEvent& event);
	void Redo(wxCommandEvent& event);
	void Scores(wxCommandEvent& event);
	void ToggleRightButtonUndo(wxCommandEvent& event);
	void ToggleHelpingHand(wxCommandEvent& event);
        void ToggleCardSize(wxCommandEvent& event);

        FortyCanvas* GetCanvas() { return m_canvas; }

	DECLARE_EVENT_TABLE()

private:
	enum MenuCommands { NEW_GAME = 10, SCORES, EXIT,
						UNDO, REDO,
                                                RIGHT_BUTTON_UNDO, HELPING_HAND, LARGE_CARDS,
						ABOUT };

	wxMenuBar*		m_menuBar;
	FortyCanvas*	m_canvas;
};

//----------------------------------------------------------------------------
// stAboutDialog
//----------------------------------------------------------------------------

class FortyAboutDialog: public wxDialog
{
public:
    // constructors and destructors
    FortyAboutDialog( wxWindow *parent, wxWindowID id, const wxString &title,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxDEFAULT_DIALOG_STYLE );
    
    bool AddControls(wxWindow* parent);

private:
    DECLARE_EVENT_TABLE()
};

#define ID_ABOUT_HTML_WINDOW    1000

#endif
