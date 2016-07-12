#! /bin/sh
AM_VERSION=
AC_VERSION=

set -e -x

if [ ! -e ChangeLog ]; then
  if which git >/dev/null 2>&1 && [ -e ./build/changelog.sh ]; then
    ./build/changelog.sh > ChangeLog
  else
    mv ChangeLog.old ChangeLog
  fi
fi

if [ "x${ACLOCAL_DIR}" != "x" ]; then
  ACLOCAL_ARG=-I ${ACLOCAL_DIR}
fi

${ACLOCAL:-aclocal$AM_VERSION} ${ACLOCAL_ARG}
${AUTOHEADER:-autoheader$AC_VERSION}
if [ "`uname`" = "Darwin" ]; then
    AUTOMAKE=${AUTOMAKE:-automake$AM_VERSION} glibtoolize -c --automake --force
else
    AUTOMAKE=${AUTOMAKE:-automake$AM_VERSION} libtoolize -c --automake --force
fi
AUTOMAKE=${AUTOMAKE:-automake$AM_VERSION} intltoolize -c --automake --force
${AUTOMAKE:-automake$AM_VERSION} --add-missing --copy --include-deps
${AUTOCONF:-autoconf$AC_VERSION}

rm -rf autom4te.cache
