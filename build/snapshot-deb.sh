#! /usr/bin/env bash

[ -z "${DEBFULLNAME}" ] && read -p "DEBFULLNAME: " DEBFULLNAME && export DEBFULLNAME
[ -z "${DEBEMAIL}" ] && read -p "DEBEMAIL: " DEBEMAIL && export DEBEMAIL

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

VER="$(../configure --version | head -n1 | awk '{ print $3 }')"

[ ! -f 'Makefile' ] && ../configure

if make dist-bzip2 > /dev/null; then
    [ -d "pcmanx-gtk2-${VER}" ] && rm -fr "pcmanx-gtk2-${VER}"
    [ -f "pcmanx-gtk2_${VER}.orig.tar.gz" ] && rm -f "pcmanx-gtk2_${VER}.orig.tar.gz"
    tar xjf "pcmanx-gtk2-${VER}.tar.bz2"
else
    exit 1
fi

pushd "pcmanx-gtk2-${VER}"
dh_make -s -c gpl -f ../"pcmanx-gtk2-${VER}.tar.bz2"
cat > debian/control <<ENDLINE
Source: pcmanx-gtk2
Section: universe/x11
Priority: optional
Maintainer: ${DEBFULLNAME} <${DEBEMAIL}>
Build-Depends: debhelper (>= 7), autotools-dev, libgtk2.0-dev, libnotify-dev, xulrunner-1.9-dev | xulrunner-1.9.1-dev, libx11-dev
Standards-Version: 3.8.0
Homepage: http://code.google.com/p/pcmanx-gtk2/

Package: pcmanx-gtk2
Architecture: any
Depends: ${shlibs:Depends}, ${misc:Depends}, libglib2.0-0, libgtk2.0-0, libpango1.0-0, libstdc++6, libx11-6
Description: user-friendly telnet client mainly targets BBS users
 PCMan X is a newly developed GPL'd version of PCMan, a full-featured
 famous BBS client. It aimed to be an easy-to-use yet full-featured telnet
 client facilitating BBS browsing with the ability to process double-byte
 characters. Some handy functions like tabbed-browsing, auto-login and
 a built-in ANSI editor enabling colored text editing are also provided.
 .
 This version is developed with pure gtk2 and xft, thus has much low
 dependency.
ENDLINE
cat > debian/changelog <<ENDLINE
pcmanx-gtk2 (${VER}-svn${REV}) unstable; urgency=low

  * Initial release (Revision Number: ${REV})

 -- ${DEBFULLNAME} <${DEBEMAIL}>  $(LANG=C date -R)
ENDLINE
sed -i "s,./configure \(.*\)$,./configure --enable-debug --enable-plugin --enable-iplookup --enable-proxy --enable-wget --enable-libnotify \1," debian/rules
debuild -S
debuild -b
popd
