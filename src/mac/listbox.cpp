///////////////////////////////////////////////////////////////////////////////
// Name:        listbox.cpp
// Purpose:     wxListBox
// Author:      AUTHOR
// Modified by:
// Created:     ??/??/98
// RCS-ID:      $Id: listbox.cpp,v 1.43.2.5 2003/04/13 22:55:39 JS Exp $
// Copyright:   (c) AUTHOR
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "listbox.h"
#endif

#include "wx/app.h"
#include "wx/listbox.h"
#include "wx/button.h"
#include "wx/settings.h"
#include "wx/toplevel.h"
#include "wx/dynarray.h"
#include "wx/log.h"

#include "wx/utils.h"

#if !USE_SHARED_LIBRARY
  IMPLEMENT_DYNAMIC_CLASS(wxListBox, wxControl)

BEGIN_EVENT_TABLE(wxListBox, wxControl)
	EVT_SIZE( wxListBox::OnSize )
	EVT_CHAR( wxListBox::OnChar )
END_EVENT_TABLE()
#endif

#include "wx/mac/uma.h"

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

#if TARGET_CARBON
const short kwxMacListItemHeight = 19 ;
#else
const short kwxMacListItemHeight = 14 ;
#endif

extern "C"
{
static pascal void wxMacListDefinition( short message, Boolean isSelected, Rect *drawRect,
                                     Cell cell, short dataOffset, short dataLength,
                                     ListHandle listHandle ) ;
}

static pascal void wxMacListDefinition( short message, Boolean isSelected, Rect *drawRect,
                                     Cell cell, short dataOffset, short dataLength,
                                     ListHandle listHandle )
{
    GrafPtr savePort;
    GrafPtr grafPtr;
    RgnHandle savedClipRegion;
    SInt32 savedPenMode;
    wxListBox*          list;
  GetPort(&savePort);
  SetPort((**listHandle).port);
  grafPtr = (**listHandle).port ;
    // typecast our refCon
    list = (wxListBox*) GetControlReference( (ControlHandle) GetListRefCon(listHandle) );

    //  Calculate the cell rect.

    switch( message ) {
        case lInitMsg:
            break;

        case lCloseMsg:
            break;

        case lDrawMsg:
        {
            const wxString text = list->m_stringArray[cell.v] ;

            //  Save the current clip region, and set the clip region to the area we are about
            //  to draw.

            savedClipRegion = NewRgn();
            GetClip( savedClipRegion );

            ClipRect( drawRect );
            EraseRect( drawRect );

			wxFontRefData * font = (wxFontRefData*) list->GetFont().GetRefData() ;

			if ( font )
			{
				::TextFont( font->m_macFontNum ) ;
				::TextSize( short(font->m_macFontSize) ) ;
				::TextFace( font->m_macFontStyle ) ;
			}
			else
			{
		        ::TextFont( kFontIDMonaco ) ;
		        ::TextSize( 9  );
		        ::TextFace( 0 ) ;
			}

#if TARGET_CARBON
	        bool useDrawThemeText = ( DrawThemeTextBox != (void*) kUnresolvedCFragSymbolAddress ) ;

           	if ( useDrawThemeText )
           	{
	            Rect frame = { drawRect->top, drawRect->left + 4,
	                           drawRect->top + kwxMacListItemHeight, drawRect->right + 10000 } ;
                CFStringRef sString = CFStringCreateWithBytes( NULL , (UInt8*) text.c_str(), text.Length(), CFStringGetSystemEncoding(), false ) ;
                CFMutableStringRef mString = CFStringCreateMutableCopy( NULL , 0 , sString ) ;
        	    CFRelease( sString ) ;
  				::TruncateThemeText( mString , kThemeCurrentPortFont, kThemeStateActive, drawRect->right - drawRect->left , truncEnd , NULL ) ;
        		::DrawThemeTextBox( mString,
        							kThemeCurrentPortFont,
        							kThemeStateActive,
        							false,
        							&frame,
        							teJustLeft,
        							nil );
        	    CFRelease( mString ) ;
            }
   		    else
#endif
   		    {
		        MoveTo(drawRect->left + 4 , drawRect->top + 10 );
        		DrawText(text, 0 , text.Length());
		    }

            //  If the cell is hilited, do the hilite now. Paint the cell contents with the
            //  appropriate QuickDraw transform mode.

            if( isSelected ) {
                savedPenMode = GetPortPenMode( (CGrafPtr) grafPtr );
                SetPortPenMode( (CGrafPtr)grafPtr, hilitetransfermode );
                PaintRect( drawRect );
                SetPortPenMode( (CGrafPtr)grafPtr, savedPenMode );
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
            savedPenMode = GetPortPenMode( (CGrafPtr)grafPtr );
            SetPortPenMode( (CGrafPtr)grafPtr, hilitetransfermode );
            PaintRect( drawRect );
            SetPortPenMode( (CGrafPtr)grafPtr, savedPenMode );
            break;
        default :
          break ;
    }
    SetPort(savePort);
}

extern "C" void MacDrawStringCell(Rect *cellRect, Cell lCell, ListHandle theList, long refCon) ;
// resources ldef ids
const short kwxMacListWithVerticalScrollbar = 128 ;
const short kwxMacListWithVerticalAndHorizontalScrollbar = 129 ;

// ============================================================================
// list box control implementation
// ============================================================================

// Listbox item
wxListBox::wxListBox()
{
  m_noItems = 0;
  m_selected = 0;
  m_macList = NULL ;
}

static ListDefUPP macListDefUPP = NULL ;

bool wxListBox::Create(wxWindow *parent, wxWindowID id,
                       const wxPoint& pos,
                       const wxSize& size,
                       int n, const wxString choices[],
                       long style,
                       const wxValidator& validator,
                       const wxString& name)
{
    m_noItems = 0 ; // this will be increased by our append command
    m_selected = 0;

    Rect bounds ;
    Str255 title ;

    MacPreControlCreate( parent , id ,  "" , pos , size ,style, validator , name , &bounds , title ) ;

    ListDefSpec listDef;
    listDef.defType = kListDefUserProcType;
    if ( macListDefUPP == NULL )
    {
      macListDefUPP = NewListDefUPP( wxMacListDefinition );
    }
        listDef.u.userProc = macListDefUPP ;

    Str255 fontName ;
    SInt16 fontSize ;
    Style fontStyle ;
	SInt16 fontNum ;
#if TARGET_CARBON
	GetThemeFont(kThemeViewsFont , GetApplicationScript() , fontName , &fontSize , &fontStyle ) ;
#else
	GetFontName( kFontIDMonaco , fontName ) ;
	fontSize = 9 ;
	fontStyle = normal ;
#endif 
    CopyPascalStringToC( fontName , (char*) fontName ) ;
    SetFont( wxFont (fontSize, wxSWISS, wxNORMAL, wxNORMAL , false , fontName ) ) ;
#if TARGET_CARBON
    Size asize;


    CreateListBoxControl( MAC_WXHWND(parent->MacGetRootWindow()), &bounds, false, 0, 1, (style & wxLB_HSCROLL), true,
                          kwxMacListItemHeight, kwxMacListItemHeight, false, &listDef, (ControlRef *)&m_macControl );

    GetControlData( (ControlHandle) m_macControl, kControlNoPart, kControlListBoxListHandleTag,
                   sizeof(ListHandle), (Ptr) &m_macList, &asize);

    SetControlReference( (ControlHandle) m_macControl, (long) this);
    SetControlVisibility( (ControlHandle) m_macControl, false, false);

#else

    long    result ;
    wxStAppResource resload ;
    m_macControl = ::NewControl( MAC_WXHWND(parent->MacGetRootWindow()) , &bounds , title , false ,
                  (style & wxLB_HSCROLL) ? kwxMacListWithVerticalAndHorizontalScrollbar : kwxMacListWithVerticalScrollbar , 
                  0 , 0, kControlListBoxProc , (long) this ) ;
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
    pt.v = kwxMacListItemHeight ;
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

    for ( int i = 0 ; i < n ; i++ )
    {
        Append( choices[i] ) ;
    }

    MacPostControlCreate() ;

    LSetDrawingMode( true , (ListHandle)m_macList ) ;

    return TRUE;
}

wxListBox::~wxListBox()
{
    FreeData() ;
    if ( m_macList )
    {
#if !TARGET_CARBON
      DisposeHandle( (**(ListHandle)m_macList).listDefProc ) ;
      (**(ListHandle)m_macList).listDefProc = NULL ;
#endif
        m_macList = NULL ;
    }
}

void wxListBox::FreeData()
{
#if wxUSE_OWNER_DRAWN
    if ( m_windowStyle & wxLB_OWNERDRAW )
    {
        size_t uiCount = m_aItems.Count();
        while ( uiCount-- != 0 ) {
            delete m_aItems[uiCount];
            m_aItems[uiCount] = NULL;
        }

        m_aItems.Clear();
    }
    else
#endif // wxUSE_OWNER_DRAWN
    if ( HasClientObjectData() )
    {
        for ( size_t n = 0; n < (size_t)m_noItems; n++ )
        {
            delete GetClientObject(n);
        }
    }
}

void  wxListBox::DoSetSize(int x, int y,
            int width, int height,
            int sizeFlags )
{
    wxControl::DoSetSize( x , y , width , height , sizeFlags ) ;
#if TARGET_CARBON
    Rect bounds ;
    GetControlBounds( (ControlHandle) m_macControl , &bounds ) ;
    ControlRef control = GetListVerticalScrollBar( (ListHandle)m_macList ) ;
    if ( control )
    {
        Rect scrollbounds ;
        GetControlBounds( control , &scrollbounds ) ;
        if( scrollbounds.right != bounds.right + 1 )
        {
            UMAMoveControl( control , bounds.right - (scrollbounds.right - scrollbounds.left) + 1 ,
                scrollbounds.top ) ;
        }
    }
#endif
}
void wxListBox::DoSetFirstItem(int N)
{
    MacScrollTo( N ) ;
}

void wxListBox::Delete(int N)
{
    wxCHECK_RET( N >= 0 && N < m_noItems,
                 wxT("invalid index in wxListBox::Delete") );

#if wxUSE_OWNER_DRAWN
    delete m_aItems[N];
    m_aItems.RemoveAt(N);
#else // !wxUSE_OWNER_DRAWN
    if ( HasClientObjectData() )
    {
        delete GetClientObject(N);
    }
#endif // wxUSE_OWNER_DRAWN/!wxUSE_OWNER_DRAWN
    m_stringArray.RemoveAt( N ) ;
    m_dataArray.RemoveAt( N ) ;
    m_noItems --;

    MacDelete( N ) ;
}

int wxListBox::DoAppend(const wxString& item)
{
    int index = m_noItems ;
    if( wxApp::s_macDefaultEncodingIsPC )
    {
        m_stringArray.Add( wxMacMakeMacStringFromPC( item ) ) ;
        m_dataArray.Add( NULL );
    }
    else {
        m_stringArray.Add( item ) ;
        m_dataArray.Add( NULL );
    }
    m_noItems ++;
    DoSetItemClientData( index , NULL ) ;
    MacAppend( item ) ;

    return index ;
}

void wxListBox::DoSetItems(const wxArrayString& choices, void** clientData)
{
  MacSetRedraw( false ) ;
  Clear() ;
  int n = choices.GetCount();

  for( int i = 0 ; i < n ; ++i )
  {
    if ( clientData )
    {
#if wxUSE_OWNER_DRAWN
            wxASSERT_MSG(clientData[i] == NULL,
                         wxT("Can't use client data with owner-drawn listboxes"));
#else // !wxUSE_OWNER_DRAWN
        Append( choices[i] , clientData[i] ) ;
 #endif
    }
    else
        Append( choices[i] ) ;
  }

#if wxUSE_OWNER_DRAWN
    if ( m_windowStyle & wxLB_OWNERDRAW ) {
        // first delete old items
        size_t ui = m_aItems.Count();
        while ( ui-- != 0 ) {
            delete m_aItems[ui];
            m_aItems[ui] = NULL;
        }
        m_aItems.Empty();

        // then create new ones
        for ( ui = 0; ui < (size_t)m_noItems; ui++ ) {
            wxOwnerDrawn *pNewItem = CreateItem(ui);
            pNewItem->SetName(choices[ui]);
            m_aItems.Add(pNewItem);
        }
    }
#endif // wxUSE_OWNER_DRAWN
  MacSetRedraw( true ) ;
}

bool wxListBox::HasMultipleSelection() const
{
    return (m_windowStyle & wxLB_MULTIPLE) || (m_windowStyle & wxLB_EXTENDED);
}

int wxListBox::FindString(const wxString& st) const
{
    wxString s ;
    if( wxApp::s_macDefaultEncodingIsPC )
    {
        s = wxMacMakeMacStringFromPC( st ) ;
    }
    else
        s = st ;

    if ( s.Right(1) == "*" )
    {
        wxString search = s.Left( s.Length() - 1 ) ;
        int len = search.Length() ;
        Str255 s1 , s2 ;

#if TARGET_CARBON
        c2pstrcpy( (StringPtr) s2 , search.c_str() ) ;
#else
        strcpy( (char *) s2 , search.c_str() ) ;
        c2pstr( (char *) s2 ) ;
#endif

        for ( int i = 0 ; i < m_noItems ; ++ i )
        {
#if TARGET_CARBON
            c2pstrcpy( (StringPtr) s1 , m_stringArray[i].Left( len ).c_str() ) ;
#else
            strcpy( (char *) s1 , m_stringArray[i].Left( len ).c_str() ) ;
            c2pstr( (char *) s1 ) ;
#endif
            if ( EqualString( s1 , s2 , false , false ) )
                return i ;
        }
        if ( s.Left(1) == "*" && s.Length() > 1 )
        {
            s = st ;
            s.MakeLower() ;
            for ( int i = 0 ; i < m_noItems ; ++i )
            {
                if ( GetString(i).Lower().Matches(s) )
                    return i ;
            }
        }

    }
    else
    {
        Str255 s1 , s2 ;

#if TARGET_CARBON
        c2pstrcpy( (StringPtr) s2 , s.c_str() ) ;
#else
        strcpy( (char *) s2 , s.c_str() ) ;
        c2pstr( (char *) s2 ) ;
#endif

        for ( int i = 0 ; i < m_noItems ; ++ i )
        {
#if TARGET_CARBON
            c2pstrcpy( (StringPtr) s1 , m_stringArray[i].c_str() ) ;
#else
            strcpy( (char *) s1 , m_stringArray[i].c_str() ) ;
            c2pstr( (char *) s1 ) ;
#endif
            if ( EqualString( s1 , s2 , false , false ) )
                return i ;
        }
   }
   return -1;
}

void wxListBox::Clear()
{
  FreeData();
  m_noItems = 0;
  m_stringArray.Empty() ;
  m_dataArray.Empty() ;
  MacClear() ;
}

void wxListBox::SetSelection(int N, bool select)
{
    wxCHECK_RET( N >= 0 && N < m_noItems,
                 "invalid index in wxListBox::SetSelection" );
      MacSetSelection( N , select ) ;
      GetSelections( m_selectionPreImage ) ;
}

bool wxListBox::IsSelected(int N) const
{
    wxCHECK_MSG( N >= 0 && N < m_noItems, FALSE,
                 "invalid index in wxListBox::Selected" );

      return MacIsSelected( N ) ;
}

void *wxListBox::DoGetItemClientData(int N) const
{
    wxCHECK_MSG( N >= 0 && N < m_noItems, NULL,
                 wxT("invalid index in wxListBox::GetClientData"));

    return (void *)m_dataArray[N];
}

wxClientData *wxListBox::DoGetItemClientObject(int N) const
{
    return (wxClientData *) DoGetItemClientData( N ) ;
}

void wxListBox::DoSetItemClientData(int N, void *Client_data)
{
    wxCHECK_RET( N >= 0 && N < m_noItems,
                 "invalid index in wxListBox::SetClientData" );

#if wxUSE_OWNER_DRAWN
    if ( m_windowStyle & wxLB_OWNERDRAW )
    {
        // client data must be pointer to wxOwnerDrawn, otherwise we would crash
        // in OnMeasure/OnDraw.
        wxFAIL_MSG(wxT("Can't use client data with owner-drawn listboxes"));
    }
#endif // wxUSE_OWNER_DRAWN
    wxASSERT_MSG( m_dataArray.GetCount() >= (size_t) N , "invalid client_data array" ) ;

    if ( m_dataArray.GetCount() > (size_t) N )
    {
        m_dataArray[N] = (char*) Client_data ;
    }
    else
    {
        m_dataArray.Add( (char*) Client_data ) ;
    }
}

void wxListBox::DoSetItemClientObject(int n, wxClientData* clientData)
{
    DoSetItemClientData(n, clientData);
}

// Return number of selections and an array of selected integers
int wxListBox::GetSelections(wxArrayInt& aSelections) const
{
    return MacGetSelections( aSelections ) ;
}

// Get single selection, for single choice list items
int wxListBox::GetSelection() const
{
    return MacGetSelection() ;
}

// Find string for position
wxString wxListBox::GetString(int N) const
{
    if( wxApp::s_macDefaultEncodingIsPC )
    {
        return      wxMacMakePCStringFromMac( m_stringArray[N] ) ;
    }
    else
        return m_stringArray[N]  ;
}

void wxListBox::DoInsertItems(const wxArrayString& items, int pos)
{
    wxCHECK_RET( pos >= 0 && pos <= m_noItems,
                 wxT("invalid index in wxListBox::InsertItems") );

    int nItems = items.GetCount();

    for ( int i = 0 ; i < nItems ; i++ )
    {
        m_stringArray.Insert( items[i] , pos + i ) ;
        m_dataArray.Insert( NULL , pos + i ) ;
        MacInsert( pos + i , items[i] ) ;
    }

    m_noItems += nItems;
}

void wxListBox::SetString(int N, const wxString& s)
{
    wxString str ;
    if( wxApp::s_macDefaultEncodingIsPC )
    {
        str = wxMacMakeMacStringFromPC( s )  ;
    }
    else
        str = s ;
    m_stringArray[N] = str ;
    MacSet( N , s ) ;
}

wxSize wxListBox::DoGetBestSize() const
{
    int lbWidth = 100;  // some defaults
    int lbHeight = 110;
    int wLine;

	{
		wxMacPortStateHelper st( UMAGetWindowPort( (WindowRef) MacGetRootWindow() ) ) ; 
 		Rect drawRect ;

		wxFontRefData * font = (wxFontRefData*) m_font.GetRefData() ;

		if ( font )
		{
			::TextFont( font->m_macFontNum ) ;
			::TextSize( short(font->m_macFontSize) ) ;
			::TextFace( font->m_macFontStyle ) ;
		}
		else
		{
	        ::TextFont( kFontIDMonaco ) ;
	        ::TextSize( 9  );
	        ::TextFace( 0 ) ;
		}

	    // Find the widest line
	    for(int i = 0; i < GetCount(); i++) {
	        wxString str(GetString(i));
	        wLine = ::TextWidth( str.c_str() , 0 , str.Length() ) ;
	        lbWidth = wxMax(lbWidth, wLine);
	    }

	    // Add room for the scrollbar
	    lbWidth += wxSystemSettings::GetMetric(wxSYS_VSCROLL_X);

	    // And just a bit more
	    int cy = 12 ;
	    int cx = ::TextWidth( "X" , 0 , 1 ) ;
	    lbWidth += cx ;
	    
	    // don't make the listbox too tall (limit height to around 10 items) but don't
	    // make it too small neither
	    lbHeight = (cy+4) * wxMin(wxMax(GetCount(), 3), 10);
	}
    return wxSize(lbWidth, lbHeight);
}

int wxListBox::GetCount() const
{
    return m_noItems;
}

void wxListBox::SetupColours()
{
    SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
    SetForegroundColour(GetParent()->GetForegroundColour());
}

void wxListBox::Refresh(bool eraseBack, const wxRect *rect)
{
    wxControl::Refresh( eraseBack , rect ) ;
//    MacRedrawControl() ;
}

#if wxUSE_OWNER_DRAWN

class wxListBoxItem : public wxOwnerDrawn
{
public:
    wxListBoxItem(const wxString& str = "");
};

wxListBoxItem::wxListBoxItem(const wxString& str) : wxOwnerDrawn(str, FALSE)
{
    // no bitmaps/checkmarks
    SetMarginWidth(0);
}

wxOwnerDrawn *wxListBox::CreateItem(size_t n)
{
    return new wxListBoxItem();
}

#endif  //USE_OWNER_DRAWN

// ============================================================================
// list box control implementation
// ============================================================================

/*
void MacDrawStringCell(Rect *cellRect, Cell lCell, ListHandle theList, long refCon)
{
    wxListBox*          list;
    // typecast our refCon
    list = (wxListBox*)refCon;

    MoveTo(cellRect->left + 4 , cellRect->top + 10 );
    const wxString text = list->m_stringArray[lCell.v] ;
    ::TextFont( kFontIDMonaco ) ;
    ::TextSize( 9  );
    ::TextFace( 0 ) ;
    DrawText(text, 0 , text.Length());

}
*/
void wxListBox::MacDelete( int N )
{
    LDelRow( 1 , N , (ListHandle)m_macList) ;
    Refresh();
}

void wxListBox::MacInsert( int n , const char * text)
{
    Cell cell = { 0 , 0 } ;
    cell.v = n ;
    LAddRow( 1 , cell.v , (ListHandle)m_macList ) ;
//    LSetCell(text, strlen(text), cell, m_macList);
    Refresh();
}

void wxListBox::MacAppend( const char * text)
{
    Cell cell = { 0 , 0 } ;
    cell.v = (**(ListHandle)m_macList).dataBounds.bottom ;
    LAddRow( 1 , cell.v , (ListHandle)m_macList ) ;
 //   LSetCell(text, strlen(text), cell, m_macList);
    Refresh();
}

void wxListBox::MacClear()
{
    LDelRow( (**(ListHandle)m_macList).dataBounds.bottom , 0 ,(ListHandle) m_macList ) ;
    Refresh();
}

void wxListBox::MacSetSelection( int n , bool select )
{
    Cell cell = { 0 , 0 } ;
    if ( ! (m_windowStyle & wxLB_MULTIPLE) )
    {
        if ( LGetSelect( true , &cell , (ListHandle)m_macList ) )
        {
            LSetSelect( false , cell , (ListHandle)m_macList ) ;
        }
    }

    cell.v = n ;
    LSetSelect( select , cell , (ListHandle)m_macList ) ;
    LAutoScroll( (ListHandle)m_macList ) ;
    Refresh();
}

bool wxListBox::MacIsSelected( int n ) const
{
    Cell cell = { 0 , 0 } ;
    cell.v = n ;
    return LGetSelect( false , &cell , (ListHandle)m_macList ) ;
}

void wxListBox::MacDestroy()
{
//    DisposeExtLDEFInfo( m_macList ) ;
}

int wxListBox::MacGetSelection() const
{
    Cell cell = { 0 , 0 } ;
    if ( LGetSelect( true , &cell , (ListHandle)m_macList ) )
        return cell.v ;
    else
        return -1 ;
}

int wxListBox::MacGetSelections( wxArrayInt& aSelections ) const
{
    int no_sel = 0 ;

    aSelections.Empty();

    Cell cell = { 0 , 0 } ;
    cell.v = 0 ;

    while ( LGetSelect( true , &cell ,(ListHandle) m_macList ) )
    {
        aSelections.Add( cell.v ) ;
        no_sel++ ;
        cell.v++ ;
    }
    return no_sel ;
}

void wxListBox::MacSet( int n , const char * text )
{
    // our implementation does not store anything in the list
    // so we just have to redraw
    Cell cell = { 0 , 0 } ;
    cell.v = n ;
//  LSetCell(text, strlen(text), cell, m_macList);
    Refresh();
}

void wxListBox::MacScrollTo( int n )
{
    // TODO implement scrolling
}

void wxListBox::OnSize( const wxSizeEvent &event)
{
    Point pt;

#if TARGET_CARBON
    GetListCellSize((ListHandle)m_macList, &pt);
#else
    pt = (**(ListHandle)m_macList).cellSize ;
#endif
    pt.h =  m_width - 15  ;
    LCellSize( pt , (ListHandle)m_macList ) ;
}

void wxListBox::MacHandleControlClick( WXWidget control , wxInt16 controlpart )
{
    Boolean wasDoubleClick = false ;
    long    result ;

    ::GetControlData( (ControlHandle) m_macControl , kControlNoPart , kControlListBoxDoubleClickTag , sizeof( wasDoubleClick ) , (char*) &wasDoubleClick  , &result ) ;
    if ( !wasDoubleClick )
    {
        MacDoClick() ;
    }
    else
    {
        MacDoDoubleClick() ;
    }
}

void wxListBox::MacSetRedraw( bool doDraw )
{
    LSetDrawingMode( doDraw , (ListHandle)m_macList ) ;

}

void wxListBox::MacDoClick()
{
    wxArrayInt aSelections;
    int n ;
    size_t count = GetSelections(aSelections);

    if ( count == m_selectionPreImage.GetCount() )
    {
        bool hasChanged = false ;
        for ( size_t i = 0 ; i < count ; ++i )
        {
            if ( aSelections[i] != m_selectionPreImage[i] )
            {
                hasChanged = true ;
                break ;
            }
        }
        if ( !hasChanged )
        {
            return ;
        }
    }

    m_selectionPreImage = aSelections;

    wxCommandEvent event(wxEVT_COMMAND_LISTBOX_SELECTED, m_windowId);
    event.SetEventObject( this );

    if ( count > 0 )
    {
        n = aSelections[0];
        if ( HasClientObjectData() )
            event.SetClientObject( GetClientObject(n) );
        else if ( HasClientUntypedData() )
            event.SetClientData( GetClientData(n) );
        event.SetString( GetString(n) );
    }
    else
    {
         n = -1;
    }

    event.m_commandInt = n;

    GetEventHandler()->ProcessEvent(event);
}

void wxListBox::MacDoDoubleClick()
{
    wxCommandEvent event(wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, m_windowId);
    event.SetEventObject( this );
	GetEventHandler()->ProcessEvent(event) ;
}

void wxListBox::OnChar(wxKeyEvent& event)
{
    if ( event.KeyCode() == WXK_RETURN || event.KeyCode() == WXK_NUMPAD_ENTER)
    {
    	wxWindow* parent = GetParent() ;
    	while( parent  && !parent->IsTopLevel() && parent->GetDefaultItem() == NULL )
    		parent = parent->GetParent() ;

    	if ( parent && parent->GetDefaultItem() )
    	{
        	wxButton *def = wxDynamicCast(parent->GetDefaultItem(),
                                               wxButton);
	        if ( def && def->IsEnabled() )
	        {
	             wxCommandEvent event(wxEVT_COMMAND_BUTTON_CLICKED, def->GetId() );
	             event.SetEventObject(def);
	             def->Command(event);
	             return ;
	        }
	    }
	    event.Skip() ;
    }
    /* generate wxID_CANCEL if command-. or <esc> has been pressed (typically in dialogs) */
    else if (event.KeyCode() == WXK_ESCAPE || (event.KeyCode() == '.' && event.MetaDown() ) )
    {
    	wxWindow* win = GetParent()->FindWindow( wxID_CANCEL ) ;
        // FIXME: try ancestor, not just parent
        if (win)
        {
            wxCommandEvent new_event(wxEVT_COMMAND_BUTTON_CLICKED,wxID_CANCEL);
            new_event.SetEventObject( win );
            win->GetEventHandler()->ProcessEvent( new_event );
        }
    }
    else if ( event.KeyCode() == WXK_TAB )
    {
        wxNavigationKeyEvent new_event;
        new_event.SetEventObject( this );
        new_event.SetDirection( !event.ShiftDown() );
        /* CTRL-TAB changes the (parent) window, i.e. switch notebook page */
        new_event.SetWindowChange( event.ControlDown() );
        new_event.SetCurrentFocus( this );
        if ( !GetEventHandler()->ProcessEvent( new_event ) )
        	event.Skip() ;
    }
	else if ( event.KeyCode() == WXK_DOWN || event.KeyCode() == WXK_UP )
	{
		// perform the default key handling first
		wxControl::OnKeyDown( event ) ;

        wxCommandEvent event(wxEVT_COMMAND_LISTBOX_SELECTED, m_windowId);
        event.SetEventObject( this );

        wxArrayInt aSelections;
        int n, count = GetSelections(aSelections);
        if ( count > 0 )
        {
               n = aSelections[0];
               if ( HasClientObjectData() )
                  event.SetClientObject( GetClientObject(n) );
              else if ( HasClientUntypedData() )
                  event.SetClientData( GetClientData(n) );
              event.SetString( GetString(n) );
        }
        else
        {
             n = -1;
        }

        event.m_commandInt = n;

        GetEventHandler()->ProcessEvent(event);
	}
	else
	{
	  if ( event.GetTimestamp() > m_lastTypeIn + 60 )
	  {
	    m_typeIn = "" ;
	  }
	  m_lastTypeIn = event.GetTimestamp() ;
	  m_typeIn += (char) event.KeyCode() ;
	  int line = FindString("*"+m_typeIn+"*") ;
	  if ( line >= 0 )
	  {
	  	if ( GetSelection() != line )
	  	{
	    	SetSelection(line) ;
	        wxCommandEvent event(wxEVT_COMMAND_LISTBOX_SELECTED, m_windowId);
	        event.SetEventObject( this );

            if ( HasClientObjectData() )
              event.SetClientObject( GetClientObject( line ) );
          	else if ( HasClientUntypedData() )
              event.SetClientData( GetClientData(line) );
          	event.SetString( GetString(line) );

	        event.m_commandInt = line ;

	        GetEventHandler()->ProcessEvent(event);
	    }
	  }
	}
}

