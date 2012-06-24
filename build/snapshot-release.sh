#! /usr/bin/env bash

DEB=0
RPM=0

if [ -z "$*" ]; then
    echo "Usage: $0 {deb,rpm}"
    exit
fi

for arg in $*; do
    case $arg in
	('deb')
	    DEB=1
	    ;;
	('rpm')
	    RPM=1
	    ;;
	(*)
	    echo "Usage: $0 {deb,rpm}"
	    exit
	    ;;
    esac
done

set -e -x

DATE=$(date -d"$(git show -s --format=%ci HEAD)" +%Y%m%d%H%M%S)
HASH=$(git show -s --format=%h HEAD)

## change version number ##
sed -i "s/AC_INIT(\[pcmanx-gtk2\],\[\([0-9]*\)\.\([0-9]*\)\]/AC_INIT([pcmanx-gtk2],[\1.\2+${DATE}+${HASH}]/" ../configure.ac

[ ! -f '../configure' ] && cd .. && ./autogen.sh && cd -
[ ! -f 'Makefile' ] && ../configure

VER="$(../configure --version | head -n1 | awk '{ print $3 }')"

if make dist > /dev/null; then
    [ -d "pcmanx-gtk2-${VER}" ] && rm -fr "pcmanx-gtk2-${VER}"
    [ -f "pcmanx-gtk2_${VER}.orig.tar.xz" ] && rm -f "pcmanx-gtk2_${VER}.orig.tar.xz"
    mv "pcmanx-gtk2-${VER}.tar.xz" "pcmanx-gtk2_${VER}.orig.tar.xz"
    tar xf "pcmanx-gtk2_${VER}.orig.tar.xz"
else
    exit 1
fi

## rollback version number ##
sed -i "s/AC_INIT(\[pcmanx-gtk2\],\[\([0-9]*\)\.\([0-9]*\)+\([0-9]*+[0-9a-f]*\)\]/AC_INIT([pcmanx-gtk2],[\1.\2]/" ../configure.ac

## Build Debian source package ##
if [ $DEB = 1 ]; then
    [ -z "${DEBFULLNAME}" ] && export DEBFULLNAME="PCManX Development Group"
    [ -z "${DEBEMAIL}" ] && export DEBEMAIL="pcmanx@googlegroups.com"
    pushd "pcmanx-gtk2-${VER}"
    cp -a ../../debian .
    mkdir -p debian/source
    echo "3.0 (quilt)" > debian/source/format
    cat > debian/changelog <<ENDLINE
pcmanx-gtk2 (${VER}-0~UNRELEASED1) UNRELEASED; urgency=low

  * Development release.

 -- ${DEBFULLNAME} <${DEBEMAIL}>  $(LANG=C date -R)
ENDLINE
    case "$(lsb_release -s -i)" in
	(Ubuntu)
	    for series in lucid maverick natty oneiric precise quantal; do
		sed -i "s/UNRELEASED/$series/g" debian/changelog
		dpkg-buildpackage -uc -us -S
		sed -i "s/$series/UNRELEASED/g" debian/changelog
	    done
	    ;;
	(Debian)
	    for series in oldstable stable testing unstable; do
		sed -i "s/UNRELEASED/$series/g" debian/changelog
		dpkg-buildpackage -uc -us -S
		sed -i "s/$series/UNRELEASED/g" debian/changelog
	    done
	    ;;
	(*)
	    debuild -us -uc
	    ;;
    esac
    popd
fi

## Build RPM source package ##
if [ $RPM = 1 ]; then
    cp -av pcmanx-gtk2_${VER}.orig.tar.xz $HOME/rpmbuild/SOURCES/pcmanx-gtk2-${VER}.tar.xz
    cp -av pcmanx-gtk2.spec $HOME/rpmbuild/SPECS/
    rpmbuild -bs $HOME/rpmbuild/SPECS/pcmanx-gtk2.spec
fi
