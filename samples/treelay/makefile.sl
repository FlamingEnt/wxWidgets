# Purpose: makefile for treelay example (Salford C++)
# Created: 2000-03-15

PROGRAM = treelay
OBJECTS = $(PROGRAM).obj

include ..\..\src\makeprog.sl

all:        wx $(TARGET)

wx:
    cd $(WXDIR)\src\msw ^ mk32 -f makefile.sl all
    cd $(WXDIR)\samples\treelay

