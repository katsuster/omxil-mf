#!/bin/bash

find -name configure      | xargs rm -f
find -name Makefile       | xargs rm -f
find -name Makefile.in    | xargs rm -f
find -name config.h.in    | xargs rm -f
find -name config.h.in~   | xargs rm -f
find -name aclocal.m4     | xargs rm -f
find -name libtool.m4     | xargs rm -f
find -name lt~obsolete.m4 | xargs rm -f
find -name ltoptions.m4   | xargs rm -f
find -name ltsugar.m4     | xargs rm -f
find -name ltversion.m4   | xargs rm -f
find -name ltmain.sh      | xargs rm -f
find -name config.guess   | xargs rm -f
find -name config.sub     | xargs rm -f
find -name compile     -a -executable | xargs rm -f
find -name depcomp     -a -executable | xargs rm -f
find -name install-sh  -a -executable | xargs rm -f
find -name missing     -a -executable | xargs rm -f
find -name test-driver -a -executable | xargs rm -f

find -name autom4te.cache | xargs rm -rf
find -name .deps          | xargs rm -rf
find -name doxygen_sqlite3.db | xargs rm -rf
