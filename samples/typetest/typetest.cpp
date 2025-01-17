/////////////////////////////////////////////////////////////////////////////
// Name:        typetest.cpp
// Purpose:     Types wxWindows sample
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id: typetest.cpp,v 1.34.2.2 2002/12/14 18:20:07 MBN Exp $
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:       wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "typetest.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/time.h"
#include "wx/date.h"
#include "wx/variant.h"
#include "wx/mimetype.h"

#include "typetest.h"

#if defined(__WXGTK__) || defined(__WXX11__) || defined(__WXMOTIF__) || defined(__WXMAC__) || defined(__WXMGL__)
#include "mondrian.xpm"
#endif

#ifdef new
#undef new
#endif

#include "wx/ioswrap.h"

#if wxUSE_IOSTREAMH
    #include <fstream.h>
#else
    #include <fstream>
#endif

#include "wx/wfstream.h"
#include "wx/datstrm.h"
#include "wx/txtstrm.h"
#include "wx/mstream.h"

// Create a new application object
IMPLEMENT_APP    (MyApp)

IMPLEMENT_DYNAMIC_CLASS    (MyApp, wxApp)

BEGIN_EVENT_TABLE(MyApp, wxApp)
#if wxUSE_TIMEDATE
    EVT_MENU(TYPES_DATE,      MyApp::DoDateDemo)
    EVT_MENU(TYPES_TIME,      MyApp::DoTimeDemo)
#endif // wxUSE_TIMEDATE
    EVT_MENU(TYPES_VARIANT,   MyApp::DoVariantDemo)
    EVT_MENU(TYPES_BYTEORDER, MyApp::DoByteOrderDemo)
#if wxUSE_UNICODE
    EVT_MENU(TYPES_UNICODE,   MyApp::DoUnicodeDemo)
#endif // wxUSE_UNICODE
    EVT_MENU(TYPES_STREAM, MyApp::DoStreamDemo)
    EVT_MENU(TYPES_STREAM2, MyApp::DoStreamDemo2)
    EVT_MENU(TYPES_STREAM3, MyApp::DoStreamDemo3)
    EVT_MENU(TYPES_STREAM4, MyApp::DoStreamDemo4)
    EVT_MENU(TYPES_STREAM5, MyApp::DoStreamDemo5)
    EVT_MENU(TYPES_STREAM6, MyApp::DoStreamDemo6)
    EVT_MENU(TYPES_STREAM7, MyApp::DoStreamDemo7)
    EVT_MENU(TYPES_MIME, MyApp::DoMIMEDemo)
END_EVENT_TABLE()

bool MyApp::OnInit()
{
    // Create the main frame window
    MyFrame *frame = new MyFrame((wxFrame *) NULL, _T("wxWindows Types Demo"),
                                 wxPoint(50, 50), wxSize(450, 340));

    // Give it an icon
    frame->SetIcon(wxICON(mondrian));

    // Make a menubar
    wxMenu *file_menu = new wxMenu;

    file_menu->Append(TYPES_ABOUT, _T("&About"));
    file_menu->AppendSeparator();
    file_menu->Append(TYPES_QUIT, _T("E&xit\tAlt-X"));

    wxMenu *test_menu = new wxMenu;
#if wxUSE_TIMEDATE
    test_menu->Append(TYPES_DATE, _T("&Date test"));
    test_menu->Append(TYPES_TIME, _T("&Time test"));
#endif // wxUSE_TIMEDATE
    test_menu->Append(TYPES_VARIANT, _T("&Variant test"));
    test_menu->Append(TYPES_BYTEORDER, _T("&Byteorder test"));
#if wxUSE_UNICODE
    test_menu->Append(TYPES_UNICODE, _T("&Unicode test"));
#endif // wxUSE_UNICODE
    test_menu->Append(TYPES_STREAM, _T("&Stream test"));
    test_menu->Append(TYPES_STREAM2, _T("&Stream seek test"));
    test_menu->Append(TYPES_STREAM3, _T("&Stream error test"));
    test_menu->Append(TYPES_STREAM4, _T("&Stream buffer test"));
    test_menu->Append(TYPES_STREAM5, _T("&Stream peek test"));
    test_menu->Append(TYPES_STREAM6, _T("&Stream ungetch test"));
    test_menu->Append(TYPES_STREAM7, _T("&Stream ungetch test for a buffered stream"));
    test_menu->AppendSeparator();
    test_menu->Append(TYPES_MIME, _T("&MIME database test"));

    wxMenuBar *menu_bar = new wxMenuBar;
    menu_bar->Append(file_menu, _T("&File"));
    menu_bar->Append(test_menu, _T("&Tests"));
    frame->SetMenuBar(menu_bar);

    m_textCtrl = new wxTextCtrl(frame, -1, _T(""), wxPoint(0, 0), wxDefaultSize, wxTE_MULTILINE);

    // Show the frame
    frame->Show(TRUE);

    SetTopWindow(frame);

    return TRUE;
}

void MyApp::DoStreamDemo(wxCommandEvent& WXUNUSED(event))
{
    wxTextCtrl& textCtrl = * GetTextCtrl();

    textCtrl.Clear();
    textCtrl << _T("\nTest fstream vs. wxFileStream:\n\n");

    textCtrl.WriteText( _T("Writing to ofstream and wxFileOutputStream:\n") );

    wxSTD ofstream std_file_output( "test_std.dat" );
    wxFileOutputStream file_output( wxString(_T("test_wx.dat")) );
    wxBufferedOutputStream buf_output( file_output );
    wxTextOutputStream text_output( buf_output );

    wxString tmp;
    signed int si = 0xFFFFFFFF;
    tmp.Printf( _T("Signed int: %d\n"), si );
    textCtrl.WriteText( tmp );
    text_output << si << _T("\n");
    std_file_output << si << _T("\n");

    unsigned int ui = 0xFFFFFFFF;
    tmp.Printf( _T("Unsigned int: %u\n"), ui );
    textCtrl.WriteText( tmp );
    text_output << ui << _T("\n");
    std_file_output << ui << _T("\n");

    double d = 2.01234567890123456789;
    tmp.Printf( _T("Double: %f\n"), d );
    textCtrl.WriteText( tmp );
    text_output << d << _T("\n");
    std_file_output << d << _T("\n");

    float f = (float)0.00001;
    tmp.Printf( _T("Float: %f\n"), f );
    textCtrl.WriteText( tmp );
    text_output << f << _T("\n");
    std_file_output << f << _T("\n");

    wxString str( _T("Hello!") );
    tmp.Printf( _T("String: %s\n"), str.c_str() );
    textCtrl.WriteText( tmp );
    text_output << str << _T("\n");
    std_file_output << str.c_str() << _T("\n");

    textCtrl.WriteText( _T("\nReading from ifstream:\n") );

    wxSTD ifstream std_file_input( "test_std.dat" );

    std_file_input >> si;
    tmp.Printf( _T("Signed int: %d\n"), si );
    textCtrl.WriteText( tmp );

    std_file_input >> ui;
    tmp.Printf( _T("Unsigned int: %u\n"), ui );
    textCtrl.WriteText( tmp );

    std_file_input >> d;
    tmp.Printf( _T("Double: %f\n"), d );
    textCtrl.WriteText( tmp );

    std_file_input >> f;
    tmp.Printf( _T("Float: %f\n"), f );
    textCtrl.WriteText( tmp );

    // Why doesn't this work?
#if 0
    char std_buf[200];
    std_file_input >> std_buf;
    tmp.Printf( _T("String: %s\n"), std_buf );
    textCtrl.WriteText( tmp );
#endif

    textCtrl.WriteText( _T("\nReading from wxFileInputStream:\n") );

    buf_output.Sync();

    wxFileInputStream file_input( wxString(_T("test_wx.dat")) );
    wxBufferedInputStream buf_input( file_input );
    wxTextInputStream text_input( file_input );

    text_input >> si;
    tmp.Printf( _T("Signed int: %d\n"), si );
    textCtrl.WriteText( tmp );

    text_input >> ui;
    tmp.Printf( _T("Unsigned int: %u\n"), ui );
    textCtrl.WriteText( tmp );

    text_input >> d;
    tmp.Printf( _T("Double: %f\n"), d );
    textCtrl.WriteText( tmp );

    text_input >> f;
    tmp.Printf( _T("Float: %f\n"), f );
    textCtrl.WriteText( tmp );

    text_input >> str;
    tmp.Printf( _T("String: %s\n"), str.c_str() );
    textCtrl.WriteText( tmp );



    textCtrl << _T("\nTest for wxDataStream:\n\n");

    textCtrl.WriteText( _T("Writing to wxDataOutputStream:\n") );

    file_output.SeekO( 0 );
    wxDataOutputStream data_output( buf_output );

    wxInt16 i16 = (unsigned short)0xFFFF;
    tmp.Printf( _T("Signed int16: %d\n"), (int)i16 );
    textCtrl.WriteText( tmp );
    data_output.Write16( i16 );

    wxUint16 ui16 = 0xFFFF;
    tmp.Printf( _T("Unsigned int16: %u\n"), (unsigned int) ui16 );
    textCtrl.WriteText( tmp );
    data_output.Write16( ui16 );

    d = 2.01234567890123456789;
    tmp.Printf( _T("Double: %f\n"), d );
    textCtrl.WriteText( tmp );
    data_output.WriteDouble( d );

    str = _T("Hello!");
    tmp.Printf( _T("String: %s\n"), str.c_str() );
    textCtrl.WriteText( tmp );
    data_output.WriteString( str );

    buf_output.Sync();

    textCtrl.WriteText( _T("\nReading from wxDataInputStream:\n") );

    file_input.SeekI( 0 );
    wxDataInputStream data_input( buf_input );

    i16 = data_input.Read16();
    tmp.Printf( _T("Signed int16: %d\n"), (int)i16 );
    textCtrl.WriteText( tmp );

    ui16 = data_input.Read16();
    tmp.Printf( _T("Unsigned int16: %u\n"), (unsigned int) ui16 );
    textCtrl.WriteText( tmp );

    d = data_input.ReadDouble();
    tmp.Printf( _T("Double: %f\n"), d );
    textCtrl.WriteText( tmp );

    str = data_input.ReadString();
    tmp.Printf( _T("String: %s\n"), str.c_str() );
    textCtrl.WriteText( tmp );
}

void MyApp::DoStreamDemo2(wxCommandEvent& WXUNUSED(event))
{
    wxTextCtrl& textCtrl = * GetTextCtrl();

    textCtrl.Clear();
    textCtrl << _T("\nTesting wxBufferedStream:\n\n");

    char ch,ch2;

    textCtrl.WriteText( _T("Writing number 0 to 9 to buffered wxFileOutputStream:\n\n") );

    wxFileOutputStream file_output( wxString(_T("test_wx.dat")) );
    wxBufferedOutputStream buf_output( file_output );
    for (ch = 0; ch < 10; ch++)
        buf_output.Write( &ch, 1 );
    buf_output.Sync();

    wxFileInputStream file_input( wxString(_T("test_wx.dat")) );
    for (ch2 = 0; ch2 < 10; ch2++)
    {
        file_input.Read( &ch, 1 );
        textCtrl.WriteText( (wxChar)(ch + _T('0')) );
    }
    textCtrl.WriteText( _T("\n\n\n") );

    textCtrl.WriteText( _T("Writing number 0 to 9 to buffered wxFileOutputStream, then\n") );
    textCtrl.WriteText( _T("seeking back to #3 and writing 0:\n\n") );

    wxFileOutputStream file_output2( wxString(_T("test_wx2.dat")) );
    wxBufferedOutputStream buf_output2( file_output2 );
    for (ch = 0; ch < 10; ch++)
        buf_output2.Write( &ch, 1 );
    buf_output2.SeekO( 3 );
    ch = 0;
    buf_output2.Write( &ch, 1 );
    buf_output2.Sync();

    wxFileInputStream file_input2( wxString(_T("test_wx2.dat")) );
    for (ch2 = 0; ch2 < 10; ch2++)
    {
        file_input2.Read( &ch, 1 );
        textCtrl.WriteText( (wxChar)(ch + _T('0')) );
    }
    textCtrl.WriteText( _T("\n\n\n") );

    // now append 2000 bytes to file (bigger than buffer)
    buf_output2.SeekO( 0, wxFromEnd );
    ch = 1;
    for (int i = 0; i < 2000; i++)
       buf_output2.Write( &ch, 1 );
    buf_output2.Sync();

    textCtrl.WriteText( _T("Reading number 0 to 9 from buffered wxFileInputStream, then\n") );
    textCtrl.WriteText( _T("seeking back to #3 and reading the 0:\n\n") );

    wxFileInputStream file_input3( wxString(_T("test_wx2.dat")) );
    wxBufferedInputStream buf_input3( file_input3 );
    for (ch2 = 0; ch2 < 10; ch2++)
    {
        buf_input3.Read( &ch, 1 );
        textCtrl.WriteText( (wxChar)(ch + _T('0')) );
    }
    for (int j = 0; j < 2000; j++)
       buf_input3.Read( &ch, 1 );
    textCtrl.WriteText( _T("\n") );
    buf_input3.SeekI( 3 );
    buf_input3.Read( &ch, 1 );
    textCtrl.WriteText( (wxChar)(ch + _T('0')) );
    textCtrl.WriteText( _T("\n\n\n") );

}

void MyApp::DoStreamDemo3(wxCommandEvent& WXUNUSED(event))
{
    wxTextCtrl& textCtrl = * GetTextCtrl();

    textCtrl.Clear();
    textCtrl << _T("\nTesting wxFileInputStream's and wxFFileInputStream's error handling:\n\n");

    char ch,ch2;

    textCtrl.WriteText( _T("Writing number 0 to 9 to wxFileOutputStream:\n\n") );

    wxFileOutputStream file_output( wxString(_T("test_wx.dat")) );
    for (ch = 0; ch < 10; ch++)
        file_output.Write( &ch, 1 );

    // Testing wxFileInputStream

    textCtrl.WriteText( _T("Reading 0 to 10 to wxFileInputStream:\n\n") );

    wxFileInputStream file_input( wxString(_T("test_wx.dat")) );
    for (ch2 = 0; ch2 < 11; ch2++)
    {
        file_input.Read( &ch, 1 );
        textCtrl.WriteText( _T("Value read: ") );
        textCtrl.WriteText( (wxChar)(ch + '0') );
        textCtrl.WriteText( _T(";  stream.GetLastError() returns: ") );
        switch (file_input.GetLastError())
        {
            case wxSTREAM_NO_ERROR:      textCtrl.WriteText( _T("wxSTREAM_NO_ERROR\n") ); break;
            case wxSTREAM_EOF:          textCtrl.WriteText( _T("wxSTREAM_EOF\n") ); break;
            case wxSTREAM_READ_ERROR:   textCtrl.WriteText( _T("wxSTREAM_READ_ERROR\n") ); break;
            case wxSTREAM_WRITE_ERROR:  textCtrl.WriteText( _T("wxSTREAM_WRITE_ERROR\n") ); break;
            default: textCtrl.WriteText( _T("Huh?\n") ); break;
        }
    }
    textCtrl.WriteText( _T("\n") );

    textCtrl.WriteText( _T("Seeking to 0;  stream.GetLastError() returns: ") );
    file_input.SeekI( 0 );
    switch (file_input.GetLastError())
    {
        case wxSTREAM_NO_ERROR:      textCtrl.WriteText( _T("wxSTREAM_NO_ERROR\n") ); break;
        case wxSTREAM_EOF:          textCtrl.WriteText( _T("wxSTREAM_EOF\n") ); break;
        case wxSTREAM_READ_ERROR:   textCtrl.WriteText( _T("wxSTREAM_READ_ERROR\n") ); break;
        case wxSTREAM_WRITE_ERROR:  textCtrl.WriteText( _T("wxSTREAM_WRITE_ERROR\n") ); break;
        default: textCtrl.WriteText( _T("Huh?\n") ); break;
    }
    textCtrl.WriteText( _T("\n") );

    file_input.Read( &ch, 1 );
    textCtrl.WriteText( _T("Value read: ") );
    textCtrl.WriteText( (wxChar)(ch + _T('0')) );
    textCtrl.WriteText( _T(";  stream.GetLastError() returns: ") );
    switch (file_input.GetLastError())
    {
        case wxSTREAM_NO_ERROR:      textCtrl.WriteText( _T("wxSTREAM_NO_ERROR\n") ); break;
        case wxSTREAM_EOF:          textCtrl.WriteText( _T("wxSTREAM_EOF\n") ); break;
        case wxSTREAM_READ_ERROR:   textCtrl.WriteText( _T("wxSTREAM_READ_ERROR\n") ); break;
        case wxSTREAM_WRITE_ERROR:  textCtrl.WriteText( _T("wxSTREAM_WRITE_ERROR\n") ); break;
        default: textCtrl.WriteText( _T("Huh?\n") ); break;
    }
    textCtrl.WriteText( _T("\n\n") );


    // Testing wxFFileInputStream

    textCtrl.WriteText( _T("Reading 0 to 10 to wxFFileInputStream:\n\n") );

    wxFFileInputStream ffile_input( wxString(_T("test_wx.dat")) );
    for (ch2 = 0; ch2 < 11; ch2++)
    {
        ffile_input.Read( &ch, 1 );
        textCtrl.WriteText( _T("Value read: ") );
        textCtrl.WriteText( (wxChar)(ch + '0') );
        textCtrl.WriteText( _T(";  stream.GetLastError() returns: ") );
        switch (ffile_input.GetLastError())
        {
            case wxSTREAM_NO_ERROR:      textCtrl.WriteText( _T("wxSTREAM_NO_ERROR\n") ); break;
            case wxSTREAM_EOF:          textCtrl.WriteText( _T("wxSTREAM_EOF\n") ); break;
            case wxSTREAM_READ_ERROR:   textCtrl.WriteText( _T("wxSTREAM_READ_ERROR\n") ); break;
            case wxSTREAM_WRITE_ERROR:  textCtrl.WriteText( _T("wxSTREAM_WRITE_ERROR\n") ); break;
            default: textCtrl.WriteText( _T("Huh?\n") ); break;
        }
    }
    textCtrl.WriteText( _T("\n") );

    textCtrl.WriteText( _T("Seeking to 0;  stream.GetLastError() returns: ") );
    ffile_input.SeekI( 0 );
    switch (ffile_input.GetLastError())
    {
        case wxSTREAM_NO_ERROR:      textCtrl.WriteText( _T("wxSTREAM_NO_ERROR\n") ); break;
        case wxSTREAM_EOF:          textCtrl.WriteText( _T("wxSTREAM_EOF\n") ); break;
        case wxSTREAM_READ_ERROR:   textCtrl.WriteText( _T("wxSTREAM_READ_ERROR\n") ); break;
        case wxSTREAM_WRITE_ERROR:  textCtrl.WriteText( _T("wxSTREAM_WRITE_ERROR\n") ); break;
        default: textCtrl.WriteText( _T("Huh?\n") ); break;
    }
    textCtrl.WriteText( _T("\n") );

    ffile_input.Read( &ch, 1 );
    textCtrl.WriteText( _T("Value read: ") );
    textCtrl.WriteText( (wxChar)(ch + '0') );
    textCtrl.WriteText( _T(";  stream.GetLastError() returns: ") );
    switch (ffile_input.GetLastError())
    {
        case wxSTREAM_NO_ERROR:      textCtrl.WriteText( _T("wxSTREAM_NO_ERROR\n") ); break;
        case wxSTREAM_EOF:          textCtrl.WriteText( _T("wxSTREAM_EOF\n") ); break;
        case wxSTREAM_READ_ERROR:   textCtrl.WriteText( _T("wxSTREAM_READ_ERROR\n") ); break;
        case wxSTREAM_WRITE_ERROR:  textCtrl.WriteText( _T("wxSTREAM_WRITE_ERROR\n") ); break;
        default: textCtrl.WriteText( _T("Huh?\n") ); break;
    }
    textCtrl.WriteText( _T("\n\n") );

    // Testing wxFFileInputStream

    textCtrl.WriteText( _T("Reading 0 to 10 to buffered wxFFileInputStream:\n\n") );

    wxFFileInputStream ffile_input2( wxString(_T("test_wx.dat")) );
    wxBufferedInputStream buf_input( ffile_input2 );
    for (ch2 = 0; ch2 < 11; ch2++)
    {
        buf_input.Read( &ch, 1 );
        textCtrl.WriteText( _T("Value read: ") );
        textCtrl.WriteText( (wxChar)(ch + '0') );
        textCtrl.WriteText( _T(";  stream.GetLastError() returns: ") );
        switch (buf_input.GetLastError())
        {
            case wxSTREAM_NO_ERROR:      textCtrl.WriteText( _T("wxSTREAM_NO_ERROR\n") ); break;
            case wxSTREAM_EOF:          textCtrl.WriteText( _T("wxSTREAM_EOF\n") ); break;
            case wxSTREAM_READ_ERROR:   textCtrl.WriteText( _T("wxSTREAM_READ_ERROR\n") ); break;
            case wxSTREAM_WRITE_ERROR:  textCtrl.WriteText( _T("wxSTREAM_WRITE_ERROR\n") ); break;
            default: textCtrl.WriteText( _T("Huh?\n") ); break;
        }
    }
    textCtrl.WriteText( _T("\n") );

    textCtrl.WriteText( _T("Seeking to 0;  stream.GetLastError() returns: ") );
    buf_input.SeekI( 0 );
    switch (buf_input.GetLastError())
    {
        case wxSTREAM_NO_ERROR:      textCtrl.WriteText( _T("wxSTREAM_NO_ERROR\n") ); break;
        case wxSTREAM_EOF:          textCtrl.WriteText( _T("wxSTREAM_EOF\n") ); break;
        case wxSTREAM_READ_ERROR:   textCtrl.WriteText( _T("wxSTREAM_READ_ERROR\n") ); break;
        case wxSTREAM_WRITE_ERROR:  textCtrl.WriteText( _T("wxSTREAM_WRITE_ERROR\n") ); break;
        default: textCtrl.WriteText( _T("Huh?\n") ); break;
    }
    textCtrl.WriteText( _T("\n") );

    buf_input.Read( &ch, 1 );
    textCtrl.WriteText( _T("Value read: ") );
    textCtrl.WriteText( (wxChar)(ch + '0') );
    textCtrl.WriteText( _T(";  stream.GetLastError() returns: ") );
    switch (buf_input.GetLastError())
    {
        case wxSTREAM_NO_ERROR:      textCtrl.WriteText( _T("wxSTREAM_NO_ERROR\n") ); break;
        case wxSTREAM_EOF:          textCtrl.WriteText( _T("wxSTREAM_EOF\n") ); break;
        case wxSTREAM_READ_ERROR:   textCtrl.WriteText( _T("wxSTREAM_READ_ERROR\n") ); break;
        case wxSTREAM_WRITE_ERROR:  textCtrl.WriteText( _T("wxSTREAM_WRITE_ERROR\n") ); break;
        default: textCtrl.WriteText( _T("Huh?\n") ); break;
    }
}

void MyApp::DoStreamDemo4(wxCommandEvent& WXUNUSED(event))
{
    wxTextCtrl& textCtrl = * GetTextCtrl();

    wxString msg;

    textCtrl.Clear();
    textCtrl << _T("\nTesting wxStreamBuffer:\n\n");

    // bigger than buffer
    textCtrl.WriteText( _T("Writing 2000x 1 to wxFileOutputStream.\n\n") );

    wxFileOutputStream file_output( wxString(_T("test_wx.dat")) );
    for (int i = 0; i < 2000; i++)
    {
        char ch = 1;
        file_output.Write( &ch, 1 );
    }

    textCtrl.WriteText( _T("Opening with a buffered wxFileInputStream:\n\n") );

    wxFileInputStream file_input( wxString(_T("test_wx.dat")) );
    wxBufferedInputStream buf_input( file_input );

    textCtrl.WriteText( _T("wxBufferedInputStream.GetLastError() returns: ") );
    switch (buf_input.GetLastError())
    {
        case wxSTREAM_NO_ERROR:      textCtrl.WriteText( _T("wxSTREAM_NO_ERROR\n") ); break;
        case wxSTREAM_EOF:          textCtrl.WriteText( _T("wxSTREAM_EOF\n") ); break;
        case wxSTREAM_READ_ERROR:   textCtrl.WriteText( _T("wxSTREAM_READ_ERROR\n") ); break;
        case wxSTREAM_WRITE_ERROR:  textCtrl.WriteText( _T("wxSTREAM_WRITE_ERROR\n") ); break;
        default: textCtrl.WriteText( _T("Huh?\n") ); break;
    }
    msg.Printf( wxT("wxBufferedInputStream.LastRead() returns: %d\n"), (int)buf_input.LastRead() );
    textCtrl.WriteText( msg );
    msg.Printf( wxT("wxBufferedInputStream.TellI() returns: %d\n"), (int)buf_input.TellI() );
    textCtrl.WriteText( msg );
    textCtrl.WriteText( _T("\n\n") );


    textCtrl.WriteText( _T("Seeking to position 300:\n\n") );

    buf_input.SeekI( 300 );

    textCtrl.WriteText( _T("wxBufferedInputStream.GetLastError() returns: ") );
    switch (buf_input.GetLastError())
    {
        case wxSTREAM_NO_ERROR:      textCtrl.WriteText( _T("wxSTREAM_NO_ERROR\n") ); break;
        case wxSTREAM_EOF:          textCtrl.WriteText( _T("wxSTREAM_EOF\n") ); break;
        case wxSTREAM_READ_ERROR:   textCtrl.WriteText( _T("wxSTREAM_READ_ERROR\n") ); break;
        case wxSTREAM_WRITE_ERROR:  textCtrl.WriteText( _T("wxSTREAM_WRITE_ERROR\n") ); break;
        default: textCtrl.WriteText( _T("Huh?\n") ); break;
    }
    msg.Printf( wxT("wxBufferedInputStream.LastRead() returns: %d\n"), (int)buf_input.LastRead() );
    textCtrl.WriteText( msg );
    msg.Printf( wxT("wxBufferedInputStream.TellI() returns: %d\n"), (int)buf_input.TellI() );
    textCtrl.WriteText( msg );
    textCtrl.WriteText( _T("\n\n") );


    char buf[2000];

    textCtrl.WriteText( _T("Reading 500 bytes:\n\n") );

    buf_input.Read( buf, 500 );

    textCtrl.WriteText( _T("wxBufferedInputStream.GetLastError() returns: ") );
    switch (buf_input.GetLastError())
    {
        case wxSTREAM_NO_ERROR:      textCtrl.WriteText( _T("wxSTREAM_NO_ERROR\n") ); break;
        case wxSTREAM_EOF:          textCtrl.WriteText( _T("wxSTREAM_EOF\n") ); break;
        case wxSTREAM_READ_ERROR:   textCtrl.WriteText( _T("wxSTREAM_READ_ERROR\n") ); break;
        case wxSTREAM_WRITE_ERROR:  textCtrl.WriteText( _T("wxSTREAM_WRITE_ERROR\n") ); break;
        default: textCtrl.WriteText( _T("Huh?\n") ); break;
    }
    msg.Printf( wxT("wxBufferedInputStream.LastRead() returns: %d\n"), (int)buf_input.LastRead() );
    textCtrl.WriteText( msg );
    msg.Printf( wxT("wxBufferedInputStream.TellI() returns: %d\n"), (int)buf_input.TellI() );
    textCtrl.WriteText( msg );
    textCtrl.WriteText( _T("\n\n") );

    textCtrl.WriteText( _T("Reading another 500 bytes:\n\n") );

    buf_input.Read( buf, 500 );

    textCtrl.WriteText( _T("wxBufferedInputStream.GetLastError() returns: ") );
    switch (buf_input.GetLastError())
    {
        case wxSTREAM_NO_ERROR:      textCtrl.WriteText( _T("wxSTREAM_NO_ERROR\n") ); break;
        case wxSTREAM_EOF:          textCtrl.WriteText( _T("wxSTREAM_EOF\n") ); break;
        case wxSTREAM_READ_ERROR:   textCtrl.WriteText( _T("wxSTREAM_READ_ERROR\n") ); break;
        case wxSTREAM_WRITE_ERROR:  textCtrl.WriteText( _T("wxSTREAM_WRITE_ERROR\n") ); break;
        default: textCtrl.WriteText( _T("Huh?\n") ); break;
    }
    msg.Printf( wxT("wxBufferedInputStream.LastRead() returns: %d\n"), (int)buf_input.LastRead() );
    textCtrl.WriteText( msg );
    msg.Printf( wxT("wxBufferedInputStream.TellI() returns: %d\n"), (int)buf_input.TellI() );
    textCtrl.WriteText( msg );
    textCtrl.WriteText( _T("\n\n") );

    textCtrl.WriteText( _T("Reading another 500 bytes:\n\n") );

    buf_input.Read( buf, 500 );

    textCtrl.WriteText( _T("wxBufferedInputStream.GetLastError() returns: ") );
    switch (buf_input.GetLastError())
    {
        case wxSTREAM_NO_ERROR:      textCtrl.WriteText( _T("wxSTREAM_NO_ERROR\n") ); break;
        case wxSTREAM_EOF:          textCtrl.WriteText( _T("wxSTREAM_EOF\n") ); break;
        case wxSTREAM_READ_ERROR:   textCtrl.WriteText( _T("wxSTREAM_READ_ERROR\n") ); break;
        case wxSTREAM_WRITE_ERROR:  textCtrl.WriteText( _T("wxSTREAM_WRITE_ERROR\n") ); break;
        default: textCtrl.WriteText( _T("Huh?\n") ); break;
    }
    msg.Printf( wxT("wxBufferedInputStream.LastRead() returns: %d\n"), (int)buf_input.LastRead() );
    textCtrl.WriteText( msg );
    msg.Printf( wxT("wxBufferedInputStream.TellI() returns: %d\n"), (int)buf_input.TellI() );
    textCtrl.WriteText( msg );
    textCtrl.WriteText( _T("\n\n") );

    textCtrl.WriteText( _T("Reading another 500 bytes:\n\n") );

    buf_input.Read( buf, 500 );

    textCtrl.WriteText( _T("wxBufferedInputStream.GetLastError() returns: ") );
    switch (buf_input.GetLastError())
    {
        case wxSTREAM_NO_ERROR:      textCtrl.WriteText( _T("wxSTREAM_NO_ERROR\n") ); break;
        case wxSTREAM_EOF:          textCtrl.WriteText( _T("wxSTREAM_EOF\n") ); break;
        case wxSTREAM_READ_ERROR:   textCtrl.WriteText( _T("wxSTREAM_READ_ERROR\n") ); break;
        case wxSTREAM_WRITE_ERROR:  textCtrl.WriteText( _T("wxSTREAM_WRITE_ERROR\n") ); break;
        default: textCtrl.WriteText( _T("Huh?\n") ); break;
    }
    msg.Printf( wxT("wxBufferedInputStream.LastRead() returns: %d\n"), (int)buf_input.LastRead() );
    textCtrl.WriteText( msg );
    msg.Printf( wxT("wxBufferedInputStream.TellI() returns: %d\n"), (int)buf_input.TellI() );
    textCtrl.WriteText( msg );
    textCtrl.WriteText( _T("\n\n") );
}

void MyApp::DoStreamDemo5(wxCommandEvent& WXUNUSED(event))
{
    wxTextCtrl& textCtrl = * GetTextCtrl();

    textCtrl.Clear();
    textCtrl << _T("\nTesting wxFileInputStream's Peek():\n\n");

    char ch;
    wxString str;

    textCtrl.WriteText( _T("Writing number 0 to 9 to wxFileOutputStream:\n\n") );

    wxFileOutputStream file_output( wxString(_T("test_wx.dat")) );
    for (ch = 0; ch < 10; ch++)
        file_output.Write( &ch, 1 );

    file_output.Sync();

    wxFileInputStream file_input( wxString(_T("test_wx.dat")) );

    ch = file_input.Peek();
    str.Printf( wxT("First char peeked: %d\n"), (int) ch );
    textCtrl.WriteText( str );

    ch = file_input.GetC();
    str.Printf( wxT("First char read: %d\n"), (int) ch );
    textCtrl.WriteText( str );

    ch = file_input.Peek();
    str.Printf( wxT("Second char peeked: %d\n"), (int) ch );
    textCtrl.WriteText( str );

    ch = file_input.GetC();
    str.Printf( wxT("Second char read: %d\n"), (int) ch );
    textCtrl.WriteText( str );

    ch = file_input.Peek();
    str.Printf( wxT("Third char peeked: %d\n"), (int) ch );
    textCtrl.WriteText( str );

    ch = file_input.GetC();
    str.Printf( wxT("Third char read: %d\n"), (int) ch );
    textCtrl.WriteText( str );


    textCtrl << _T("\n\n\nTesting wxMemoryInputStream's Peek():\n\n");

    char buf[] = { 0,1,2,3,4,5,6,7,8,9,10 };
    wxMemoryInputStream input( buf, 10 );

    ch = input.Peek();
    str.Printf( wxT("First char peeked: %d\n"), (int) ch );
    textCtrl.WriteText( str );

    ch = input.GetC();
    str.Printf( wxT("First char read: %d\n"), (int) ch );
    textCtrl.WriteText( str );

    ch = input.Peek();
    str.Printf( wxT("Second char peeked: %d\n"), (int) ch );
    textCtrl.WriteText( str );

    ch = input.GetC();
    str.Printf( wxT("Second char read: %d\n"), (int) ch );
    textCtrl.WriteText( str );

    ch = input.Peek();
    str.Printf( wxT("Third char peeked: %d\n"), (int) ch );
    textCtrl.WriteText( str );

    ch = input.GetC();
    str.Printf( wxT("Third char read: %d\n"), (int) ch );
    textCtrl.WriteText( str );
}

void MyApp::DoStreamDemo6(wxCommandEvent& WXUNUSED(event))
{
    wxTextCtrl& textCtrl = * GetTextCtrl();

    textCtrl.Clear();
    textCtrl.WriteText( _T("\nTesting Ungetch():\n\n") );

    char ch = 0;
    size_t pos = 0;
    wxString str;

    textCtrl.WriteText( _T("Writing number 0 to 9 to wxFileOutputStream...\n\n") );

    wxFileOutputStream file_output( wxString(_T("test_wx.dat")) );
    for (ch = 0; ch < 10; ch++)
        file_output.Write( &ch, 1 );

    file_output.Sync();

    textCtrl.WriteText( _T("Reading char from wxFileInputStream:\n\n") );

    wxFileInputStream file_input( wxString(_T("test_wx.dat")) );

    ch = file_input.GetC();
    pos = file_input.TellI();
    str.Printf( wxT("Read char: %d. Now at position %d\n\n"), (int) ch, (int) pos );
    textCtrl.WriteText( str );

    textCtrl.WriteText( _T("Reading another char from wxFileInputStream:\n\n") );

    ch = file_input.GetC();
    pos = file_input.TellI();
    str.Printf( wxT("Read char: %d. Now at position %d\n\n"), (int) ch, (int) pos );
    textCtrl.WriteText( str );

    textCtrl.WriteText( _T("Reading yet another char from wxFileInputStream:\n\n") );

    ch = file_input.GetC();
    pos = file_input.TellI();
    str.Printf( wxT("Read char: %d. Now at position %d\n\n"), (int) ch, (int) pos );
    textCtrl.WriteText( str );

    textCtrl.WriteText( _T("Now calling Ungetch( 5 ) from wxFileInputStream...\n\n") );

    file_input.Ungetch( 5 );
    pos = file_input.TellI();
    str.Printf( wxT("Now at position %d\n\n"), (int) pos );
    textCtrl.WriteText( str );

    textCtrl.WriteText( _T("Reading char from wxFileInputStream:\n\n") );

    ch = file_input.GetC();
    pos = file_input.TellI();
    str.Printf( wxT("Read char: %d. Now at position %d\n\n"), (int) ch, (int) pos );
    textCtrl.WriteText( str );

    textCtrl.WriteText( _T("Reading another char from wxFileInputStream:\n\n") );

    ch = file_input.GetC();
    pos = file_input.TellI();
    str.Printf( wxT("Read char: %d. Now at position %d\n\n"), (int) ch, (int) pos );
    textCtrl.WriteText( str );

    textCtrl.WriteText( _T("Now calling Ungetch( 5 ) from wxFileInputStream again...\n\n") );

    file_input.Ungetch( 5 );
    pos = file_input.TellI();
    str.Printf( wxT("Now at position %d\n\n"), (int) pos );
    textCtrl.WriteText( str );

    textCtrl.WriteText( _T("Seeking to pos 3 in wxFileInputStream. This invalidates the writeback buffer.\n\n") );

    file_input.SeekI( 3 );

    ch = file_input.GetC();
    pos = file_input.TellI();
    str.Printf( wxT("Read char: %d. Now at position %d\n\n"), (int) ch, (int) pos );
    textCtrl.WriteText( str );
}

void MyApp::DoStreamDemo7(wxCommandEvent& WXUNUSED(event))
{
    wxTextCtrl& textCtrl = * GetTextCtrl();

    textCtrl.Clear();
    textCtrl.WriteText( _T("\nTesting Ungetch() in buffered input stream:\n\n") );

    char ch = 0;
    size_t pos = 0;
    wxString str;

    textCtrl.WriteText( _T("Writing number 0 to 9 to wxFileOutputStream...\n\n") );

    wxFileOutputStream file_output( wxString(_T("test_wx.dat")) );
    for (ch = 0; ch < 10; ch++)
        file_output.Write( &ch, 1 );

    file_output.Sync();

    textCtrl.WriteText( _T("Reading char from wxBufferedInputStream via wxFileInputStream:\n\n") );

    wxFileInputStream file_input( wxString(_T("test_wx.dat")) );
    wxBufferedInputStream buf_input( file_input );

    ch = buf_input.GetC();
    pos = buf_input.TellI();
    str.Printf( wxT("Read char: %d. Now at position %d\n\n"), (int) ch, (int) pos );
    textCtrl.WriteText( str );

    textCtrl.WriteText( _T("Reading another char from wxBufferedInputStream:\n\n") );

    ch = buf_input.GetC();
    pos = buf_input.TellI();
    str.Printf( wxT("Read char: %d. Now at position %d\n\n"), (int) ch, (int) pos );
    textCtrl.WriteText( str );

    textCtrl.WriteText( _T("Reading yet another char from wxBufferedInputStream:\n\n") );

    ch = buf_input.GetC();
    pos = buf_input.TellI();
    str.Printf( wxT("Read char: %d. Now at position %d\n\n"), (int) ch, (int) pos );
    textCtrl.WriteText( str );

    textCtrl.WriteText( _T("Now calling Ungetch( 5 ) from wxBufferedInputStream...\n\n") );

    buf_input.Ungetch( 5 );
    pos = buf_input.TellI();
    str.Printf( wxT("Now at position %d\n\n"), (int) pos );
    textCtrl.WriteText( str );

    textCtrl.WriteText( _T("Reading char from wxBufferedInputStream:\n\n") );

    ch = buf_input.GetC();
    pos = buf_input.TellI();
    str.Printf( wxT("Read char: %d. Now at position %d\n\n"), (int) ch, (int) pos );
    textCtrl.WriteText( str );

    textCtrl.WriteText( _T("Reading another char from wxBufferedInputStream:\n\n") );

    ch = buf_input.GetC();
    pos = buf_input.TellI();
    str.Printf( wxT("Read char: %d. Now at position %d\n\n"), (int) ch, (int) pos );
    textCtrl.WriteText( str );

    textCtrl.WriteText( _T("Now calling Ungetch( 5 ) from wxBufferedInputStream again...\n\n") );

    buf_input.Ungetch( 5 );
    pos = buf_input.TellI();
    str.Printf( wxT("Now at position %d\n\n"), (int) pos );
    textCtrl.WriteText( str );

    textCtrl.WriteText( _T("Seeking to pos 3 in wxBufferedInputStream. This invalidates the writeback buffer.\n\n") );

    buf_input.SeekI( 3 );

    ch = buf_input.GetC();
    pos = buf_input.TellI();
    str.Printf( wxT("Read char: %d. Now at position %d\n\n"), (int) ch, (int) pos );
    textCtrl.WriteText( str );
}

#if wxUSE_UNICODE
void MyApp::DoUnicodeDemo(wxCommandEvent& WXUNUSED(event))
{
    wxTextCtrl& textCtrl = * GetTextCtrl();

    textCtrl.Clear();
    textCtrl << _T("\nTest wchar_t to char (Unicode to ANSI/Multibyte) converions:");

    wxString str;
    str = _T("Robert R�bling\n");

    printf( "\n\nConversion with wxConvLocal:\n" );
    wxConvCurrent = &wxConvLocal;
    printf( (const char*) str.mbc_str() );
#if defined(__WXGTK__)
    printf( "\n\nConversion with wxConvGdk:\n" );
    wxConvCurrent = &wxConvGdk;
    printf( (const char*) str.mbc_str() );
#endif
    printf( "\n\nConversion with wxConvLibc:\n" );
    wxConvCurrent = &wxConvLibc;
    printf( (const char*) str.mbc_str() );

}
#endif

void MyApp::DoMIMEDemo(wxCommandEvent& WXUNUSED(event))
{
    static wxString s_defaultExt = _T("xyz");

    wxString ext = wxGetTextFromUser(_T("Enter a file extension: "),
                                     _T("MIME database test"),
                                     s_defaultExt);
    if ( !!ext )
    {
        s_defaultExt = ext;

        // init MIME database if not done yet
        if ( !m_mimeDatabase )
        {
            m_mimeDatabase = new wxMimeTypesManager;

            static const wxFileTypeInfo fallbacks[] =
            {
                wxFileTypeInfo(_T("application/xyz"),
                               _T("XyZ %s"),
                               _T("XyZ -p %s"),
                               _T("The one and only XYZ format file"),
                               _T("xyz"), _T("123"), NULL),
                wxFileTypeInfo(_T("text/html"),
                               _T("lynx %s"),
                               _T("lynx -dump %s | lpr"),
                               _T("HTML document (from fallback)"),
                               _T("htm"), _T("html"), NULL),

                // must terminate the table with this!
                wxFileTypeInfo()
            };

            m_mimeDatabase->AddFallbacks(fallbacks);
        }

        wxTextCtrl& textCtrl = * GetTextCtrl();

        wxFileType *filetype = m_mimeDatabase->GetFileTypeFromExtension(ext);
        if ( !filetype )
        {
            textCtrl << _T("Unknown extension '") << ext << _T("'\n");
        }
        else
        {
            wxString type, desc, open;
            filetype->GetMimeType(&type);
            filetype->GetDescription(&desc);

            wxString filename = _T("filename");
            filename << _T(".") << ext;
            wxFileType::MessageParameters params(filename, type);
            filetype->GetOpenCommand(&open, params);

            textCtrl << _T("MIME information about extension '") << ext << _T("'\n")
                     << _T("\tMIME type: ") << ( !type ? wxT("unknown")
                                                   : type.c_str() ) << '\n'
                     << _T("\tDescription: ") << ( !desc ? wxT("") : desc.c_str() )
                        << '\n'
                     << _T("\tCommand to open: ") << ( !open ? wxT("no") : open.c_str() )
                        << '\n';

            delete filetype;
        }
    }
    //else: cancelled by user
}

void MyApp::DoByteOrderDemo(wxCommandEvent& WXUNUSED(event))
{
    wxTextCtrl& textCtrl = * GetTextCtrl();

    textCtrl.Clear();
    textCtrl << _T("\nTest byte order macros:\n\n");

    if (wxBYTE_ORDER == wxLITTLE_ENDIAN)
        textCtrl << _T("This is a little endian system.\n\n");
    else
        textCtrl << _T("This is a big endian system.\n\n");

    wxString text;

    wxInt32 var = 0xF1F2F3F4;
    text = _T("");
    text.Printf( _T("Value of wxInt32 is now: %#x.\n\n"), var );
    textCtrl.WriteText( text );

    text = _T("");
    text.Printf( _T("Value of swapped wxInt32 is: %#x.\n\n"), wxINT32_SWAP_ALWAYS( var ) );
    textCtrl.WriteText( text );

    text = _T("");
    text.Printf( _T("Value of wxInt32 swapped on little endian is: %#x.\n\n"), wxINT32_SWAP_ON_LE( var ) );
    textCtrl.WriteText( text );

    text = _T("");
    text.Printf( _T("Value of wxInt32 swapped on big endian is: %#x.\n\n"), wxINT32_SWAP_ON_BE( var ) );
    textCtrl.WriteText( text );
}

#if wxUSE_TIMEDATE

void MyApp::DoTimeDemo(wxCommandEvent& WXUNUSED(event))
{
    wxTextCtrl& textCtrl = * GetTextCtrl();

    textCtrl.Clear();
    textCtrl << _T("\nTest class wxTime:\n");
    wxTime now;
    textCtrl << _T("It is now ") << (wxString) now << _T("\n");
}

void MyApp::DoDateDemo(wxCommandEvent& WXUNUSED(event))
{
    wxTextCtrl& textCtrl = * GetTextCtrl();

    textCtrl.Clear();
    textCtrl << _T("\nTest class wxDate") << _T("\n");

    // Various versions of the constructors
    // and various output

    wxDate x(10,20,1962);

    textCtrl << x.FormatDate(wxFULL) << _T("  (full)\n");

    // constuctor with a string, just printing the day of the week
    wxDate y(_T("8/8/1988"));

    textCtrl << y.FormatDate(wxDAY) << _T("  (just day)\n");

    // constructor with a julian
    wxDate z( 2450000L );
    textCtrl << z.FormatDate(wxFULL) << _T("  (full)\n");

    // using date addition and subtraction
    wxDate a = x + 10;
    textCtrl << a.FormatDate(wxFULL) << _T("  (full)\n");
    a = a - 25;
    textCtrl << a.FormatDate(wxEUROPEAN) << _T("  (European)\n");

    // Using subtraction of two date objects
    wxDate a1 = wxString(_T("7/13/1991"));
    wxDate a2 = a1 + 14;
    textCtrl << (a1-a2) << _T("\n");
    textCtrl << (a2+=10) << _T("\n");

    a1++;
    textCtrl << _T("Tomorrow= ") << a1.FormatDate(wxFULL) << _T("\n");

    wxDate tmpDate1(_T("08/01/1991"));
    wxDate tmpDate2(_T("07/14/1991"));
    textCtrl << _T("a1 (7-14-91) < 8-01-91 ? ==> ") << ((a1 < tmpDate1) ? _T("TRUE") : _T("FALSE")) << _T("\n");
    textCtrl << _T("a1 (7-14-91) > 8-01-91 ? ==> ") << ((a1 > tmpDate1) ? _T("TRUE") : _T("FALSE")) << _T("\n");
    textCtrl << _T("a1 (7-14-91)== 7-14-91 ? ==> ") << ((a1==tmpDate2) ? _T("TRUE") : _T("FALSE")) << _T("\n");

    wxDate a3 = a1;
    textCtrl << _T("a1 (7-14-91)== a3 (7-14-91) ? ==> ") << ((a1==a3) ? _T("TRUE") : _T("FALSE")) << _T("\n");
    wxDate a4 = a1;
    textCtrl << _T("a1 (7-14-91)== a4 (7-15-91) ? ==> ") << ((a1==++a4) ? _T("TRUE") : _T("FALSE")) << _T("\n");

    wxDate a5 = wxString(_T("today"));
    textCtrl << _T("Today is: ") << a5 << _T("\n");
    a4 = _T("TODAY");
    textCtrl << _T("Today (a4) is: ") << a4 << _T("\n");

    textCtrl << _T("Today + 4 is: ") << (a4+=4) << _T("\n");
    a4 = _T("TODAY");
    textCtrl << _T("Today - 4 is: ") << (a4-=4) << _T("\n");

    textCtrl << _T("=========== Leap Year Test ===========\n");
    a1 = _T("1/15/1992");
    textCtrl << a1.FormatDate(wxFULL) << _T("  ") << ((a1.IsLeapYear()) ? _T("Leap") : _T("non-Leap"));
    textCtrl << _T("  ") << _T("day of year:  ") << a1.GetDayOfYear() << _T("\n");

    a1 = _T("2/16/1993");
    textCtrl << a1.FormatDate(wxFULL) << _T("  ") << ((a1.IsLeapYear()) ? _T("Leap") : _T("non-Leap"));
    textCtrl << _T("  ") << _T("day of year:  ") << a1.GetDayOfYear() << _T("\n");

    textCtrl << _T("================== string assignment test ====================\n");
    wxString date_string=a1;
    textCtrl << _T("a1 as a string (s/b 2/16/1993) ==> ") << date_string << _T("\n");

    textCtrl << _T("================== SetFormat test ============================\n");
    a1.SetFormat(wxFULL);
    textCtrl << _T("a1 (s/b FULL format) ==> ") << a1 << _T("\n");
    a1.SetFormat(wxEUROPEAN);
    textCtrl << _T("a1 (s/b EUROPEAN format) ==> ") << a1 << _T("\n");

    textCtrl << _T("================== SetOption test ============================\n");
    textCtrl << _T("Date abbreviation ON\n");

    a1.SetOption(wxDATE_ABBR);
    a1.SetFormat(wxMONTH);
    textCtrl << _T("a1 (s/b MONTH format) ==> ") << a1 << _T("\n");
    a1.SetFormat(wxDAY);
    textCtrl << _T("a1 (s/b DAY format) ==> ") << a1 << _T("\n");
    a1.SetFormat(wxFULL);
    textCtrl << _T("a1 (s/b FULL format) ==> ") << a1 << _T("\n");
    a1.SetFormat(wxEUROPEAN);
    textCtrl << _T("a1 (s/b EUROPEAN format) ==> ") << a1 << _T("\n");
    textCtrl << _T("Century suppression ON\n");
    a1.SetOption(wxNO_CENTURY);
    a1.SetFormat(wxMDY);
    textCtrl << _T("a1 (s/b MDY format) ==> ") << a1 << _T("\n");
    textCtrl << _T("Century suppression OFF\n");
    a1.SetOption(wxNO_CENTURY,FALSE);
    textCtrl << _T("a1 (s/b MDY format) ==> ") << a1 << _T("\n");
    textCtrl << _T("Century suppression ON\n");
    a1.SetOption(wxNO_CENTURY);
    textCtrl << _T("a1 (s/b MDY format) ==> ") << a1 << _T("\n");
    a1.SetFormat(wxFULL);
    textCtrl << _T("a1 (s/b FULL format) ==> ") << a1 << _T("\n");

    textCtrl << _T("\n=============== Version 4.0 Enhancement Test =================\n");

    wxDate v4(_T("11/26/1966"));
    textCtrl << _T("\n---------- Set Stuff -----------\n");
    textCtrl << _T("First, 'Set' to today...") << _T("\n");
    textCtrl << _T("Before 'Set' => ") << v4 << _T("\n");
    textCtrl << _T("After  'Set' => ") << v4.Set() << _T("\n\n");

    textCtrl << _T("Set to 11/26/66 => ") << v4.Set(11,26,1966) << _T("\n");
    textCtrl << _T("Current Julian  => ") << v4.GetJulianDate() << _T("\n");
    textCtrl << _T("Set to Julian 2450000L => ") << v4.Set(2450000L) << _T("\n");
    textCtrl << _T("See! => ") << v4.GetJulianDate() << _T("\n");

    textCtrl << _T("---------- Add Stuff -----------\n");
    textCtrl << _T("Start => ") << v4 << _T("\n");
    textCtrl << _T("Add 4 Weeks => ") << v4.AddWeeks(4) << _T("\n");
    textCtrl << _T("Sub 1 Month => ") << v4.AddMonths(-1) << _T("\n");
    textCtrl << _T("Add 2 Years => ") << v4.AddYears(2) << _T("\n");

    textCtrl << _T("---------- Misc Stuff -----------\n");
    textCtrl << _T("The date aboves' day of the month is => ") << v4.GetDay() << _T("\n");
    textCtrl << _T("There are ") << v4.GetDaysInMonth() << _T(" days in this month.\n");
    textCtrl << _T("The first day of this month lands on ") << v4.GetFirstDayOfMonth() << _T("\n");
    textCtrl << _T("This day happens to be ") << v4.GetDayOfWeekName() << _T("\n");
    textCtrl << _T("the ") << v4.GetDayOfWeek() << _T(" day of the week,") << _T("\n");
    textCtrl << _T("on the ") << v4.GetWeekOfYear() << _T(" week of the year,") << _T("\n");
    textCtrl << _T("on the ") << v4.GetWeekOfMonth() << _T(" week of the month, ") << _T("\n");
    textCtrl << _T("(which is ") << v4.GetMonthName() << _T(")\n");
    textCtrl << _T("the ")<< v4.GetMonth() << _T("th month in the year.\n");
    textCtrl << _T("The year alone is ") << v4.GetYear() << _T("\n");

    textCtrl << _T("---------- First and Last Stuff -----------\n");
    v4.Set();
    textCtrl << _T("The first date of this month is ") << v4.GetMonthStart() << _T("\n");
    textCtrl << _T("The last date of this month is ") << v4.GetMonthEnd() << _T("\n");
    textCtrl << _T("The first date of this year is ") << v4.GetYearStart() << _T("\n");
    textCtrl << _T("The last date of this year is ") << v4.GetYearEnd() << _T("\n");
}

#endif // wxUSE_TIMEDATE

void MyApp::DoVariantDemo(wxCommandEvent& WXUNUSED(event) )
{
    wxTextCtrl& textCtrl = * GetTextCtrl();

    wxVariant var1 = _T("String value");
    textCtrl << _T("var1 = ") << var1.MakeString() << _T("\n");

    // Conversion
    wxString str = var1.MakeString();

    var1 = 123.456;
    textCtrl << _T("var1 = ") << var1.GetReal() << _T("\n");

    // Implicit conversion
    double v = var1;

    var1 = 9876L;
    textCtrl << _T("var1 = ") << var1.GetLong() << _T("\n");

    // Implicit conversion
    long l = var1;

    // suppress compile warnings about unused variables
    if ( l < v )
    {
        ;
    }

    wxStringList stringList;
    stringList.Add(_T("one")); stringList.Add(_T("two")); stringList.Add(_T("three"));
    var1 = stringList;
    textCtrl << _T("var1 = ") << var1.MakeString() << _T("\n");

    var1.ClearList();
    var1.Append(wxVariant(1.2345));
    var1.Append(wxVariant(_T("hello")));
    var1.Append(wxVariant(54321L));

    textCtrl << _T("var1 = ") << var1.MakeString() << _T("\n");

    size_t n = var1.GetCount();
    size_t i;
    for (i = (size_t) 0; i < n; i++)
    {
        textCtrl << _T("var1[") << (int) i << _T("] (type ") << var1[i].GetType() << _T(") = ") << var1[i].MakeString() << _T("\n");
    }
}

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(TYPES_QUIT, MyFrame::OnQuit)
    EVT_MENU(TYPES_ABOUT, MyFrame::OnAbout)
END_EVENT_TABLE()

// My frame constructor
MyFrame::MyFrame(wxFrame *parent, const wxString& title,
       const wxPoint& pos, const wxSize& size):
  wxFrame(parent, -1, title, pos, size)
{}

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event) )
{
  Close(TRUE);
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event) )
{
  wxMessageDialog dialog(this, _T("Tests various wxWindows types"),
      _T("About Types"), wxYES_NO|wxCANCEL);

  dialog.ShowModal();
}


