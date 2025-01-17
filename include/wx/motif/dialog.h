/////////////////////////////////////////////////////////////////////////////
// Name:        dialog.h
// Purpose:     wxDialog class
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id: dialog.h,v 1.14 2002/02/05 16:34:30 JS Exp $
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DIALOG_H_
#define _WX_DIALOG_H_

#ifdef __GNUG__
#pragma interface "dialog.h"
#endif

WXDLLEXPORT_DATA(extern const char*) wxDialogNameStr;

// Dialog boxes
class WXDLLEXPORT wxDialog : public wxDialogBase
{
    DECLARE_DYNAMIC_CLASS(wxDialog)
        
public:
    wxDialog();
    
    // Constructor with a modal flag, but no window id - the old convention
    wxDialog(wxWindow *parent,
        const wxString& title, bool modal,
        int x = -1, int y= -1, int width = 500, int height = 500,
        long style = wxDEFAULT_DIALOG_STYLE,
        const wxString& name = wxDialogNameStr)
    {
        long modalStyle = modal ? wxDIALOG_MODAL : wxDIALOG_MODELESS ;
        Create(parent, -1, title, wxPoint(x, y), wxSize(width, height), style|modalStyle, name);
    }
    
    // Constructor with no modal flag - the new convention.
    wxDialog(wxWindow *parent, wxWindowID id,
        const wxString& title,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxDEFAULT_DIALOG_STYLE,
        const wxString& name = wxDialogNameStr)
    {
        Create(parent, id, title, pos, size, style, name);
    }
    
    bool Create(wxWindow *parent, wxWindowID id,
        const wxString& title, // bool modal = FALSE, // TODO make this a window style?
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxDEFAULT_DIALOG_STYLE,
        const wxString& name = wxDialogNameStr);
    
    ~wxDialog();
    
    virtual bool Destroy();
    
    bool Show(bool show);
    void Iconize(bool iconize);
    void Raise();
    void Lower();
    
    virtual bool IsIconized() const;
    
    virtual bool IsTopLevel() const { return TRUE; }
    
    void SetTitle(const wxString& title);
    wxString GetTitle() const ;
    
    void SetModal(bool flag);
    
    virtual bool IsModal() const
    { return ((GetWindowStyleFlag() & wxDIALOG_MODAL) == wxDIALOG_MODAL); }
    
    virtual int ShowModal();
    virtual void EndModal(int retCode);
    
    // Implementation
    virtual void ChangeFont(bool keepOriginalSize = TRUE);
    virtual void ChangeBackgroundColour();
    virtual void ChangeForegroundColour();
    inline WXWidget GetTopWidget() const { return m_mainWidget; }
    inline WXWidget GetClientWidget() const { return m_mainWidget; }
    
    // Standard buttons
    void OnOK(wxCommandEvent& event);
    void OnApply(wxCommandEvent& event);
    void OnCancel(wxCommandEvent& event);
    
    void OnPaint(wxPaintEvent &event);
    
    // Responds to colour changes
    void OnSysColourChanged(wxSysColourChangedEvent& event);
    
    //  bool OnClose();
    void OnCharHook(wxKeyEvent& event);
    void OnCloseWindow(wxCloseEvent& event);
    
    // Responds to size changes
    void OnSize(wxSizeEvent& event);
    
public:
    //// Motif-specific
    bool          m_modalShowing;
    wxString      m_dialogTitle;
    
protected:
    virtual void DoSetSize(int x, int y,
        int width, int height,
        int sizeFlags = wxSIZE_AUTO);
    
    virtual void DoSetClientSize(int width, int height);
    
private:
    DECLARE_EVENT_TABLE()
};

#endif
// _WX_DIALOG_H_
