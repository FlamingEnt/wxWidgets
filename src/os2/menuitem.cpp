///////////////////////////////////////////////////////////////////////////////
// Name:        menuitem.cpp
// Purpose:     wxMenuItem implementation
// Author:      David Webster
// Modified by:
// Created:     10/10/98
// RCS-ID:      $Id: MENUITEM.CPP,v 1.25.2.1 2002/12/16 18:34:26 DW Exp $
// Copyright:   (c) David Webster
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// headers & declarations
// ============================================================================

#ifdef __GNUG__
    #pragma implementation "menuitem.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
    #include "wx/font.h"
    #include "wx/bitmap.h"
    #include "wx/settings.h"
    #include "wx/font.h"
    #include "wx/window.h"
    #include "wx/accel.h"
    #include "wx/menu.h"
    #include "wx/string.h"
#endif

#include "wx/menuitem.h"
#include "wx/log.h"

#if wxUSE_ACCEL
    #include "wx/accel.h"
#endif // wxUSE_ACCEL

#include "wx/os2/private.h"

// ---------------------------------------------------------------------------
// macro
// ---------------------------------------------------------------------------

// hide the ugly cast
#define GetHMenuOf(menu)    ((HMENU)menu->GetHMenu())

// conditional compilation
#if wxUSE_OWNER_DRAWN
    #define OWNER_DRAWN_ONLY( code ) if ( IsOwnerDrawn() ) code
#else // !wxUSE_OWNER_DRAWN
    #define OWNER_DRAWN_ONLY( code )
#endif // wxUSE_OWNER_DRAWN/!wxUSE_OWNER_DRAWN

// ----------------------------------------------------------------------------
// static function for translating menu labels
// ----------------------------------------------------------------------------

static wxString TextToLabel(
  const wxString&                   rsTitle
)
{
    wxString                        sTitle;
    const wxChar*                   zPc;

    if (rsTitle.IsEmpty())
        return(sTitle);

    for (zPc = rsTitle.c_str(); *zPc != wxT('\0'); zPc++)
    {
        if (*zPc == wxT('&'))
        {
            if (*(zPc + 1) == wxT('&'))
            {
                zPc++;
                sTitle << wxT('&');
            }
            else
                sTitle << wxT('~');
        }
        else
        {
            if ( *zPc == wxT('~'))
            {
                //
                // Tildes must be doubled to prevent them from being
                // interpreted as accelerator character prefix by PM ???
                //
                sTitle << *zPc;
            }
            sTitle << *zPc;
        }
    }
    return(sTitle);
} // end of TextToLabel

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// dynamic classes implementation
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxMenuItem, wxObject)

// ----------------------------------------------------------------------------
// wxMenuItem
// ----------------------------------------------------------------------------

// ctor & dtor
// -----------

wxMenuItem::wxMenuItem(
  wxMenu*                           pParentMenu
, int                               nId
, const wxString&                   rsText
, const wxString&                   rsHelp
, wxItemKind                        eKind
, wxMenu*                           pSubMenu
)
: wxMenuItemBase( pParentMenu
                 ,nId
                 ,TextToLabel(rsText)
                 ,rsHelp
                 ,eKind
                 ,pSubMenu
                )
#if wxUSE_OWNER_DRAWN
,  wxOwnerDrawn( TextToLabel(rsText)
                ,eKind == wxITEM_CHECK
               )
#endif // owner drawn
{
    wxASSERT_MSG(pParentMenu != NULL, wxT("a menu item should have a parent"));
    memset(&m_vMenuData, '\0', sizeof(m_vMenuData));
    m_vMenuData.id = (USHORT)nId;

    Init();
} // end of wxMenuItem::wxMenuItem

wxMenuItem::wxMenuItem(
  wxMenu*                           pParentMenu
, int                               nId
, const wxString&                   rsText
, const wxString&                   rsHelp
, bool                              bIsCheckable
, wxMenu*                           pSubMenu
)
: wxMenuItemBase( pParentMenu
                 ,nId
                 ,TextToLabel(rsText)
                 ,rsHelp
                 ,bIsCheckable ? wxITEM_CHECK : wxITEM_NORMAL
                 ,pSubMenu
                )
#if wxUSE_OWNER_DRAWN
,  wxOwnerDrawn( TextToLabel(rsText)
                ,bIsCheckable
               )
#endif // owner drawn
{
    wxASSERT_MSG(pParentMenu != NULL, wxT("a menu item should have a parent"));
    memset(&m_vMenuData, '\0', sizeof(m_vMenuData));
    m_vMenuData.id = (USHORT)nId;

    Init();
} // end of wxMenuItem::wxMenuItem

void wxMenuItem::Init()
{
    m_vRadioGroup.m_nStart = -1;
    m_bIsRadioGroupStart = FALSE;

#if  wxUSE_OWNER_DRAWN
    //
    // Set default menu colors
    //
    #define SYS_COLOR(c) (wxSystemSettings::GetColour(wxSYS_COLOUR_##c))

    SetTextColour(SYS_COLOR(MENUTEXT));
    SetBackgroundColour(SYS_COLOR(MENU));

    //
    // We don't want normal items be owner-drawn
    //
    ResetOwnerDrawn();
    #undef  SYS_COLOR

    //
    // Tell the owner drawing code to to show the accel string as well
    //
    SetAccelString(m_text.AfterFirst(_T('\t')));
#endif // wxUSE_OWNER_DRAWN
} // end of wxMenuItem::Init

wxMenuItem::~wxMenuItem()
{
} // end of wxMenuItem::~wxMenuItem

//
// Misc
// ----

//
// Return the id for calling Win32 API functions
//
int wxMenuItem::GetRealId() const
{
    return m_subMenu ? (int)m_subMenu->GetHMenu() : GetId();
} // end of wxMenuItem::GetRealId

//
// Get item state
// --------------
bool wxMenuItem::IsChecked() const
{
    USHORT                          uFlag = SHORT1FROMMR(::WinSendMsg( GetHMenuOf(m_parentMenu)
                                                                      ,MM_QUERYITEMATTR
                                                                      ,MPFROM2SHORT(GetId(), TRUE)
                                                                      ,MPFROMSHORT(MIA_CHECKED)
                                                                     ));

    return (uFlag & MIA_CHECKED);
} // end of wxMenuItem::IsChecked

wxString wxMenuItemBase::GetLabelFromText(
  const wxString&                   rsText
)
{
    wxString                        sLabel;

    for (const char* zPc = rsText.c_str(); *zPc; zPc++)
    {
        if (*zPc == wxT('~') || *zPc == wxT('&'))
        {
            //
            // '~' is the escape character for OS/2PM and '&' is the one for
            // wxWindows - skip both of them
            //
            continue;
        }
        sLabel += *zPc;
    }
    return sLabel;
} // end of wxMenuItemBase::GetLabelFromText

//
// Radio group stuff
// -----------------
//
void wxMenuItem::SetAsRadioGroupStart()
{
    m_bIsRadioGroupStart = TRUE;
} // end of wxMenuItem::SetAsRadioGroupStart

void wxMenuItem::SetRadioGroupStart(
  int                               nStart
)
{
    wxASSERT_MSG( !m_bIsRadioGroupStart
                 ,_T("should only be called for the next radio items")
                );

    m_vRadioGroup.m_nStart = nStart;
} // wxMenuItem::SetRadioGroupStart

void wxMenuItem::SetRadioGroupEnd(
  int                               nEnd
)
{
    wxASSERT_MSG( m_bIsRadioGroupStart
                 ,_T("should only be called for the first radio item")
                );
    m_vRadioGroup.m_nEnd = nEnd;
} // end of wxMenuItem::SetRadioGroupEnd

// change item state
// -----------------

void wxMenuItem::Enable(
  bool                              bEnable
)
{
    bool                            bOk;

    if (m_isEnabled == bEnable)
        return;
    if (bEnable)
        bOk = (bool)::WinSendMsg( GetHMenuOf(m_parentMenu)
                                 ,MM_SETITEMATTR
                                 ,MPFROM2SHORT(GetRealId(), TRUE)
                                 ,MPFROM2SHORT(MIA_DISABLED, FALSE)
                                );
    else
        bOk = (bool)::WinSendMsg( GetHMenuOf(m_parentMenu)
                                 ,MM_SETITEMATTR
                                 ,MPFROM2SHORT(GetRealId(), TRUE)
                                 ,MPFROM2SHORT(MIA_DISABLED, MIA_DISABLED)
                                );
    if (!bOk)
    {
        wxLogLastError("EnableMenuItem");
    }
    wxMenuItemBase::Enable(bEnable);
} // end of wxMenuItem::Enable

void wxMenuItem::Check(
  bool                              bCheck
)
{
    bool                            bOk;

    wxCHECK_RET( IsCheckable(), wxT("only checkable items may be checked") );
    if (m_isChecked == bCheck)
        return;

    HMENU                           hMenu = GetHmenuOf(m_parentMenu);

    if (GetKind() == wxITEM_RADIO)
    {
        //
        // It doesn't make sense to uncheck a radio item - what would this do?
        //
        if (!bCheck)
            return;

        //
        // Get the index of this item in the menu
        //
        const wxMenuItemList&       rItems = m_parentMenu->GetMenuItems();
        int                         nPos = rItems.IndexOf(this);

        wxCHECK_RET( nPos != wxNOT_FOUND
                    ,_T("menuitem not found in the menu items list?")
                   );

        //
        // Get the radio group range
        //
        int                         nStart;
        int                         nEnd;

        if (m_bIsRadioGroupStart)
        {
            //
            // We already have all information we need
            //
            nStart = nPos;
            nEnd   = m_vRadioGroup.m_nEnd;
        }
        else // next radio group item
        {
            //
            // Get the radio group end from the start item
            //
            nStart = m_vRadioGroup.m_nStart;
            nEnd = rItems.Item(nStart)->GetData()->m_vRadioGroup.m_nEnd;
        }

        //
        // Also uncheck all the other items in this radio group
        //
        wxMenuItemList::Node*       pNode = rItems.Item(nStart);

        for (int n = nStart; n <= nEnd && pNode; n++)
        {
            if (n == nPos)
            {
                ::WinSendMsg( hMenu
                             ,MM_SETITEMATTR
                             ,MPFROM2SHORT(n, TRUE)
                             ,MPFROM2SHORT(MIA_CHECKED, MIA_CHECKED)
                            );
            }
            if (n != nPos)
            {
                pNode->GetData()->m_isChecked = FALSE;
                ::WinSendMsg( hMenu
                             ,MM_SETITEMATTR
                             ,MPFROM2SHORT(n, TRUE)
                             ,MPFROM2SHORT(MIA_CHECKED, FALSE)
                            );
            }
            pNode = pNode->GetNext();
        }
    }
    else // check item
    {
        if (bCheck)
            bOk = (bool)::WinSendMsg( hMenu
                                     ,MM_SETITEMATTR
                                     ,MPFROM2SHORT(GetRealId(), TRUE)
                                     ,MPFROM2SHORT(MIA_CHECKED, MIA_CHECKED)
                                    );
        else
            bOk = (bool)::WinSendMsg( hMenu
                                     ,MM_SETITEMATTR
                                     ,MPFROM2SHORT(GetRealId(), TRUE)
                                     ,MPFROM2SHORT(MIA_CHECKED, FALSE)
                                    );
    }
    if (!bOk)
    {
        wxLogLastError("CheckMenuItem");
    }
    wxMenuItemBase::Check(bCheck);
} // end of wxMenuItem::Check

void wxMenuItem::SetText(
  const wxString&                   rText
)
{
    //
    // Don't do anything if label didn't change
    //

    wxString                        sText = TextToLabel(rText);
    if (m_text == sText)
        return;

    wxMenuItemBase::SetText(sText);
    OWNER_DRAWN_ONLY(wxOwnerDrawn::SetName(sText));
#if  wxUSE_OWNER_DRAWN
    SetAccelString(rText.AfterFirst(_T('\t')));
#endif // wxUSE_OWNER_DRAWN

    HWND                            hMenu = GetHmenuOf(m_parentMenu);

    wxCHECK_RET(hMenu, wxT("menuitem without menu"));

#if wxUSE_ACCEL
    m_parentMenu->UpdateAccel(this);
#endif // wxUSE_ACCEL

    USHORT                          uId = GetRealId();
    MENUITEM                        vItem;
    USHORT                          uFlagsOld;

    if (!::WinSendMsg( hMenu
                      ,MM_QUERYITEM
                      ,MPFROM2SHORT(uId, TRUE)
                      ,(MPARAM)&vItem
                     ))
    {
        wxLogLastError("GetMenuState");
    }
    else
    {
        uFlagsOld = vItem.afStyle;
        if (IsSubMenu())
        {
            uFlagsOld |= MIS_SUBMENU;
        }

        BYTE*                       pData;

#if wxUSE_OWNER_DRAWN
        if (IsOwnerDrawn())
        {
            uFlagsOld |= MIS_OWNERDRAW;
            pData = (BYTE*)this;
        }
        else
#endif  //owner drawn
        {
            uFlagsOld |= MIS_TEXT;
            pData = (BYTE*)sText.c_str();
        }

        //
        // Set the style
        //
        if (!::WinSendMsg( hMenu
                          ,MM_SETITEM
                          ,MPFROM2SHORT(uId, TRUE)
                          ,(MPARAM)&vItem
                         ))
        {
            wxLogLastError(wxT("ModifyMenu"));
        }

        //
        // Set the text
        //
        if (::WinSendMsg( hMenu
                         ,MM_SETITEMTEXT
                         ,MPFROMSHORT(uId)
                         ,(MPARAM)pData
                        ))
        {
            wxLogLastError(wxT("ModifyMenu"));
        }
    }
} // end of wxMenuItem::SetText

void wxMenuItem::SetCheckable(
  bool                              bCheckable
)
{
    wxMenuItemBase::SetCheckable(bCheckable);
    OWNER_DRAWN_ONLY(wxOwnerDrawn::SetCheckable(bCheckable));
} // end of wxMenuItem::SetCheckable

// ----------------------------------------------------------------------------
// wxMenuItemBase
// ----------------------------------------------------------------------------

wxMenuItem* wxMenuItemBase::New(
  wxMenu*                           pParentMenu
, int                               nId
, const wxString&                   rName
, const wxString&                   rHelp
, wxItemKind                        kind
, wxMenu*                           pSubMenu
)
{
    return new wxMenuItem( pParentMenu
                          ,nId
                          ,rName
                          ,rHelp
                          ,kind
                          ,pSubMenu
                         );
} // end of wxMenuItemBase::New

