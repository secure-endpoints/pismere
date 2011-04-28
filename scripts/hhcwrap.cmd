@echo off
if not "%~1"=="--inner" goto :doinner

setlocal ENABLEEXTENSIONS ENABLEDELAYEDEXPANSION
set ERRORLEVEL=
set NOFILE=No output file could be located.

rem Find guiwrap.exe.  If %GUIWRAP% exists, use that, otherwise search the path.

if not defined guiwrap for /f %%i in ("guiwrap.exe") do set guiwrap=%%~$PATH:i
if not defined guiwrap if exist "guiwrap.exe" set guiwrap=%~dp0guiwrap.exe
if not defined guiwrap goto :noguiwrap
if not exist "!guiwrap!" goto :noguiwrap
if exist "!guiwrap!\guiwrap.exe" set guiwrap=!guiwrap!\guiwrap.exe
echo guiwrap location=!guiwrap!

rem Parse commandline arguments

if "%~2"=="" goto :usage
set infiledir=%~dp2
set infile=%~dpn2.hhp
set desiredoutfile=%~3

echo input file = !infile!

rem Parse the input file for the location of the output file it generates.

if not "!infile!"=="" (
 if exist "!infile!" (
  for /f "usebackq eol=[ tokens=1,2 delims==" %%a in ("!infile!") do (
   if "%%a"=="Compiled file" set outfile=%%b
   )
  if defined outfile (
   pushd !infiledir!
   for /f "delims=" %%i in ("!outfile!") do set outfile=%%~dpnxi
   popd
  ) else ( 
   set outfile=!infile:~0,-4!.chm
  )
 )
)

if not defined desiredoutfile set desiredoutfile=!outfile!
echo temporary output file=!outfile!
echo permanent output file=!desiredoutfile!

rem Check for hhc.exe in the path
set hhcloc=
for /f %%i in ("hhc.exe") do set hhcloc=%%~$PATH:i
if not defined hhcloc goto :nohhc
echo hhc location = %hhcloc%

rem hhc "!infile!"
"!guiwrap!" "Error" #32770 --failure 0 --success 1 --exec hhc.exe "!infile!"

if not errorlevel 1 goto :couldnotcompile

move !outfile! !desiredoutfile!
if errorlevel 1 exit 1

endlocal
exit 0


:couldnotcompile
 echo.
 echo hhc.exe could not compile the project.
 if exist "!outfile!" del "!outfile!"
 exit 1

:nohhc
 echo.
 echo **********************************************************
 echo ERROR: HHC.EXE IS NOT IN THE PATH!!!!
 echo.
 echo You must either install the HTML Help Workshop or have 
 echo access to \\ntb.mit.edu\locker\htmlhelp. 
 echo.
 echo To install, run htmlhelp.exe which comes with Visual
 echo Studio. If HTML Help is already installed, add its 
 echo directory ^(e.g. !programfiles!\html help workshop^) 
 echo to your path.
 echo **********************************************************
 echo.
 exit 1

:noguiwrap
 echo.
 echo **********************************************************
 echo ERROR: COULD NOT FIND GUIWRAP.EXE!!!!
 echo.
 echo Please put guiwrap.exe in the path, or set an environment
 echo varible named GUIWRAP to the path and file, for instance
 echo SET GUIWRAP=%systemdrive%\tools\guiwrap\guiwrap.exe
 echo and make sure that guiwrap.exe actually exists there.
 echo **********************************************************
 echo.
 exit 1

:usage
 echo %~n0 is a wrapper for the HTML Help Workshop complier, hhc.exe.
 echo.
 echo Usage:   %~n0 helpproject.hhp [output]
 echo     where helpproject is the name of a help project file (the 
 echo     extension must be .hhp) and
 echo     where output is the optional name of the outputted file
 echo     or directory.
 echo.
 echo Example:  %~n0 %systemdrive%\srcdir\myproject.hhp %systemdrive%\srcdir\obj\myproject.chm
 echo.
 echo Note: because the hhp file specifies an output file, ("Compiled file=xxx") 
 echo     hhc.exe will create this file temporarily even if you specify an output.  
 echo     Make sure that this temporary name corresponds to a writeable directory and
 echo     does not conflict in name with a file you care about, for example:
 echo     "Compiled file=%%temp%%\tmpoutfile.chm" 
 endlocal
 exit 0

:doinner
  @cmd /c %~dp0\hhcwrap --inner %*