del /Q GDeskTunes.zip
rmdir /S /Q GDeskTunes
mkdir GDeskTunes
xcopy /S ..\..\..\Builds\build-GDeskTunes-Desktop_Qt_5_4_MSVC2012_OpenGL_32bit-Release\release GDeskTunes
del GDeskTunes\*.obj
del GDeskTunes\*.cpp
del GDeskTunes\*.moc
windeployQt GDeskTunes\GDeskTunes.exe
zip -r GDeskTunes_windows.zip GDeskTunes
