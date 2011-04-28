# Microsoft Visual C++ generated build script - Do not modify

PROJ = wshtest
DEBUG = 1
!if "$(DEBUG)" == "1"
INTDIR=WinDebug
OUTDIR=WinDebug
!else
INTDIR=WinRel
OUTDIR=WinRel
!endif

!if [if not exist $(INTDIR) md $(INTDIR)]
!endif
!if [if not exist $(OUTDIR) md $(OUTDIR)]
!endif

PROGTYPE = 0
CALLER = 
ARGS = 
DLLS = c:\net\mit\wshelper.dll
D_RCDEFINES = /d_DEBUG
R_RCDEFINES = /dNDEBUG
ORIGIN = MSVC
ORIGIN_VER = 1.00
PROJPATH = G:\USER\DALMEIDA\SRC\WSHELPER\WSHTEST\
USEMFC = 1
CC = cl
CPP = cl
CXX = cl
CCREATEPCHFLAG = 
CPPCREATEPCHFLAG = /YcSTDAFX.H
CUSEPCHFLAG = 
CPPUSEPCHFLAG = /YuSTDAFX.H
FIRSTC =             
FIRSTCPP = STDAFX.CPP  
RC = rc
CFLAGS_D_WEXE = /nologo /G2 /W3 /Zi /ALw /Od /D "_PATH_RESCONF"="\"C:\\NET\\TCP\\RESOLV.CFG\"" /D "_DEBUG" /I "..\include" /FR /GA /Fp"$(INTDIR)\stdafx.pch" /Fd"$(INTDIR)\wshtest.pdb" 
CFLAGS_R_WEXE = /nologo /Gs /G2 /W3 /ALw /O1 /D "_PATH_RESCONF"="\"C:\\NET\\TCP\\RESOLV.CFG\"" /D "NDEBUG" /I "..\include" /FR /GA /Fp"$(INTDIR)\stdafx.pch" 
LFLAGS_D_WEXE = /NOLOGO /NOD /PACKC:61440 /STACK:10240 /ALIGN:16 /ONERROR:NOEXE /CO  
LFLAGS_R_WEXE = /NOLOGO /NOD /PACKC:61440 /STACK:10240 /ALIGN:16 /ONERROR:NOEXE  
LIBS_D_WEXE = lafxcwd winsock oldnames libw llibcew commdlg.lib olecli.lib olesvr.lib shell.lib ..\wshelper\$(OUTDIR)\wshelper
LIBS_R_WEXE = lafxcw winsock oldnames libw llibcew commdlg.lib olecli.lib olesvr.lib shell.lib ..\wshelper\$(OUTDIR)\wshelper
RCFLAGS = /nologo /z
RESFLAGS = /nologo /t
RUNFLAGS = 
DEFFILE = WSHTEST.DEF
OBJS_EXT = 
LIBS_EXT = 
!if "$(DEBUG)" == "1"
CFLAGS = $(CFLAGS_D_WEXE)
LFLAGS = $(LFLAGS_D_WEXE)
LIBS = $(LIBS_D_WEXE)
MAPFILE = nul
RCDEFINES = $(D_RCDEFINES)
!else
CFLAGS = $(CFLAGS_R_WEXE)
LFLAGS = $(LFLAGS_R_WEXE)
LIBS = $(LIBS_R_WEXE)
MAPFILE = nul
RCDEFINES = $(R_RCDEFINES)
!endif
!if [if exist $(INTDIR)\msvc.bnd del $(INTDIR)\msvc.bnd]
!endif
SBRS = $(INTDIR)\stdafx.sbr \
		$(INTDIR)\wshtest.sbr \
		$(INTDIR)\mainfrm.sbr \
		$(INTDIR)\wshtedoc.sbr \
		$(INTDIR)\wshtevw.sbr \
		$(INTDIR)\querydlg.sbr \
		$(INTDIR)\respdlg.sbr \
		$(INTDIR)\hesdlg.sbr \
		$(INTDIR)\hespdlg.sbr






















all:	$(OUTDIR)\$(PROJ).exe $(OUTDIR)\$(PROJ).bsc

$(INTDIR)\wshtest.res:	WSHTEST.RC $(WSHTEST_RCDEP)
	$(RC) $(RCFLAGS) $(RCDEFINES) -fo $@ -r WSHTEST.RC

$(INTDIR)\stdafx.obj:	STDAFX.CPP $(STDAFX_DEP)
	$(CPP) $(CFLAGS) $(CPPCREATEPCHFLAG) /Fo$@ /FR$(@R).sbr /c STDAFX.CPP

$(INTDIR)\wshtest.obj:	WSHTEST.CPP $(WSHTEST_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /Fo$@ /FR$(@R).sbr /c WSHTEST.CPP

$(INTDIR)\mainfrm.obj:	MAINFRM.CPP $(MAINFRM_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /Fo$@ /FR$(@R).sbr /c MAINFRM.CPP

$(INTDIR)\wshtedoc.obj:	WSHTEDOC.CPP $(WSHTEDOC_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /Fo$@ /FR$(@R).sbr /c WSHTEDOC.CPP

$(INTDIR)\wshtevw.obj:	WSHTEVW.CPP $(WSHTEVW_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /Fo$@ /FR$(@R).sbr /c WSHTEVW.CPP

$(INTDIR)\querydlg.obj:	QUERYDLG.CPP $(QUERYDLG_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /Fo$@ /FR$(@R).sbr /c QUERYDLG.CPP

$(INTDIR)\respdlg.obj:	RESPDLG.CPP $(RESPDLG_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /Fo$@ /FR$(@R).sbr /c RESPDLG.CPP

$(INTDIR)\hesdlg.obj:	HESDLG.CPP $(HESDLG_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /Fo$@ /FR$(@R).sbr /c HESDLG.CPP

$(INTDIR)\hespdlg.obj:	HESPDLG.CPP $(HESPDLG_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /Fo$@ /FR$(@R).sbr /c HESPDLG.CPP


$(OUTDIR)\$(PROJ).exe::	$(INTDIR)\wshtest.res

$(OUTDIR)\$(PROJ).exe::	$(INTDIR)\stdafx.obj $(INTDIR)\wshtest.obj $(INTDIR)\mainfrm.obj $(INTDIR)\wshtedoc.obj $(INTDIR)\wshtevw.obj $(INTDIR)\querydlg.obj \
	$(INTDIR)\respdlg.obj $(INTDIR)\hesdlg.obj $(INTDIR)\hespdlg.obj $(OBJS_EXT) $(DEFFILE)
	echo >NUL @<<$(OUTDIR)\$(PROJ).crf
$(INTDIR)\stdafx.obj +
$(INTDIR)\wshtest.obj +
$(INTDIR)\mainfrm.obj +
$(INTDIR)\wshtedoc.obj +
$(INTDIR)\wshtevw.obj +
$(INTDIR)\querydlg.obj +
$(INTDIR)\respdlg.obj +
$(INTDIR)\hesdlg.obj +
$(INTDIR)\hespdlg.obj +
$(OBJS_EXT)
$(OUTDIR)\$(PROJ).exe
$(MAPFILE)
F:\MSVC15\LIB\+
F:\MSVC15\MFC\LIB\+
$(LIBS)
$(DEFFILE);
<<
	link $(LFLAGS) @$(OUTDIR)\$(PROJ).crf
	$(RC) $(RESFLAGS) $(INTDIR)\wshtest.res $@
	@copy $(OUTDIR)\$(PROJ).crf $(INTDIR)\msvc.bnd

$(OUTDIR)\$(PROJ).exe::	$(INTDIR)\wshtest.res
	if not exist $(INTDIR)\msvc.bnd 	$(RC) $(RESFLAGS) $(INTDIR)\wshtest.res $@

run: $(OUTDIR)\$(PROJ).exe
	$(PROJ) $(RUNFLAGS)


$(OUTDIR)\$(PROJ).bsc: $(SBRS)
	bscmake @<<
/o$@ $(SBRS)
<<
