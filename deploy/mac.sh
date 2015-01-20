#!/bin/sh

# GDESKTUNES=~/builds/build-GDeskTunes-Desktop_Qt_5_4_clang_64bit-Release
GDESKTUNES=../
QTDIR=/Users/gert/Qt/5.4/clang_64/bin

rm -rf build
mkdir build
cd build
$QTDIR/qmake ../../GDeskTunes.pro -r -spec macx-clang CONFIG+=x86_64
make 
make install
cd ..

rm -rf GDeskTunes.app
rm -rf installer/packages/org.gearlux.gdesktunes/data/*

cp -a build/src/release/GDeskTunes.app .
$QTDIR/macdeployqt GDeskTunes.app
make
rm -rf GDeskTunes.app

# hdiutil create -volname GDeskTunesInstaller -srcFolder GDeskTunesInstaller -ov -format UDZO GDeskTunes.dmg

# The 1.5.0 installer does not work on mac
#cp -a GDeskTunes.app installer/packages/org.gearlux.gdesktunes/data
#rm -f GDeskTunesInstaller.dmg

#~/Qt/QtIFW-1.5.0/bin/binarycreator --offline-only -c installer/config/config.xml -p installer/packages GDeskTunesInstaller.dmg
