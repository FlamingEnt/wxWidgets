/////////////////////////////////////////////////////////////////////////////
// Name:        glcanvas.h
// Purpose:     wxGLCanvas, for using OpenGL/Mesa with wxWindows and GTK
// Author:      Robert Roebling
// Modified by:
// Created:     17/8/98
// RCS-ID:      $Id: glcanvas.h,v 1.6.2.1 2002/12/02 21:21:23 VZ Exp $
// Copyright:   (c) Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "glcanvas.h"
#endif

#ifndef _WX_GLCANVAS_H_
#define _WX_GLCANVAS_H_

#include "wx/scrolwin.h"
#include "wx/app.h"

extern "C" {
#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glu.h>
}

//---------------------------------------------------------------------------
// Constants for attriblist
//---------------------------------------------------------------------------

enum
{
  WX_GL_RGBA=1,          /* use true color palette */
  WX_GL_BUFFER_SIZE,     /* bits for buffer if not WX_GL_RGBA */
  WX_GL_LEVEL,           /* 0 for main buffer, >0 for overlay, <0 for underlay */
  WX_GL_DOUBLEBUFFER,    /* use doublebuffer */
  WX_GL_STEREO,          /* use stereoscopic display */
  WX_GL_AUX_BUFFERS,     /* number of auxiliary buffers */
  WX_GL_MIN_RED,         /* use red buffer with most bits (> MIN_RED bits) */
  WX_GL_MIN_GREEN,       /* use green buffer with most bits (> MIN_GREEN bits) */
  WX_GL_MIN_BLUE,        /* use blue buffer with most bits (> MIN_BLUE bits) */
  WX_GL_MIN_ALPHA,       /* use blue buffer with most bits (> MIN_ALPHA bits) */
  WX_GL_DEPTH_SIZE,      /* bits for Z-buffer (0,16,32) */
  WX_GL_STENCIL_SIZE,    /* bits for stencil buffer */
  WX_GL_MIN_ACCUM_RED,   /* use red accum buffer with most bits (> MIN_ACCUM_RED bits) */
  WX_GL_MIN_ACCUM_GREEN, /* use green buffer with most bits (> MIN_ACCUM_GREEN bits) */
  WX_GL_MIN_ACCUM_BLUE,  /* use blue buffer with most bits (> MIN_ACCUM_BLUE bits) */
  WX_GL_MIN_ACCUM_ALPHA  /* use blue buffer with most bits (> MIN_ACCUM_ALPHA bits) */
};

//---------------------------------------------------------------------------
// classes
//---------------------------------------------------------------------------

class WXDLLEXPORT wxGLContext;
class WXDLLEXPORT wxGLCanvas;

//---------------------------------------------------------------------------
// wxGLContext
//---------------------------------------------------------------------------


class WXDLLEXPORT wxGLContext: public wxObject
{
public:
    wxGLContext( bool isRGB, wxWindow *win, const wxPalette& palette = wxNullPalette );
    wxGLContext(
               bool WXUNUSED(isRGB), wxWindow *win,
               const wxPalette& WXUNUSED(palette),
               const wxGLContext *other        /* for sharing display lists */
    );
    ~wxGLContext();

    void SetCurrent();
    void SetColour(const wxChar *colour);
    void SwapBuffers();

    void SetupPixelFormat();
    void SetupPalette(const wxPalette& palette);
    wxPalette CreateDefaultPalette();

    inline wxPalette* GetPalette() const { return (wxPalette*) & m_palette; }
    inline wxWindow* GetWindow() const { return m_window; }
    inline GtkWidget* GetWidget() const { return m_widget; }
    inline GLXContext GetContext() const { return m_glContext; }

public:
   GLXContext       m_glContext;

   GtkWidget       *m_widget;
   wxPalette        m_palette;
   wxWindow*        m_window;

private:
  DECLARE_CLASS(wxGLContext)
};

//---------------------------------------------------------------------------
// wxGLCanvas
//---------------------------------------------------------------------------

class WXDLLEXPORT wxGLCanvas: public wxWindow
{
public:
   inline wxGLCanvas() {
      m_glContext = (wxGLContext*) NULL;
      m_sharedContext = (wxGLContext*) NULL;
      m_glWidget = (GtkWidget*) NULL;
      m_vi = (void*) NULL;
      m_exposed = FALSE;
   }
   wxGLCanvas( wxWindow *parent, wxWindowID id = -1,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = 0, const wxString& name = wxGLCanvasName,
        int *attribList = (int*) NULL,
        const wxPalette& palette = wxNullPalette );
   wxGLCanvas( wxWindow *parent, const wxGLContext *shared = (wxGLContext *)NULL,
        wxWindowID id = -1,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = 0, const wxString& name = wxGLCanvasName,
        int *attribList = (int*) NULL,
        const wxPalette& palette = wxNullPalette );
   wxGLCanvas( wxWindow *parent, const wxGLCanvas *shared = (wxGLCanvas *)NULL,
        wxWindowID id = -1,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = 0, const wxString& name = wxGLCanvasName,
        int *attribList = (int*) NULL,
        const wxPalette& palette = wxNullPalette );

   bool Create( wxWindow *parent,
                const wxGLContext *shared = (wxGLContext*)NULL,
                const wxGLCanvas *shared_context_of = (wxGLCanvas*)NULL,
                wxWindowID id = -1,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxString& name = wxGLCanvasName,
                int *attribList = (int*) NULL,
                const wxPalette& palette = wxNullPalette );

   ~wxGLCanvas();

   void SetCurrent();
   void SetColour(const wxChar *colour);
   void SwapBuffers();

   void OnSize(wxSizeEvent& event);

   void OnInternalIdle();

   inline wxGLContext* GetContext() const { return m_glContext; }

 // implementation

    wxGLContext      *m_glContext,
                     *m_sharedContext;
    wxGLCanvas       *m_sharedContextOf;
    void             *m_vi; // actually an XVisualInfo*
    bool              m_canFreeVi;
    GtkWidget        *m_glWidget;
    bool              m_exposed;

    // returns an XVisualInfo* based on desired GL attributes;
    // returns NULL if an appropriate visual is not found. The
    // caller is reponsible for using XFree() to deallocate
    // the returned structure.
    static void* ChooseGLVisual(int *attribList);

private:
    DECLARE_EVENT_TABLE()
    DECLARE_CLASS(wxGLCanvas)
};

#endif
    // _WX_GLCANVAS_H_

