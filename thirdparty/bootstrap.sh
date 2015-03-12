#!/bin/sh

if [ ! -d  "SPMediaKeyTap" ]; then
    git clone https://github.com/nevyn/SPMediaKeyTap
    cp SPMediaKeyTap.pro SPMediaKeyTap
fi

if [ ! -d "liblastfm" ]; then
    git clone https://github.com/lastfm/liblastfm
    cp liblastfm.pro liblastfm
fi

if [ ! -d "qt-solutions" ]; then
    git clone https://gitorious.org/qt-solutions/qt-solutions.git
    cp qt-solutions.pro qt-solutions
fi

if [ ! -d "QtWaitingSpinner" ]; then
    git clone https://github.com/snowwlex/QtWaitingSpinner.git
    cp QtWaitingSpinner.pro QtWaitingSpinner
fi

if [ ! -d "libsass" ]; then
    git clone https://github.com/sass/libsass.git
    make -C libsass -j5
fi

if [ ! -d "libsass/sassc" ]; then
    # only needed for sassc and/or testsuite
    git clone https://github.com/sass/sassc.git libsass/sassc
    make -C libsass -j5 sassc
fi
