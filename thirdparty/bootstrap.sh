#!/bin/sh

cd thirdparty

if [ -d  "SPMediaKeyTap" ]; then
    cd SPMediaKeyTap
    git pull --rebase
    cd -
else
    git clone https://github.com/nevyn/SPMediaKeyTap
fi

if [ -d "liblastfm" ]; then
	cd liblastfm
	git pull --rebase
	cd -
else
	git clone https://github.com/lastfm/liblastfm
fi

if [ -d "qt-solutions" ]; then
	cd qt-solutions
	git pull --rebase
	cd -
else
	git clone https://gitorious.org/qt-solutions/qt-solutions.git
fi

if [ -d "QtWaitingSpinner" ]; then
	cd QtWaitingSpinner
	git pull --rebase
	cd -
else
	git clone https://github.com/snowwlex/QtWaitingSpinner.git
fi
