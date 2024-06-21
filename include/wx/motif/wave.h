/////////////////////////////////////////////////////////////////////////////
// Name:        wave.h
// Purpose:     wxWave class (loads and plays short Windows .wav files).
//              Optional on non-Windows platforms.
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id: wave.h,v 1.2 2002/02/05 16:34:31 JS Exp $
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WAVE_H_
#define _WX_WAVE_H_

#ifdef __GNUG__
#pragma interface "wave.h"
#endif

#include "wx/object.h"

class wxWave : public wxObject
{
public:
    wxWave();
    wxWave(const wxString& fileName, bool isResource = FALSE);
    ~wxWave();
    
public:
    bool  Create(const wxString& fileName, bool isResource = FALSE);
    bool  IsOk() const { return (m_waveData ? TRUE : FALSE); };
    bool  Play(bool async = TRUE, bool looped = FALSE) const;
    
protected:
    bool  Free();
    
private:
    char* m_waveData;
    int   m_waveLength;
    bool  m_isResource;
};

#endif
// _WX_WAVE_H_
