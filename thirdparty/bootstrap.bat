@echo off
cd thirdparty

echo SPMediaKeyTap

if not exist "SPMediaKeyTap" goto CLONESPMEDIAKEYTAP

cd SPMediaKeyTap 
git pull --rebase
cd ..

goto LIBLASTFM

:CLONESPMEDIAKEYTAP

git clone  https://github.com/nevyn/SPMediaKeyTap

:LIBLASTFM
echo liblastfm

if not exist "liblastfm" goto CLONELIBLASTFM

cd liblastfm 
git pull --rebase
cd ..

goto QTSOLUTIONS

:CLONELIBLASTFM

git clone https://github.com/lastfm/liblastfm

:QTSOLUTIONS
echo qt-solutions

if not exist "qt-solutions" goto CLONEQTSOLUTIONS

cd qt-solutions 
git pull --rebase
cd ..

goto END

:CLONEQTSOLUTIONS

git clone https://gitorious.org/qt-solutions/qt-solutions.git

:END