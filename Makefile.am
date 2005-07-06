## Makefile.am -- Process this file with automake to produce Makefile.in

AUX_DIST = \
	$(ac_aux_dir)/config.guess \
	$(ac_aux_dir)/config.sub \
	$(ac_aux_dir)/install-sh \
	$(ac_aux_dir)/ltconfig \
	$(ac_aux_dir)/ltmain.sh \
	$(ac_aux_dir)/missing \
	$(ac_aux_dir)/depcomp

EXTRA_DIST = \
	mkinstalldirs \
	autogen.sh \
	intltool-extract.in \
	intltool-merge.in \
	intltool-update.in \
	ChangeLog

## @end 1
AUTOMAKE_OPTIONS = gnu
SUBDIRS = src po data
## @start 1
MAINTAINERCLEANFILES = Makefile.in aclocal.m4 configure config.h.in $(AUX_DIST)

CLEANFILES = *.bak src/*.bak 

DISTCLEANFILES = intltool-extract intltool-merge intltool-update
