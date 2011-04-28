# Microsoft Developer Studio Generated NMAKE File, Based on wshtest32.dsp
!IF "$(CFG)" == ""
CFG=wshtest32 - Win32 Debug
!MESSAGE No configuration specified. Defaulting to wshtest32 - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "wshtest32 - Win32 Release" && "$(CFG)" != "wshtest32 - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "wshtest32.mak" CFG="wshtest32 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "wshtest32 - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "wshtest32 - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "wshtest32 - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\wshtest32.exe"


CLEAN :
	-@erase "$(INTDIR)\hesdlg.obj"
	-@erase "$(INTDIR)\hespdlg.obj"
	-@erase "$(INTDIR)\mainfrm.obj"
	-@erase "$(INTDIR)\querydlg.obj"
	-@erase "$(INTDIR)\respdlg.obj"
	-@erase "$(INTDIR)\stdafx.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\wshtedoc.obj"
	-@erase "$(INTDIR)\wshtest.obj"
	-@erase "$(INTDIR)\wshtest.res"
	-@erase "$(INTDIR)\wshtevw.obj"
	-@erase "$(OUTDIR)\wshtest32.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MD /W3 /GX /O2 /I "..\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /Fp"$(INTDIR)\wshtest32.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

MTL=midl.exe
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\wshtest.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\wshtest32.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=wshelp32.lib wsock32.lib /nologo /subsystem:windows /incremental:no /pdb:"$(OUTDIR)\wshtest32.pdb" /machine:I386 /out:"$(OUTDIR)\wshtest32.exe" /libpath:"../wshelper/$(OUTDIR)" 
LINK32_OBJS= \
	"$(INTDIR)\hesdlg.obj" \
	"$(INTDIR)\hespdlg.obj" \
	"$(INTDIR)\mainfrm.obj" \
	"$(INTDIR)\querydlg.obj" \
	"$(INTDIR)\respdlg.obj" \
	"$(INTDIR)\stdafx.obj" \
	"$(INTDIR)\wshtedoc.obj" \
	"$(INTDIR)\wshtest.obj" \
	"$(INTDIR)\wshtevw.obj" \
	"$(INTDIR)\wshtest.res"

"$(OUTDIR)\wshtest32.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "wshtest32 - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\wshtest32.exe"


CLEAN :
	-@erase "$(INTDIR)\hesdlg.obj"
	-@erase "$(INTDIR)\hespdlg.obj"
	-@erase "$(INTDIR)\mainfrm.obj"
	-@erase "$(INTDIR)\querydlg.obj"
	-@erase "$(INTDIR)\respdlg.obj"
	-@erase "$(INTDIR)\stdafx.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\wshtedoc.obj"
	-@erase "$(INTDIR)\wshtest.obj"
	-@erase "$(INTDIR)\wshtest.res"
	-@erase "$(INTDIR)\wshtevw.obj"
	-@erase "$(OUTDIR)\wshtest32.exe"
	-@erase "$(OUTDIR)\wshtest32.ilk"
	-@erase "$(OUTDIR)\wshtest32.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /Fp"$(INTDIR)\wshtest32.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

MTL=midl.exe
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\wshtest.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\wshtest32.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=wshelp32.lib wsock32.lib /nologo /subsystem:windows /incremental:yes /pdb:"$(OUTDIR)\wshtest32.pdb" /debug /machine:I386 /out:"$(OUTDIR)\wshtest32.exe" /pdbtype:sept /libpath:"../wshelper/$(OUTDIR)" 
LINK32_OBJS= \
	"$(INTDIR)\hesdlg.obj" \
	"$(INTDIR)\hespdlg.obj" \
	"$(INTDIR)\mainfrm.obj" \
	"$(INTDIR)\querydlg.obj" \
	"$(INTDIR)\respdlg.obj" \
	"$(INTDIR)\stdafx.obj" \
	"$(INTDIR)\wshtedoc.obj" \
	"$(INTDIR)\wshtest.obj" \
	"$(INTDIR)\wshtevw.obj" \
	"$(INTDIR)\wshtest.res"

"$(OUTDIR)\wshtest32.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("wshtest32.dep")
!INCLUDE "wshtest32.dep"
!ELSE 
!MESSAGE Warning: cannot find "wshtest32.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "wshtest32 - Win32 Release" || "$(CFG)" == "wshtest32 - Win32 Debug"
SOURCE=.\hesdlg.cpp

"$(INTDIR)\hesdlg.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\hespdlg.cpp

"$(INTDIR)\hespdlg.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\mainfrm.cpp

"$(INTDIR)\mainfrm.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\querydlg.cpp

"$(INTDIR)\querydlg.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\respdlg.cpp

"$(INTDIR)\respdlg.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\stdafx.cpp

"$(INTDIR)\stdafx.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\wshtedoc.cpp

"$(INTDIR)\wshtedoc.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\wshtest.cpp

"$(INTDIR)\wshtest.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\wshtevw.cpp

"$(INTDIR)\wshtevw.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\wshtest.rc

"$(INTDIR)\wshtest.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 

