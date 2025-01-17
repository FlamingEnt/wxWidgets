#!#############################################################################
#! File:    wat.t
#! Purpose: tmake template file from which makefile.wat is generated by running
#!          tmake -t wat wxwin.pro -o makefile.wat
#! Author:  Vadim Zeitlin
#! Created: 14.07.99
#! Version: $Id: watbase.t,v 1.1.2.2 2003/09/22 09:16:15 CE Exp $
#! Changelist: 2003-02-25 - Juergen Ulbts - update from wxWindows 2.5.x/HEAD branch
#!#############################################################################
#${
    #! include the code which parses filelist.txt file and initializes
    #! %wxCommon, %wxGeneric and %wxMSW hashes.
    IncludeTemplate("filelist.t");

    #! now transform these hashes into $project tags
    foreach $file (sort keys %wxCommon) {
        next if $wxCommon{$file} !~ /\bB\b/;

        $isCFile = $file =~ /\.c$/;
        $file =~ s/cp?p?$/obj/;
        $project{"WXCOMMONOBJS"} .= $file . " ";
        $project{"WXCOBJS"} .= $file . " " if $isCFile;
    }

    foreach $file (sort keys %wxMSW) {
        next if $wxMSW{$file} !~ /\bB\b/;

        my $isCFile = $file =~ /\.c$/;
        $file =~ s/cp?p?$/obj/;
        $project{"WXMSWOBJS"} .= $file . " ";
        $project{"WXCOBJS"} .= $file . " " if $isCFile;
    }

    foreach $file (sort keys %wxBase) {
        my $isCFile = $file =~ /\.c$/;
        $file =~ s/cp?p?$/obj/;
        $project{"WXCOMMONOBJS"} .= $file . " ";
        $project{"WXCOBJS"} .= $file . " " if $isCFile;
    }

#$}
#! an attempt to embed '#' directly in the string somehow didn't work...
#$ $text = chr(35) . '!/binb/wmake.exe';

# This file was automatically generated by tmake 
# DO NOT CHANGE THIS FILE, YOUR CHANGES WILL BE LOST! CHANGE WATBASE.T!

##############################################################################
# Name:         makebase.wat
# Purpose:      Builds wxBase library for Watcom C++ under Win32
# Author:       Vadim Zeitlin
# Created:      21.01.03
# RCS-ID:       $Id: watbase.t,v 1.1.2.2 2003/09/22 09:16:15 CE Exp $
# Copyright:    (c) 2003 Vadim Zeitlin <vadim@wxwindows.org>
# Licence:      wxWindows licence
##############################################################################

wxUSE_GUI=0

!include ..\makewat.env

LIBTARGET   = $(WXDIR)\lib\$(LIBNAME).lib

EXTRATARGETS = zlib regex
EXTRATARGETSCLEAN = clean_zlib clean_regex
COMMDIR=$(WXDIR)\src\common
MSWDIR=$(WXDIR)\src\msw
OLEDIR=$(MSWDIR)\ole

COMMONOBJS = &
	#$ ExpandGlue("WXCOMMONOBJS", "\$(OUTPUTDIR)\\", " &\n\t\$(OUTPUTDIR)\\")

MSWOBJS = &
	#$ ExpandGlue("WXMSWOBJS", "\$(OUTPUTDIR)\\", " &\n\t\$(OUTPUTDIR)\\")

OBJECTS = $(COMMONOBJS) $(MSWOBJS)

SETUP_H=$(ARCHINCDIR)\wx\setup.h

all: $(SETUP_H) $(OUTPUTDIR) $(OBJECTS) $(LIBTARGET) $(EXTRATARGETS) .SYMBOLIC

$(ARCHINCDIR)\wx:
	mkdir $(ARCHINCDIR)
	mkdir $(ARCHINCDIR)\wx

$(OUTPUTDIR):
	mkdir $(OUTPUTDIR)

$(SETUP_H): $(WXDIR)\include\wx\msw\setup.h $(ARCHINCDIR)\wx
	copy $(WXDIR)\include\wx\msw\setup.h $@

LBCFILE=$(OUTPUTDIR)\wx$(TOOLKIT).lbc
$(LIBTARGET) : $(OBJECTS)
    %create $(LBCFILE)
    @for %i in ( $(OBJECTS) ) do @%append $(LBCFILE) +%i
    wlib /q /b /c /n /p=512 $^@ @$(LBCFILE)


clean:   .SYMBOLIC $(EXTRATARGETSCLEAN)
    -erase $(OUTPUTDIR)\*.obj
    -erase $(LIBTARGET)
    -erase $(OUTPUTDIR)\*.pch
    -erase $(OUTPUTDIR)\*.err
    -erase $(OUTPUTDIR)\*.lbc

cleanall:   clean
    -erase $(LBCFILE)

#${
    $_ = $project{"WXMSWOBJS"};
    my @objs = split;
    foreach (@objs) {
        $text .= "\$(OUTPUTDIR)\\" . $_ . ':     $(';
        s/\.obj$//;
        $text .= 'MSWDIR)\\';

        my $suffix, $cc;
        if ( $project{"WXCOBJS"} =~ /\b\Q$_\E\b/ ) {
            $suffix = "c";
            $cc="CC";
            $flags="CFLAGS";
        }
        else {
            $suffix = "cpp";
            $cc="CXX";
            $flags="CXXFLAGS";
        }
        $text .= $_ . ".$suffix\n" .
                 "  *\$($cc) \$($flags) \$<" . "\n\n";
    }
#$}

########################################################
# Common objects (always compiled)

#${
    $_ = $project{"WXCOMMONOBJS"};
    my @objs = split;
    foreach (@objs) {
        $text .= "\$(OUTPUTDIR)\\" . $_;
        s/\.obj$//;
        $text .= ':     $(COMMDIR)\\';
        my $suffix, $cc;
        if ( $project{"WXCOBJS"} =~ /\b\Q$_\E\b/ ) {
            $suffix = "c";
            $cc="CC";
            $flags="CFLAGS";
        }
        else {
            $suffix = "cpp";
            $cc="CXX";
            $flags="CXXFLAGS";
        }
        $text .= $_ . ".$suffix\n" .
                 "  *\$($cc) \$($flags) \$<" . "\n\n";
    }
#$}

zlib:   .SYMBOLIC
    cd $(WXDIR)\src\zlib
    wmake $(__MAKEOPTS__) -f makefile.wat all
    cd $(WXDIR)\src\msw

clean_zlib:   .SYMBOLIC
    cd $(WXDIR)\src\zlib
    wmake $(__MAKEOPTS__) -f makefile.wat clean
    cd $(WXDIR)\src\msw

regex:    .SYMBOLIC
    cd $(WXDIR)\src\regex
    wmake $(__MAKEOPTS__) -f makefile.wat all
    cd $(WXDIR)\src\msw

clean_regex:   .SYMBOLIC
    cd $(WXDIR)\src\regex
    wmake $(__MAKEOPTS__) -f makefile.wat clean
    cd $(WXDIR)\src\msw

MFTYPE=watbase
self : .SYMBOLIC $(WXDIR)\distrib\msw\tmake\filelist.txt $(WXDIR)\distrib\msw\tmake\$(MFTYPE).t
	cd $(WXDIR)\distrib\msw\tmake
	tmake -t $(MFTYPE) wxwin.pro -o makebase.wat
	copy makebase.wat $(WXDIR)\src\msw
