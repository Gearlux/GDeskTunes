PATH=%PATH%;C:\Qt\5.4\msvc2013_opengl\bin
REM PATH=%PATH%;E:\Source\Qt\5.4\msvc2013_opengl\bin

REM Cleanup install distribution files
del /Q GDeskTunes_windows.zip
del /Q GDeskTunes.zip
rmdir /S /Q GDeskTunes
del /Q installer\packages\com.gearlux.gdesktunes\data\*

mkdir GDeskTunes
xcopy /S %USERPROFILE%\builds\build-GDeskTunes-Desktop_Qt_5_4_0_MSVC2013_OpenGL_32bit-Release\release GDeskTunes
REM xcopy /S %USERPROFILE%\Builds\build-GDeskTunes-Desktop_Qt_5_4_0_MSVC2013_32bit-Release\release GDeskTunes
del GDeskTunes\*.obj
del GDeskTunes\*.cpp
del GDeskTunes\*.moc
windeployQt GDeskTunes\GDeskTunes.exe
REM zip -r GDeskTunes_windows.zip GDeskTunes
xcopy /S /Y GDeskTunes\* installer\packages\org.gearlux.gdesktunes\data
REM type nul >> installer\packages\org.gearlux.gdesktunes\data\Uninstall.exe
del /Q GDeskTunesInstaller.exe
C:\Qt\QtIFW-1.5.0\bin\binarycreator.exe --offline-only -c installer\config\config.xml -p installer\packages GDeskTunesInstaller.exe
REM E:\Source\Qt\QtIFW-1.5.0\bin\binarycreator.exe --offline-only -c installer\config\config.xml -p installer\packages GDeskTunesInstaller.exe
