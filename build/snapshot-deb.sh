#! /usr/bin/env bash

set -e -x

[ -z "${DEBFULLNAME}" ] && export DEBFULLNAME="PCManX Development Group"
[ -z "${DEBEMAIL}" ] && export DEBEMAIL="pcmanx@googlegroups.com"

DATE=$(date -d"$(git show -s --format=%ci HEAD)" +%Y%m%d%H%M%S)
HASH=$(git show -s --format=%h HEAD)

## change version number ##
if ! grep AC_INIT ../configure.ac | cut -d ',' -f 2 | grep "svn${REV}" > /dev/null; then
    sed -i "s/AC_INIT(\[pcmanx-gtk2\],\[\([0-9]*\)\.\([0-9]*\)\]/AC_INIT([pcmanx-gtk2],[\1.\2+${DATE}~git${HASH}]/" ../configure.ac
fi

[ ! -e ../ChangeLog ] && ./changelog.sh > ../ChangeLog

[ ! -f '../configure' ] && cd .. && ./autogen.sh && cd build
[ ! -f 'Makefile' ] && ../configure

VER="$(../configure --version | head -n1 | awk '{ print $3 }')"

if make dist-gzip > /dev/null; then
    [ -d "pcmanx-gtk2-${VER}" ] && rm -fr "pcmanx-gtk2-${VER}"
    [ -f "pcmanx-gtk2_${VER}.orig.tar.gz" ] && rm -f "pcmanx-gtk2_${VER}.orig.tar.gz"
    mv "pcmanx-gtk2-${VER}.tar.gz" "pcmanx-gtk2_${VER}.orig.tar.gz"
    tar xf "pcmanx-gtk2_${VER}.orig.tar.gz"
else
    exit 1
fi

## rollback version number ##
sed -i "s/AC_INIT(\[pcmanx-gtk2\],\[\([0-9]*\)\.\([0-9]*\)+\([0-9]*~git[0-9a-f]*\)\]/AC_INIT([pcmanx-gtk2],[\1.\2]/" ../configure.ac

pushd "pcmanx-gtk2-${VER}"
cp -a ../../debian .
mkdir -p debian/source
echo "3.0 (quilt)" > debian/source/format
cat > debian/changelog <<ENDLINE
pcmanx-gtk2 (${VER}-0) UNRELEASED; urgency=low

  * Development release.

 -- ${DEBFULLNAME} <${DEBEMAIL}>  $(LANG=C date -R)
ENDLINE
debuild -uc -us
popd
