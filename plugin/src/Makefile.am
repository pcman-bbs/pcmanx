EXTRA_DIST = pcmanx_interface.idl

if USE_PLUGIN

#####################################
# Building the Firefox/Mozilla plugin
#####################################

INCLUDES = \
	-I$(top_srcdir)/src/core \
	-I$(top_srcdir)/src/view \
	-I$(top_srcdir)/plugin/src \
	-I$(top_srcdir)/src \
	$(GUI_CFLAGS) \
	$(MOZILLA_CFLAGS) \
	-DMOZ_PLUGIN \
	-DDATADIR=\"$(datadir)\"

AM_CXXFLAGS = -fPIC

noinst_LIBRARIES = $(noinst_LIBRARIES_mozilla)

MOSTLYCLEANFILES = \
	$(LIBRARIES_mozilla) \
	pcmanx_interface.xpt \
	pcmanx_interface.h

BUILT_SOURCES = $(BUILT_SOURCES_mozilla)

SOURCES_view = \
	$(top_srcdir)/src/view/telnetview.cpp \
	$(top_srcdir)/src/view/telnetview.h \
	$(top_srcdir)/src/view/telnetcon.cpp \
	$(top_srcdir)/src/view/telnetcon.h

SOURCES_appconfig = \
	$(top_srcdir)/src/appconfig.cpp \
	$(top_srcdir)/src/appconfig.h \
	$(top_srcdir)/src/configfile.cpp \
	$(top_srcdir)/src/configfile.h

SOURCES_mozilla = \
	np_entry.cpp \
	npn_gate.cpp \
	npp_gate.cpp \
	nsScriptablePeer.cpp nsScriptablePeer.h \
	plugin.cpp plugin.h pluginbase.h npplat.h npupp.h

LIBRARIES_mozilla = pcmanx-plugin.so

noinst_LIBRARIES_mozilla = libplugin.a

libplugin_a_SOURCES = \
	$(SOURCES_mozilla) \
	$(SOURCES_view) \
	$(SOURCES_appconfig)

BUILT_SOURCES_mozilla = pcmanx_interface.h
$(SOURCES_mozilla): pcmanx_interface.h

plugin_DATA = $(LIBRARIES_mozilla)
plugindir = $(MOZILLA_LIB_DIR_SEARCH)/plugins

# We MUST make sure plugin was built as -shared, and we must use some tricky
# approaches to hanle libtool issues.
$(LIBRARIES_mozilla): $(libplugin_a_OBJECTS)
	$(CXX) $(LDFLAGS) -L$(top_builddir)/src/core/.libs \
	  -o $(LIBRARIES_mozilla) $(libplugin_a_OBJECTS) \
	  -shared $(GUI_LIBS) \
	-lpcmanx_core $(LIBUTIL)

pcmanx_interface_xpt_DATA = pcmanx_interface.xpt
pcmanx_interface_xptdir = $(MOZILLA_LIB_DIR_SEARCH)/components
pcmanx_interface.xpt: pcmanx_interface.idl
	$(XPIDL) $(XPIDL_INCL) \
	  -m typelib -o pcmanx_interface \
	  -w -v \
	  $(srcdir)/pcmanx_interface.idl

pcmanx_interface.h: pcmanx_interface.idl
	$(XPIDL) $(XPIDL_INCL) \
	  -m header -o pcmanx_interface \
	  -w -v \
	  $(srcdir)/pcmanx_interface.idl

endif
