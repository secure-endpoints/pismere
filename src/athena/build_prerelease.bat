@echo off
set AWK=gawk
set APPVER=5.0
set TARGETOS=WINNT
SET _WIN32_IE=0x0500
SET _WIN32_WINNT=0x0500
unset KH_AFSPATH KH_KFWPATH KFWSDKDIR
REM OFFICIAL or PRERELEASE or PRIVATE
set KH_RELEASE=PRERELEASE
cmd.exe /c c:\perl64\bin\perl.exe ..\scripts\build.pl --tools /nologo BUILD_KFW=1 DEBUG_SYMBOL=1 %1 %2 %3 %4 %5 %6 %7 %8 %9