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

autoreconf --install --force
AUTOMAKE=${AUTOMAKE:-automake$AM_VERSION} intltoolize -c --automake --force

rm -rf autom4te.cache
