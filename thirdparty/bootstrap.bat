@echo off
cd thirdparty

if exist "SPMediaKeyTap" goto LIBLASTFM

echo SPMediaKeyTap
git clone  https://github.com/nevyn/SPMediaKeyTap

:LIBLASTFM

if exist "liblastfm" goto QTSOLUTIONS

echo liblastfm
git clone https://github.com/lastfm/liblastfm

:QTSOLUTIONS

if exist "qt-solutions" goto QTWAITINGSPINNER

echo qt-solutions
git clone https://gitorious.org/qt-solutions/qt-solutions.git

:QTWAITINGSPINNER

if exist "QtWaitingSpinner" goto END

echo QtWaitingSpinner
git clone https://github.com/snowwlex/QtWaitingSpinner.git

:END
