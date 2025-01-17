/////////////////////////////////////////////////////////////////////////////
// Name:        listbox.h
// Purpose:     wxListBox class
// Author:      David Webster
// Modified by:
// Created:     10/09/99
// RCS-ID:      $Id: LISTBOX.H,v 1.11.2.1 2002/12/27 14:49:42 JS Exp $
// Copyright:   (c) David Webster
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_LISTBOX_H_
#define _WX_LISTBOX_H_

// ----------------------------------------------------------------------------
// simple types
// ----------------------------------------------------------------------------

#if wxUSE_OWNER_DRAWN
  class WXDLLEXPORT wxOwnerDrawn;

  // define the array of list box items
  #include  <wx/dynarray.h>

  WX_DEFINE_EXPORTED_ARRAY(wxOwnerDrawn *, wxListBoxItemsArray);
#endif // wxUSE_OWNER_DRAWN

// forward decl for GetSelections()
class wxArrayInt;

// ----------------------------------------------------------------------------
// List box control
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxListBox : public wxListBoxBase
{
public:
    // ctors and such
    wxListBox();
    wxListBox( wxWindow*          pParent
              ,wxWindowID         vId
              ,const wxPoint&     rPos = wxDefaultPosition
              ,const wxSize&      rSize = wxDefaultSize
              ,int                n = 0
              ,const wxString     asChoices[] = NULL
              ,long               lStyle = 0
              ,const wxValidator& rValidator = wxDefaultValidator
              ,const wxString&    rsName = wxListBoxNameStr)
    {
        Create( pParent
               ,vId
               ,rPos
               ,rSize
               ,n
               ,asChoices
               ,lStyle
               ,rValidator
               ,rsName
              );
    }

    bool Create( wxWindow*          pParent
                ,wxWindowID         vId
                ,const wxPoint&     rPos = wxDefaultPosition
                ,const wxSize&      rSize = wxDefaultSize
                ,int                n = 0
                ,const wxString     asChoices[] = NULL
                ,long               lStyle = 0
                ,const wxValidator& rValidator = wxDefaultValidator
                ,const wxString&    rsName = wxListBoxNameStr
               );

    virtual ~wxListBox();

    //
    // Implement base class pure virtuals
    //
    virtual void          Clear(void);
    virtual void          Delete(int n);

    virtual int           GetCount(void) const;
    virtual wxString      GetString(int n) const;
    virtual void          SetString( int             n
                                    ,const wxString& rsString
                                   );
    virtual int           FindString(const wxString& rsString) const;

    virtual bool          IsSelected(int n) const;
    virtual void          SetSelection( int  n
                                       ,bool bSelect = TRUE
                                      );
    virtual int           GetSelection(void) const;
    virtual int           GetSelections(wxArrayInt& raSelections) const;

    virtual int           DoAppend(const wxString& rsItem);
    virtual void          DoInsertItems( const wxArrayString& raItems
                                        ,int                  rPos
                                       );
    virtual void          DoSetItems( const wxArrayString& raItems
                                     ,void **              ppClientData
                                    );

    virtual void          DoSetFirstItem(int n);

    virtual void          DoSetItemClientData( int   n
                                              ,void* pClientData
                                             );
    virtual void*         DoGetItemClientData(int n) const;
    virtual void          DoSetItemClientObject( int           n
                                                ,wxClientData* pClientData
                                               );
    virtual wxClientData* DoGetItemClientObject(int n) const;

    //
    // wxCheckListBox support
    //
#if wxUSE_OWNER_DRAWN
    long                  OS2OnMeasure(WXMEASUREITEMSTRUCT *item);
    bool                  OS2OnDraw(WXDRAWITEMSTRUCT *item);

    virtual wxOwnerDrawn* CreateItem(size_t n);
    wxOwnerDrawn*         GetItem(size_t n) const { return m_aItems[n]; }
    int                   GetItemIndex(wxOwnerDrawn *item) const { return m_aItems.Index(item); }
#endif // wxUSE_OWNER_DRAWN

    bool                  OS2Command( WXUINT uParam
                                     ,WXWORD wId
                                    );
    virtual void          SetupColours(void);

protected:

    bool                  HasMultipleSelection(void) const;
    virtual wxSize        DoGetBestSize(void) const;

    int                             m_nNumItems;
    int                             m_nSelected;


#if wxUSE_OWNER_DRAWN
    //
    // Control items
    //
    wxListBoxItemsArray             m_aItems;
#endif

private:
#if wxUSE_WX_RESOURCES
#  if wxUSE_OWNER_DRAWN
    virtual wxControl*    CreateItem( const wxItemResource*  pChildResource
                                     ,const wxItemResource*  pParentResource
                                     ,const wxResourceTable* pTable = (const wxResourceTable *) NULL
                                    )
    {
        return(wxWindowBase::CreateItem( pChildResource
                                        ,pParentResource
                                        ,pTable
                                       )
              );
    }
#  endif
#endif
    DECLARE_DYNAMIC_CLASS(wxListBox)
}; // end of wxListBox

#endif
    // _WX_LISTBOX_H_
