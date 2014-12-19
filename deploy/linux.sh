#!/bin/bash

QTDIR=~/Qt/5.4/gcc_64

rm -f GDeskTunes_linux.tgz
rm -rf GDeskTunes
mkdir GDeskTunes
cp -a ~/GDeskTunes/build-GDeskTunes-Desktop_Qt_Qt_Version_GCC_64bit-Release/release/GDeskTunes GDeskTunes
./cpld.bash GDeskTunes/GDeskTunes GDeskTunes
cp -a $QTDIR/plugins/* GDeskTunes
cp GDeskTunes.sh GDeskTunes

for f in $QTDIR/platforms/*
do
    echo $f
    ./cpld.bash $f GDeskTunes
done

for f in $QTDIR/imageformats/*
do
    echo $f
    ./cpld.bash $f GDeskTunes
done

for f in libQt5Test.so.5
do
    cp -a $QTDIR/lib/$f GDeskTunes
    ./cpld.bash $QTDIR/lib/$f GDeskTunes
done
tar cvfz GDeskTunes_linux.tgz GDeskTunes
