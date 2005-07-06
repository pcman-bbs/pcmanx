localedir = $(datadir)/locale

bin_PROGRAMS = pcmanx

INCLUDES = \
	$(GUI_CFLAGS) \
	-DLOCALEDIR=\"$(localedir)\" \
	-DDATADIR=\"$(datadir)\"

pcmanx_LDADD = $(GUI_LIBS)

OPTIONAL_DOCKLET = \
	docklet/clipboard.c \
	docklet/eggtrayicon.h \
	docklet/eggtrayicon.c

EXTRA_DIST = \
	Makefile-byhand \
	conn_xpm.xpm \
	pcmanx_xpm.xpm \
	$(OPTIONAL_DOCKLET)

if USE_DOCKLET
DOCKLET_SOURCES = $(OPTIONAL_DOCKLET)
endif

pcmanx_SOURCES = \
	$(DOCKLET_SOURCES) \
	appconfig.cpp appconfig.h \
	font.cpp font.h \
	notebook.cpp notebook.h \
	sitelistdlg.cpp sitelistdlg.h \
	termdata.cpp termdata.h \
	caret.cpp caret.h \
	generalprefpage.cpp generalprefpage.h \
	pcmanx_pure_gtk2.cpp \
	sitepage.cpp sitepage.h \
	termview.cpp termview.h \
	configfile.cpp configfile.h \
	inputdialog.cpp inputdialog.h \
	prefdlg.cpp prefdlg.h \
	stringutil.cpp stringutil.h \
	view.cpp view.h \
	dialog.cpp dialog.h \
	listbox.cpp listbox.h \
	site.cpp site.h \
	telnetcon.cpp telnetcon.h \
	widget.cpp widget.h \
	editfavdlg.cpp editfavdlg.h \
	mainframe.cpp mainframe.h \
	sitedlg.cpp sitedlg.h \
	telnetview.cpp telnetview.h

all-local:
	strip pcmanx
