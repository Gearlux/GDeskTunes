REM PATH=%PATH%;C:\Qt\5.4\msvc2013_opengl\bin
REM PATH=%PATH%;E:\Source\Qt\5.4\msvc2013_opengl\bin
PATH=%PATH%;D:\Source\Qt\5.4\msvc2012_opengl\bin

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
REM rmdir /S /Q GDeskTunes\audio
REM rmdir /S /Q GDeskTunes\bearer
REM rmdir /S /Q GDeskTunes\iconengines
REM rmdir /S /Q GDeskTunes\imageformats
REM rmdir /S /Q GDeskTunes\mediaservice
REM rmdir /S /Q GDeskTunes\playlistformats
REM rmdir /S /Q GDeskTunes\position
REM rmdir /S /Q GDeskTunes\printsupport
REM rmdir /S /Q GDeskTunes\sensorgestures
REM rmdir /S /Q GDeskTunes\sensors
REM rmdir /S /Q GDeskTunes\sqldrivers
REM del /S GDeskTunes\*.qm
REM del /S GDeskTunes\Qt5Multimedia*
REM del /S GDeskTunes\Qt5OpenGL.dll
REM del /S GDeskTunes\Qt5Positioning.dll
REM del /S GDeskTunes\Qt5PrintSupport.dll
REM del /S GDeskTunes\Qt5Qml.dll
REM del /S GDeskTunes\Qt5Quick.dll
REM del /S GDeskTunes\Qt5Sensors.dll
REM del /S GDeskTunes\Qt5Sql.dll
REM del /S GDeskTunes\Qt5Svg.dll
REM del /S GDeskTunes\Qt5WebChannel.dll
REM del /S GDeskTunes\icu*


rmdir /Q /S installer\packages\org.gearlux.gdesktunes\data
mkdir installer\packages\org.gearlux.gdesktunes\data
xcopy /S /Y GDeskTunes\* installer\packages\org.gearlux.gdesktunes\data
xcopy /Y installer\packages\org.gearlux.gdesktunes\meta\shortcut.bat installer\packages\org.gearlux.gdesktunes\data

del /Q GDeskTunesInstaller.exe
REM C:\Qt\QtIFW-1.5.0\bin\binarycreator.exe --offline-only -c installer\config\config.xml -p installer\packages GDeskTunesInstaller.exe
REM E:\Source\Qt\QtIFW-1.5.0\bin\binarycreator.exe --offline-only -c installer\config\config.xml -p installer\packages GDeskTunesInstaller.exe
D:\Source\QtIFW-1.5.0\bin\binarycreator.exe --offline-only -c installer\config\config.xml -p installer\packages GDeskTunesInstaller.exe
