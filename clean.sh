#!/bin/bash

rm -rf conf
rm -rf m4
find -name configure      | xargs rm -rf
find -name Makefile       | xargs rm -rf
find -name Makefile.in    | xargs rm -rf
find -name config.h.in    | xargs rm -rf
find -name config.h.in~   | xargs rm -rf
find -name aclocal.m4     | xargs rm -rf
find -name autom4te.cache | xargs rm -rf
find -name .deps          | xargs rm -rf
find -name doxygen_sqlite3.db | xargs rm -rf
