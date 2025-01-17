/////////////////////////////////////////////////////////////////////////////
// Name:        window.h
// Purpose:     wxWindow class
// Author:      David Webster
// Modified by:
// Created:     10/12/99
// RCS-ID:      $Id: WINDOW.H,v 1.45 2002/09/03 22:26:27 DW Exp $
// Copyright:   (c) David Webster
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WINDOW_H_
#define _WX_WINDOW_H_

#define wxUSE_MOUSEEVENT_HACK 0

// ---------------------------------------------------------------------------
// headers
// ---------------------------------------------------------------------------
#define INCL_DOS
#define INCL_PM
#define INCL_GPI
#include <os2.h>


// ---------------------------------------------------------------------------
// forward declarations
// ---------------------------------------------------------------------------
#ifndef CW_USEDEFAULT
#  define  CW_USEDEFAULT ((int)0x80000000)
#endif

// ---------------------------------------------------------------------------
// forward declarations
// ---------------------------------------------------------------------------

class WXDLLEXPORT wxButton;

// ---------------------------------------------------------------------------
// constants
// ---------------------------------------------------------------------------

// FIXME does anybody use those? they're unused by wxWindows...
enum
{
    wxKEY_SHIFT = 1,
    wxKEY_CTRL  = 2
};

// ---------------------------------------------------------------------------
// wxWindow declaration for OS/2 PM
// ---------------------------------------------------------------------------

class WXDLLEXPORT wxWindowOS2 : public wxWindowBase
{
public:
    wxWindowOS2()
    {
        Init();
    }

    wxWindowOS2( wxWindow*       pParent
                ,wxWindowID      vId
                ,const wxPoint&  rPos = wxDefaultPosition
                ,const wxSize&   rSize = wxDefaultSize
                ,long            lStyle = 0
                ,const wxString& rName = wxPanelNameStr
               )
    {
        Init();
        Create( pParent
               ,vId
               ,rPos
               ,rSize
               ,lStyle
               ,rName
              );
    }

    virtual ~wxWindowOS2();

    bool Create( wxWindow*       pParent
                ,wxWindowID      vId
                ,const wxPoint&  rPos = wxDefaultPosition
                ,const wxSize&   rSize = wxDefaultSize
                ,long            lStyle = 0
                ,const wxString& rName = wxPanelNameStr
               );

    // implement base class pure virtuals
    virtual void     SetTitle(const wxString& rTitle);
    virtual wxString GetTitle(void) const;
    virtual void     Raise(void);
    virtual void     Lower(void);
    virtual bool     Show(bool bShow = TRUE);
    virtual bool     Enable(bool bEnable = TRUE);
    virtual void     SetFocus(void);
    virtual void     SetFocusFromKbd(void);
    virtual bool     Reparent(wxWindow* pNewParent);
    virtual void     WarpPointer( int x
                                 ,int y
                                );
    virtual void     Refresh( bool          bEraseBackground = TRUE
                             ,const wxRect* pRect = (const wxRect *)NULL
                            );
    virtual void     Clear(void);
    virtual void     Freeze(void);
    virtual void     Update(void);
    virtual void     Thaw(void);
    virtual void     SetWindowStyleFlag(long lStyle);
    virtual bool     SetCursor(const wxCursor& rCursor);
    virtual bool     SetFont(const wxFont& rFont);
    virtual int      GetCharHeight(void) const;
    virtual int      GetCharWidth(void) const;
    virtual void     GetTextExtent( const wxString& rString
                                   ,int*            pX
                                   ,int*            pY
                                   ,int*            pDescent = (int *)NULL
                                   ,int*            pExternalLeading = (int *)NULL
                                   ,const wxFont*   pTheFont = (const wxFont *)NULL
                                  ) const;
#if wxUSE_MENUS_NATIVE
    virtual bool     DoPopupMenu( wxMenu* pMenu
                                 ,int     nX
                                 ,int     nY
                                );
#endif // wxUSE_MENUS_NATIVE

    virtual void     SetScrollbar( int  nOrient
                                  ,int  nPos
                                  ,int  nThumbVisible
                                  ,int  nRange
                                  ,bool bRefresh = TRUE
                                 );
    virtual void     SetScrollPos( int  nOrient
                                  ,int  nPos
                                  ,bool bRefresh = TRUE
                                 );
    virtual int      GetScrollPos(int nOrient) const;
    virtual int      GetScrollThumb(int nOrient) const;
    virtual int      GetScrollRange(int nOrient) const;
    virtual void     ScrollWindow( int           nDx
                                  ,int           nDy
                                  ,const wxRect* pRect = (wxRect *)NULL
                                 );

    inline HWND                   GetScrollBarHorz(void) const {return m_hWndScrollBarHorz;}
    inline HWND                   GetScrollBarVert(void) const {return m_hWndScrollBarVert;};
#if wxUSE_DRAG_AND_DROP
    virtual void SetDropTarget(wxDropTarget* pDropTarget);
#endif // wxUSE_DRAG_AND_DROP

    // Accept files for dragging
    virtual void DragAcceptFiles(bool bAccept);

#if WXWIN_COMPATIBILITY
    // Set/get scroll attributes
    virtual void SetScrollRange( int  nOrient
                                ,int  nRange
                                ,bool bRefresh = TRUE
                               );
    virtual void SetScrollPage( int  nOrient
                               ,int  nPage
                               ,bool bRefresh = TRUE
                              );
    virtual int  OldGetScrollRange(int nOrient) const;
    virtual int  GetScrollPage(int nOrient) const;

    //
    // event handlers
    //
        // Handle a control command
    virtual void OnCommand( wxWindow&       rWin
                           ,wxCommandEvent& rEvent
                          );

        // Override to define new behaviour for default action (e.g. double
        // clicking on a listbox)
    virtual void OnDefaultAction(wxControl* WXUNUSED(pInitiatingItem)) { }
#endif // WXWIN_COMPATIBILITY

#if wxUSE_CARET && WXWIN_COMPATIBILITY
    void CreateCaret( int nWidth
                     ,int nHeight
                    );
    void CreateCaret(const wxBitmap* pBitmap);
    void DestroyCaret(void);
    void ShowCaret(bool bShow);
    void SetCaretPos( int nX
                     ,int nY
                    );
    void GetCaretPos( int* pX
                     ,int* pY
                    ) const;
#endif // wxUSE_CARET

#ifndef __WXUNIVERSAL__
    // Native resource loading (implemented in src/os2/nativdlg.cpp)
    // FIXME: should they really be all virtual?
    virtual bool LoadNativeDialog( wxWindow*   pParent
                                  ,wxWindowID& vId
                                 );
    virtual bool LoadNativeDialog( wxWindow*       pParent
                                  ,const wxString& rName
                                 );
    wxWindow*    GetWindowChild1(wxWindowID vId);
    wxWindow*    GetWindowChild(wxWindowID vId);
#endif //__WXUNIVERSAL__

    // implementation from now on
    // --------------------------

    // simple accessors
    // ----------------

    WXHWND           GetHWND(void) const { return m_hWnd; }
    void             SetHWND(WXHWND hWnd) { m_hWnd = hWnd; }
    virtual WXWidget GetHandle(void) const { return GetHWND(); }
    bool             GetUseCtl3D(void) const { return m_bUseCtl3D; }
    bool             GetTransparentBackground(void) const { return m_bBackgroundTransparent; }
    void             SetTransparent(bool bT = TRUE) { m_bBackgroundTransparent = bT; }

    // event handlers
    // --------------
    void OnSetFocus(wxFocusEvent& rEvent);
    void OnEraseBackground(wxEraseEvent& rEvent);
    void OnIdle(wxIdleEvent& rEvent);

public:

    // For implementation purposes - sometimes decorations make the client area
    // smaller
    virtual wxPoint GetClientAreaOrigin(void) const;

    // Windows subclassing
    void SubclassWin(WXHWND hWnd);
    void UnsubclassWin(void);

    WXFARPROC OS2GetOldWndProc(void) const { return m_fnOldWndProc; }
    void OS2SetOldWndProc(WXFARPROC fnProc) { m_fnOldWndProc = fnProc; }
    //
    // Return TRUE if the window is of a standard (i.e. not wxWindows') class
    //
    bool IsOfStandardClass(void) const { return m_fnOldWndProc != NULL; }

    wxWindow* FindItem(long lId) const;
    wxWindow* FindItemByHWND( WXHWND hWnd
                             ,bool   bControlOnly = FALSE
                            ) const;

    // Make a Windows extended style from the given wxWindows window style ?? applicable to OS/2??
    static WXDWORD MakeExtendedStyle( long lStyle
                                     ,bool bEliminateBorders = TRUE
                                    );
    // Determine whether 3D effects are wanted
    WXDWORD Determine3DEffects( WXDWORD dwDefaultBorderStyle
                               ,bool*   pbWant3D
                              ) const;

    // PM only: TRUE if this control is part of the main control
    virtual bool ContainsHWND(WXHWND WXUNUSED(hWnd)) const { return FALSE; };

    // translate wxWindows style flags for this control into the PM style
    // and optional extended style for the corresponding native control
    //
    // this is the function that should be overridden in the derived classes,
    // but you will mostly use OS2GetCreateWindowFlags() below
    virtual WXDWORD OS2GetStyle( long     lFlags
                                ,WXDWORD* pdwExstyle = NULL
                               ) const;

    // get the MSW window flags corresponding to wxWindows ones
    //
    // the functions returns the flags (WS_XXX) directly and puts the ext
    // (WS_EX_XXX) flags into the provided pointer if not NULL
    WXDWORD OS2GetCreateWindowFlags(WXDWORD* pdwExflags = NULL) const
        { return OS2GetStyle(GetWindowStyle(), pdwExflags); }


    // get the HWND to be used as parent of this window with CreateWindow()
    virtual WXHWND OS2GetParent(void) const;

    // returns TRUE if the window has been created
    bool         OS2Create( PSZ            zClass
                           ,const char*    zTitle
                           ,WXDWORD        dwStyle
                           ,const wxPoint& rPos
                           ,const wxSize&  rSize
                           ,void*          pCtlData
                           ,WXDWORD        dwExStyle
                           ,bool           bIsChild
                          );
    virtual bool OS2Command( WXUINT uParam
                            ,WXWORD nId
                           );

#if WXWIN_COMPATIBILITY
    wxObject*    GetChild(int nNumber) const;
    virtual void OS2DeviceToLogical( float* pfX
                                    ,float* pfY
                                   ) const;
#endif // WXWIN_COMPATIBILITY

#ifndef __WXUNIVERSAL__
    // Create an appropriate wxWindow from a HWND
    virtual wxWindow* CreateWindowFromHWND( wxWindow* pParent
                                           ,WXHWND    hWnd
                                          );

    // Make sure the window style reflects the HWND style (roughly)
    virtual void AdoptAttributesFromHWND(void);
#endif

    // Setup background and foreground colours correctly
    virtual void SetupColours(void);

    // ------------------------------------------------------------------------
    // helpers for message handlers: these perform the same function as the
    // message crackers from <windowsx.h> - they unpack WPARAM and LPARAM into
    // the correct parameters
    // ------------------------------------------------------------------------

    void UnpackCommand( WXWPARAM wParam
                       ,WXLPARAM lParam,
                        WXWORD*  pId
                       ,WXHWND*  pHwnd
                       ,WXWORD*  pCmd
                      );
    void UnpackActivate( WXWPARAM wParam
                        ,WXLPARAM lParam
                        ,WXWORD*  pState
                        ,WXHWND*  pHwnd
                       );
    void UnpackScroll( WXWPARAM wParam
                      ,WXLPARAM lParam
                      ,WXWORD*  pCode
                      ,WXWORD*  pPos
                      ,WXHWND*  pHwnd
                     );
    void UnpackMenuSelect( WXWPARAM wParam
                          ,WXLPARAM lParam
                          ,WXWORD*  pTtem
                          ,WXWORD*  pFlags
                          ,WXHMENU* pHmenu
                         );

    // ------------------------------------------------------------------------
    // internal handlers for OS2 messages: all handlers return a boolen value:
    // TRUE means that the handler processed the event and FALSE that it didn't
    // ------------------------------------------------------------------------

    // there are several cases where we have virtual functions for PM
    // message processing: this is because these messages often require to be
    // processed in a different manner in the derived classes. For all other
    // messages, however, we do *not* have corresponding OS2OnXXX() function
    // and if the derived class wants to process them, it should override
    // OS2WindowProc() directly.

    // scroll event (both horizontal and vertical)
    virtual bool OS2OnScroll( int    nOrientation
                             ,WXWORD nSBCode
                             ,WXWORD pos
                             ,WXHWND control
                            );

    // owner-drawn controls need to process these messages
    virtual bool OS2OnDrawItem( int               nId
                               ,WXDRAWITEMSTRUCT* pItem
                              );
    virtual long OS2OnMeasureItem( int                  nId
                                  ,WXMEASUREITEMSTRUCT* pItem
                                 );

    // the rest are not virtual
    bool HandleCreate( WXLPCREATESTRUCT vCs
                      ,bool*            pMayCreate
                     );
    bool HandleInitDialog(WXHWND hWndFocus);
    bool HandleDestroy(void);
    bool HandlePaint(void);
    bool HandleEraseBkgnd(WXHDC vDC);
    bool HandleMinimize(void);
    bool HandleMaximize(void);
    bool HandleSize( int    nX
                    ,int    nY
                    ,WXUINT uFlag
                   );
    bool HandleGetMinMaxInfo(PSWP pMmInfo);
    bool HandleShow( bool bShow
                    ,int  nStatus
                   );
    bool HandleActivate( int    nFlag
                        ,WXHWND hActivate
                       );
    bool HandleCommand( WXWORD nId
                       ,WXWORD nCmd
                       ,WXHWND hControl
                      );
    bool HandleSysCommand( WXWPARAM wParam
                          ,WXLPARAM lParam
                         );
    bool HandlePaletteChanged(void);
    bool HandleQueryNewPalette(void);
    bool HandleSysColorChange(void);
    bool HandleDisplayChange(void);
    bool HandleCaptureChanged(WXHWND hBainedCapture);

    bool HandleCtlColor(WXHBRUSH* hBrush);
    bool HandleSetFocus(WXHWND hWnd);
    bool HandleKillFocus(WXHWND hWnd);
    bool HandleEndDrag(WXWPARAM wParam);
    bool HandleMouseEvent( WXUINT uMsg
                          ,int    nX
                          ,int    nY
                          ,WXUINT uFlags
                         );
    bool HandleMouseMove( int    nX
                         ,int    nY
                         ,WXUINT uFlags
                        );
    bool HandleChar( WXWPARAM wParam
                    ,WXLPARAM lParam
                    ,bool     bIsASCII = FALSE
                   );
    bool HandleKeyDown( WXWPARAM wParam
                       ,WXLPARAM lParam
                      );
    bool HandleKeyUp( WXWPARAM wParam
                     ,WXLPARAM lParam
                    );
    bool HandleQueryDragIcon(WXHICON* phIcon);
    bool HandleSetCursor( USHORT vId
                         ,WXHWND hWnd
                        );

    bool IsMouseInWindow(void) const;
    bool OS2GetCreateWindowCoords( const wxPoint& rPos
                                  ,const wxSize&  rSize
                                  ,int&           rnX
                                  ,int&           rnY
                                  ,int&           rnWidth
                                  ,int&           rnHeight
                                 ) const;

    // Window procedure
    virtual MRESULT OS2WindowProc( WXUINT   uMsg
                                  ,WXWPARAM wParam
                                  ,WXLPARAM lParam
                                 );

    // Calls an appropriate default window procedure
    virtual MRESULT OS2DefWindowProc( WXUINT   uMsg
                                     ,WXWPARAM wParam
                                     ,WXLPARAM lParam
                                    );
    virtual bool    OS2ProcessMessage(WXMSG* pMsg);
    virtual bool    OS2ShouldPreProcessMessage(WXMSG* pMsg);
    virtual bool    OS2TranslateMessage(WXMSG* pMsg);
    virtual void    OS2DestroyWindow(void);

    // this function should return the brush to paint the window background
    // with or 0 for the default brush
    virtual WXHBRUSH OnCtlColor( WXHDC    hDC
                                ,WXHWND   hWnd
                                ,WXUINT   uCtlColor
                                ,WXUINT   uMessage
                                ,WXWPARAM wParam
                                ,WXLPARAM lParam
                               );

#if WXWIN_COMPATIBILITY
    void SetShowing(bool bShow) { (void)Show(show); }
    bool IsUserEnabled(void) const { return IsEnabled(); }
#endif // WXWIN_COMPATIBILITY

    // Responds to colour changes: passes event on to children.
    void OnSysColourChanged(wxSysColourChangedEvent& rEvent);

    // initialize various fields of wxMouseEvent (common part of OS2OnMouseXXX)
    void InitMouseEvent( wxMouseEvent& rEvent
                        ,int           nX
                        ,int           nY
                        ,WXUINT        uFlags
                       );

    void MoveChildren(int nDiff);
    PSWP GetSwp(void) {return &m_vWinSwp;}

protected:
    // PM can't create some MSW styles natively but can perform these after
    // creation by sending messages
    typedef enum extra_flags { kFrameToolWindow = 0x0001
                              ,kVertCaption     = 0x0002
                              ,kHorzCaption     = 0x0004
                             } EExtraFlags;
    // Some internal sizeing id's to make it easy for event handlers
    typedef enum size_types { kSizeNormal
                             ,kSizeMax
                             ,kSizeMin
                            } ESizeTypes;
    // the window handle
    WXHWND                          m_hWnd;

    // the old window proc (we subclass all windows)
    WXFARPROC                       m_fnOldWndProc;

    // additional (OS2 specific) flags
    bool                            m_bUseCtl3D:1; // Using CTL3D for this control
    bool                            m_bBackgroundTransparent:1;
    bool                            m_bMouseInWindow:1;
    bool                            m_bLastKeydownProcessed:1;
    bool                            m_bWinCaptured:1;
    WXDWORD                         m_dwExStyle;

    // the size of one page for scrolling
    int                             m_nXThumbSize;
    int                             m_nYThumbSize;

#if wxUSE_MOUSEEVENT_HACK
    // the coordinates of the last mouse event and the type of it
    long                            m_lLastMouseX,
    long                            m_lLastMouseY;
    int                             m_nLastMouseEvent;
#endif // wxUSE_MOUSEEVENT_HACK

    WXHMENU                         m_hMenu; // Menu, if any
    unsigned long                   m_ulMenubarId; // it's Id, if any

    // the return value of WM_GETDLGCODE handler
    long                            m_lDlgCode;

    // implement the base class pure virtuals
    virtual void DoClientToScreen( int* pX
                                  ,int* pY
                                 ) const;
    virtual void DoScreenToClient( int* pX
                                  ,int* pY
                                 ) const;
    virtual void DoGetPosition( int* pX
                               ,int* pY
                              ) const;
    virtual void DoGetSize( int* pWidth
                           ,int* pHeight
                          ) const;
    virtual void DoGetClientSize( int* pWidth
                                 ,int* pHeight
                                ) const;
    virtual void DoSetSize( int nX
                           ,int nY
                           ,int nWidth
                           ,int nHeight
                           ,int nSizeFlags = wxSIZE_AUTO
                          );
    virtual void DoSetClientSize( int nWidth
                                 ,int nHeight
                                );

    virtual void     DoCaptureMouse(void);
    virtual void     DoReleaseMouse(void);

    // move the window to the specified location and resize it: this is called
    // from both DoSetSize() and DoSetClientSize() and would usually just call
    // ::WinSetWindowPos() except for composite controls which will want to arrange
    // themselves inside the given rectangle
    virtual void DoMoveWindow( int nX
                              ,int nY
                              ,int nWidth
                              ,int nHeight
                             );

#if wxUSE_TOOLTIPS
    virtual void DoSetToolTip(wxToolTip* pTip);
#endif // wxUSE_TOOLTIPS

    int  GetOS2ParentHeight(wxWindowOS2* pParent);

private:
    // common part of all ctors
    void Init(void);

    // the (non-virtual) handlers for the events
    bool HandleMove( int nX
                    ,int nY
                   );
    bool HandleJoystickEvent( WXUINT uMsg
                             ,int    pX
                             ,int    pY
                             ,WXUINT uFlags
                            );

    bool HandleNotify( int       nIdCtrl
                      ,WXLPARAM  lParam
                      ,WXLPARAM* pResult
                     );
    // the helper functions used by HandleChar/KeyXXX methods
    wxKeyEvent CreateKeyEvent( wxEventType evType
                              ,int         nId
                              ,WXLPARAM    lParam = 0
                              ,WXWPARAM    wParam = 0
                             ) const;

    wxWindowList*                   m_pChildrenDisabled;
    HWND                            m_hWndScrollBarHorz;
    HWND                            m_hWndScrollBarVert;
    SWP                             m_vWinSwp;

    DECLARE_DYNAMIC_CLASS(wxWindowOS2);
    DECLARE_NO_COPY_CLASS(wxWindowOS2)
    DECLARE_EVENT_TABLE()

    //
    // Virtual function hiding supression
    //
    inline virtual bool Reparent(wxWindowBase* pNewParent)
    { return(wxWindowBase::Reparent(pNewParent));}
}; // end of wxWindow

class wxWindowCreationHook
{
public:
    wxWindowCreationHook(wxWindow* pWinBeingCreated);
    ~wxWindowCreationHook();
}; // end of CLASS wxWindowCreationHook

// ---------------------------------------------------------------------------
// global functions
// ---------------------------------------------------------------------------

// kbd code translation
WXDLLEXPORT int wxCharCodeOS2ToWX(int nKeySym);
WXDLLEXPORT int wxCharCodeWXToOS2( int   nId
                                  ,bool* pbIsVirtual
                                 );
#endif
    // _WX_WINDOW_H_
