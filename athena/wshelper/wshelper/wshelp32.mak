# Microsoft Developer Studio Generated NMAKE File, Based on wshelp32.dsp
!IF "$(CFG)" == ""
CFG=wshelp32 - Win32 Debug
!MESSAGE No configuration specified. Defaulting to wshelp32 - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "wshelp32 - Win32 Release" && "$(CFG)" != "wshelp32 - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "wshelp32.mak" CFG="wshelp32 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "wshelp32 - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "wshelp32 - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "wshelp32 - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\wshelp32.dll"


CLEAN :
	-@erase "$(INTDIR)\gethna.obj"
	-@erase "$(INTDIR)\herror.obj"
	-@erase "$(INTDIR)\hesiod.obj"
	-@erase "$(INTDIR)\hesmailh.obj"
	-@erase "$(INTDIR)\hespwnam.obj"
	-@erase "$(INTDIR)\hesservb.obj"
	-@erase "$(INTDIR)\inetaton.obj"
	-@erase "$(INTDIR)\res_comp.obj"
	-@erase "$(INTDIR)\res_debu.obj"
	-@erase "$(INTDIR)\res_init.obj"
	-@erase "$(INTDIR)\res_mkqu.obj"
	-@erase "$(INTDIR)\res_quer.obj"
	-@erase "$(INTDIR)\res_send.obj"
	-@erase "$(INTDIR)\resolve.obj"
	-@erase "$(INTDIR)\resource.res"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\wshelp32.dll"
	-@erase "$(OUTDIR)\wshelp32.exp"
	-@erase "$(OUTDIR)\wshelp32.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MT /W3 /GX /O2 /I "..\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "WSHELP32_EXPORTS" /Fp"$(INTDIR)\wshelp32.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\resource.res" /i "..\include" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\wshelp32.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=wsock32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /incremental:no /pdb:"$(OUTDIR)\wshelp32.pdb" /machine:I386 /def:".\wshelp32.def" /out:"$(OUTDIR)\wshelp32.dll" /implib:"$(OUTDIR)\wshelp32.lib" 
DEF_FILE= \
	".\wshelp32.def"
LINK32_OBJS= \
	"$(INTDIR)\gethna.obj" \
	"$(INTDIR)\herror.obj" \
	"$(INTDIR)\hesiod.obj" \
	"$(INTDIR)\hesmailh.obj" \
	"$(INTDIR)\hespwnam.obj" \
	"$(INTDIR)\hesservb.obj" \
	"$(INTDIR)\inetaton.obj" \
	"$(INTDIR)\res_comp.obj" \
	"$(INTDIR)\res_debu.obj" \
	"$(INTDIR)\res_init.obj" \
	"$(INTDIR)\res_mkqu.obj" \
	"$(INTDIR)\res_quer.obj" \
	"$(INTDIR)\res_send.obj" \
	"$(INTDIR)\resolve.obj" \
	"$(INTDIR)\resource.res"

"$(OUTDIR)\wshelp32.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "wshelp32 - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\wshelp32.dll"


CLEAN :
	-@erase "$(INTDIR)\gethna.obj"
	-@erase "$(INTDIR)\herror.obj"
	-@erase "$(INTDIR)\hesiod.obj"
	-@erase "$(INTDIR)\hesmailh.obj"
	-@erase "$(INTDIR)\hespwnam.obj"
	-@erase "$(INTDIR)\hesservb.obj"
	-@erase "$(INTDIR)\inetaton.obj"
	-@erase "$(INTDIR)\res_comp.obj"
	-@erase "$(INTDIR)\res_debu.obj"
	-@erase "$(INTDIR)\res_init.obj"
	-@erase "$(INTDIR)\res_mkqu.obj"
	-@erase "$(INTDIR)\res_quer.obj"
	-@erase "$(INTDIR)\res_send.obj"
	-@erase "$(INTDIR)\resolve.obj"
	-@erase "$(INTDIR)\resource.res"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\wshelp32.dll"
	-@erase "$(OUTDIR)\wshelp32.exp"
	-@erase "$(OUTDIR)\wshelp32.ilk"
	-@erase "$(OUTDIR)\wshelp32.lib"
	-@erase "$(OUTDIR)\wshelp32.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "WSHELP32_EXPORTS" /Fp"$(INTDIR)\wshelp32.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

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
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\resource.res" /i "..\include" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\wshelp32.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=wsock32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\wshelp32.pdb" /debug /machine:I386 /def:".\wshelp32.def" /out:"$(OUTDIR)\wshelp32.dll" /implib:"$(OUTDIR)\wshelp32.lib" /pdbtype:sept 
DEF_FILE= \
	".\wshelp32.def"
LINK32_OBJS= \
	"$(INTDIR)\gethna.obj" \
	"$(INTDIR)\herror.obj" \
	"$(INTDIR)\hesiod.obj" \
	"$(INTDIR)\hesmailh.obj" \
	"$(INTDIR)\hespwnam.obj" \
	"$(INTDIR)\hesservb.obj" \
	"$(INTDIR)\inetaton.obj" \
	"$(INTDIR)\res_comp.obj" \
	"$(INTDIR)\res_debu.obj" \
	"$(INTDIR)\res_init.obj" \
	"$(INTDIR)\res_mkqu.obj" \
	"$(INTDIR)\res_quer.obj" \
	"$(INTDIR)\res_send.obj" \
	"$(INTDIR)\resolve.obj" \
	"$(INTDIR)\resource.res"

"$(OUTDIR)\wshelp32.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("wshelp32.dep")
!INCLUDE "wshelp32.dep"
!ELSE 
!MESSAGE Warning: cannot find "wshelp32.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "wshelp32 - Win32 Release" || "$(CFG)" == "wshelp32 - Win32 Debug"
SOURCE=.\gethna.c

"$(INTDIR)\gethna.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\herror.c

"$(INTDIR)\herror.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\hesiod.c

"$(INTDIR)\hesiod.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\hesmailh.c

"$(INTDIR)\hesmailh.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\hespwnam.c

"$(INTDIR)\hespwnam.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\hesservb.c

"$(INTDIR)\hesservb.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\inetaton.c

"$(INTDIR)\inetaton.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\res_comp.c

"$(INTDIR)\res_comp.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\res_debu.c

"$(INTDIR)\res_debu.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\res_init.c

"$(INTDIR)\res_init.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\res_mkqu.c

"$(INTDIR)\res_mkqu.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\res_quer.c

"$(INTDIR)\res_quer.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\res_send.c

"$(INTDIR)\res_send.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\resolve.c

"$(INTDIR)\resolve.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\resource.rc

"$(INTDIR)\resource.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 

