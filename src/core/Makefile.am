localedir = $(datadir)/locale

noinst_LTLIBRARIES = libpcmanx_core.la

AM_CPPFLAGS = \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/src/view \
	-I$(top_srcdir)/src/charset \
	$(GUI_CFLAGS) \
	-DLOCALEDIR=\"$(localedir)\" \
	-DDATADIR=\"$(datadir)\"

if USE_PROXY
PROXY_SOURCES = \
	proxy.c proxy.h
endif

libpcmanx_core_la_LIBADD = \
	$(top_builddir)/src/charset/libpcmanx_charset.la \
	$(GUI_LIBS)
libpcmanx_core_la_SOURCES = \
	$(PROXY_SOURCES) \
	pcmanx_utils.h \
	caret.cpp caret.h \
	site.cpp site.h \
	termdata.cpp termdata.h \
	view.cpp view.h \
	font.cpp font.h \
	cfontconfig.cpp cfontconfig.h \
	stringutil.cpp stringutil.h \
	widget.cpp widget.h \
	termview.cpp termview.h \
	termsel.cpp termsel.h \
	fileutil.c fileutil.h \
	pkgver.c
