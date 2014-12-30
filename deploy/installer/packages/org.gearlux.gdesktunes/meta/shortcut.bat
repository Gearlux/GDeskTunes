@echo off
echo Set oWS = WScript.CreateObject("WScript.Shell") > %TMP%\CreateShortcut.vbs
echo sLinkFile = %2 >> %TMP%\CreateShortcut.vbs
echo Set oLink = oWS.CreateShortcut(sLinkFile) >> %TMP%\CreateShortcut.vbs
echo oLink.TargetPath = %1 >> %TMP%\CreateShortcut.vbs
echo oLink.Save >> %TMP%\CreateShortcut.vbs
cscript %TMP%\CreateShortcut.vbs
del %TMP%\CreateShortcut.vbs
