/////////////////////////////////////////////////////////////////////////////
// Name:        xh_split.cpp
// Purpose:     XRC resource for wxSplitterWindow
// Author:      panga@freemail.hu, Vaclav Slavik
// Created:     2003/01/26
// RCS-ID:      $Id: xh_split.cpp,v 1.1.2.1 2003/05/25 13:20:24 JS Exp $
// Copyright:   (c) 2003 panga@freemail.hu, Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////
 
#ifdef __GNUG__
#pragma implementation "xh_split.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/xrc/xh_split.h"
#include "wx/splitter.h"
#include "wx/log.h"

wxSplitterWindowXmlHandler::wxSplitterWindowXmlHandler() : wxXmlResourceHandler()
{
    XRC_ADD_STYLE(wxSP_3D);
    XRC_ADD_STYLE(wxSP_3DSASH);
    XRC_ADD_STYLE(wxSP_3DBORDER);
    XRC_ADD_STYLE(wxSP_FULLSASH);
    XRC_ADD_STYLE(wxSP_BORDER);
    XRC_ADD_STYLE(wxSP_NOBORDER);
    XRC_ADD_STYLE(wxSP_PERMIT_UNSPLIT);
    XRC_ADD_STYLE(wxSP_LIVE_UPDATE);
    AddWindowStyles();
}

wxObject *wxSplitterWindowXmlHandler::DoCreateResource()
{
    XRC_MAKE_INSTANCE(splitter, wxSplitterWindow);

    splitter->Create(m_parentAsWindow,
                     GetID(),
                     GetPosition(), GetSize(),
                     GetStyle(wxT("style"), wxSP_3D),
                     GetName());

    SetupWindow(splitter);

    long sashpos = GetLong(wxT("sashpos"), 0);
    long minpanesize = GetLong(wxT("minsize"), -1);
    if (minpanesize != -1)
         splitter->SetMinimumPaneSize(minpanesize);

    wxWindow *win1 = NULL, *win2 = NULL;
    wxXmlNode *n = m_node->GetChildren();
    while (n)
    {
        if ((n->GetType() == wxXML_ELEMENT_NODE) && 
            (n->GetName() == wxT("object") ||
             n->GetName() == wxT("object_ref")))
        {
            wxObject *created = CreateResFromNode(n, splitter, NULL);
            wxWindow *win = wxDynamicCast(created, wxWindow);
            if (win1 == NULL)
            {
                win1 = win;
            }
            else
            {
                win2 = win;
                break;
            }            
        }
        n = n->GetNext();
    }

    if (win1 == NULL)
        wxLogError(wxT("wxSplitterWindow node must contain at least one window."));

    bool horizontal = (GetParamValue(wxT("orientation")) != wxT("vertical"));
    if (win1 && win2)
    {
        if (horizontal)
            splitter->SplitHorizontally(win1, win2, sashpos);
        else
            splitter->SplitVertically(win1, win2, sashpos);
    }
    else
    {
        splitter->Initialize(win1);
    }

    return splitter;
}

bool wxSplitterWindowXmlHandler::CanHandle(wxXmlNode *node)
{
     return IsOfClass(node, wxT("wxSplitterWindow"));
}


