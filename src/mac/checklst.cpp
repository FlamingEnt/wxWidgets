///////////////////////////////////////////////////////////////////////////////
// Name:        checklst.cpp
// Purpose:     implementation of wxCheckListBox class
// Author:      AUTHOR
// Modified by: 
// Created:     ??/??/98
// RCS-ID:      $Id: checklst.cpp,v 1.13.2.1 2002/11/04 22:42:56 VZ Exp $
// Copyright:   (c) AUTHOR
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// headers & declarations
// ============================================================================

#ifdef __GNUG__
#pragma implementation "checklst.h"
#endif

#include "wx/defs.h"

#if wxUSE_CHECKLISTBOX

#include "wx/checklst.h"

#include "wx/mac/uma.h"
#include "Appearance.h"

// ============================================================================
// implementation of wxCheckListBox
// ============================================================================

IMPLEMENT_DYNAMIC_CLASS(wxCheckListBox, wxListBox)

const short kwxMacListWithVerticalScrollbar = 128 ;
const short kwxMacListItemHeight = 14 ;
const short kwxMacListCheckboxWidth = 14 ;

#if PRAGMA_STRUCT_ALIGN
    #pragma options align=mac68k
#elif PRAGMA_STRUCT_PACKPUSH
    #pragma pack(push, 2)
#elif PRAGMA_STRUCT_PACK
    #pragma pack(2)
#endif

typedef struct {
 unsigned short instruction;
 void (*function)();
} ldefRec, *ldefPtr, **ldefHandle;

#if PRAGMA_STRUCT_ALIGN
    #pragma options align=reset
#elif PRAGMA_STRUCT_PACKPUSH
    #pragma pack(pop)
#elif PRAGMA_STRUCT_PACK
    #pragma pack()
#endif

extern "C"
{
static pascal void wxMacCheckListDefinition( short message, Boolean isSelected, Rect *drawRect,
                                     Cell cell, short dataOffset, short dataLength,
                                     ListHandle listHandle ) ;
}

static pascal void wxMacCheckListDefinition( short message, Boolean isSelected, Rect *drawRect,
                                     Cell cell, short dataOffset, short dataLength,
                                     ListHandle listHandle )
{
    GrafPtr savePort;
    GrafPtr grafPtr;
    RgnHandle savedClipRegion;
    SInt32 savedPenMode;
    wxCheckListBox*          list;
  GetPort(&savePort);
  SetPort((**listHandle).port);
  grafPtr = (**listHandle).port ;
    // typecast our refCon
    list = (wxCheckListBox*) GetControlReference( (ControlHandle) GetListRefCon(listHandle) );
    
    //  Calculate the cell rect.
    
    switch( message ) {
        case lInitMsg:
            break;

        case lCloseMsg:
            break;

        case lDrawMsg:
        {
            const wxString text = list->m_stringArray[cell.v] ;
            int checked = list->m_checks[cell.v] ;

            //  Save the current clip region, and set the clip region to the area we are about
            //  to draw.
            
            savedClipRegion = NewRgn();
            GetClip( savedClipRegion );

            ClipRect( drawRect );
            EraseRect( drawRect );
            
			wxFontRefData * font = (wxFontRefData*) (list->GetFont().GetRefData()) ;

			if ( font )
			{
				::TextFont( font->m_macFontNum ) ;
				::TextSize( font->m_macFontSize)  ;
				::TextFace( font->m_macFontStyle ) ;
            }
                       
            ThemeButtonDrawInfo info ;
            info.state = kThemeStateActive ;
            info.value = checked ? kThemeButtonOn : kThemeButtonOff ;
            info.adornment = kThemeAdornmentNone ;
            Rect checkRect = *drawRect ;
                        
            
            checkRect.left +=0 ;
            checkRect.top +=0 ;
            checkRect.right = checkRect.left + list->m_checkBoxWidth ;
            checkRect.bottom = checkRect.top + list->m_checkBoxHeight ;
            DrawThemeButton(&checkRect,kThemeCheckBox,
              &info,NULL,NULL, NULL,0);

        MoveTo(drawRect->left + 2 + list->m_checkBoxWidth+2, drawRect->top + list->m_TextBaseLineOffset );
 
        DrawText(text, 0 , text.Length());
            //  If the cell is hilited, do the hilite now. Paint the cell contents with the
            //  appropriate QuickDraw transform mode.
            
            if( isSelected ) {
                savedPenMode = GetPortPenMode( (CGrafPtr) grafPtr );
                SetPortPenMode( (CGrafPtr) grafPtr, hilitetransfermode );
                PaintRect( drawRect );
                SetPortPenMode( (CGrafPtr) grafPtr, savedPenMode );
            }
            
            //  Restore the saved clip region.
            
            SetClip( savedClipRegion );
            DisposeRgn( savedClipRegion );
            }
            break;
        case lHiliteMsg:
            
            //  Hilite or unhilite the cell. Paint the cell contents with the
            //  appropriate QuickDraw transform mode.
            
            GetPort( &grafPtr );
            savedPenMode = GetPortPenMode( (CGrafPtr) grafPtr );
            SetPortPenMode( (CGrafPtr) grafPtr, hilitetransfermode );
            PaintRect( drawRect );
            SetPortPenMode( (CGrafPtr) grafPtr, savedPenMode );
            break;
        default :
          break ;
    }
    SetPort(savePort);  
}

extern "C" void MacDrawStringCell(Rect *cellRect, Cell lCell, ListHandle theList, long refCon) ;

static ListDefUPP macCheckListDefUPP = NULL ;

// ----------------------------------------------------------------------------
// creation
// ----------------------------------------------------------------------------

void wxCheckListBox::Init()
{
}

bool wxCheckListBox::Create(wxWindow *parent,
                            wxWindowID id,
                            const wxPoint &pos,
                            const wxSize &size,
                            int n,
                            const wxString choices[],
                            long style,
                            const wxValidator& validator,
                            const wxString &name)
{
    m_noItems = 0 ; // this will be increased by our append command
    m_selected = 0;
    
    m_checkBoxWidth = 12;
    m_checkBoxHeight= 10;
    
    long h = m_checkBoxHeight ;
#if TARGET_CARBON
    GetThemeMetric(kThemeMetricCheckBoxWidth,(long *)&m_checkBoxWidth);    
    GetThemeMetric(kThemeMetricCheckBoxHeight,&h);
#endif
	  wxFontRefData * font = (wxFontRefData*) (GetFont().GetRefData()) ;

    FontInfo finfo;
    FetchFontInfo(font->m_macFontNum,short(font->m_macFontSize),font->m_macFontStyle,&finfo);
    
    m_TextBaseLineOffset= finfo.leading+finfo.ascent;
    m_checkBoxHeight= finfo.leading+finfo.ascent+finfo.descent;
    
	if (m_checkBoxHeight<h)
	{
		m_TextBaseLineOffset+= (h-m_checkBoxHeight)/2;
		m_checkBoxHeight= h;
	}
		
    Rect bounds ;
    Str255 title ;
    
    MacPreControlCreate( parent , id ,  "" , pos , size ,style, validator , name , &bounds , title ) ;

    ListDefSpec listDef;
    listDef.defType = kListDefUserProcType;
    if ( macCheckListDefUPP == NULL )
    {
      macCheckListDefUPP = NewListDefUPP( wxMacCheckListDefinition ); 
    }
        listDef.u.userProc = macCheckListDefUPP ;

#if TARGET_CARBON
    Size asize;


    CreateListBoxControl( MAC_WXHWND(parent->MacGetRootWindow()), &bounds, false, 0, 1, false, true,
                          m_checkBoxHeight+2, 14, false, &listDef, (ControlRef *)&m_macControl );

    GetControlData( (ControlHandle) m_macControl, kControlNoPart, kControlListBoxListHandleTag,
                   sizeof(ListHandle), (Ptr) &m_macList, &asize);

    SetControlReference( (ControlHandle) m_macControl, (long) this);
    SetControlVisibility( (ControlHandle) m_macControl, false, false);

#else

    long    result ;

    wxStAppResource resload ;
    m_macControl = ::NewControl( MAC_WXHWND(parent->MacGetRootWindow()) , &bounds , title , false ,
                  kwxMacListWithVerticalScrollbar , 0 , 0, 
                  kControlListBoxProc , (long) this ) ;
    ::GetControlData( (ControlHandle) m_macControl , kControlNoPart , kControlListBoxListHandleTag ,
               sizeof( ListHandle ) , (char*) &m_macList  , &result ) ;

    HLock( (Handle) m_macList ) ;
    ldefHandle ldef ;
    ldef = (ldefHandle) NewHandle( sizeof(ldefRec) ) ;
    if (  (**(ListHandle)m_macList).listDefProc != NULL )
    {
      (**ldef).instruction = 0x4EF9;  /* JMP instruction */
      (**ldef).function = (void(*)()) listDef.u.userProc;
      (**(ListHandle)m_macList).listDefProc = (Handle) ldef ;
    }
        
    Point pt = (**(ListHandle)m_macList).cellSize ;
    pt.v = 14 ;
    LCellSize( pt , (ListHandle)m_macList ) ;
    LAddColumn( 1 , 0 , (ListHandle)m_macList ) ;
#endif
    OptionBits  options = 0;
    if ( style & wxLB_MULTIPLE )
    {
        options += lNoExtend ;
    }
    else if ( style & wxLB_EXTENDED )
    {
        options += lExtendDrag ;
    }
    else
    {
        options = (OptionBits) lOnlyOne ;
    }
    SetListSelectionFlags((ListHandle)m_macList, options);
    
    MacPostControlCreate() ;
    
    for ( int i = 0 ; i < n ; i++ )
    {
        Append( choices[i] ) ;
    }
    
    LSetDrawingMode( true , (ListHandle) m_macList ) ;

    return TRUE;
}

// ----------------------------------------------------------------------------
// wxCheckListBox functions
// ----------------------------------------------------------------------------

bool wxCheckListBox::IsChecked(size_t item) const
{
    wxCHECK_MSG( item < m_checks.GetCount(), FALSE,
                 _T("invalid index in wxCheckListBox::IsChecked") );

    return m_checks[item] != 0;
}

void wxCheckListBox::Check(size_t item, bool check)
{
    wxCHECK_RET( item < m_checks.GetCount(),
                 _T("invalid index in wxCheckListBox::Check") );

    // intermediate var is needed to avoid compiler warning with VC++
    bool isChecked = m_checks[item] != 0;
    if ( check != isChecked )
    {
        m_checks[item] = check;

        MacRedrawControl() ;
    }
}

// ----------------------------------------------------------------------------
// methods forwarded to wxListBox
// ----------------------------------------------------------------------------

void wxCheckListBox::Delete(int n)
{
    wxCHECK_RET( n < GetCount(), _T("invalid index in wxListBox::Delete") );

    wxListBox::Delete(n);

    m_checks.RemoveAt(n);
}

int wxCheckListBox::DoAppend(const wxString& item)
{
    LSetDrawingMode( false , (ListHandle) m_macList ) ;
    int pos = wxListBox::DoAppend(item);

    // the item is initially unchecked
    m_checks.Insert(FALSE, pos);
    LSetDrawingMode( true , (ListHandle) m_macList ) ;

    return pos;
}

void wxCheckListBox::DoInsertItems(const wxArrayString& items, int pos)
{
    wxListBox::DoInsertItems(items, pos);

    size_t count = items.GetCount();
    for ( size_t n = 0; n < count; n++ )
    {
        m_checks.Insert(FALSE, pos + n);
    }
}

void wxCheckListBox::DoSetItems(const wxArrayString& items, void **clientData)
{
    // call it first as it does DoClear()
    wxListBox::DoSetItems(items, clientData);

    size_t count = items.GetCount();
    for ( size_t n = 0; n < count; n++ )
    {
        m_checks.Add(FALSE);
    }
}

void wxCheckListBox::DoClear()
{
    m_checks.Empty();
}

BEGIN_EVENT_TABLE(wxCheckListBox, wxListBox)
  EVT_CHAR(wxCheckListBox::OnChar)
  EVT_LEFT_DOWN(wxCheckListBox::OnLeftClick)
END_EVENT_TABLE()

// this will only work as soon as 

void wxCheckListBox::OnChar(wxKeyEvent& event)
{
  if ( event.KeyCode() == WXK_SPACE )
  {
    int index = GetSelection() ;
    if ( index >= 0 )
    {
      Check(index, !IsChecked(index) ) ;
      wxCommandEvent event(wxEVT_COMMAND_CHECKLISTBOX_TOGGLED, GetId());
      event.SetInt(index);
      event.SetEventObject(this);
      GetEventHandler()->ProcessEvent(event);
    }
  }
  else
    event.Skip();
}

void wxCheckListBox::OnLeftClick(wxMouseEvent& event)
{
  // clicking on the item selects it, clicking on the checkmark toggles
  if ( event.GetX() <= 20 /*check width*/ ) {
    int lineheight ;
    int topcell ;
#if TARGET_CARBON
    Point pt ;
    GetListCellSize( (ListHandle)m_macList , &pt ) ;
    lineheight = pt.v ;
    ListBounds visible ;
    GetListVisibleCells( (ListHandle)m_macList , &visible ) ;
    topcell = visible.top ;
#else
    lineheight =  (**(ListHandle)m_macList).cellSize.v ;
    topcell = (**(ListHandle)m_macList).visible.top ;
#endif
    size_t nItem = ((size_t)event.GetY()) / lineheight + topcell ;
    
    if ( nItem < (size_t)m_noItems )
    {
      Check(nItem, !IsChecked(nItem) ) ;
      wxCommandEvent event(wxEVT_COMMAND_CHECKLISTBOX_TOGGLED, GetId());
      event.SetInt(nItem);
      event.SetEventObject(this);
      GetEventHandler()->ProcessEvent(event);
    }
    //else: it's not an error, just click outside of client zone
  }
  else {
    // implement default behaviour: clicking on the item selects it
    event.Skip();
  }
}

#endif // wxUSE_CHECKLISTBOX
