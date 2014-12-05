#!/bin/sh

cd thirdparty

echo "SPMediaKeyTap" 
if [ -d  "SPMediaKeyTap" ]; then
    cd SPMediaKeyTap
    git pull --rebase
    cd -
else
    git clone https://github.com/nevyn/SPMediaKeyTap
fi

echo "liblastfm" 
if [ -d "liblastfm" ]; then
	cd liblastfm
	git pull --rebase
	cd -
else
	git clone https://github.com/lastfm/liblastfm
fi

echo "qt-solutions"
if [ -d "qt-solutions" ]; then
	cd qt-solutions
	git pull --rebase
	cd -
else
	git clone https://gitorious.org/qt-solutions/qt-solutions.git
fi
