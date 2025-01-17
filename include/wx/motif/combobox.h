/////////////////////////////////////////////////////////////////////////////
// Name:        combobox.h
// Purpose:     wxComboBox class
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id: combobox.h,v 1.8 2002/02/05 16:34:30 JS Exp $
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_COMBOBOX_H_
#define _WX_COMBOBOX_H_

#ifdef __GNUG__
#pragma interface "combobox.h"
#endif

#include "wx/choice.h"

WXDLLEXPORT_DATA(extern const char*) wxComboBoxNameStr;
WXDLLEXPORT_DATA(extern const char*) wxEmptyString;

// Combobox item
class WXDLLEXPORT wxComboBox: public wxChoice
{
    DECLARE_DYNAMIC_CLASS(wxComboBox)
        
public:
    inline wxComboBox() {}
    ~wxComboBox();
    
    inline wxComboBox(wxWindow *parent, wxWindowID id,
        const wxString& value = wxEmptyString,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        int n = 0, const wxString choices[] = NULL,
        long style = 0,
        const wxValidator& validator = wxDefaultValidator,
        const wxString& name = wxComboBoxNameStr)
    {
        Create(parent, id, value, pos, size, n, choices, style, validator, name);
    }
    
    bool Create(wxWindow *parent, wxWindowID id,
        const wxString& value = wxEmptyString,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        int n = 0, const wxString choices[] = NULL,
        long style = 0,
        const wxValidator& validator = wxDefaultValidator,
        const wxString& name = wxComboBoxNameStr);
    
    // List functions
    virtual void Append(const wxString& item);
    virtual void Delete(int n);
    virtual void Clear();
    virtual int GetSelection() const ;
    virtual void SetSelection(int n);
    virtual int FindString(const wxString& s) const;
    virtual wxString GetString(int n) const ;
    virtual wxString GetStringSelection() const ;
    virtual bool SetStringSelection(const wxString& sel);
    virtual inline int Number() const { return m_noStrings; }
    
    // Text field functions
    virtual wxString GetValue() const ;
    virtual void SetValue(const wxString& value);
    
    // Clipboard operations
    virtual void Copy();
    virtual void Cut();
    virtual void Paste();
    virtual void SetInsertionPoint(long pos);
    virtual void SetInsertionPointEnd();
    virtual long GetInsertionPoint() const ;
    virtual long GetLastPosition() const ;
    virtual void Replace(long from, long to, const wxString& value);
    virtual void Remove(long from, long to);
    virtual void SetSelection(long from, long to);
    virtual void SetEditable(bool editable);
    
    // Implementation
    virtual void ChangeFont(bool keepOriginalSize = TRUE);
    virtual void ChangeBackgroundColour();
    virtual void ChangeForegroundColour();
    WXWidget GetTopWidget() const { return m_mainWidget; }
    WXWidget GetMainWidget() const { return m_mainWidget; }
    
protected:
    virtual void DoSetSize(int x, int y,
        int width, int height,
        int sizeFlags = wxSIZE_AUTO);
};

#endif
// _WX_COMBOBOX_H_
