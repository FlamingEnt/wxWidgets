#*****************************************************************************
#                                                                            *
# Make file for VMS                                                          *
# Author : J.Jansen (joukj@hrem.stm.tudelft.nl)                              *
# Date : 10 November 1999                                                     *
#                                                                            *
#*****************************************************************************
.first
	define wx [--.include.wx]

.ifdef __WXMOTIF__
CXX_DEFINE = /define=(__WXMOTIF__=1)/name=(as_is,short)\
	   /assume=(nostdnew,noglobal_array_new)
CC_DEFINE = /define=(__WXMOTIF__=1)/name=(as_is,short)
.else
CXX_DEFINE =
.endif

.suffixes : .cpp

.cpp.obj :
	cxx $(CXXFLAGS)$(CXX_DEFINE) $(MMS$TARGET_NAME).cpp
.c.obj :
	cc $(CFLAGS)$(CC_DEFINE) $(MMS$TARGET_NAME).c

OBJECTS = \
		xmcombo.obj,\
		accel.obj,\
		app.obj,\
		bitmap.obj,\
		bmpbuttn.obj,\
		brush.obj,\
		button.obj,\
		checkbox.obj,\
		checklst.obj,\
		choice.obj,\
		clipbrd.obj,\
		colour.obj,\
		combobox.obj,\
		control.obj,\
		cursor.obj,\
		data.obj,\
		dataobj.obj,\
		dc.obj,\
		dcclient.obj,\
		dcmemory.obj,\
		dcscreen.obj,\
		dialog.obj,\
		filedlg.obj,\
		font.obj,\
		frame.obj,\
		gauge.obj,\
		gdiobj.obj,\
		glcanvas.obj,\
		gsockmot.obj,\
		icon.obj,\
		listbox.obj,\
		main.obj,\
		mdi.obj,\
		menu.obj,\
		menuitem.obj,\
		minifram.obj,\
		msgdlg.obj,\
		palette.obj,\
		pen.obj,\
		radiobox.obj,\
		radiobut.obj,\
		region.obj,\
		scrolbar.obj,\
		settings.obj,\
		slider.obj,\
		spinbutt.obj,\
		statbmp.obj,\
		statbox.obj,\
		stattext.obj,\
		textctrl.obj,\
		timer.obj,\
		toolbar.obj,\
		utils.obj,\
		window.obj

SOURCES = \
		accel.cpp,\
		app.cpp,\
		bitmap.cpp,\
		bmpbuttn.cpp,\
		brush.cpp,\
		button.cpp,\
		checkbox.cpp,\
		checklst.cpp,\
		choice.cpp,\
		clipbrd.cpp,\
		colour.cpp,\
		combobox.cpp,\
		control.cpp,\
		cursor.cpp,\
		data.cpp,\
		dataobj.cpp,\
		dc.cpp,\
		dcclient.cpp,\
		dcmemory.cpp,\
		dcscreen.cpp,\
		dialog.cpp,\
		filedlg.cpp,\
		font.cpp,\
		frame.cpp,\
		gauge.cpp,\
		gdiobj.cpp,\
		glcanvas.cpp,\
		gsockmot.c,\
		icon.cpp,\
		listbox.cpp,\
		main.cpp,\
		mdi.cpp,\
		menu.cpp,\
		menuitem.cpp,\
		minifram.cpp,\
		msgdlg.cpp,\
		palette.cpp,\
		pen.cpp,\
		radiobox.cpp,\
		radiobut.cpp,\
		region.cpp,\
		scrolbar.cpp,\
		settings.cpp,\
		slider.cpp,\
		spinbutt.cpp,\
		statbmp.cpp,\
		statbox.cpp,\
		stattext.cpp,\
		textctrl.cpp,\
		timer.cpp,\
		toolbar.cpp,\
		utils.cpp,\
		window.cpp,\
		[.xmcombo]xmcombo.c\

all : $(SOURCES)
	$(MMS)$(MMSQUALIFIERS) $(OBJECTS)
.ifdef __WXMOTIF__
	library [--.lib]libwx_motif.olb $(OBJECTS)
.endif

xmcombo.obj : [.xmcombo]xmcombo.c
	cc $(CFLAGS)$(CC_DEFINE) [.xmcombo]xmcombo.c

accel.obj : accel.cpp
app.obj : app.cpp
bitmap.obj : bitmap.cpp
bmpbuttn.obj : bmpbuttn.cpp
brush.obj : brush.cpp
button.obj : button.cpp
checkbox.obj : checkbox.cpp
checklst.obj : checklst.cpp
choice.obj : choice.cpp
clipbrd.obj : clipbrd.cpp
colour.obj : colour.cpp
combobox.obj : combobox.cpp
control.obj : control.cpp
cursor.obj : cursor.cpp
data.obj : data.cpp
dataobj.obj : dataobj.cpp
dc.obj : dc.cpp
dcclient.obj : dcclient.cpp
dcmemory.obj : dcmemory.cpp
dcscreen.obj : dcscreen.cpp
dialog.obj : dialog.cpp
filedlg.obj : filedlg.cpp
font.obj : font.cpp
frame.obj : frame.cpp
gauge.obj : gauge.cpp
gdiobj.obj : gdiobj.cpp
glcanvas.obj : glcanvas.cpp
gsockmot.obj : gsockmot.c
icon.obj : icon.cpp
listbox.obj : listbox.cpp
main.obj : main.cpp
mdi.obj : mdi.cpp
menu.obj : menu.cpp
menuitem.obj : menuitem.cpp
minifram.obj : minifram.cpp
msgdlg.obj : msgdlg.cpp
palette.obj : palette.cpp
pen.obj : pen.cpp
radiobox.obj : radiobox.cpp
radiobut.obj : radiobut.cpp
region.obj : region.cpp
scrolbar.obj : scrolbar.cpp
settings.obj : settings.cpp
slider.obj : slider.cpp
spinbutt.obj : spinbutt.cpp
statbmp.obj : statbmp.cpp
statbox.obj : statbox.cpp
stattext.obj : stattext.cpp
textctrl.obj : textctrl.cpp
timer.obj : timer.cpp
toolbar.obj : toolbar.cpp
utils.obj : utils.cpp
window.obj : window.cpp
