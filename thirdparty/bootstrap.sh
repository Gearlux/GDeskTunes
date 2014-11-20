#!/bin/sh

if [[ -d  "SPMediaKeyTap" ]]; then
    cd SPMediaKeyTap
    git pull --rebase
    cd -
else
    git clone https://github.com/nevyn/SPMediaKeyTap
fi

if [[ -d "liblastfm" ]]; then
	cd liblastfm
	git pull --rebase
	cd -
else
	git clone https://github.com/lastfm/liblastfm
fi
