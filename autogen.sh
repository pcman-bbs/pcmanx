#! /bin/sh
AM_VERSION=-1.9
AC_VERSION=

set -x

if [ "x${ACLOCAL_DIR}" != "x" ]; then
  ACLOCAL_ARG=-I ${ACLOCAL_DIR}
fi

${ACLOCAL:-aclocal$AM_VERSION} ${ACLOCAL_ARG}
${AUTOHEADER:-autoheader$AC_VERSION}
AUTOMAKE=${AUTOMAKE:-automake$AM_VERSION} libtoolize -c --automake 
AUTOMAKE=${AUTOMAKE:-automake$AM_VERSION} intltoolize -c --automake
${AUTOMAKE:-automake$AM_VERSION} --add-missing --copy --include-deps
${AUTOCONF:-autoconf$AC_VERSION}

# mkinstalldirs was not correctly installed in some cases.
cp -f /usr/share/automake-1.9/mkinstalldirs .

# quick fix against the strange intltool behavior
if [ -f po/Makefile.in.in ]; then
	sed -i -e 's/\@GENCAT\@/\/usr\/bin\/gencat/' \
	    -e 's/\@INTLTOOL_UPDATE\@/\/usr\/bin\/intltool-update/' \
	    -e 's/\@INTLTOOL_EXTRACT\@/\/usr\/bin\/intltool-extract/' \
	    po/Makefile.in.in
fi

rm -rf autom4te.cache
