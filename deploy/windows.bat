PATH=%PATH%;C:\Qt\5.4\msvc2013_opengl\bin
del /Q GDeskTunes.zip
rmdir /S /Q GDeskTunes
mkdir GDeskTunes
REM xcopy /S ..\..\..\Builds\build-GDeskTunes-Desktop_Qt_5_4_MSVC2012_OpenGL_32bit-Release\release GDeskTunes
xcopy /S ..\..\build-GDeskTunes-Desktop_Qt_5_4_0_MSVC2013_OpenGL_32bit-Release\release GDeskTunes
del GDeskTunes\*.obj
del GDeskTunes\*.cpp
del GDeskTunes\*.moc
windeployQt GDeskTunes\GDeskTunes.exe
zip -r GDeskTunes_windows.zip GDeskTunes
