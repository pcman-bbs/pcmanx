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
cd debian && rm -f *.ex *.EX && cd ..
mkdir debian/source && echo "3.0 (quilt)" > debian/source/format
cat > debian/control <<ENDLINE
Source: pcmanx-gtk2
Section: x11
Priority: optional
Maintainer: ${DEBFULLNAME} <${DEBEMAIL}>
Build-Depends: debhelper (>= 7.0.50~), autotools-dev, libgtk2.0-dev, libnotify-dev, xulrunner-1.9.2-dev, libx11-dev
Standards-Version: 3.8.4
Homepage: http://code.google.com/p/pcmanx-gtk2/

Package: pcmanx-gtk2
Architecture: any
Depends: \${shlibs:Depends}, \${misc:Depends}, libglib2.0-0, libgtk2.0-0, libpango1.0-0, libstdc++6, libx11-6
Description: user-friendly telnet client mainly targets BBS users
 PCMan X is a newly developed GPL'd version of PCMan, a full-featured
 famous BBS client. It aimed to be an easy-to-use yet full-featured telnet
 client facilitating BBS browsing with the ability to process double-byte
 characters. Some handy functions like tabbed-browsing, auto-login and
 a built-in ANSI editor enabling colored text editing are also provided.
 .
 This version is developed with pure gtk2 and xft, thus has much low
 dependency.

Package: mozilla-plugin-pcmanx
Section: web
Architecture: any
Depends: \${shlibs:Depends}, \${misc:Depends}
Recommends: iceweasel | iceape-browser | firefox | abrowser | seamonkey-browser
Description: pcmanx plugin for Mozilla based browser
 This plugin adds support for telnet:// protocol to your Mozilla based
 browser. The protocol engine is done by PCManX and the output window
 is embedded in a webpage in the browser window.
 .
 PCMan X is a newly developed GPL'd version of PCMan, a full-featured
 famous BBS client. It aimed to be an easy-to-use yet full-featured telnet
 client facilitating BBS browsing with the ability to process double-byte
 characters. Some handy functions like tabbed-browsing, auto-login and
 a built-in ANSI editor enabling colored text editing are also provided.

Package: libpcmanx-core0
Section: libs
Architecture: any
Depends: \${shlibs:Depends}, \${misc:Depends}
Description: core rendering library of pcmanx
 This package contains the core rendering function of pcmanx in dynamic
 linked library format.
 .
 PCMan X is a newly developed GPL'd version of PCMan, a full-featured
 famous BBS client. It aimed to be an easy-to-use yet full-featured telnet
 client facilitating BBS browsing with the ability to process double-byte
 characters. Some handy functions like tabbed-browsing, auto-login and
 a built-in ANSI editor enabling colored text editing are also provided.
ENDLINE
cat > debian/pcmanx-gtk2.install <<ENDLINE
usr/bin
usr/share
ENDLINE
cat > debian/libpcmanx-core0.install <<ENDLINE
usr/lib/libpcmanx_core*.so.*
ENDLINE
cat > debian/mozilla-plugin-pcmanx.install <<ENDLINE
usr/lib/xulrunner-devel-*/bin/plugins /usr/lib/pcmanx-gtk2
usr/lib/xulrunner-devel-*/bin/components /usr/lib/pcmanx-gtk2
ENDLINE
cat > debian/mozilla-plugin-pcmanx.links <<ENDLINE
usr/lib/pcmanx-gtk2/plugins/pcmanx-plugin.so usr/lib/mozilla/plugins/pcmanx-plugin.so
usr/lib/pcmanx-gtk2/components/TelnetProtocol.js usr/lib/mozilla/components/TelnetProtocol.js
usr/lib/pcmanx-gtk2/components/pcmanx_interface.xpt usr/lib/mozilla/components/pcmanx_interface.xpt
usr/lib/pcmanx-gtk2/components/pcmanx.html usr/lib/mozilla/components/pcmanx.html
usr/lib/pcmanx-gtk2/components/pcmanx.png usr/lib/mozilla/components/pcmanx.png
usr/lib/pcmanx-gtk2/plugins/pcmanx-plugin.so usr/lib/xulrunner-addons/plugins/pcmanx-plugin.so
usr/lib/pcmanx-gtk2/components/TelnetProtocol.js usr/lib/xulrunner-addons/components/TelnetProtocol.js
usr/lib/pcmanx-gtk2/components/pcmanx_interface.xpt usr/lib/xulrunner-addons/components/pcmanx_interface.xpt
usr/lib/pcmanx-gtk2/components/pcmanx.html usr/lib/xulrunner-addons/components/pcmanx.html
usr/lib/pcmanx-gtk2/components/pcmanx.png usr/lib/xulrunner-addons/components/pcmanx.png
ENDLINE
cat > debian/changelog <<ENDLINE
pcmanx-gtk2 (${VER}-svn${REV}) experimental; urgency=low

  * Initial release (Revision Number: ${REV})

 -- ${DEBFULLNAME} <${DEBEMAIL}>  $(LANG=C date -R)
ENDLINE
cat >> debian/rules <<ENDLINE
override_dh_auto_configure:
	dh_auto_configure -- --enable-debug --enable-plugin --enable-iplookup --enable-proxy --enable-wget --enable-libnotify
ENDLINE
debuild -uc -us
popd
