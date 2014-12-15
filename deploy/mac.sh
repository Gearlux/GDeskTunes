#!/bin/sh

GDESKTUNES=~/builds/build-GDeskTunes-Desktop_Qt_5_4_0_clang_64bit-Release

rm -f GDeskTunes.dmg
rm -rf GDeskTunes.app
rm -rf installer/packages/org.gearlux.gdesktunes/data/*

cp -a $GDESKTUNES/release/GDeskTunes.app .
~/Qt/5.4/clang_64/bin/macdeployqt GDeskTunes.app
hdiutil create -volname GDeskTunes -srcFolder GDeskTunes.app -ov -format UDZO GDeskTunes.dmg

# The 1.5.0 installer does not work on mac
#cp -a GDeskTunes.app installer/packages/org.gearlux.gdesktunes/data
#rm -f GDeskTunesInstaller.dmg

#~/Qt/QtIFW-1.5.0/bin/binarycreator --offline-only -c installer/config/config.xml -p installer/packages GDeskTunesInstaller.dmg
