PATH=%PATH%;C:\Qt\5.4\msvc2013_opengl\bin

REM Cleanup install distribution files
del /Q GDeskTunes_windows.zip
del /Q GDeskTunes.zip
rmdir /S /Q GDeskTunes
del /Q installer\packages\com.gearlux.gdesktunes\data\*

mkdir GDeskTunes
REM xcopy /S ..\..\..\Builds\build-GDeskTunes-Desktop_Qt_5_4_MSVC2012_OpenGL_32bit-Release\release GDeskTunes
xcopy /S ..\..\build-GDeskTunes-Desktop_Qt_5_4_0_MSVC2013_OpenGL_32bit-Release\release GDeskTunes
del GDeskTunes\*.obj
del GDeskTunes\*.cpp
del GDeskTunes\*.moc
windeployQt GDeskTunes\GDeskTunes.exe
REM zip -r GDeskTunes_windows.zip GDeskTunes
xcopy /S /Y GDeskTunes\* installer\packages\org.gearlux.gdesktunes\data
del /Q GDeskTunesInstaller.exe
C:\Qt\QtIFW-1.5.0\bin\binarycreator.exe --offline-only -c installer\config\config.xml -p installer\packages GDeskTunesInstaller.exe
