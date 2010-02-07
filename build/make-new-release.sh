#! /usr/bin/env bash

abort() {
    echo $*
    exit 1
}

features=('debug' 'plugin' 'iplookup' 'proxy' 'wget' 'libnotify')

VERSION=

[ ! -f 'Makefile' ] && ../configure

clear
echo "Preparing new release, please stand by..."
if make dist-bzip2 >/dev/null; then
    VERSION=$(../configure --help | head -n1 | awk '{ print $4 }')
else
    abort Tarball generation fails.
fi
echo

for ((i=0; i<${#features[@]}; i++)); do
    echo "Attempting to build ${features[$i]} support..."
    tar xjf pcmanx-gtk2-${VERSION}.tar.bz2
    pushd pcmanx-gtk2-${VERSION}
    if ./configure --enable-${features[$i]} > build.log && make >> build.log 2>&1; then
        echo -e "\033[44;37mPassed!\033[m"
    else
        abort "feature '${features[$i]}' build fails."
    fi
    popd
    rm -rf pcmanx-gtk2-${VERSION}
    echo
done

echo "Tarball pcmanx-gtk2-${VERSION} is ready now!"
echo
