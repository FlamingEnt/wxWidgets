/////////////////////////////////////////////////////////////////////////////
// Name:        radiobox.cpp
// Purpose:     wxRadioBox
// Author:      AUTHOR
// Modified by: JS Lair (99/11/15) first implementation
// Created:     ??/??/98
// RCS-ID:      $Id: radiobox.cpp,v 1.24 2002/08/21 20:20:20 GD Exp $
// Copyright:   (c) AUTHOR
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------------------
// 		headers
//-------------------------------------------------------------------------------------

#ifdef __GNUG__
  #pragma implementation "radioboxbase.h"
  #pragma implementation "radiobox.h"
#endif

#include "wx/defs.h"

#include "wx/radiobox.h"
#include "wx/radiobut.h"
#include "wx/mac/uma.h"

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxRadioBox, wxControl)
#endif

#pragma mark -
#pragma mark ### Constructors & destructor ###

//-------------------------------------------------------------------------------------
// 		� wxRadioBox()
//-------------------------------------------------------------------------------------
// Default constructor
BEGIN_EVENT_TABLE(wxRadioBox, wxControl)
EVT_RADIOBUTTON( -1 , wxRadioBox::OnRadioButton )
END_EVENT_TABLE()

void wxRadioBox::OnRadioButton( wxCommandEvent &outer )
{
  if ( outer.IsChecked() )
  {
    wxCommandEvent event(wxEVT_COMMAND_RADIOBOX_SELECTED, m_windowId);
    int i = GetSelection() ;
    event.SetInt( i );
    event.SetString( GetString( i ) );
    event.SetEventObject( this );
    ProcessCommand(event);
  }
}

wxRadioBox::wxRadioBox()
{
    m_noItems = 0;
    m_noRowsOrCols = 0;
    m_majorDim = 0 ;
    m_radioButtonCycle = NULL;
}

//-------------------------------------------------------------------------------------
// 		� wxRadioBox(wxWindow*, wxWindowID, const wxString&, const wxPoint&, 
//						const wxSize&, int, const wxString[], int, long, 
//						const wxValidator&, const wxString&)
//-------------------------------------------------------------------------------------
// Contructor, creating and showing a radiobox
//
// inline defined
//

//-------------------------------------------------------------------------------------
// 		� ~wxRadioBox
//-------------------------------------------------------------------------------------
// Destructor, destroying the radiobox item

wxRadioBox::~wxRadioBox()
{
	wxRadioButton *next,*current;
	
    current=m_radioButtonCycle->NextInCycle();
    next=current->NextInCycle();
    while (current!=m_radioButtonCycle) {
    	delete current;
    	current=next;
    	next=current->NextInCycle();
    	}
    delete current;    
}

//-------------------------------------------------------------------------------------
// 		� Create
//-------------------------------------------------------------------------------------
// Create the radiobox for two-step construction

bool wxRadioBox::Create(wxWindow *parent, wxWindowID id, const wxString& label,
             const wxPoint& pos, const wxSize& size,
             int n, const wxString choices[],
             int majorDim, long style,
             const wxValidator& val, const wxString& name)
{
    int i;
    
    m_noItems = n;
    m_noRowsOrCols = majorDim;
    m_radioButtonCycle = NULL;

    if (majorDim==0)
        m_majorDim = n ;
    else
        m_majorDim = majorDim ;


    Rect bounds ;
    Str255 title ;
    
    MacPreControlCreate( parent , id ,  wxStripMenuCodes(label) , pos , size ,style, val , name , &bounds , title ) ;
    
    m_macControl = ::NewControl( MAC_WXHWND(parent->MacGetRootWindow()) , &bounds , title , false , 0 , 0 , 1, 
                                 kControlGroupBoxTextTitleProc , (long) this ) ;
    
    for (i = 0; i < n; i++)
    {
        wxRadioButton *radBtn = new wxRadioButton(this, NewControlId(), wxStripMenuCodes(choices[i]),wxPoint(5,20*i+10),
                                                  wxDefaultSize , i == 0 ? wxRB_GROUP : 0 ) ;
        if ( i == 0 )
          m_radioButtonCycle = radBtn ;
//        m_radioButtonCycle=radBtn->AddInCycle(m_radioButtonCycle);
    }

    SetSelection(0);
    MacPostControlCreate() ;

    return TRUE;
}


#pragma mark -
#pragma mark ### Specific functions (reference v2) ###

//-------------------------------------------------------------------------------------
// 		� Enable(bool)
//-------------------------------------------------------------------------------------
// Enables or disables the entire radiobox

bool wxRadioBox::Enable(bool enable)
{
    int i;
    wxRadioButton *current;
    
    if (!wxControl::Enable(enable))
    	return false;
    
    current = m_radioButtonCycle;
    for (i = 0; i < m_noItems; i++) {
    	current->Enable(enable);
    	current = current->NextInCycle();
    }
    return true;
}

//-------------------------------------------------------------------------------------
// 		� Enable(int, bool)
//-------------------------------------------------------------------------------------
// Enables or disables an given button

void wxRadioBox::Enable(int item, bool enable)
{
    int i;
    wxRadioButton *current;
    
    if ((item < 0) || (item >= m_noItems))
        return;
    
    i = 0;
    current = m_radioButtonCycle;
    while (i != item) {
    	i++;
    	current = current->NextInCycle();
    }
    current->Enable(enable);
}

//-------------------------------------------------------------------------------------
// 		� GetLabel()
//-------------------------------------------------------------------------------------
// Returns the radiobox label

wxString wxRadioBox::GetLabel() const
{
    return wxControl::GetLabel();
}

//-------------------------------------------------------------------------------------
// 		� GetLabel(int)
//-------------------------------------------------------------------------------------
// Returns the label for the given button

wxString wxRadioBox::GetString(int item) const
{
    int i;
    wxRadioButton *current;
    
    if ((item < 0) || (item >= m_noItems))
        return wxString("");
    
    i = 0;
    current = m_radioButtonCycle;
    while (i != item) {
    	i++;
    	current = current->NextInCycle();
    }
    return current->GetLabel();
}

//-------------------------------------------------------------------------------------
// 		� GetSelection
//-------------------------------------------------------------------------------------
// Returns the zero-based position of the selected button

int wxRadioBox::GetSelection() const
{
    int i;
    wxRadioButton *current;
    
    i=0;
    current=m_radioButtonCycle;
    while (!current->GetValue()) {
    	i++;
    	current=current->NextInCycle();
    	}

    return i;
}

//-------------------------------------------------------------------------------------
// 		� Number
//-------------------------------------------------------------------------------------
// Returns the number of buttons in the radiobox
//
// inline defined
//

//-------------------------------------------------------------------------------------
// 		� SetLabel(const wxString&)
//-------------------------------------------------------------------------------------
// Sets the radiobox label

void wxRadioBox::SetLabel(const wxString& label)
{
    return wxControl::SetLabel(label);
}

//-------------------------------------------------------------------------------------
// 		� SetLabel(int, const wxString&)
//-------------------------------------------------------------------------------------
// Sets the label of a given button

void wxRadioBox::SetString(int item,const wxString& label)
{
   	int i;
    wxRadioButton *current;
    
    if ((item < 0) || (item >= m_noItems))
        return;
	i=0;
    current=m_radioButtonCycle;
    while (i!=item) {
    	i++;
    	current=current->NextInCycle();
    	}
    return current->SetLabel(label);
}

//-------------------------------------------------------------------------------------
// 		� SetSelection
//-------------------------------------------------------------------------------------
// Sets a button by passing the desired position. This does not cause 
// wxEVT_COMMAND_RADIOBOX_SELECTED event to get emitted

void wxRadioBox::SetSelection(int item)
{
    int i;
    wxRadioButton *current;
    
    if ((item < 0) || (item >= m_noItems))
        return;
    i=0;
    current=m_radioButtonCycle;
    while (i!=item) {
    	i++;
    	current=current->NextInCycle();
    	}
    current->SetValue(true);
    
}

//-------------------------------------------------------------------------------------
// 		� Show(bool)
//-------------------------------------------------------------------------------------
// Shows or hides the entire radiobox 

bool wxRadioBox::Show(bool show)
{
    int i;
    wxRadioButton *current;
    
    wxControl::Show(show);

    current=m_radioButtonCycle;
   	for (i=0;i<m_noItems;i++) {
    	current->Show(show);
    	current=current->NextInCycle();
    	}
    return true;
}

//-------------------------------------------------------------------------------------
// 		� Show(int, bool)
//-------------------------------------------------------------------------------------
// Shows or hides the given button 

void wxRadioBox::Show(int item, bool show)
{
   	int i;
    wxRadioButton *current;
    
    if ((item < 0) || (item >= m_noItems))
        return;
    i=0;
    current=m_radioButtonCycle;
    while (i!=item) {
    	i++;
    	current=current->NextInCycle();
    	}
    current->Show(show);
}

#pragma mark -
#pragma mark ### Other external functions ###

//-------------------------------------------------------------------------------------
// 		� Command
//-------------------------------------------------------------------------------------
// Simulates the effect of the user issuing a command to the item

void wxRadioBox::Command (wxCommandEvent & event)
{
    SetSelection (event.GetInt());
    ProcessCommand (event);
}

//-------------------------------------------------------------------------------------
// 		� SetFocus
//-------------------------------------------------------------------------------------
// Sets the selected button to receive keyboard input

void wxRadioBox::SetFocus()
{
    int i;
    wxRadioButton *current;
    
    i=0;
    current=m_radioButtonCycle;
    while (!current->GetValue()) {
    	i++;
    	current=current->NextInCycle();
    	}
	current->SetFocus();
}


#pragma mark -
#pragma mark ### Internal functions ###

//-------------------------------------------------------------------------------------
// 		� DoSetSize
//-------------------------------------------------------------------------------------
// Simulates the effect of the user issuing a command to the item

#define RADIO_SIZE 40

void wxRadioBox::DoSetSize(int x, int y, int width, int height, int sizeFlags)
{
	int i;
	wxRadioButton *current;

// define the position

	int x_current, y_current;
	int x_offset,y_offset;
    int widthOld, heightOld;
    GetSize(&widthOld, &heightOld);

	x_offset = x;
	y_offset = y;
	GetPosition(&x_current, &y_current);
	if ((x == -1) && !(sizeFlags & wxSIZE_ALLOW_MINUS_ONE))
		x_offset = x_current;
	if ((y == -1)&& !(sizeFlags & wxSIZE_ALLOW_MINUS_ONE))
		y_offset = y_current;

// define size

	int charWidth,charHeight;
	int maxWidth,maxHeight;
	int eachWidth[128],eachHeight[128];
    int totWidth,totHeight;

	SetFont(GetParent()->GetFont());
	GetTextExtent(wxString("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"), &charWidth, &charHeight);
	charWidth/=52;

	maxWidth=-1;
	maxHeight=-1;
	for (i = 0 ; i < m_noItems; i++)
		{
		GetTextExtent(GetString(i), &eachWidth[i], &eachHeight[i]);
		eachWidth[i] = (int)(eachWidth[i] + RADIO_SIZE);
		eachHeight[i] = (int)((3*eachHeight[i])/2);
		if (maxWidth<eachWidth[i]) maxWidth = eachWidth[i];
		if (maxHeight<eachHeight[i]) maxHeight = eachHeight[i];
  		}

	totHeight = GetRowCount() * (maxHeight + charHeight/2) + charHeight*3/2;
	totWidth  = GetColumnCount() * (maxWidth + charWidth) + charWidth;

    // only change our width/height if asked for
    if ( width == -1 )
    {
        if ( sizeFlags & wxSIZE_AUTO_WIDTH )
            width = totWidth ;
        else
            width = widthOld;
    }

    if ( height == -1 )
    {
        if ( sizeFlags & wxSIZE_AUTO_HEIGHT )
            height = totHeight ;
        else
            height = heightOld;
    }

	wxControl::DoSetSize(x_offset,y_offset,width,height,wxSIZE_AUTO);

// arrange radiobuttons

	int x_start,y_start;
	
	
	x_start = charWidth;
	y_start = 15 ;
	x_offset = x_start;
	y_offset = y_start;
	
	current=m_radioButtonCycle;  
	for ( i = 0 ; i < m_noItems; i++)
		{
		if (i&&((i%m_majorDim)==0)) // not to do for the zero button!
    		{
      		if (m_windowStyle & wxRA_VERTICAL)
      			{
 			    x_offset += maxWidth + charWidth;
        		y_offset = y_start;
				}
      		else
      			{
				x_offset = x_start;
        		y_offset += maxHeight ; /*+ charHeight/2;*/
				}
			}

		current->SetSize(x_offset,y_offset,eachWidth[i],eachHeight[i]);
		current=current->NextInCycle();
		
		if (m_windowStyle & wxRA_SPECIFY_ROWS)
			y_offset += maxHeight ; /*+ charHeight/2;*/
		else
			x_offset += maxWidth + charWidth;
		}
}

wxSize wxRadioBox::DoGetBestSize() const
{
    int charWidth, charHeight;
    int maxWidth, maxHeight;
    int eachWidth, eachHeight;
    int totWidth, totHeight;

    wxFont font = GetParent()->GetFont();
    GetTextExtent(wxString("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"),
                  &charWidth, &charHeight, NULL, NULL, &font);
    charWidth /= 52;

   maxWidth = -1;
   maxHeight = -1;

   for (int i = 0 ; i < m_noItems; i++)
   {
        GetTextExtent(GetString(i), &eachWidth, &eachHeight);
        eachWidth  = (int)(eachWidth + RADIO_SIZE) ;
        eachHeight = (int)((3 * eachHeight) / 2);
        if (maxWidth < eachWidth)     maxWidth = eachWidth;
        if (maxHeight < eachHeight)   maxHeight = eachHeight;
    }

    totHeight = GetRowCount() * (maxHeight + charHeight/2) + charHeight * 3/2;
    totWidth  = GetColumnCount() * (maxWidth + charWidth) + charWidth;

    // handle radio box title as well
    GetTextExtent(GetTitle(), &eachWidth, NULL);
    eachWidth  = (int)(eachWidth + RADIO_SIZE) + 3 * charWidth ;
    if (totWidth < eachWidth)     
        totWidth = eachWidth;

   return wxSize(totWidth, totHeight);
}
//-------------------------------------------------------------------------------------
// 		� GetNumVer
//-------------------------------------------------------------------------------------
// return the number of buttons in the vertical direction

int wxRadioBox::GetRowCount() const
{
    if ( m_windowStyle & wxRA_SPECIFY_ROWS )
    {
        return m_majorDim;
    }
    else
    {
        return (m_noItems + m_majorDim - 1)/m_majorDim;
    }
}

//-------------------------------------------------------------------------------------
// 		� GetNumHor
//-------------------------------------------------------------------------------------
// return the number of buttons in the horizontal direction

int wxRadioBox::GetColumnCount() const
{
    if ( m_windowStyle & wxRA_SPECIFY_ROWS )
    {
        return (m_noItems + m_majorDim - 1)/m_majorDim;
    }
    else
    {
        return m_majorDim;
    }
}





