#!#############################################################################
#! File:    v15.t
#! Purpose: tmake template file for wxWindows makefile for VC++ 1.52, generate
#!          the makefile by executing the following command:
#!                  tmake -t v15 wxwin.pro -o Makefile
#! Author:  Vadim Zeitlin
#! Created: 16.12.99
#! Version: $Id: v15.t,v 1.3 2002/02/02 23:15:23 VS Exp $
#!#############################################################################
#${
    #! include the code which parses filelist.txt file and initializes
    #! %wxCommon, %wxGeneric and %wxMSW hashes.
    IncludeTemplate("filelist.t");

    #! because we later search for " <filename> " in this string, it should
    #! start with space
    $project{"WXCOBJS"} = " ";

    #! now transform these hashes into $project tags
    foreach $file (sort keys %wxGeneric) {
        if ( $wxGeneric{$file} =~ /\b(PS|G|U)\b/ ) {
            #! this file for some reason wasn't compiled for VC++ 1.52
            next unless $file =~ /^prntdlgg\./;
        }

        $file =~ s/cp?p?$/obj/;
        $project{"WXGENERICOBJS"} .= $file . " "
    }

    foreach $file (sort keys %wxCommon) {
        #! socket files don't compile under Win16 currently
        next if $wxCommon{$file} =~ /\b(32|S|U)\b/;

        $isCFile = $file =~ /\.c$/;
        $file =~ s/cp?p?$/obj/;
        $obj = $file . " ";
        #! have to split lib in 2 halves because otherwise it's too big
        $project{$file =~ "^[a-o]" ? "WXCOMMONOBJS1" : "WXCOMMONOBJS2"} .= $obj;
        $project{"WXCOBJS"} .= $obj if $isCFile;
    }

    foreach $file (sort keys %wxMSW) {
        #! don't take files not appropriate for 16-bit Windows
        next if $wxMSW{$file} =~ /\b(32|O)\b/;

        $isCFile = $file =~ /\.c$/;
        $file =~ s/cp?p?$/obj/;
        $obj = $file . " ";
        #! have to split lib in 2 halves because otherwise it's too big
        $project{$file =~ "^[a-o]" ? "WXMSWOBJS1" : "WXMSWOBJS2"} .= $obj;
        $project{"WXCOBJS"} .= $obj if $isCFile;
    }
#$}
# This file was automatically generated by tmake 
# DO NOT CHANGE THIS FILE, YOUR CHANGES WILL BE LOST! CHANGE DOS.T!

# this is needed because my usual 4NT doesn't work well when invoked from
# makefile
COMSPEC=$(SYSTEMROOT)\system32\cmd.exe

DEBUG=1

WXDIR=..
TMAKE_DIR=$(WXDIR)\distrib\msw\tmake

TMAKE=E:\Perl\5.00471\bin\MSWin32-x86\perl.exe g:\Unix\tmake\bin\tmake

.SUFFIXES :

# set up the compiler options
INCLUDE=/I. /I$(WXDIR)\include /I$(INCLUDE)
DEFINES=-D__WXMSW__ -D__WINDOWS__

# compile in large memory model
CFLAGS=$(CFLAGS) /nologo /W4 /AL

!ifdef DEBUG
CFLAGS=$(CFLAGS) /Zi /Z7
DEFINES=$(DEFINES) /D__WXDEBUG__
!endif

CFLAGS=$(CFLAGS) $(INCLUDE) $(DEFINES)
CPPFLAGS=$(CFLAGS)

PCH_CREATE=/Ycwx/wxprec.h /Fpwx.pch 
PCH_USE=/Yuwx/wxprec.h /Fpwx.pch 

# the targets
WXLIB=wx.lib

# we can't have all objects in one list because the library becomes too big
COMMONOBJS1= #$ ExpandList("WXCOMMONOBJS1");
COMMONOBJS2= #$ ExpandList("WXCOMMONOBJS2");
GENERICOBJS= #$ ExpandList("WXGENERICOBJS");
MSWOBJS1= #$ ExpandList("WXMSWOBJS1");
MSWOBJS2= #$ ExpandList("WXMSWOBJS2");

OBJECTS=$(COMMONOBJS1) $(COMMONOBJS2) $(GENERICOBJS) $(MSWOBJS1) $(MSWOBJS2)

# the rules
all: Makefile $(WXLIB)

$(WXLIB) : pch $(OBJECTS)

clean:
	-del *.obj *.pch

pch: wx/msw/setup.h dummy.obj

dummy.obj: $(WXDIR)/src/msw/dummy.cpp
	$(CPP) $(CPPFLAGS) $(PCH_CREATE) /c /Tp $?

#${
    sub generate_rules_for_dir
    {
        my $dir = $_[0];
        $_ = $_[1];
        my @objs = split;

        foreach (@objs) {
            my $isC = $project{"WXCOBJS"} =~ / \Q$_\E /;
            my $name = $_;
            $name =~ s/.obj$//;
            my $suffix = $isC ? "c" : "cpp";
            my $cc = $isC ? '$(CC)' : '$(CPP)';
            my $flags = $isC ? '$(CFLAGS)' : '$(CPPFLAGS) $(PCH_USE)';
            $text .= "$name.obj: \$(WXDIR)/src/$dir/$name.$suffix\n" .
                     "\t $cc $flags /c /Tp \$?\n"
        }
    }

    &generate_rules_for_dir("msw", $project{"WXMSWOBJS1"} . $project{"WXMSWOBJS2"});
    $text .= "\n";
    &generate_rules_for_dir("common", $project{"WXCOMMONOBJS1"} . $project{"WXCOMMONOBJS2"});
    $text .= "\n";
    &generate_rules_for_dir("generic", $project{"WXGENERICOBJS"});
#$}

Makefile: $(TMAKE_DIR)\makefile.v15
	copy $? Makefile

$(TMAKE_DIR)\makefile.v15: $(TMAKE_DIR)\filelist.txt $(TMAKE_DIR)\v15.t
	cd $(TMAKE_DIR)
	$(TMAKE) -t v15 wxwin.pro -o makefile.v15
