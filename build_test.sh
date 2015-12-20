#!/bin/sh

autoreconf --force
./configure --enable-debug --enable-use-inner-lib && make -j2 check

