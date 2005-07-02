localedir = $(datadir)/locale

bin_PROGRAMS = pcmanx

INCLUDES = $(GUI_CFLAGS) -DLOCALEDIR=\"$(localedir)\"

pcmanx_LDADD = $(GUI_LIBS)

EXTRA_DIST = \
	Makefile-byhand \
	conn_xpm.xpm \
	pcmanx_xpm.xpm

pcmanx_SOURCES = \
	appconfig.cpp \
	font.cpp \
	notebook.cpp \
	sitelistdlg.cpp \
	termdata.cpp \
	caret.cpp \
	generalprefpage.cpp \
	pcmanx_pure_gtk2.cpp \
	sitepage.cpp \
	termview.cpp \
	configfile.cpp \
	inputdialog.cpp \
	prefdlg.cpp \
	stringutil.cpp \
	view.cpp \
	dialog.cpp \
	listbox.cpp \
	site.cpp \
	telnetcon.cpp \
	widget.cpp \
	editfavdlg.cpp \
	mainframe.cpp \
	sitedlg.cpp \
	telnetview.cpp
