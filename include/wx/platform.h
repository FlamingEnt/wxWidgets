/*
 * Name:        wx/platform.h
 * Purpose:     define the OS and compiler identification macros
 * Author:      Vadim Zeitlin
 * Modified by:
 * Created:     29.10.01 (extracted from wx/defs.h)
 * RCS-ID:      $Id: platform.h,v 1.8.4.4 2003/08/14 11:46:19 CE Exp $
 * Copyright:   (c) 1997-2001 wxWindows team
 * Licence:     wxWindows license
 */

/* THIS IS A C FILE, DON'T USE C++ FEATURES (IN PARTICULAR COMMENTS) IN IT */

#ifndef _WX_PLATFORM_H_
#define _WX_PLATFORM_H_


/*
    Codewarrior doesn't define any Windows symbols until some headers are included
*/
#if __MWERKS__
    #include <stddef.h>
    #if defined(WIN32) || defined(_WIN32)
        #ifndef WINVER
            #define WINVER  0x0400
        #endif
    #endif
#endif


/*
   first define Windows symbols if they're not defined on the command line: we
   can autodetect everything we need if _WIN32 is defined
 */
#if defined(_WIN32) || defined(WIN32) || defined(__NT__)
    #ifndef __WXMSW__
        #define __WXMSW__
    #endif

    #ifndef __WIN32__
        #define __WIN32__
    #endif

    /*
       see MSDN for the description of possible WINVER values, this one is a
       good default and, anyhow, we check for most of the features we use
       during run-time.
     */
    #ifndef WINVER
        #define WINVER  0x0400
    #endif

    /* Win95 means Win95-style UI, i.e. Win9x/NT 4+ */
    #if !defined(__WIN95__) && defined(WINVER) && (WINVER >= 0x0400)
        #define __WIN95__
    #endif
#endif /* Win32 */

#if defined(__WXMSW__) || defined(__WIN32__)
    #if !defined(__WINDOWS__)
        #define __WINDOWS__
    #endif
#endif

#ifdef __WXWINE__
    #ifndef __WIN32__
        #define __WIN32__
    #endif
    #ifndef __WIN95__
        #define __WIN95__
    #endif
    #ifndef STRICT
        #define STRICT
    #endif
#endif /* WINE */

#if defined(TWIN32) && !defined(__TWIN32__)
    #define __TWIN32__
#endif /* Twin32 */

/*
   Include wx/setup.h for the Unix platform defines generated by configure and
   the library compilation options
 */
#include "wx/setup.h"

/* check the consistency of the settings in setup.h */
#include "wx/chkconf.h"

/*
   some compilers don't support iostream.h any longer, so override the users
   setting here in such case.
 */
#if defined(_MSC_VER) && (_MSC_VER >= 1310)
    #undef wxUSE_IOSTREAMH
    #define wxUSE_IOSTREAMH 0
#endif /* compilers not supporting iostream.h */

/*
   old C++ headers (like <iostream.h>) declare classes in the global namespace
   while the new, standard ones (like <iostream>) do it in std:: namespace

   using this macro allows constuctions like "wxSTD iostream" to work in
   either case
 */
#if !wxUSE_IOSTREAMH
  #define wxSTD std::
#else
 #define wxSTD
#endif

/*
   OS: first of all, test for MS-DOS platform. We must do this before testing
       for Unix, because DJGPP compiler defines __unix__ under MS-DOS
 */
#if defined(__GO32__) || defined(__DJGPP__) || defined(__DOS__)
    #ifndef __DOS__
        #define __DOS__
    #endif
    /* size_t is the same as unsigned int for Watcom 11 compiler, */
    /* so define it if it hadn't been done by configure yet */
    #if !defined(wxSIZE_T_IS_UINT) && !defined(wxSIZE_T_IS_ULONG)
        #ifdef __WATCOMC__
            #define wxSIZE_T_IS_UINT
        #endif
        #ifdef __DJGPP__
            #define wxSIZE_T_IS_ULONG
        #endif
    #endif

/*
   OS: then test for generic Unix defines, then for particular flavours and
       finally for Unix-like systems
 */
#elif defined(__UNIX__) || defined(__unix) || defined(__unix__) || \
      defined(____SVR4____) || defined(__LINUX__) || defined(__sgi) || \
      defined(__hpux) || defined(sun) || defined(__SUN__) || defined(_AIX) || \
      defined(__EMX__) || defined(__VMS) || defined(__BEOS__)

    #define __UNIX_LIKE__

    /* Helps SGI compilation, apparently */
    #ifdef __SGI__
        #ifdef __GNUG__
            #define __need_wchar_t
        #else /* !gcc */
            /*
               Note I use the term __SGI_CC__ for both cc and CC, its not a good
               idea to mix gcc and cc/CC, the name mangling is different
             */
            #define __SGI_CC__
        #endif /* gcc/!gcc */
    #endif  /* SGI */

    #if defined(sun) || defined(__SUN__)
        #ifndef __GNUG__
            #ifndef __SUNCC__
                #define __SUNCC__
            #endif /* Sun CC */
        #endif
    #endif /* Sun */

    #ifdef __EMX__
        #define OS2EMX_PLAIN_CHAR
    #endif

    /* define __HPUX__ for HP-UX where standard macro is __hpux */
    #if defined(__hpux) && !defined(__HPUX__)
        #define __HPUX__
    #endif /* HP-UX */

    #if defined(__CYGWIN__)
        #if !defined(wxSIZE_T_IS_UINT)
            #define wxSIZE_T_IS_UINT
        #endif
    #endif
#elif defined(applec) || \
      defined(THINK_C) || \
      (defined(__MWERKS__) && !defined(__INTEL__))
      /* MacOS */
    #if !defined(wxSIZE_T_IS_UINT) && !defined(wxSIZE_T_IS_ULONG)
        #define wxSIZE_T_IS_ULONG
    #endif
#elif defined(__WXMAC__) && defined(__APPLE__)
    /* Mac OS X */
    #define __UNIX_LIKE__

    /*
      These defines are needed when compiling using Project Builder
      with a non generated setup0.h
    */
    #ifndef __UNIX__
        #define __UNIX__ 1
    #endif
    #ifndef __BSD__
        #define __BSD__ 1
    #endif
    #ifndef __DARWIN__
        #define __DARWIN__ 1
    #endif
    #ifndef __POWERPC__
        #define __POWERPC__ 1
    #endif

    #if !defined(wxSIZE_T_IS_UINT) && !defined(wxSIZE_T_IS_ULONG)
        #define wxSIZE_T_IS_ULONG
    #endif
    /*
       Some code has been added to workaround defects(?) in the
       bundled gcc compiler. These corrections are identified by
       __DARWIN__ for Darwin related corrections (wxMac, wxMotif)
     */
#elif defined(__OS2__)
    #if defined(__IBMCPP__)
        #define __VISAGEAVER__ __IBMCPP__
    #endif
    #ifndef __WXOS2__
        #define __WXOS2__
    #endif
    #ifndef __WXPM__
        #define __WXPM__
    #endif

    /* Place other OS/2 compiler environment defines here */
    #if defined(__VISAGECPP__)
        /* VisualAge is the only thing that understands _Optlink */
        #define LINKAGEMODE _Optlink
    #endif
    #define wxSIZE_T_IS_UINT

#else   /* Windows */
    #ifndef __WINDOWS__
        #define __WINDOWS__
    #endif  /* Windows */

    /* to be changed for Win64! */
    #ifndef __WIN32__
        #define __WIN16__
    #endif

    /*
       define another standard symbol for Microsoft Visual C++: the standard
       one (_MSC_VER) is also defined by Metrowerks compiler
     */
    #if defined(_MSC_VER) && !defined(__MWERKS__)
        #define __VISUALC__ _MSC_VER
    #elif defined(__BCPLUSPLUS__) && !defined(__BORLANDC__)
        #define __BORLANDC__
      #elif defined(__WATCOMC__)
    #elif defined(__SC__)
        #define __SYMANTECC__
    #endif  /* compiler */

    /* size_t is the same as unsigned int for all Windows compilers we know, */
    /* so define it if it hadn't been done by configure yet */
    #if !defined(wxSIZE_T_IS_UINT) && !defined(wxSIZE_T_IS_ULONG)
        #define wxSIZE_T_IS_UINT
    #endif
#endif  /* OS */

/*
   if we're on a Unix system but didn't use configure (so that setup.h didn't
   define __UNIX__), do define __UNIX__ now
 */
#if !defined(__UNIX__) && defined(__UNIX_LIKE__)
    #define __UNIX__
#endif /* Unix */

#if defined(__HPUX__) && !defined(__WXGTK__)
    #ifndef __WXMOTIF__
        #define __WXMOTIF__
    #endif /* __WXMOTIF__ */
#endif

#if defined(__WXMOTIF__) || defined(__WXX11__)
    #define __X__
#endif

#ifdef __SC__
    #ifdef __DMC__
         #define __DIGITALMARS__
    #else
         #define __SYMANTEC__
    #endif
#endif

/*
   This macro can be used to test the gcc version and can be used like this:

    #if wxCHECK_GCC_VERSION(3, 1)
        ... we have gcc 3.1 or later ...
    #else
        ... no gcc at all or gcc < 3.1 ...
    #endif
*/
#define wxCHECK_GCC_VERSION( major, minor ) \
    ( defined(__GNUC__) && defined(__GNUC_MINOR__) \
    && ( ( __GNUC__ > (major) ) \
        || ( __GNUC__ == (major) && __GNUC_MINOR__ >= (minor) ) ) )

/*
   This macro can be used to check that the version of mingw32 compiler is
   at least maj.min
 */
#if defined( __GNUWIN32__ ) || defined( __MINGW32__ ) || defined( __CYGWIN__ ) || defined(__DIGITALMARS__)
    #include "wx/msw/gccpriv.h"
#else
    #undef wxCHECK_W32API_VERSION
    #define wxCHECK_W32API_VERSION(maj, min) (0)
#endif

#endif /* _WX_PLATFORM_H_ */

