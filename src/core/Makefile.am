localedir = $(datadir)/locale

lib_LTLIBRARIES = libpcmanx_core.la

INCLUDES = \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/src/view \
	$(GUI_CFLAGS) \
	-DLOCALEDIR=\"$(localedir)\" \
	-DDATADIR=\"$(datadir)\"

libpcmanx_core_la_LIBADD = $(GUI_LIBS)
libpcmanx_core_la_LDFLAGS = \
	-export-dynamic \
	@LIBTOOL_EXPORT_OPTIONS@ \
	-rpath $(libdir)
libpcmanx_core_la_SOURCES = \
	debug.h \
	caret.cpp caret.h \
	site.cpp site.h \
	termdata.cpp termdata.h \
	view.cpp view.h \
	font.cpp font.h \
	stringutil.cpp stringutil.h \
	widget.cpp widget.h \
	termview.cpp termview.h \
	termsel.cpp termsel.h \
	fileutil.cpp fileutil.h