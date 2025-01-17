///////////////////////////////////////////////////////////////////////////////
// Name:        checklst.cpp
// Purpose:     implementation of wxCheckListBox class
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id: checklst.cpp,v 1.9.2.1 2002/11/10 18:26:04 MBN Exp $
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// headers & declarations
// ============================================================================

#ifdef __GNUG__
#pragma implementation "checklst.h"
#endif

#include "wx/defs.h"

#include "wx/checklst.h"

// ============================================================================
// implementation
// ============================================================================

IMPLEMENT_DYNAMIC_CLASS(wxCheckListBox, wxListBox)

// ----------------------------------------------------------------------------
// implementation of wxCheckListBox class
// ----------------------------------------------------------------------------

// define event table
// ------------------
BEGIN_EVENT_TABLE(wxCheckListBox, wxListBox)
END_EVENT_TABLE()

// control creation
// ----------------

static const wxString prefixChecked = "[x] ";
static const wxString prefixUnchecked = "[ ] ";
static const char checkChar = 'x', uncheckChar = ' ';

static inline const wxString& Prefix(bool checked)
    { return checked ? prefixChecked : prefixUnchecked; }
static inline bool IsChecked(const wxString& s)
    { wxASSERT(s.length() >=4); return s[1] == checkChar; }

static void CopyStringsAddingPrefix(const wxArrayString& orig,
                                    wxArrayString& copy)
{
    copy.Clear();

    for(size_t i = 0; i < orig.GetCount(); ++i )
        copy.Add( Prefix(FALSE) + orig[i] );
}

// def ctor: use Create() to really create the control
wxCheckListBox::wxCheckListBox() : wxCheckListBoxBase()
{
}

// ctor which creates the associated control
wxCheckListBox::wxCheckListBox(wxWindow *parent, wxWindowID id,
                               const wxPoint& pos, const wxSize& size,
                               int nStrings, const wxString choices[],
                               long style, const wxValidator& val,
                               const wxString& name)
                               : wxCheckListBoxBase()
{
    Create(parent, id, pos, size, nStrings, choices,
           style, val, name);
}

bool wxCheckListBox::Create(wxWindow *parent, wxWindowID id,
                            const wxPoint& pos,
                            const wxSize& size,
                            int n, const wxString choices[],
                            long style,
                            const wxValidator& validator,
                            const wxString& name)
{
    // wxListBox::Create calls set, which adds the prefixes
    bool retVal = wxListBox::Create(parent, id, pos, size, n, choices,
                                    style, validator, name);
    return retVal;
}   

// check items
// -----------

bool wxCheckListBox::IsChecked(size_t uiIndex) const
{
    return ::IsChecked(wxListBox::GetString(uiIndex));
}

void wxCheckListBox::Check(size_t uiIndex, bool bCheck)
{
    wxString label = wxListBox::GetString(uiIndex);
    if(::IsChecked(label) == bCheck) return;
    label[1u] = bCheck ? checkChar : uncheckChar;
    wxListBox::SetString(uiIndex, label);
}

void wxCheckListBox::DoToggleItem( int n, int x )
{
    if( x < 23 )
    {
        wxString label = wxListBox::GetString(n);
        label[1u] = (!::IsChecked(label)) ? checkChar : uncheckChar;
        wxListBox::SetString(n, label);

        wxCommandEvent event(wxEVT_COMMAND_CHECKLISTBOX_TOGGLED, GetId());
        if( HasClientObjectData() )
            event.SetClientObject( GetClientObject(n) );
        else if( HasClientUntypedData() )
            event.SetClientData( GetClientData(n) );
        event.m_commandInt = n;
        event.m_extraLong = TRUE;
        event.SetEventObject(this);
        event.SetString( GetString( n ) );

        GetEventHandler()->ProcessEvent(event);
    }
}

int wxCheckListBox::DoAppend(const wxString& item)
{
    return wxListBox::DoAppend( Prefix(FALSE) + item );
}

int wxCheckListBox::FindString(const wxString& s) const
{
    int n1 = wxListBox::FindString(Prefix(FALSE) + s);
    int n2 = wxListBox::FindString(Prefix(TRUE) + s);
    int min = wxMin(n1, n2), max = wxMax(n1, n2);

    // why this works:
    // n1 == -1, n2 == -1 => return -1 OK
    // n1 != -1 || n2 != -1 => min == -1 => return the other one
    // both != -1 => return the first one.
    if( min == -1 ) return max;
    return min;
}

void wxCheckListBox::SetString(int n, const wxString& s)
{
    wxListBox::SetString( n, Prefix(IsChecked(n)) + s );
}

wxString wxCheckListBox::GetString(int n) const
{
    return wxListBox::GetString(n).substr(4);
}

void wxCheckListBox::DoInsertItems(const wxArrayString& items, int pos)
{
    wxArrayString copy;
    CopyStringsAddingPrefix(items, copy);
    wxListBox::DoInsertItems(copy, pos);
}

void wxCheckListBox::DoSetItems(const wxArrayString& items, void **clientData)
{
    wxArrayString copy;
    CopyStringsAddingPrefix(items, copy);
    wxListBox::DoSetItems(copy, clientData);
}
