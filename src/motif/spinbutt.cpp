/////////////////////////////////////////////////////////////////////////////
// Name:        spinbutt.cpp
// Purpose:     wxSpinButton
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id: spinbutt.cpp,v 1.15.2.1 2003/01/07 21:40:39 MBN Exp $
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
    #pragma implementation "spinbutt.h"
#endif

#include "wx/spinbutt.h"
#include "wx/spinctrl.h"
#include "wx/timer.h"

#ifdef __VMS__
#pragma message disable nosimpint
#endif
#include <Xm/ArrowBG.h>
#include <Xm/ArrowB.h>
#ifdef __VMS__
#pragma message enable nosimpint
#endif

#include "wx/motif/private.h"

// helper class
enum ArrowDirection
{
    wxARROW_UP,
    wxARROW_DOWN,
    wxARROW_LEFT,
    wxARROW_RIGHT
};

class wxArrowButtonTimer;
class wxArrowButton;

// ----------------------------------------------------------------------------
// wxArrowButtonTimer
// ----------------------------------------------------------------------------

static const unsigned int TICK_BEFORE_START = 10;
static const unsigned int TICK_BEFORE_EXPONENTIAL = 40;
static const unsigned int MAX_INCREMENT = 150;
static const unsigned int TICK_INTERVAL = 113;

class wxArrowButtonTimer : public wxTimer
{
public:
    wxArrowButtonTimer( wxArrowButton* btn, int sign )
        : m_sign( sign ),
          m_button( btn )
        { Reset(); };

    void Notify();
    void Reset() { m_ticks = 0; m_increment = 1; }
private:
    unsigned int m_ticks;
    unsigned int m_increment;
    int m_sign;
    wxArrowButton* m_button;
};

// ----------------------------------------------------------------------------
// wxArrowButton
// ----------------------------------------------------------------------------

class wxArrowButton : public wxControl
{
    friend class wxArrowButtonTimer;
public:
    wxArrowButton( int increment ) 
        : m_increment( increment ),
          m_timer( 0 ) {}

    wxArrowButton( wxSpinButton* parent, wxWindowID id, ArrowDirection d,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxDefaultSize, int increment = 1 )
        : wxControl(),
          m_increment( increment ),
          m_timer( 0 )
    {
        Create( parent, id, d, pos, size );
    }

    ~wxArrowButton()
        { delete m_timer; }

    bool Create( wxSpinButton* parent, wxWindowID id, ArrowDirection d,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize );
private:
    // creates a new timer object, or stops the currently running one
    wxTimer* GetFreshTimer();
    wxSpinButton* GetSpinButton() { return (wxSpinButton*)GetParent(); }
    static void SpinButtonCallback( Widget w, XtPointer clientData,
                                    XtPointer WXUNUSED(ptr) );
    static void StartTimerCallback( Widget w, XtPointer clientData,
                                    XtPointer WXUNUSED(ptr) );

    static void  StopTimerCallback( Widget w, XtPointer clientData,
                                    XtPointer WXUNUSED(ptr) );

    int m_increment;
    wxArrowButtonTimer* m_timer;
};

// ----------------------------------------------------------------------------
// wxArrowButtonTimer implementation
// ----------------------------------------------------------------------------

void wxArrowButtonTimer::Notify()
{
    ++m_ticks;
    if( m_ticks < TICK_BEFORE_START ) return;
    // increment every other tick
    if( m_ticks <= TICK_BEFORE_EXPONENTIAL && m_ticks & 1 )
        return;
    if( m_ticks > TICK_BEFORE_EXPONENTIAL )
        m_increment = 2 * m_increment;
    if( m_increment >= MAX_INCREMENT ) m_increment = MAX_INCREMENT;
    m_button->GetSpinButton()->Increment( m_sign * m_increment );
}

// ----------------------------------------------------------------------------
// wxArrowButton implementation
// ----------------------------------------------------------------------------

wxTimer* wxArrowButton::GetFreshTimer()
{
    if( m_timer )
    {
        m_timer->Stop();
        m_timer->Reset();
    }
    else
        m_timer = new wxArrowButtonTimer( this, m_increment );

    return m_timer;
}

void wxArrowButton::SpinButtonCallback( Widget w, XtPointer clientData,
                                        XtPointer WXUNUSED(ptr) )
{
    if( !wxGetWindowFromTable( w ) )
        // Widget has been deleted!
        return;

    wxArrowButton* btn = (wxArrowButton*)clientData;

    btn->GetSpinButton()->Increment( btn->m_increment );
}

void wxArrowButton::StartTimerCallback( Widget w, XtPointer clientData,
                                        XtPointer WXUNUSED(ptr) )
{
    if( !wxGetWindowFromTable( w ) )
        // Widget has been deleted!
        return;

    wxArrowButton* btn = (wxArrowButton*)clientData;
    btn->GetFreshTimer()->Start( TICK_INTERVAL );
}

void wxArrowButton::StopTimerCallback( Widget w, XtPointer clientData,
                                       XtPointer WXUNUSED(ptr) )
{
    if( !wxGetWindowFromTable( w ) )
        // Widget has been deleted!
        return;

    wxArrowButton* btn = (wxArrowButton*)clientData;
    delete btn->m_timer;
    btn->m_timer = 0;
}

bool wxArrowButton::Create( wxSpinButton* parent, wxWindowID id,
                            ArrowDirection d,
                            const wxPoint& pos, const wxSize& size )
{
    int arrow_dir;

    switch( d )
    {
    case wxARROW_UP:
        arrow_dir = XmARROW_UP;
        break;
    case wxARROW_DOWN:
        arrow_dir = XmARROW_DOWN;
        break;
    case wxARROW_LEFT:
        arrow_dir = XmARROW_LEFT;
        break;
    case wxARROW_RIGHT:
        arrow_dir = XmARROW_RIGHT;
        break;
    }

    if( parent ) parent->AddChild( this );

    Widget parentWidget = (Widget) parent->GetClientWidget();
    m_mainWidget = (WXWidget) XtVaCreateManagedWidget( "XmArrowButton",
        xmArrowButtonWidgetClass,
        parentWidget,
        XmNarrowDirection, arrow_dir,
        XmNborderWidth, 0,
        NULL );

    XtAddCallback( (Widget) m_mainWidget,
                   XmNactivateCallback, (XtCallbackProc) SpinButtonCallback,
                   (XtPointer) this );
    XtAddCallback( (Widget) m_mainWidget,
                   XmNarmCallback, (XtCallbackProc) StartTimerCallback,
                   (XtPointer) this );
    XtAddCallback( (Widget) m_mainWidget,
                   XmNactivateCallback, (XtCallbackProc) StopTimerCallback,
                   (XtPointer) this );

    AttachWidget( parent, m_mainWidget, (WXWidget) NULL,
                  pos.x, pos.y, size.x, size.y );

    return TRUE;
}

// ----------------------------------------------------------------------------
// wxSpinButton
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxSpinButton, wxControl);
IMPLEMENT_DYNAMIC_CLASS(wxSpinEvent, wxNotifyEvent);

static void CalcSizes( wxPoint pt, wxSize sz,
                       wxPoint& pt1, wxSize& sz1,
                       wxPoint& pt2, wxSize& sz2,
                       bool isVertical )
{
    typedef int wxSize::* CDPTR1;
    typedef int wxPoint::* CDPTR2;

    sz1 = sz2 = sz;
    pt2 = pt1 = pt;

    CDPTR1 szm = isVertical ? &wxSize::y : &wxSize::x;
    CDPTR2 ptm = isVertical ? &wxPoint::y : &wxPoint::x;
    int dim = sz.*szm, half = dim/2;

    sz1.*szm = half;
    sz2.*szm = dim - half;
    pt2.*ptm += half + 1;
}

bool wxSpinButton::Create( wxWindow *parent, wxWindowID id,
                           const wxPoint& pos, const wxSize& size,
                           long style, const wxString& name )
{
    m_windowStyle = style;

    wxSize newSize = GetBestSize();
    if( size.x != -1 ) newSize.x = size.x;
    if( size.y != -1 ) newSize.y = size.y;

    if( !wxControl::Create( parent, id, pos, newSize, style ) )
    {
        return FALSE;
    }

    SetName(name);

    InitBase();

    m_windowId = ( id == -1 ) ? NewControlId() : id;

    bool isVert = IsVertical();
    wxPoint pt1, pt2;
    wxSize sz1, sz2;
    CalcSizes( wxPoint(0,0), newSize, pt1, sz1, pt2, sz2, isVert );
    m_up = new wxArrowButton( this, -1, isVert ? wxARROW_UP : wxARROW_RIGHT,
                              pt1, sz1, 1 );
    m_down = new wxArrowButton( this, -1,
                                isVert ? wxARROW_DOWN : wxARROW_LEFT,
                                pt2, sz2, -1 );

    return TRUE;
}

wxSpinButton::~wxSpinButton()
{
}

void wxSpinButton::DoMoveWindow(int x, int y, int width, int height)
{
    wxControl::DoMoveWindow( x, y, width, height );

    wxPoint pt1, pt2;
    wxSize sz1, sz2;

    CalcSizes( wxPoint(0,0), wxSize(width,height), pt1,
               sz1, pt2, sz2, IsVertical() );
    m_up->SetSize( pt1.x, pt1.y, sz1.x, sz1.y );
    m_down->SetSize( pt2.x, pt2.y, sz2.x, sz2.y );
}

void wxSpinButton::DoSetSize(int x, int y, int width, int height,
                             int sizeFlags)
{
#ifdef __VMS__
#pragma message disable codcauunr
#endif
    if( sizeFlags & wxSIZE_USE_EXISTING && width == -1 )
        width = GetSize().x;
    if( sizeFlags & wxSIZE_USE_EXISTING && height == -1 )
        height = GetSize().y;
#ifdef __VMS__
#pragma message enable codcauunr
#endif

    wxControl::DoSetSize(x, y, width, height, 0);
}

void wxSpinButton::Increment( int delta )
{
    if( m_pos < m_min ) m_pos = m_min;
    if( m_pos > m_max ) m_pos = m_max;

    int npos = m_pos + delta;

    if( npos < m_min )
    {
        if( GetWindowStyle() & wxSP_WRAP )
            npos = m_max;
        else
            npos = m_min;
    }
    if( npos > m_max )
    {
        if( GetWindowStyle() & wxSP_WRAP )
            npos = m_min;
        else
            npos = m_max;
    }
    if( npos == m_pos ) return;

    wxSpinEvent event( delta > 0 ? wxEVT_SCROLL_LINEUP : wxEVT_SCROLL_LINEDOWN,
                       m_windowId );
    event.SetPosition( npos );
    event.SetEventObject( this );

    GetEventHandler()->ProcessEvent( event );

    if( event.IsAllowed() )
    {
        m_pos = npos;
        event.SetEventType( wxEVT_SCROLL_THUMBTRACK );
        event.SetPosition( m_pos );

        GetEventHandler()->ProcessEvent( event );
    }
}

wxSize wxSpinButton::DoGetBestSize() const
{
    return IsVertical() ? wxSize( 24, 34 ) : wxSize( 34, 24 );
}

// Attributes
////////////////////////////////////////////////////////////////////////////

int wxSpinButton::GetValue() const
{
    return m_pos;
}

void wxSpinButton::SetValue(int val)
{
    m_pos = val;
}

void wxSpinButton::SetRange(int minVal, int maxVal)
{
    wxSpinButtonBase::SetRange(minVal, maxVal);
}

void wxSpinButton::ChangeFont(bool WXUNUSED(keepOriginalSize))
{
    // TODO
}

void wxSpinButton::ChangeBackgroundColour()
{
    wxControl::ChangeBackgroundColour();
}

void wxSpinButton::ChangeForegroundColour()
{
    // TODO
}
