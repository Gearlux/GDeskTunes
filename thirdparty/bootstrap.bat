@echo off

if exist "SPMediaKeyTap" goto LIBLASTFM

echo SPMediaKeyTap
git clone  https://github.com/nevyn/SPMediaKeyTap
copy SPMediaKeyTap.pro SPMediaKeyTap

:LIBLASTFM

if exist "liblastfm" goto QTSOLUTIONS

echo liblastfm
git clone https://github.com/lastfm/liblastfm
copy liblastfm.pro liblastfm

:QTSOLUTIONS

if exist "qt-solutions" goto QTWAITINGSPINNER

echo qt-solutions
git clone https://gitorious.org/qt-solutions/qt-solutions.git
copy qt-solutions.pro qt-solutions

:QTWAITINGSPINNER

if exist "QtWaitingSpinner" goto LIBSASS

echo QtWaitingSpinner
git clone https://github.com/snowwlex/QtWaitingSpinner.git
copy QtWaitingSpinner.pro QtWaitingSpinner

:LIBSASS

if exist "libsass" goto SASSC

echo LibSass
git clone https://github.com/sass/libsass.git

:SASSC

if exist "libsass\sassc" goto BUILDSASS

echo SassC
git clone https://github.com/sass/sassc.git libsass/sassc
cd libsass
git apply -v ..\libsass.vcxproj.patch
cd ..

:BUILDSASS
msbuild libsass\win\libsass.sln /p:Configuration=Release /m

:END
