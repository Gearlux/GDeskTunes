REM PATH=%PATH%;C:\Qt\5.4\msvc2013_opengl\bin
REM PATH=%PATH%;E:\Source\Qt\5.4\msvc2013_opengl\bin

rmdir /S /Q build
mkdir build
cd build
qmake ..\..\GDeskTunes.pro
nmake release install
cd ..

REM Cleanup install distribution files
rmdir /S /Q GDeskTunes
mkdir GDeskTunes
xcopy /S build\src\release GDeskTunes
del GDeskTunes\*.obj
del GDeskTunes\*.cpp
del GDeskTunes\*.moc
windeployQt GDeskTunes\GDeskTunes.exe

del /Q installer\packages\com.gearlux.gdesktunes\data\*
xcopy /S /Y GDeskTunes\* installer\packages\org.gearlux.gdesktunes\data
xcopy /Y installer\packages\org.gearlux.gdesktunes\meta\shortcut.bat installer\packages\org.gearlux.gdesktunes\data

del /Q GDeskTunesInstaller.exe
REM C:\Qt\QtIFW-1.5.0\bin\binarycreator.exe --offline-only -c installer\config\config.xml -p installer\packages GDeskTunesInstaller.exe
E:\Source\Qt\QtIFW-1.5.0\bin\binarycreator.exe --offline-only -c installer\config\config.xml -p installer\packages GDeskTunesInstaller.exe
