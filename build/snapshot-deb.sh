#! /usr/bin/env bash

[ -z "${DEBFULLNAME}" ] && export DEBFULLNAME="PCManX Development Group"
[ -z "${DEBEMAIL}" ] && export DEBEMAIL="pcmanx@googlegroups.com"

[ -d '.svn' ] && SCM='svn'
[ -d '../.git' ] && SCM='git'

case "${SCM}" in
    ('svn')
    REV="$(LANG=C svn info .. | grep 'Last Changed Rev' | awk '{ print $4 }')"
    ;;
    ('git')
    REV="$(LANG=C cd .. && git svn info | grep 'Last Changed Rev' | awk '{ print $4 }')"
    ;;
esac

## change version number ##
if ! grep AC_INIT ../configure.ac | cut -d ',' -f 2 | grep "svn${REV}" > /dev/null; then
    sed -i "s/AC_INIT(\[pcmanx-gtk2\],\[\([0-9]*\)\.\([0-9]*\)\.\([0-9]*\)\]/AC_INIT([pcmanx-gtk2],[\1.\2.\3-svn${REV}]/" ../configure.ac
fi

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
sed -i "s/AC_INIT(\[pcmanx-gtk2\],\[\([0-9]*\)\.\([0-9]*\)\.\([0-9]*\)-svn\([0-9]*\)\]/AC_INIT([pcmanx-gtk2],[\1.\2.\3]/" ../configure.ac

pushd "pcmanx-gtk2-${VER}"
cp -a ../../debian .
cat > debian/changelog <<ENDLINE
pcmanx-gtk2 (${VER}-1) experimental; urgency=low

  * Development release.

 -- ${DEBFULLNAME} <${DEBEMAIL}>  $(LANG=C date -R)
ENDLINE
debuild -uc -us
popd
