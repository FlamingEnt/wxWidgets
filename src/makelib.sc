##############################################################################
# Name:			src/makelib.sc
# Purpose:		build library Digital Mars 8.33 compiler
# Author:		Chris Elliott
# Created:		21.01.03
# RCS-ID:		$Id: makelib.sc,v 1.1.4.1 2003/08/14 12:06:23 CE Exp $
# Licence:		wxWindows licence
##############################################################################




all:  $(LIBTARGET) 

$(LIBTARGET): $(OBJECTS)
    lib -c $(LIBTARGET) $(OBJECTS)


clean: 
	-del $(THISDIR)\*.obj
	-del $(LIBTARGET)

