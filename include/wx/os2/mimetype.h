/////////////////////////////////////////////////////////////////////////////
// Name:        wx/mimetype.h
// Purpose:     classes and functions to manage MIME types
// Author:      David Webster
// Modified by:
// Created:     01.21.99
// RCS-ID:      $Id: MIMETYPE.H,v 1.3 2002/08/25 18:20:54 SN Exp $
// Copyright:   adopted from msw port -- (c) 1998 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows license (part of wxExtra library)
/////////////////////////////////////////////////////////////////////////////

#ifndef _MIMETYPE_IMPL_H
#define _MIMETYPE_IMPL_H

#include "wx/defs.h"

#include "wx/mimetype.h"


class WXDLLEXPORT wxFileTypeImpl
{
public:
    // ctor
    wxFileTypeImpl() { m_info = NULL; }

    // one of these Init() function must be called (ctor can't take any
    // arguments because it's common)

        // initialize us with our file type name and extension - in this case
        // we will read all other data from the registry
    void Init(const wxString& strFileType, const wxString& ext)
        { m_strFileType = strFileType; m_ext = ext; }

        // initialize us with a wxFileTypeInfo object - it contains all the
        // data
    void Init(const wxFileTypeInfo& info)
        { m_info = &info; }

    // implement accessor functions
    bool GetExtensions(wxArrayString& extensions);
    bool GetMimeType(wxString *mimeType) const;
    bool GetMimeTypes(wxArrayString& mimeTypes) const;
    bool GetIcon(wxIcon *icon, wxString *sCommand = NULL, int *iIndex = NULL) const;
    bool GetDescription(wxString *desc) const;
    bool GetOpenCommand(wxString *openCmd,
                        const wxFileType::MessageParameters& params) const;
    bool GetPrintCommand(wxString *printCmd,
                         const wxFileType::MessageParameters& params) const;

    size_t GetAllCommands(wxArrayString * verbs, wxArrayString * commands,
                          const wxFileType::MessageParameters& params) const;

    bool Unassociate();

    // set an arbitrary command, ask confirmation if it already exists and
    // overwriteprompt is TRUE
    bool SetCommand(const wxString& cmd,
                    const wxString& verb,
                    bool overwriteprompt = TRUE);

    bool SetDefaultIcon(const wxString& cmd = wxEmptyString, int index = 0);

    // this is called  by Associate
    bool SetDescription (const wxString& desc);

private:
    // helper function: reads the command corresponding to the specified verb
    // from the registry (returns an empty string if not found)
    wxString GetCommand(const wxChar *verb) const;

    // we use either m_info or read the data from the registry if m_info == NULL
    const wxFileTypeInfo *m_info;
    wxString m_strFileType,         // may be empty
             m_ext;
};



class WXDLLEXPORT wxMimeTypesManagerImpl
{
public:
    // nothing to do here, we don't load any data but just go and fetch it from
    // the registry when asked for
    wxMimeTypesManagerImpl() { }

    // implement containing class functions
    wxFileType *GetFileTypeFromExtension(const wxString& ext);
    wxFileType *GetOrAllocateFileTypeFromExtension(const wxString& ext);
    wxFileType *GetFileTypeFromMimeType(const wxString& mimeType);

    size_t EnumAllFileTypes(wxArrayString& mimetypes);

    // these are NOPs under Windows
    bool ReadMailcap(const wxString& filename, bool fallback = TRUE)
        { return TRUE; }
    bool ReadMimeTypes(const wxString& filename)
        { return TRUE; }

    void AddFallback(const wxFileTypeInfo& ft) { m_fallbacks.Add(ft); }

private:
    wxArrayFileTypeInfo m_fallbacks;
};


#endif
  //_MIMETYPE_IMPL_H

