#! /usr/bin/env bash

abort() {
    echo $*
    exit 1
}

features=('debug' 'plugin' 'iplookup' 'proxy' 'wget' 'libnotify')

VERSION=

clear
echo "Preparing new release, please stand by..."
if make dist-bzip2 >/dev/null; then
    VERSION=`grep AC_INIT configure.ac | sed -e 's#AC_INIT(\[pcmanx-gtk2\],\[##g' -e 's#\],\[http://groups.google.com/group/PCManX\])##g'`
else
    abort Tarball generation fails.
fi
echo

for ((i=0; i<${#features[@]}; i++)); do
    echo "Attempting to build ${features[$i]} support..."
    tar jxf pcmanx-gtk2-${VERSION}.tar.bz2
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
