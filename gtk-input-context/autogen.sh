#!/bin/sh
[ -e config.cache ] && rm -f config.cache

libtoolize --automake
aclocal -I m4
autoconf
autoheader
automake -a
./configure $@
exit

