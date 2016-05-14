#!/bin/bash

find -name conf -a -type d | xargs rm -rf
find -name m4 -a -type d   | xargs rm -rf

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
find -name autom4te.cache | xargs rm -rf
find -name .deps          | xargs rm -rf
find -name doxygen_sqlite3.db | xargs rm -rf
