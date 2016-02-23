copy /Y renameme32.sys KLInjDriver32.sys
copy /Y renameme64.sys KLInjDriver64.sys

madConfigDrv KLInjDriver32.sys KioskLogixAPI PrintMonitorXP.dll PrintMonitor32.dll -unsafeStopAllowed
madConfigDrv KLInjDriver64.sys KioskLogixAPI PrintMonitorXP.dll PrintMonitor32.dll PrintMonitor64.dll -unsafeStopAllowed

"C:\Users\MH\Documents\Visual Studio 2008\Projects\Hooks\Build\signtool.exe" sign /ph /v /f  "C:\Users\MH\Documents\Visual Studio 2008\Projects\Hooks\Build\OS201411046820.pfx" /p kiosklogix1862 /d "KioskLogix" /du "http://www.kiosklogix.com" /t "http://timestamp.globalsign.com/scripts/timstamp.dll" "C:\Users\MH\Documents\Visual Studio 2008\Projects\Hooks\Build\KLInjDriver32.sys"

"C:\Users\MH\Documents\Visual Studio 2008\Projects\Hooks\Build\signtool.exe" sign /a /v /ph /ac "C:\Users\MH\Documents\Visual Studio 2008\Projects\Hooks\Build\Kiosklogix_Certificate_2015.cer" /s my /n "Kiosk Logix LLC" /d "KioskLogix" /du "http://www.kiosklogix.com" /t "http://timestamp.globalsign.com/scripts/timstamp.dll" "C:\Users\MH\Documents\Visual Studio 2008\Projects\Hooks\Build\KLInjDriver32.sys"

"C:\Users\MH\Documents\Visual Studio 2008\Projects\Hooks\Build\signtool.exe" sign /v /ph /f  "C:\Users\MH\Documents\Visual Studio 2008\Projects\Hooks\Build\OS201411046820.pfx" /p kiosklogix1862 /d "KioskLogix" /du "http://www.kiosklogix.com" /t "http://timestamp.globalsign.com/scripts/timstamp.dll" "C:\Users\MH\Documents\Visual Studio 2008\Projects\Hooks\Build\KLInjDriver64.sys"

"C:\Users\MH\Documents\Visual Studio 2008\Projects\Hooks\Build\signtool.exe" sign /a /v /ph /ac "C:\Users\MH\Documents\Visual Studio 2008\Projects\Hooks\Build\Kiosklogix_Certificate_2015.cer" /s my /n "Kiosk Logix LLC" /d "KioskLogix" /du "http://www.kiosklogix.com" /t "http://timestamp.globalsign.com/scripts/timstamp.dll" "C:\Users\MH\Documents\Visual Studio 2008\Projects\Hooks\Build\KLInjDriver64.sys"

pause