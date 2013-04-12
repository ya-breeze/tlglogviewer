#!/bin/bash

git archive HEAD -o ../tlglogviewer_`head -n1 debian/changelog | cut -d\( -f2 | cut -d- -f1`.orig.tar.gz

mv -f src/src.pro.user /tmp

make -C src distclean || echo
debuild clean
debuild

mv -f /tmp/src.pro.user src

debuild clean
