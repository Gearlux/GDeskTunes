#!/bin/sh

cd thirdparty

if [ ! -d  "SPMediaKeyTap" ]; then
    git clone https://github.com/nevyn/SPMediaKeyTap
fi

if [ ! -d "liblastfm" ]; then
	git clone https://github.com/lastfm/liblastfm
fi

if [ ! -d "qt-solutions" ]; then
	git clone https://gitorious.org/qt-solutions/qt-solutions.git
fi

if [ ! -d "QtWaitingSpinner" ]; then
	git clone https://github.com/snowwlex/QtWaitingSpinner.git
fi
