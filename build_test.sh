#!/bin/sh

autoreconf --force --install
./configure --enable-debug --enable-use-inner-lib && make -j2 check

