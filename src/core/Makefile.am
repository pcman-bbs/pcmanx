localedir = $(datadir)/locale

lib_LTLIBRARIES = libpcmanx_core.la

INCLUDES = \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/src/view \
	$(GUI_CFLAGS) \
	-DLOCALEDIR=\"$(localedir)\" \
	-DDATADIR=\"$(datadir)\"

if USE_PROXY
PROXY_SOURCES = \
	proxy.c proxy.h
endif

libpcmanx_core_la_LIBADD = $(GUI_LIBS)
libpcmanx_core_la_LDFLAGS = \
	-version-info @LIBTOOL_VERSION_INFO@ \
	-rpath $(libdir)
libpcmanx_core_la_SOURCES = \
	$(PROXY_SOURCES) \
	pcmanx_utils.h \
	caret.cpp caret.h \
	site.cpp site.h \
	termdata.cpp termdata.h \
	view.cpp view.h \
	font.cpp font.h \
	stringutil.cpp stringutil.h \
	widget.cpp widget.h \
	termview.cpp termview.h \
	termsel.cpp termsel.h \
	fileutil.c fileutil.h \
	pkgver.c
