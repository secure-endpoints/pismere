@echo off
setlocal

set WRAP_EXE=guiwrap.exe
set TEST_EXE=guiwrap-test.exe

set WRAP=%~dp0exe\obj\i386\dbg\%WRAP_EXE%
set TEST=%~dp0test\obj\i386\dbg\%TEST_EXE%

%WRAP% "Caption" #32770 "Caption2" #32770 --exec %TEST%

endlocal
