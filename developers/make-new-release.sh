#!/bin/bash

abort() {
  echo $*
  exit 1
}

VERSION=

clear
echo "Preparing new release, please stand by..."
if make dist-bzip2 >/dev/null; then
    VERSION=`grep AC_INIT configure.ac | sed -e 's#AC_INIT(\[pcmanx-gtk2\],\[##g' -e 's#\],\[http://groups.google.com/group/PCManX\])##g'`
else
    abort Tarball generation fails.    
fi
echo

echo "Attempting to build plugin support..."
tar jxf pcmanx-gtk2-${VERSION}.tar.bz2
pushd pcmanx-gtk2-${VERSION}
if ./configure --enable-plugin >/dev/null && make >/dev/null 2>/dev/null; then
  echo -e "\033[44;37mPassed!\033[m"
else
  abort Plugin build fails.
fi
popd
rm -rf pcmanx-gtk2-${VERSION}
echo

echo "Attempting to build wget support..."
tar jxf pcmanx-gtk2-${VERSION}.tar.bz2
pushd pcmanx-gtk2-${VERSION}
if ./configure --enable-wget >/dev/null && make >/dev/null 2>/dev/null; then
  echo -e "\033[44;37mPassed!\033[m"
else
  abort External wget support build fails.
fi
popd
rm -rf pcmanx-gtk2-${VERSION}
echo

echo "Tarball pcmanx-gtk2-${VERSION} is ready now!"
echo
