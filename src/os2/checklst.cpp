///////////////////////////////////////////////////////////////////////////////
// Name:        checklst.cpp
// Purpose:     implementation of wxCheckListBox class
// Author:      David Webster
// Modified by:
// Created:     10/13/99
// RCS-ID:      $Id: CHECKLST.CPP,v 1.12.2.1 2002/12/27 14:49:34 JS Exp $
// Copyright:   (c) David Webster
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// headers & declarations
// ============================================================================

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_OWNER_DRAWN

#include "wx/object.h"
#include "wx/colour.h"
#include "wx/font.h"
#include "wx/bitmap.h"
#include "wx/window.h"
#include "wx/listbox.h"
#include "wx/ownerdrw.h"
#include "wx/settings.h"
#include "wx/dcmemory.h"
#include "wx/dcscreen.h"
#include "wx/os2/checklst.h"
#include "wx/log.h"

#define INCL_PM
#include <os2.h>

// ----------------------------------------------------------------------------
// private functions
// ----------------------------------------------------------------------------

// get item (converted to right type)
#define GetItem(n)    ((wxCheckListBoxItem *)(GetItem(n)))

// ============================================================================
// implementation
// ============================================================================

IMPLEMENT_DYNAMIC_CLASS(wxCheckListBox, wxListBox)

// ----------------------------------------------------------------------------
// declaration and implementation of wxCheckListBoxItem class
// ----------------------------------------------------------------------------

class wxCheckListBoxItem : public wxOwnerDrawn
{
    friend class wxCheckListBox;
public:
    //
    // ctor
    //
    wxCheckListBoxItem( wxCheckListBox* pParent
                       ,size_t          nIndex
                      );

    //
    // Drawing functions
    //
    virtual bool OnDrawItem( wxDC&         rDc
                            ,const wxRect& rRect
                            ,wxODAction    eAct
                            ,wxODStatus    eStat
                           );

    //
    // Simple accessors
    //
    bool IsChecked(void) const  { return m_bChecked; }
    void Check(bool bCheck);
    void Toggle(void) { Check(!IsChecked()); }

private:
    bool                            m_bChecked;
    wxCheckListBox*                 m_pParent;
    size_t                          m_nIndex;
}; // end of CLASS wxCheckListBoxItem

wxCheckListBoxItem::wxCheckListBoxItem (
  wxCheckListBox*                   pParent
, size_t                            nIndex
)
: wxOwnerDrawn( ""
               ,TRUE // checkable
              )
{
    m_bChecked = FALSE;
    m_pParent  = pParent;
    m_nIndex   = nIndex;

    //
    // We don't initialize m_nCheckHeight/Width vars because it's
    // done in OnMeasure while they are used only in OnDraw and we
    // know that there will always be OnMeasure before OnDraw
    //
    SetMarginWidth(GetDefaultMarginWidth());
} // end of wxCheckListBoxItem::wxCheckListBoxItem

bool wxCheckListBoxItem::OnDrawItem (
  wxDC&                             rDc
, const wxRect&                     rRect
, wxODAction                        eAct
, wxODStatus                        eStat
)
{
    wxRect                          vRect = rRect;

    ::WinQueryWindowRect( m_pParent->GetHWND()
                         ,&rDc.m_vRclPaint
                        );
    if (IsChecked())
        eStat = (wxOwnerDrawn::wxODStatus)(eStat | wxOwnerDrawn::wxODChecked);

    //
    // Unfortunately PM doesn't quite get the text position exact.  We need to alter
    // it down and to the right, just a little bit.  The coords in rRect are OS/2
    // coords not wxWindows coords.
    //
    vRect.x += 5;
    vRect.y -= 3;
    if (wxOwnerDrawn::OnDrawItem( rDc
                                 ,vRect
                                 ,eAct
                                 ,eStat))
    {
        size_t                      nCheckWidth  = GetDefaultMarginWidth();
        size_t                      nCheckHeight = m_pParent->GetItemHeight();
        int                         nParentHeight;
        int                         nX = rRect.GetX();
        int                         nY = rRect.GetY();
        int                         nOldY = nY;
        wxColour                    vColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
        wxPen                       vPenBack;
        wxPen                       vPenGray;
        wxPen                       vPenPrev;

        m_pParent->GetSize( NULL
                           ,&nParentHeight
                          );

        nY = nParentHeight - nY - nCheckHeight;
        vPenBack = wxPen(vColour, 1, wxSOLID);
        vPenGray = wxPen(wxColour(127, 127, 127), 1, wxSOLID);

        //
        // Erase the 1-pixel border
        //
        rDc.SetPen(vPenBack);
        rDc.DrawRectangle( nX
                          ,nY
                          ,nCheckWidth
                          ,nCheckHeight
                         );

        //
        // Now we draw the smaller rectangle
        //
        nY++;
        nCheckWidth  -= 2;
        nCheckHeight -= 2;

        //
        // Draw hollow gray rectangle
        //
        rDc.SetPen(vPenGray);
        rDc.DrawRectangle( nX
                          ,nY
                          ,nCheckWidth
                          ,nCheckHeight
                         );

        nX++;
        if (IsChecked())
        {
            //
            // Draw the check by loading the sys standard bitmap and drawing it
            //
            HBITMAP                 hChkBmp = ::WinGetSysBitmap( HWND_DESKTOP
                                                                ,SBMP_MENUCHECK
                                                               );
            POINTL                  vPoint = {nX, nOldY + 3};

            ::WinDrawBitmap( rDc.GetHPS()
                            ,hChkBmp
                            ,NULL
                            ,&vPoint
                            ,NULL
                            ,NULL
                            ,DBM_NORMAL
                           );
        }
        return TRUE;
    }
    return FALSE;
} // end of wxCheckListBoxItem::OnDrawItem

//
// Change the state of the item and redraw it
//
void wxCheckListBoxItem::Check (
  bool                              bCheck
)
{
    m_bChecked = bCheck;

    //
    // Index may be chanegd because new items were added/deleted
    //
    if (m_pParent->GetItemIndex(this) != (int)m_nIndex)
    {
        //
        // Update it
        //
        int                         nIndex = m_pParent->GetItemIndex(this);

        wxASSERT_MSG(nIndex != wxNOT_FOUND, wxT("what does this item do here?"));

        m_nIndex = (size_t)nIndex;
    }

    HWND                            hWndListbox = (HWND)m_pParent->GetHWND();
    RECTL                           rUpdate;
    MRESULT                         mRc;

    wxCommandEvent                  vEvent( wxEVT_COMMAND_CHECKLISTBOX_TOGGLED
                                           ,m_pParent->GetId()
                                          );

    vEvent.SetInt(m_nIndex);
    vEvent.SetEventObject(m_pParent);
    m_pParent->ProcessCommand(vEvent);
} // end of wxCheckListBoxItem::Check

// ----------------------------------------------------------------------------
// implementation of wxCheckListBox class
// ----------------------------------------------------------------------------

// define event table
// ------------------
BEGIN_EVENT_TABLE(wxCheckListBox, wxListBox)
  EVT_CHAR(wxCheckListBox::OnChar)
  EVT_LEFT_DOWN(wxCheckListBox::OnLeftClick)
END_EVENT_TABLE()

//
// Control creation
// ----------------
//

//
// Default ctor: use Create() to really create the control
//
wxCheckListBox::wxCheckListBox()
: wxListBox()
{
} // end of wxCheckListBox::wxCheckListBox

//
// Ctor which creates the associated control
//
wxCheckListBox::wxCheckListBox (
  wxWindow*                         pParent
, wxWindowID                        vId
, const wxPoint&                    rPos
, const wxSize&                     rSize
, int                               nStrings
, const wxString                    asChoices[]
, long                              lStyle
, const wxValidator&                rVal
, const wxString&                   rsName
)
              : wxListBox()
{
    Create( pParent
           ,vId
           ,rPos
           ,rSize
           ,nStrings
           ,asChoices
           ,lStyle | wxLB_OWNERDRAW
           ,rVal
           ,rsName
          );
} // end of wxCheckListBox::wxCheckListBox

void wxCheckListBox::Delete(
  int                               N
)
{
    wxCHECK_RET( N >= 0 && N < m_nNumItems,
                 wxT("invalid index in wxListBox::Delete") );
    wxListBox::Delete(N);

    //
    // Free memory
    //
    delete m_aItems[N];
    m_aItems.RemoveAt(N);
} // end of wxCheckListBox::Delete

void wxCheckListBox::InsertItems (
  int                               nItems
, const wxString                    asItems[]
, int                               nPos
)
{
    int                             i;

    wxCHECK_RET( nPos >= 0 && nPos <= m_nNumItems,
                 wxT("invalid index in wxCheckListBox::InsertItems") );

    wxListBox::InsertItems( nItems
                           ,asItems
                           ,nPos
                          );
    for (i = 0; i < nItems; i++)
    {
        wxOwnerDrawn*               pNewItem = CreateItem((size_t)(nPos + i));

        pNewItem->SetName(asItems[i]);
        m_aItems.Insert(pNewItem, (size_t)(nPos + i));
        ::WinSendMsg( (HWND)GetHWND()
                     ,LM_SETITEMHANDLE
                     ,(MPARAM)(i + nPos)
                     ,MPFROMP(pNewItem)
                    );
    }
} // end of wxCheckListBox::InsertItems

bool wxCheckListBox::SetFont (
  const wxFont&                     rFont
)
{
    size_t                          i;

    for (i = 0; i < m_aItems.GetCount(); i++)
        m_aItems[i]->SetFont(rFont);
    wxListBox::SetFont(rFont);
    return TRUE;
} // end of wxCheckListBox::SetFont

//
// Create/retrieve item
// --------------------
//

//
// Create a check list box item
//
wxOwnerDrawn* wxCheckListBox::CreateItem (
  size_t                            nIndex
)
{
    wxCheckListBoxItem*             pItem = new wxCheckListBoxItem( this
                                                                   ,nIndex
                                                                  );
    return pItem;
} // end of wxCheckListBox::CreateItem

//
// Return item size
// ----------------
//
long wxCheckListBox::OS2OnMeasure (
  WXMEASUREITEMSTRUCT*             pItem
)
{
    if (!pItem)
        pItem = (WXMEASUREITEMSTRUCT*)new OWNERITEM;
    if (wxListBox::OS2OnMeasure(pItem) )
    {
        POWNERITEM                  pStruct = (POWNERITEM)pItem;

        //
        // Save item height
        //
        m_nItemHeight = pStruct->rclItem.yTop - pStruct->rclItem.yBottom;

        //
        // Add place for the check mark
        //
        pStruct->rclItem.xRight += wxOwnerDrawn::GetDefaultMarginWidth();
        return long(MRFROM2SHORT((USHORT)m_nItemHeight, (USHORT)(pStruct->rclItem.xRight - pStruct->rclItem.xLeft)));
    }
    return 0L;
} // end of wxCheckListBox::CreateItem

//
// Check items
// -----------
//
bool wxCheckListBox::IsChecked (
  size_t                            uiIndex
) const
{
    return GetItem(uiIndex)->IsChecked();
} // end of wxCheckListBox::IsChecked

void wxCheckListBox::Check (
  size_t                            uiIndex
, bool                              bCheck
)
{
    GetItem(uiIndex)->Check(bCheck);
} // end of wxCheckListBox::Check

//
// Process events
// --------------
//
void wxCheckListBox::OnChar (
  wxKeyEvent&                       rEvent
)
{
    if (rEvent.KeyCode() == WXK_SPACE)
        GetItem(GetSelection())->Toggle();
    else
        rEvent.Skip();
} // end of wxCheckListBox::OnChar

void wxCheckListBox::OnLeftClick (
  wxMouseEvent&                     rEvent
)
{
    //
    // Clicking on the item selects it, clicking on the checkmark toggles
    //
    if (rEvent.GetX() <= wxOwnerDrawn::GetDefaultMarginWidth())
    {
        int                         nParentHeight;
        wxScreenDC                  vDc;
        wxCoord                     vHeight;

        GetSize( NULL
                ,&nParentHeight
               );
        vDc.SetFont(GetFont());
        vHeight = vDc.GetCharHeight() * 2.5;

        //
        // This, of course, will not work if the LB is scrolled
        //
        int                         nY = rEvent.GetY();

        nY = nParentHeight - (nY + vHeight);

        size_t                      nItem = (size_t)(nY / vHeight);

        if (nItem < (size_t)m_nNumItems)
            GetItem(nItem)->Toggle();
        //
        // else: it's not an error, just click outside of client zone
        //
    }
    else
    {
        //
        // Implement default behaviour: clicking on the item selects it
        //
        rEvent.Skip();
    }
} // end of wxCheckListBox::OnLeftClick

#endif

