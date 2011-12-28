#!/bin/sh
# Convert git log to GNU-style ChangeLog file.

if [ -d .git ]; then
    echo
elif [ -d ../.git ]; then
    cd ..
fi

git log --date-order --date=short --since="Wed Dec 28 16:23:27 2011 +0800" | \
  sed -e '/^commit.*$/d' | \
  awk '/^Author/ {sub(/\\$/,""); getline t; print $0 t; next}; 1' | \
  sed -e 's/^Author: //g' | \
  sed -e 's/>Date:   \([0-9]*-[0-9]*-[0-9]*\)/>\t\1/g' | \
  sed -e 's/^\(.*\) \(<.*>\)\t\(.*\)/\3  \1 \2/g' -e 's/^    /\t/g'
