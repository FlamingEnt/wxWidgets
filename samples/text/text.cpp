/////////////////////////////////////////////////////////////////////////////
// Name:        text.cpp
// Purpose:     TextCtrl wxWindows sample
// Author:      Robert Roebling
// Modified by:
// RCS-ID:      $Id: text.cpp,v 1.49.2.4 2002/12/14 18:19:58 MBN Exp $
// Copyright:   (c) Robert Roebling, Julian Smart, Vadim Zeitlin
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
    #pragma implementation "text.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#if wxUSE_CLIPBOARD
    #include "wx/dataobj.h"
    #include "wx/clipbrd.h"
#endif

#if wxUSE_FILE
    #include "wx/file.h"
#endif

#if wxUSE_TOOLTIPS
    #include "wx/tooltip.h"
#endif

    #include "wx/progdlg.h"

// We test for wxUSE_DRAG_AND_DROP also, because data objects may not be
// implemented for compilers that can't cope with the OLE parts in
// wxUSE_DRAG_AND_DROP.
#if !wxUSE_DRAG_AND_DROP
    #undef wxUSE_CLIPBOARD
    #define wxUSE_CLIPBOARD 0
#endif

//----------------------------------------------------------------------
// class definitions
//----------------------------------------------------------------------

class MyApp: public wxApp
{
public:
    bool OnInit();
};

// a text ctrl which allows to call different wxTextCtrl functions
// interactively by pressing function keys in it
class MyTextCtrl : public wxTextCtrl
{
public:
    MyTextCtrl(wxWindow *parent, wxWindowID id, const wxString &value,
               const wxPoint &pos, const wxSize &size, int style = 0)
        : wxTextCtrl(parent, id, value, pos, size, style)
    {
        m_hasCapture = FALSE;
    }

    void OnKeyDown(wxKeyEvent& event);
    void OnKeyUp(wxKeyEvent& event);
    void OnChar(wxKeyEvent& event);

    void OnText(wxCommandEvent& event);
    void OnTextURL(wxTextUrlEvent& event);
    void OnTextMaxLen(wxCommandEvent& event);

    void OnMouseEvent(wxMouseEvent& event);

    void OnSetFocus(wxFocusEvent& event);
    void OnKillFocus(wxFocusEvent& event);

    static bool ms_logKey;
    static bool ms_logChar;
    static bool ms_logMouse;
    static bool ms_logText;
    static bool ms_logFocus;

private:
    static inline wxChar GetChar(bool on, wxChar c) { return on ? c : _T('-'); }
    void LogKeyEvent(const wxChar *name, wxKeyEvent& event) const;

    bool m_hasCapture;

    DECLARE_EVENT_TABLE()
};

class MyPanel: public wxPanel
{
public:
    MyPanel(wxFrame *frame, int x, int y, int w, int h);
    virtual ~MyPanel() { delete wxLog::SetActiveTarget(m_logOld); }

#if wxUSE_CLIPBOARD
    void DoPasteFromClipboard();
    void DoCopyToClipboard();
#endif // wxUSE_CLIPBOARD

    void DoRemoveText();
    void DoMoveToEndOfText();
    void DoMoveToEndOfEntry();

    void OnSize( wxSizeEvent &event );

    MyTextCtrl    *m_text;
    MyTextCtrl    *m_password;
    MyTextCtrl    *m_enter;
    MyTextCtrl    *m_tab;
    MyTextCtrl    *m_readonly;
    MyTextCtrl    *m_limited;

    MyTextCtrl    *m_multitext;
    MyTextCtrl    *m_horizontal;

    MyTextCtrl    *m_textrich;

    wxTextCtrl    *m_log;

    wxLog         *m_logOld;

private:
    // get the currently focused text control or return the default one is no
    // text ctrl has focus
    wxTextCtrl *GetFocusedText(wxTextCtrl *textDef);

    DECLARE_EVENT_TABLE()
};

class MyFrame: public wxFrame
{
public:
    MyFrame(wxFrame *frame, const wxChar *title, int x, int y, int w, int h);

    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
#if wxUSE_TOOLTIPS
    void OnSetTooltipDelay(wxCommandEvent& event);
    void OnToggleTooltips(wxCommandEvent& event);
#endif // wxUSE_TOOLTIPS

#if wxUSE_CLIPBOARD
    void OnPasteFromClipboard( wxCommandEvent &event )
        { m_panel->DoPasteFromClipboard(); }
    void OnCopyToClipboard( wxCommandEvent &event )
        { m_panel->DoCopyToClipboard(); }
#endif // wxUSE_CLIPBOARD

    void OnAddTextFreeze( wxCommandEvent& event )
        { DoAddText(true); }
    void OnAddText( wxCommandEvent& event )
        { DoAddText(false); }
    void OnRemoveText( wxCommandEvent& event )
        { m_panel->DoRemoveText(); }

    void OnMoveToEndOfText( wxCommandEvent &event )
        { m_panel->DoMoveToEndOfText(); }
    void OnMoveToEndOfEntry( wxCommandEvent &event )
        { m_panel->DoMoveToEndOfEntry(); }

    void OnScrollLineDown(wxCommandEvent& event)
    {
        if ( !m_panel->m_textrich->LineDown() )
            wxLogMessage(_T("Already at the bottom"));
    }

    void OnScrollLineUp(wxCommandEvent& event)
    {
        if ( !m_panel->m_textrich->LineUp() )
            wxLogMessage(_T("Already at the top"));
    }

    void OnScrollPageDown(wxCommandEvent& event)
    {
        if ( !m_panel->m_textrich->PageDown() )
            wxLogMessage(_T("Already at the bottom"));
    }

    void OnScrollPageUp(wxCommandEvent& event)
    {
        if ( !m_panel->m_textrich->PageUp() )
            wxLogMessage(_T("Already at the top"));
    }

    void OnLogClear(wxCommandEvent& event);
    void OnFileSave(wxCommandEvent& event);
    void OnFileLoad(wxCommandEvent& event);

    void OnSetEditable(wxCommandEvent& event);
    void OnSetEnabled(wxCommandEvent& event);

    void OnLogKey(wxCommandEvent& event)
    {
        MyTextCtrl::ms_logKey = event.IsChecked();
    }

    void OnLogChar(wxCommandEvent& event)
    {
        MyTextCtrl::ms_logChar = event.IsChecked();
    }

    void OnLogMouse(wxCommandEvent& event)
    {
        MyTextCtrl::ms_logMouse = event.IsChecked();
    }

    void OnLogText(wxCommandEvent& event)
    {
        MyTextCtrl::ms_logText = event.IsChecked();
    }

    void OnLogFocus(wxCommandEvent& event)
    {
        MyTextCtrl::ms_logFocus = event.IsChecked();
    }

    void OnSetText(wxCommandEvent& event)
    {
        m_panel->m_text->SetValue(_T("Hello, world (what else did you expect)?"));
    }

    void OnIdle( wxIdleEvent& event );

private:
    void DoAddText(bool freeze)
    {
        wxTextCtrl *text = m_panel->m_textrich;
        if ( freeze )
            text->Freeze();

        text->Clear();

        wxProgressDialog dlg(_T("Wait..."), _T("Updating"), 100, this);
        for ( int i = 0; i < 100; i++ )
        {
            dlg.Update(i);
            text->AppendText(wxString::Format(wxT("Line %i\n"), i));
        }

        text->SetInsertionPoint(0);

        if ( freeze )
            text->Thaw();
    }

    MyPanel *m_panel;

    DECLARE_EVENT_TABLE()
};

//----------------------------------------------------------------------
// main()
//----------------------------------------------------------------------

IMPLEMENT_APP(MyApp)

//----------------------------------------------------------------------
// MyApp
//----------------------------------------------------------------------

enum
{
    TEXT_QUIT = 100,
    TEXT_ABOUT,
    TEXT_LOAD,
    TEXT_SAVE,
    TEXT_CLEAR,

    // clipboard menu
    TEXT_CLIPBOARD_COPY = 200,
    TEXT_CLIPBOARD_PASTE,

    // tooltip menu
    TEXT_TOOLTIPS_SETDELAY = 300,
    TEXT_TOOLTIPS_ENABLE,

    // text menu
    TEXT_ADD_SOME = 400,
    TEXT_ADD_FREEZE,
    TEXT_MOVE_ENDTEXT,
    TEXT_MOVE_ENDENTRY,
    TEXT_SET_EDITABLE,
    TEXT_SET_ENABLED,
    TEXT_LINE_DOWN,
    TEXT_LINE_UP,
    TEXT_PAGE_DOWN,
    TEXT_PAGE_UP,
    TEXT_REMOVE,
    TEXT_SET,

    // log menu
    TEXT_LOG_KEY,
    TEXT_LOG_CHAR,
    TEXT_LOG_MOUSE,
    TEXT_LOG_TEXT,
    TEXT_LOG_FOCUS,

    TEXT_END
};

bool MyApp::OnInit()
{
    // Create the main frame window
    MyFrame *frame = new MyFrame((wxFrame *) NULL,
            _T("Text wxWindows sample"), 50, 50, 700, 420);
    frame->SetSizeHints( 500, 400 );

    wxMenu *file_menu = new wxMenu;
    file_menu->Append(TEXT_SAVE, _T("&Save file\tCtrl-S"),
                      _T("Save the text control contents to file"));
    file_menu->Append(TEXT_LOAD, _T("&Load file\tCtrl-O"),
                      _T("Load the sample file into text control"));
    file_menu->AppendSeparator();
    file_menu->Append(TEXT_ABOUT, _T("&About\tAlt-A"));
    file_menu->AppendSeparator();
    file_menu->Append(TEXT_QUIT, _T("E&xit\tAlt-X"), _T("Quit this sample"));

    wxMenuBar *menu_bar = new wxMenuBar( wxMB_DOCKABLE );
    menu_bar->Append(file_menu, _T("&File"));

#if wxUSE_TOOLTIPS
    wxMenu *tooltip_menu = new wxMenu;
    tooltip_menu->Append(TEXT_TOOLTIPS_SETDELAY, _T("Set &delay\tCtrl-D"));
    tooltip_menu->AppendSeparator();
    tooltip_menu->Append(TEXT_TOOLTIPS_ENABLE, _T("&Toggle tooltips\tCtrl-T"),
            _T("enable/disable tooltips"), TRUE);
    tooltip_menu->Check(TEXT_TOOLTIPS_ENABLE, TRUE);
    menu_bar->Append(tooltip_menu, _T("&Tooltips"));
#endif // wxUSE_TOOLTIPS

#if wxUSE_CLIPBOARD
    wxMenu *menuClipboard = new wxMenu;
    menuClipboard->Append(TEXT_CLIPBOARD_COPY, _T("&Copy\tCtrl-C"),
                          _T("Copy the first line to the clipboard"));
    menuClipboard->Append(TEXT_CLIPBOARD_PASTE, _T("&Paste\tCtrl-V"),
                          _T("Paste from clipboard to the text control"));
    menu_bar->Append(menuClipboard, _T("&Clipboard"));
#endif // wxUSE_CLIPBOARD

    wxMenu *menuText = new wxMenu;
    menuText->Append(TEXT_ADD_SOME, _T("&Append some text\tCtrl-A"));
    menuText->Append(TEXT_ADD_FREEZE, _T("&Append text with freeze/thaw\tShift-Ctrl-A"));
    menuText->Append(TEXT_REMOVE, _T("&Remove first 10 characters\tCtrl-X"));
    menuText->Append(TEXT_SET, _T("&Set the first text zone value\tCtrl-E"));
    menuText->AppendSeparator();
    menuText->Append(TEXT_MOVE_ENDTEXT, _T("Move cursor to the end of &text"));
    menuText->Append(TEXT_MOVE_ENDENTRY, _T("Move cursor to the end of &entry"));
    menuText->Append(TEXT_SET_EDITABLE, _T("Toggle &editable state"), _T(""), TRUE);
    menuText->Append(TEXT_SET_ENABLED, _T("Toggle e&nabled state"), _T(""), TRUE);
    menuText->Check(TEXT_SET_EDITABLE, TRUE);
    menuText->Check(TEXT_SET_ENABLED, TRUE);
    menuText->AppendSeparator();
    menuText->Append(TEXT_LINE_DOWN, _T("Scroll text one line down"));
    menuText->Append(TEXT_LINE_UP, _T("Scroll text one line up"));
    menuText->Append(TEXT_PAGE_DOWN, _T("Scroll text one page down"));
    menuText->Append(TEXT_PAGE_DOWN, _T("Scroll text one page up"));
    menu_bar->Append(menuText, _T("Te&xt"));

    wxMenu *menuLog = new wxMenu;
    menuLog->Append(TEXT_LOG_KEY, _T("Log &key events"), _T(""), TRUE);
    menuLog->Append(TEXT_LOG_CHAR, _T("Log &char events"), _T(""), TRUE);
    menuLog->Append(TEXT_LOG_MOUSE, _T("Log &mouse events"), _T(""), TRUE);
    menuLog->Append(TEXT_LOG_TEXT, _T("Log &text events"), _T(""), TRUE);
    menuLog->Append(TEXT_LOG_FOCUS, _T("Log &focus events"), _T(""), TRUE);
    menuLog->AppendSeparator();
    menuLog->Append(TEXT_CLEAR, _T("&Clear the log\tCtrl-C"),
                    _T("Clear the log window contents"));

    // select only the interesting events by default
    menuLog->Check(TEXT_LOG_KEY, TRUE);
    menuLog->Check(TEXT_LOG_CHAR, TRUE);
    menuLog->Check(TEXT_LOG_TEXT, TRUE);

    MyTextCtrl::ms_logKey =
    MyTextCtrl::ms_logChar =
    MyTextCtrl::ms_logText = TRUE;
    menu_bar->Append(menuLog, _T("&Log"));

    frame->SetMenuBar(menu_bar);

    frame->Show(TRUE);

    SetTopWindow(frame);

    // report success
    return TRUE;
}

//----------------------------------------------------------------------
// MyTextCtrl
//----------------------------------------------------------------------

BEGIN_EVENT_TABLE(MyTextCtrl, wxTextCtrl)
    EVT_KEY_DOWN(MyTextCtrl::OnKeyDown)
    EVT_KEY_UP(MyTextCtrl::OnKeyUp)
    EVT_CHAR(MyTextCtrl::OnChar)

    EVT_TEXT(-1, MyTextCtrl::OnText)
    EVT_TEXT_URL(-1, MyTextCtrl::OnTextURL)
    EVT_TEXT_MAXLEN(-1, MyTextCtrl::OnTextMaxLen)

    EVT_MOUSE_EVENTS(MyTextCtrl::OnMouseEvent)

    EVT_SET_FOCUS(MyTextCtrl::OnSetFocus)
    EVT_KILL_FOCUS(MyTextCtrl::OnKillFocus)
END_EVENT_TABLE()

bool MyTextCtrl::ms_logKey = FALSE;
bool MyTextCtrl::ms_logChar = FALSE;
bool MyTextCtrl::ms_logMouse = FALSE;
bool MyTextCtrl::ms_logText = FALSE;
bool MyTextCtrl::ms_logFocus = FALSE;

void MyTextCtrl::LogKeyEvent(const wxChar *name, wxKeyEvent& event) const
{
    wxString key;
    long keycode = event.KeyCode();
    {
        switch ( keycode )
        {
            case WXK_BACK: key = _T("BACK"); break;
            case WXK_TAB: key = _T("TAB"); break;
            case WXK_RETURN: key = _T("RETURN"); break;
            case WXK_ESCAPE: key = _T("ESCAPE"); break;
            case WXK_SPACE: key = _T("SPACE"); break;
            case WXK_DELETE: key = _T("DELETE"); break;
            case WXK_START: key = _T("START"); break;
            case WXK_LBUTTON: key = _T("LBUTTON"); break;
            case WXK_RBUTTON: key = _T("RBUTTON"); break;
            case WXK_CANCEL: key = _T("CANCEL"); break;
            case WXK_MBUTTON: key = _T("MBUTTON"); break;
            case WXK_CLEAR: key = _T("CLEAR"); break;
            case WXK_SHIFT: key = _T("SHIFT"); break;
            case WXK_ALT: key = _T("ALT"); break;
            case WXK_CONTROL: key = _T("CONTROL"); break;
            case WXK_MENU: key = _T("MENU"); break;
            case WXK_PAUSE: key = _T("PAUSE"); break;
            case WXK_CAPITAL: key = _T("CAPITAL"); break;
            case WXK_PRIOR: key = _T("PRIOR"); break;
            case WXK_NEXT: key = _T("NEXT"); break;
            case WXK_END: key = _T("END"); break;
            case WXK_HOME: key = _T("HOME"); break;
            case WXK_LEFT: key = _T("LEFT"); break;
            case WXK_UP: key = _T("UP"); break;
            case WXK_RIGHT: key = _T("RIGHT"); break;
            case WXK_DOWN: key = _T("DOWN"); break;
            case WXK_SELECT: key = _T("SELECT"); break;
            case WXK_PRINT: key = _T("PRINT"); break;
            case WXK_EXECUTE: key = _T("EXECUTE"); break;
            case WXK_SNAPSHOT: key = _T("SNAPSHOT"); break;
            case WXK_INSERT: key = _T("INSERT"); break;
            case WXK_HELP: key = _T("HELP"); break;
            case WXK_NUMPAD0: key = _T("NUMPAD0"); break;
            case WXK_NUMPAD1: key = _T("NUMPAD1"); break;
            case WXK_NUMPAD2: key = _T("NUMPAD2"); break;
            case WXK_NUMPAD3: key = _T("NUMPAD3"); break;
            case WXK_NUMPAD4: key = _T("NUMPAD4"); break;
            case WXK_NUMPAD5: key = _T("NUMPAD5"); break;
            case WXK_NUMPAD6: key = _T("NUMPAD6"); break;
            case WXK_NUMPAD7: key = _T("NUMPAD7"); break;
            case WXK_NUMPAD8: key = _T("NUMPAD8"); break;
            case WXK_NUMPAD9: key = _T("NUMPAD9"); break;
            case WXK_MULTIPLY: key = _T("MULTIPLY"); break;
            case WXK_ADD: key = _T("ADD"); break;
            case WXK_SEPARATOR: key = _T("SEPARATOR"); break;
            case WXK_SUBTRACT: key = _T("SUBTRACT"); break;
            case WXK_DECIMAL: key = _T("DECIMAL"); break;
            case WXK_DIVIDE: key = _T("DIVIDE"); break;
            case WXK_F1: key = _T("F1"); break;
            case WXK_F2: key = _T("F2"); break;
            case WXK_F3: key = _T("F3"); break;
            case WXK_F4: key = _T("F4"); break;
            case WXK_F5: key = _T("F5"); break;
            case WXK_F6: key = _T("F6"); break;
            case WXK_F7: key = _T("F7"); break;
            case WXK_F8: key = _T("F8"); break;
            case WXK_F9: key = _T("F9"); break;
            case WXK_F10: key = _T("F10"); break;
            case WXK_F11: key = _T("F11"); break;
            case WXK_F12: key = _T("F12"); break;
            case WXK_F13: key = _T("F13"); break;
            case WXK_F14: key = _T("F14"); break;
            case WXK_F15: key = _T("F15"); break;
            case WXK_F16: key = _T("F16"); break;
            case WXK_F17: key = _T("F17"); break;
            case WXK_F18: key = _T("F18"); break;
            case WXK_F19: key = _T("F19"); break;
            case WXK_F20: key = _T("F20"); break;
            case WXK_F21: key = _T("F21"); break;
            case WXK_F22: key = _T("F22"); break;
            case WXK_F23: key = _T("F23"); break;
            case WXK_F24: key = _T("F24"); break;
            case WXK_NUMLOCK: key = _T("NUMLOCK"); break;
            case WXK_SCROLL: key = _T("SCROLL"); break;
            case WXK_PAGEUP: key = _T("PAGEUP"); break;
            case WXK_PAGEDOWN: key = _T("PAGEDOWN"); break;
            case WXK_NUMPAD_SPACE: key = _T("NUMPAD_SPACE"); break;
            case WXK_NUMPAD_TAB: key = _T("NUMPAD_TAB"); break;
            case WXK_NUMPAD_ENTER: key = _T("NUMPAD_ENTER"); break;
            case WXK_NUMPAD_F1: key = _T("NUMPAD_F1"); break;
            case WXK_NUMPAD_F2: key = _T("NUMPAD_F2"); break;
            case WXK_NUMPAD_F3: key = _T("NUMPAD_F3"); break;
            case WXK_NUMPAD_F4: key = _T("NUMPAD_F4"); break;
            case WXK_NUMPAD_HOME: key = _T("NUMPAD_HOME"); break;
            case WXK_NUMPAD_LEFT: key = _T("NUMPAD_LEFT"); break;
            case WXK_NUMPAD_UP: key = _T("NUMPAD_UP"); break;
            case WXK_NUMPAD_RIGHT: key = _T("NUMPAD_RIGHT"); break;
            case WXK_NUMPAD_DOWN: key = _T("NUMPAD_DOWN"); break;
            case WXK_NUMPAD_PRIOR: key = _T("NUMPAD_PRIOR"); break;
            case WXK_NUMPAD_PAGEUP: key = _T("NUMPAD_PAGEUP"); break;
            case WXK_NUMPAD_PAGEDOWN: key = _T("NUMPAD_PAGEDOWN"); break;
            case WXK_NUMPAD_END: key = _T("NUMPAD_END"); break;
            case WXK_NUMPAD_BEGIN: key = _T("NUMPAD_BEGIN"); break;
            case WXK_NUMPAD_INSERT: key = _T("NUMPAD_INSERT"); break;
            case WXK_NUMPAD_DELETE: key = _T("NUMPAD_DELETE"); break;
            case WXK_NUMPAD_EQUAL: key = _T("NUMPAD_EQUAL"); break;
            case WXK_NUMPAD_MULTIPLY: key = _T("NUMPAD_MULTIPLY"); break;
            case WXK_NUMPAD_ADD: key = _T("NUMPAD_ADD"); break;
            case WXK_NUMPAD_SEPARATOR: key = _T("NUMPAD_SEPARATOR"); break;
            case WXK_NUMPAD_SUBTRACT: key = _T("NUMPAD_SUBTRACT"); break;
            case WXK_NUMPAD_DECIMAL: key = _T("NUMPAD_DECIMAL"); break;

            default:
            {
               if ( wxIsprint((int)keycode) )
                   key.Printf(_T("'%c'"), (char)keycode);
               else if ( keycode > 0 && keycode < 27 )
                   key.Printf(_("Ctrl-%c"), _T('A') + keycode - 1);
               else
                   key.Printf(_T("unknown (%ld)"), keycode);
            }
        }
    }

    wxLogMessage( _T("%s event: %s (flags = %c%c%c%c)"),
                  name,
                  key.c_str(),
                  GetChar( event.ControlDown(), _T('C') ),
                  GetChar( event.AltDown(), _T('A') ),
                  GetChar( event.ShiftDown(), _T('S') ),
                  GetChar( event.MetaDown(), _T('M') ) );
}

static wxString GetMouseEventDesc(const wxMouseEvent& ev)
{
    // click event
    wxString button;
    bool dbl, up;
    if ( ev.LeftDown() || ev.LeftUp() || ev.LeftDClick() )
    {
        button = _T("Left");
        dbl = ev.LeftDClick();
        up = ev.LeftUp();
    }
    else if ( ev.MiddleDown() || ev.MiddleUp() || ev.MiddleDClick() )
    {
        button = _T("Middle");
        dbl = ev.MiddleDClick();
        up = ev.MiddleUp();
    }
    else if ( ev.RightDown() || ev.RightUp() || ev.RightDClick() )
    {
        button = _T("Right");
        dbl = ev.RightDClick();
        up = ev.RightUp();
    }
    else
    {
        return _T("Unknown mouse event");
    }

    return wxString::Format(_T("%s mouse button %s"),
                            button.c_str(),
                            dbl ? _T("double clicked")
                                : up ? _T("released") : _T("clicked"));
}

void MyTextCtrl::OnMouseEvent(wxMouseEvent& ev)
{
    ev.Skip();

    if ( !ms_logMouse )
        return;

    if ( !ev.Moving() )
    {
        wxString msg;
        if ( ev.Entering() )
        {
            msg = _T("Mouse entered the window");
        }
        else if ( ev.Leaving() )
        {
            msg = _T("Mouse left the window");
        }
        else
        {
            msg = GetMouseEventDesc(ev);
        }

        msg << _T(" at (") << ev.GetX() << _T(", ") << ev.GetY() << _T(") ")
            << _T("Flags: ")
            << GetChar( ev.LeftIsDown(), _T('1') )
            << GetChar( ev.MiddleIsDown(), _T('2') )
            << GetChar( ev.RightIsDown(), _T('3') )
            << GetChar( ev.ControlDown(), _T('C') )
            << GetChar( ev.AltDown(), _T('A') )
            << GetChar( ev.ShiftDown(), _T('S') )
            << GetChar( ev.MetaDown(), _T('M') );

        wxLogMessage(msg);
    }
    //else: we're not interested in mouse move events
}

void MyTextCtrl::OnSetFocus(wxFocusEvent& event)
{
    if ( ms_logFocus )
        wxLogMessage( wxT("%p got focus."), this);

    event.Skip();
}

void MyTextCtrl::OnKillFocus(wxFocusEvent& event)
{
    if ( ms_logFocus )
        wxLogMessage( wxT("%p lost focus"), this);

    event.Skip();
}

void MyTextCtrl::OnText(wxCommandEvent& event)
{
    if ( !ms_logText )
        return;

    MyTextCtrl *win = (MyTextCtrl *)event.GetEventObject();
    const wxChar *data = (const wxChar *)(win->GetClientData());
    if ( data )
    {
        wxLogMessage(_T("Text changed in control '%s'"), data);
    }
    else
    {
        // wxLogMessage( event.GetString() );
        wxLogMessage(_T("Text changed in some control"));
    }
}

void MyTextCtrl::OnTextMaxLen(wxCommandEvent& event)
{
    wxLogMessage(_T("You can't enter more characters into this control."));
}

void MyTextCtrl::OnTextURL(wxTextUrlEvent& event)
{
    const wxMouseEvent& ev = event.GetMouseEvent();

    // filter out mouse moves, too many of them
    if ( ev.Moving() )
        return;

    long start = event.GetURLStart(),
         end = event.GetURLEnd();

    wxLogMessage(_T("Mouse event over URL '%s': %s"),
                 GetValue().Mid(start, end - start).c_str(),
                 GetMouseEventDesc(ev).c_str());
}

void MyTextCtrl::OnChar(wxKeyEvent& event)
{
    if ( ms_logChar )
        LogKeyEvent( _T("Char"), event);

    event.Skip();
}

void MyTextCtrl::OnKeyUp(wxKeyEvent& event)
{
    if ( ms_logKey )
        LogKeyEvent( _T("Key up"), event);

    event.Skip();
}

void MyTextCtrl::OnKeyDown(wxKeyEvent& event)
{
    switch ( event.KeyCode() )
    {
        case WXK_F1:
            // show current position and text length
            {
                long line, column, pos = GetInsertionPoint();
                PositionToXY(pos, &column, &line);

                wxLogMessage(_T("Current position: %ld\nCurrent line, column: (%ld, %ld)\nNumber of lines: %ld\nCurrent line length: %ld\nTotal text length: %u (%ld)"),
                        pos,
                        line, column,
                        (long) GetNumberOfLines(),
                        (long) GetLineLength(line),
                        GetValue().length(),
                        GetLastPosition());

                long from, to;
                GetSelection(&from, &to);

                wxString sel = GetStringSelection();

                wxLogMessage(_T("Selection: from %ld to %ld."), from, to);
                wxLogMessage(_T("Selection = '%s' (len = %u)"),
                             sel.c_str(), sel.length());
            }
            break;

        case WXK_F2:
            // go to the end
            SetInsertionPointEnd();
            break;

        case WXK_F3:
            // go to position 10
            SetInsertionPoint(10);
            break;

        case WXK_F4:
            if (!m_hasCapture)
            {
                wxLogDebug( wxT("Now capturing mouse and events.") );
                m_hasCapture = TRUE;
                CaptureMouse();
            }
            else
            {
                wxLogDebug( wxT("Stopped capturing mouse and events.") );
                m_hasCapture = FALSE;
                ReleaseMouse();
            }
            break;

        case WXK_F5:
            // insert a blank line
            WriteText(_T("\n"));
            break;

        case WXK_F6:
            wxLogMessage(_T("IsModified() before SetValue(): %d"),
                         IsModified());
            SetValue(_T("SetValue() has been called"));
            wxLogMessage(_T("IsModified() after SetValue(): %d"),
                         IsModified());
            break;

        case WXK_F7:
            wxLogMessage(_T("Position 10 should be now visible."));
            ShowPosition(10);
            break;

        case WXK_F8:
            wxLogMessage(_T("Control has been cleared"));
            Clear();
            break;

        case WXK_F9:
            WriteText(_T("WriteText() has been called"));
            break;

        case WXK_F10:
            AppendText(_T("AppendText() has been called"));
            break;
    }

    if ( ms_logKey )
        LogKeyEvent( wxT("Key down"), event);

    event.Skip();
}

//----------------------------------------------------------------------
// MyPanel
//----------------------------------------------------------------------

BEGIN_EVENT_TABLE(MyPanel, wxPanel)
    EVT_SIZE(MyPanel::OnSize)
END_EVENT_TABLE()

MyPanel::MyPanel( wxFrame *frame, int x, int y, int w, int h )
       : wxPanel( frame, -1, wxPoint(x, y), wxSize(w, h) )
{
    m_log = new wxTextCtrl( this, -1, _T("This is the log window.\n"),
                            wxPoint(5,260), wxSize(630,100),
                            wxTE_MULTILINE | wxTE_READONLY /* | wxTE_RICH */);

    m_logOld = wxLog::SetActiveTarget( new wxLogTextCtrl( m_log ) );

    // single line text controls

    m_text = new MyTextCtrl( this, -1, _T("Single line."),
                             wxPoint(10,10), wxSize(140,-1),
                             wxTE_PROCESS_ENTER);
    m_text->SetForegroundColour(*wxBLUE);
    m_text->SetBackgroundColour(*wxLIGHT_GREY);
    (*m_text) << _T(" Appended.");
    m_text->SetInsertionPoint(0);
    m_text->WriteText( _T("Prepended. ") );

    m_password = new MyTextCtrl( this, -1, _T(""),
      wxPoint(10,50), wxSize(140,-1), wxTE_PASSWORD );

    m_readonly = new MyTextCtrl( this, -1, _T("Read only"),
      wxPoint(10,90), wxSize(140,-1), wxTE_READONLY );

    m_limited = new MyTextCtrl(this, -1, _T("Max 8 ch"),
                              wxPoint(10, 130), wxSize(140, -1));
    m_limited->SetMaxLength(8);

    // multi line text controls

    m_horizontal = new MyTextCtrl( this, -1, _T("Multiline text control with a horizontal scrollbar."),
      wxPoint(10,170), wxSize(140,70), wxTE_MULTILINE | wxHSCROLL );

    // a little hack to use the command line argument for encoding testing
    if ( wxTheApp->argc == 2 )
    {
        switch ( wxTheApp->argv[1][0] )
        {
            case '2':
                m_horizontal->SetFont(wxFont(18, wxSWISS, wxNORMAL, wxNORMAL,
                                             FALSE, _T(""),
                                             wxFONTENCODING_ISO8859_2));
                m_horizontal->SetValue(_T("�lu�ou�k� k�� zb�sile �e�tina ��"));
                break;

            case '1':
                m_horizontal->SetFont(wxFont(18, wxSWISS, wxNORMAL, wxNORMAL,
                                             FALSE, _T(""),
                                             wxFONTENCODING_CP1251));
                m_horizontal->SetValue(_T("������!"));
                break;

            case '8':
                m_horizontal->SetFont(wxFont(18, wxSWISS, wxNORMAL, wxNORMAL,
                                             FALSE, _T(""),
                                             wxFONTENCODING_CP1251));
#if wxUSE_UNICODE
                m_horizontal->SetValue(L"\x0412\x0430\x0434\x0438\x043c \x0426");
#else
                m_horizontal->SetValue("������� �������");
#endif
        }
    }
    else
    {
        m_horizontal->SetValue(_T("Text in default encoding"));
    }

    m_multitext = new MyTextCtrl( this, -1, _T("Multi line."),
      wxPoint(180,10), wxSize(240,70), wxTE_MULTILINE );
    m_multitext->SetFont(*wxITALIC_FONT);
    (*m_multitext) << _T(" Appended.");
    m_multitext->SetInsertionPoint(0);
    m_multitext->WriteText( _T("Prepended. ") );
    m_multitext->SetForegroundColour(*wxRED);
    m_multitext->SetBackgroundColour(*wxLIGHT_GREY);

#if wxUSE_TOOLTIPS
    m_multitext->SetToolTip(_T("Press Fn function keys here"));
#endif

    m_tab = new MyTextCtrl( this, 100, _T("Multiline, allow <TAB> processing."),
      wxPoint(180,90), wxSize(240,70), wxTE_MULTILINE |  wxTE_PROCESS_TAB );
    m_tab->SetClientData((void *)_T("tab"));

    m_enter = new MyTextCtrl( this, 100, _T("Multiline, allow <ENTER> processing."),
      wxPoint(180,170), wxSize(240,70), wxTE_MULTILINE);
    m_enter->SetClientData((void *)_T("enter"));

    m_textrich = new MyTextCtrl(this, -1, _T("Allows more than 30Kb of text\n")
                                _T("(even under broken Win9x)\n")
                                _T("and a very very very very very ")
                                _T("very very very long line to test ")
                                _T("wxHSCROLL style"),
                                wxPoint(450, 10), wxSize(230, 230),
                                wxTE_RICH |
                                wxTE_MULTILINE |
                                // wxTE_AUTO_URL |
                                wxHSCROLL);

    m_textrich->SetStyle(0, 10, *wxRED);
    m_textrich->SetStyle(10, 20, *wxBLUE);
    m_textrich->SetStyle(30, 40,
                         wxTextAttr(*wxGREEN, wxNullColour, *wxITALIC_FONT));
    m_textrich->SetDefaultStyle(wxTextAttr());
    m_textrich->AppendText(_T("\n\nFirst 10 characters should be in red\n"));
    m_textrich->AppendText(_T("Next 10 characters should be in blue\n"));
    m_textrich->AppendText(_T("Next 10 characters should be normal\n"));
    m_textrich->AppendText(_T("And the next 10 characters should be green and italic\n"));
    m_textrich->SetDefaultStyle(wxTextAttr(*wxCYAN, *wxBLUE));
    m_textrich->AppendText(_T("This text should be cyan on blue\n"));
    m_textrich->SetDefaultStyle(wxTextAttr(*wxBLUE, *wxWHITE));
    m_textrich->AppendText(_T("And this should be in blue and the text you ")
                           _T("type should be in blue as well"));
}

void MyPanel::OnSize( wxSizeEvent &event )
{
    wxSize client_area( GetClientSize() );
    m_log->SetSize( 0, 260, client_area.x, client_area.y - 260 );
    event.Skip();
}

wxTextCtrl *MyPanel::GetFocusedText(wxTextCtrl *textDef)
{
    wxWindow *win = FindFocus();

    wxTextCtrl *text = win ? wxDynamicCast(win, wxTextCtrl) : NULL;
    return text ? text : textDef;
}

#if wxUSE_CLIPBOARD
void MyPanel::DoPasteFromClipboard()
{
    // On X11, we want to get the data from the primary selection instead
    // of the normal clipboard (which isn't normal under X11 at all). This
    // call has no effect under MSW.
    wxTheClipboard->UsePrimarySelection();

    if (!wxTheClipboard->Open())
    {
        *m_log << _T("Error opening the clipboard.\n");
        return;
    }
    else
    {
        *m_log << _T("Successfully opened the clipboard.\n");
    }

    wxTextDataObject data;

    if (wxTheClipboard->IsSupported( data.GetFormat() ))
    {
        *m_log << _T("Clipboard supports requested format.\n");

        if (wxTheClipboard->GetData( data ))
        {
            *m_log << _T("Successfully retrieved data from the clipboard.\n");
            *m_multitext << data.GetText() << _T("\n");
        }
        else
        {
            *m_log << _T("Error getting data from the clipboard.\n");
        }
    }
    else
    {
        *m_log << _T("Clipboard doesn't support requested format.\n");
    }

    wxTheClipboard->Close();

    *m_log << _T("Closed the clipboard.\n");
}

void MyPanel::DoCopyToClipboard()
{
    // On X11, we want to get the data from the primary selection instead
    // of the normal clipboard (which isn't normal under X11 at all). This
    // call has no effect under MSW.
    wxTheClipboard->UsePrimarySelection();

    wxString text( m_multitext->GetLineText(0) );

    if (text.IsEmpty())
    {
        *m_log << _T("No text to copy.\n");

        return;
    }

    if (!wxTheClipboard->Open())
    {
        *m_log << _T("Error opening the clipboard.\n");

        return;
    }
    else
    {
        *m_log << _T("Successfully opened the clipboard.\n");
    }

    wxTextDataObject *data = new wxTextDataObject( text );

    if (!wxTheClipboard->SetData( data ))
    {
        *m_log << _T("Error while copying to the clipboard.\n");
    }
    else
    {
        *m_log << _T("Successfully copied data to the clipboard.\n");
    }

    wxTheClipboard->Close();

    *m_log << _T("Closed the clipboard.\n");
}

#endif // wxUSE_CLIPBOARD

void MyPanel::DoMoveToEndOfText()
{
    m_multitext->SetInsertionPointEnd();
    m_multitext->SetFocus();
}

void MyPanel::DoMoveToEndOfEntry()
{
    m_text->SetInsertionPointEnd();
    m_text->SetFocus();
}

void MyPanel::DoRemoveText()
{
    GetFocusedText(m_multitext)->Remove(0, 10);
}

//----------------------------------------------------------------------
// MyFrame
//----------------------------------------------------------------------

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(TEXT_QUIT,   MyFrame::OnQuit)
    EVT_MENU(TEXT_ABOUT,  MyFrame::OnAbout)
    EVT_MENU(TEXT_SAVE,   MyFrame::OnFileSave)
    EVT_MENU(TEXT_LOAD,   MyFrame::OnFileLoad)

    EVT_MENU(TEXT_LOG_KEY,  MyFrame::OnLogKey)
    EVT_MENU(TEXT_LOG_CHAR, MyFrame::OnLogChar)
    EVT_MENU(TEXT_LOG_MOUSE,MyFrame::OnLogMouse)
    EVT_MENU(TEXT_LOG_TEXT, MyFrame::OnLogText)
    EVT_MENU(TEXT_LOG_FOCUS,MyFrame::OnLogFocus)
    EVT_MENU(TEXT_CLEAR,    MyFrame::OnLogClear)

#if wxUSE_TOOLTIPS
    EVT_MENU(TEXT_TOOLTIPS_SETDELAY,  MyFrame::OnSetTooltipDelay)
    EVT_MENU(TEXT_TOOLTIPS_ENABLE,    MyFrame::OnToggleTooltips)
#endif // wxUSE_TOOLTIPS

#if wxUSE_CLIPBOARD
    EVT_MENU(TEXT_CLIPBOARD_PASTE,    MyFrame::OnPasteFromClipboard)
    EVT_MENU(TEXT_CLIPBOARD_COPY,     MyFrame::OnCopyToClipboard)
#endif // wxUSE_CLIPBOARD

    EVT_MENU(TEXT_REMOVE,             MyFrame::OnRemoveText)
    EVT_MENU(TEXT_ADD_SOME,           MyFrame::OnAddText)
    EVT_MENU(TEXT_ADD_FREEZE,         MyFrame::OnAddTextFreeze)
    EVT_MENU(TEXT_MOVE_ENDTEXT,       MyFrame::OnMoveToEndOfText)
    EVT_MENU(TEXT_MOVE_ENDENTRY,      MyFrame::OnMoveToEndOfEntry)

    EVT_MENU(TEXT_SET_EDITABLE,       MyFrame::OnSetEditable)
    EVT_MENU(TEXT_SET_ENABLED,        MyFrame::OnSetEnabled)

    EVT_MENU(TEXT_LINE_DOWN,          MyFrame::OnScrollLineDown)
    EVT_MENU(TEXT_LINE_UP,            MyFrame::OnScrollLineUp)
    EVT_MENU(TEXT_PAGE_DOWN,          MyFrame::OnScrollPageDown)
    EVT_MENU(TEXT_PAGE_UP,            MyFrame::OnScrollPageUp)

    EVT_MENU(TEXT_SET,                MyFrame::OnSetText)

    EVT_IDLE(MyFrame::OnIdle)
END_EVENT_TABLE()

MyFrame::MyFrame(wxFrame *frame, const wxChar *title, int x, int y, int w, int h)
       : wxFrame(frame, -1, title, wxPoint(x, y), wxSize(w, h) )
{
    CreateStatusBar(2);

    m_panel = new MyPanel( this, 10, 10, 300, 100 );
}

void MyFrame::OnQuit (wxCommandEvent& WXUNUSED(event) )
{
    Close(TRUE);
}

void MyFrame::OnAbout( wxCommandEvent& WXUNUSED(event) )
{
    wxBeginBusyCursor();

    wxMessageDialog dialog(this,
      _T("This is a text control sample. It demonstrates the many different\n")
      _T("text control styles, the use of the clipboard, setting and handling\n")
      _T("tooltips and intercepting key and char events.\n")
      _T("\n")
        _T("Copyright (c) 1999, Robert Roebling, Julian Smart, Vadim Zeitlin"),
        _T("About wxTextCtrl Sample"),
        wxOK | wxICON_INFORMATION);

    dialog.ShowModal();

    wxEndBusyCursor();
}

#if wxUSE_TOOLTIPS
void MyFrame::OnSetTooltipDelay(wxCommandEvent& event)
{
    static long s_delay = 5000;

    wxString delay;
    delay.Printf( _T("%ld"), s_delay);

    delay = wxGetTextFromUser(_T("Enter delay (in milliseconds)"),
                              _T("Set tooltip delay"),
                              delay,
                              this);
    if ( !delay )
        return; // cancelled

    wxSscanf(delay, _T("%ld"), &s_delay);

    wxToolTip::SetDelay(s_delay);

    wxLogStatus(this, _T("Tooltip delay set to %ld milliseconds"), s_delay);
}

void MyFrame::OnToggleTooltips(wxCommandEvent& event)
{
    static bool s_enabled = TRUE;

    s_enabled = !s_enabled;

    wxToolTip::Enable(s_enabled);

    wxLogStatus(this, _T("Tooltips %sabled"), s_enabled ? _T("en") : _T("dis") );
}
#endif // tooltips

void MyFrame::OnLogClear(wxCommandEvent& WXUNUSED(event))
{
    m_panel->m_log->Clear();
}

void MyFrame::OnSetEditable(wxCommandEvent& WXUNUSED(event))
{
    static bool s_editable = TRUE;

    s_editable = !s_editable;
    m_panel->m_text->SetEditable(s_editable);
    m_panel->m_password->SetEditable(s_editable);
    m_panel->m_multitext->SetEditable(s_editable);
    m_panel->m_textrich->SetEditable(s_editable);
}

void MyFrame::OnSetEnabled(wxCommandEvent& WXUNUSED(event))
{
    bool enabled = m_panel->m_text->IsEnabled();
    enabled = !enabled;

    m_panel->m_text->Enable(enabled);
    m_panel->m_password->Enable(enabled);
    m_panel->m_multitext->Enable(enabled);
    m_panel->m_readonly->Enable(enabled);
    m_panel->m_limited->Enable(enabled);
    m_panel->m_textrich->Enable(enabled);
}

void MyFrame::OnFileSave(wxCommandEvent& event)
{
    if ( m_panel->m_textrich->SaveFile(_T("dummy.txt")) )
    {
#if wxUSE_FILE
        // verify that the fil length is correct (it wasn't under Win95)
        wxFile file(wxT("dummy.txt"));
        wxLogStatus(this,
                    _T("Successfully saved file (text len = %lu, file size = %ld)"),
                    (unsigned long)m_panel->m_textrich->GetValue().length(),
                    (long) file.Length());
#endif
    }
    else
        wxLogStatus(this, _T("Couldn't save the file"));
}

void MyFrame::OnFileLoad(wxCommandEvent& event)
{
    if ( m_panel->m_textrich->LoadFile(_T("dummy.txt")) )
        wxLogStatus(this, _T("Successfully loaded file"));
    else
        wxLogStatus(this, _T("Couldn't load the file"));
}

void MyFrame::OnIdle( wxIdleEvent& event )
{
    // track the window which has the focus in the status bar
    static wxWindow *s_windowFocus = (wxWindow *)NULL;
    wxWindow *focus = wxWindow::FindFocus();
    if ( focus && (focus != s_windowFocus) )
    {
        s_windowFocus = focus;

        wxString msg;
        msg.Printf(
#ifdef __WXMSW__
                _T("Focus: wxWindow = %p, HWND = %08x"),
#else
                _T("Focus: wxWindow = %p"),
#endif
                s_windowFocus
#ifdef __WXMSW__
                , (unsigned int) s_windowFocus->GetHWND()
#endif
                  );

        SetStatusText(msg);
    }
    event.Skip();
}
