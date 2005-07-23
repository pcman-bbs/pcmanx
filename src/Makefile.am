SUBDIRS = core

localedir = $(datadir)/locale
LIBPCMANX_CORE = $(top_builddir)/src/core/libpcmanx_core.la

bin_PROGRAMS = pcmanx

INCLUDES = \
	-I$(top_srcdir)/src/core \
	-I$(top_srcdir)/src/view \
	$(GUI_CFLAGS) \
	-DLOCALEDIR=\"$(localedir)\" \
	-DDATADIR=\"$(datadir)\"

pcmanx_LDADD = $(GUI_LIBS) $(LIBPCMANX_CORE)

OPTIONAL_DOCKLET = \
	docklet/clipboard.c \
	docklet/eggtrayicon.h \
	docklet/eggtrayicon.c

OPTIONAL_NOTIFIER = \
	notifier/api.h \
	notifier/notifier-impl.c \
	notifier/working_area.h \
	notifier/working_area.c

EXTRA_DIST = \
	Makefile-byhand \
	conn_xpm.xpm \
	pcmanx_xpm.xpm \
	pcmanx_inverse_xpm.xpm \
	$(OPTIONAL_DOCKLET) \
	$(OPTIONAL_NOTIFIER)

if USE_DOCKLET
DOCKLET_SOURCES = $(OPTIONAL_DOCKLET)
endif

if USE_NOTIFIER
NOTIFIER_SOURCES = $(OPTIONAL_NOTIFIER)
endif

VIEW_IMPLEMENTATION_SOURCES = \
	view/telnetcon.cpp view/telnetcon.h \
	view/telnetview.cpp view/telnetview.h

pcmanx_SOURCES = \
	$(DOCKLET_SOURCES) \
	$(NOTIFIER_SOURCES) \
	$(VIEW_IMPLEMENTATION_SOURCES) \
	appconfig.cpp appconfig.h \
	notebook.cpp notebook.h \
	sitelistdlg.cpp sitelistdlg.h \
	generalprefpage.cpp generalprefpage.h \
	pcmanx_pure_gtk2.cpp \
	sitepage.cpp sitepage.h \
	configfile.cpp configfile.h \
	inputdialog.cpp inputdialog.h \
	prefdlg.cpp prefdlg.h \
	dialog.cpp dialog.h \
	listbox.cpp listbox.h \
	editfavdlg.cpp editfavdlg.h \
	mainframe.cpp mainframe.h \
	sitedlg.cpp sitedlg.h \
	emoticondlg.cpp emoticondlg.h
