#!/bin/sh
rm -f GDeskTunes.dmg
rm -rf GDeskTunes.app
cp -a ../release/GDeskTunes.app .
~/Qt/5.4/clang_64/bin/macdeployqt GDeskTunes.app
hdiutil create -volname GDeskTunes -srcFolder GDeskTunes.app -ov -format UDZO GDeskTunes.dmg
