/////////////////////////////////////////////////////////////////////////////
// Name:        accel.h
// Purpose:     wxAcceleratorTable class
// Author:      David Webster
// Modified by:
// Created:     10/13/99
// RCS-ID:      $Id: ACCEL.H,v 1.7 2002/05/28 16:51:14 VZ Exp $
// Copyright:   (c) David Webster
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_ACCEL_H_
#define _WX_ACCEL_H_

#include "wx/object.h"

class WXDLLEXPORT wxAcceleratorTable;

// Hold Ctrl key down
#define wxACCEL_ALT     0x01

// Hold Ctrl key down
#define wxACCEL_CTRL    0x02

 // Hold Shift key down
#define wxACCEL_SHIFT   0x04

 // Hold no key down
#define wxACCEL_NORMAL  0x00

class WXDLLEXPORT wxAcceleratorTable: public wxObject
{
DECLARE_DYNAMIC_CLASS(wxAcceleratorTable)
public:
    wxAcceleratorTable();
    wxAcceleratorTable(const wxString& rsResource); // Load from .rc resource
    wxAcceleratorTable( int                n
                       ,const wxAcceleratorEntry vaEntries[]
                      ); // Load from array

    // Copy constructors
    inline wxAcceleratorTable(const wxAcceleratorTable& rAccel) { Ref(rAccel); }
    inline wxAcceleratorTable(const wxAcceleratorTable* pAccel) { if (pAccel) Ref(*pAccel); }

    ~wxAcceleratorTable();

    inline wxAcceleratorTable& operator = (const wxAcceleratorTable& rAccel)
    { if (*this == rAccel) return (*this); Ref(rAccel); return *this; };
    inline bool operator == (const wxAcceleratorTable& rAccel)
    { return m_refData == rAccel.m_refData; };
    inline bool operator != (const wxAcceleratorTable& rAccel)
    { return m_refData != rAccel.m_refData; };

    bool Ok() const;
    void SetHACCEL(WXHACCEL hAccel);
    WXHACCEL GetHACCEL(void) const;

    // translate the accelerator, return TRUE if done
    bool Translate( WXHWND hWnd
                   ,WXMSG* pMsg
                  ) const;
};

WXDLLEXPORT_DATA(extern wxAcceleratorTable) wxNullAcceleratorTable;

#endif
    // _WX_ACCEL_H_
