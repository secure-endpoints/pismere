# Microsoft Visual C++ generated build script - Do not modify

PROJ = wshelper
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

PROGTYPE = 1
CALLER = c:\work\wshelper\test\bin\wshtest
ARGS = 
DLLS = \net\mit\wshelper.dll
D_RCDEFINES = -d_DEBUG -dWINDOWS
R_RCDEFINES = -dNDEBUG -dWINDOWS
ORIGIN = MSVC
ORIGIN_VER = 1.00
PROJPATH = C:\WORK\WSHELPER\
USEMFC = 0
CC = cl
CPP = cl
CXX = cl
CCREATEPCHFLAG = 
CPPCREATEPCHFLAG = 
CUSEPCHFLAG = 
CPPUSEPCHFLAG = 
FIRSTC = GETHNA.C    
FIRSTCPP =             
RC = rc
CFLAGS_D_WDLL = /nologo /G2 /W3 /Zi /ALw /Gx- /Od /D "_DEBUG" /D "WINDOWS" /I "..\include" /FR /GD /GEf /GEe /Fd"$(INTDIR)\wshelper.pdb"
CFLAGS_R_WDLL = /nologo /G2 /W3 /ALw /Gx- /O1 /D "NDEBUG" /D "WINDOWS" /I "..\include" /FR /GD /GEf
LFLAGS_D_WDLL = /NOLOGO /ONERROR:NOEXE /NOD /PACKC:61440 /CO /NOE /ALIGN:16 /MAP:FULL
LFLAGS_R_WDLL = /NOLOGO /ONERROR:NOEXE /NOD /PACKC:61440 /NOE /ALIGN:16 /MAP:FULL
LIBS_D_WDLL = oldnames libw commdlg shell olecli olesvr ldllcew winsock toolhelp
LIBS_R_WDLL = oldnames libw commdlg shell olecli olesvr ldllcew winsock toolhelp
RCFLAGS = /nologo /v /i "..\include"
RESFLAGS = /nologo
RUNFLAGS = 
DEFFILE = WSHELPER.DEF
OBJS_EXT = 
LIBS_EXT = 
!if "$(DEBUG)" == "1"
CFLAGS = $(CFLAGS_D_WDLL)
LFLAGS = $(LFLAGS_D_WDLL)
LIBS = $(LIBS_D_WDLL)
MAPFILE = nul
RCDEFINES = $(D_RCDEFINES)
!else
CFLAGS = $(CFLAGS_R_WDLL)
LFLAGS = $(LFLAGS_R_WDLL)
LIBS = $(LIBS_R_WDLL)
MAPFILE = nul
RCDEFINES = $(R_RCDEFINES)
!endif
!if [if exist $(INTDIR)\msvc.bnd del $(INTDIR)\msvc.bnd]
!endif
SBRS = $(INTDIR)\gethna.sbr \
		$(INTDIR)\herror.sbr \
		$(INTDIR)\hesiod.sbr \
		$(INTDIR)\hesmailh.sbr \
		$(INTDIR)\hespwnam.sbr \
		$(INTDIR)\hesservb.sbr \
		$(INTDIR)\inetaton.sbr \
		$(INTDIR)\res_comp.sbr \
		$(INTDIR)\res_debu.sbr \
		$(INTDIR)\res_init.sbr \
		$(INTDIR)\res_mkqu.sbr \
		$(INTDIR)\res_quer.sbr \
		$(INTDIR)\res_send.sbr \
		$(INTDIR)\resolve.sbr
































all:	$(OUTDIR)\$(PROJ).dll $(OUTDIR)\$(PROJ).bsc

$(INTDIR)\gethna.obj:	GETHNA.C $(GETHNA_DEP)
	$(CC) $(CFLAGS) $(CCREATEPCHFLAG) /Fo$@ /FR$(@R).sbr /c GETHNA.C

$(INTDIR)\herror.obj:	HERROR.C $(HERROR_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /Fo$@ /FR$(@R).sbr /c HERROR.C

$(INTDIR)\hesiod.obj:	HESIOD.C $(HESIOD_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /Fo$@ /FR$(@R).sbr /c HESIOD.C

$(INTDIR)\hesmailh.obj:	HESMAILH.C $(HESMAILH_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /Fo$@ /FR$(@R).sbr /c HESMAILH.C

$(INTDIR)\hespwnam.obj:	HESPWNAM.C $(HESPWNAM_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /Fo$@ /FR$(@R).sbr /c HESPWNAM.C

$(INTDIR)\hesservb.obj:	HESSERVB.C $(HESSERVB_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /Fo$@ /FR$(@R).sbr /c HESSERVB.C

$(INTDIR)\inetaton.obj:	INETATON.C $(INETATON_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /Fo$@ /FR$(@R).sbr /c INETATON.C

$(INTDIR)\res_comp.obj:	RES_COMP.C $(RES_COMP_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /Fo$@ /FR$(@R).sbr /c RES_COMP.C

$(INTDIR)\res_debu.obj:	RES_DEBU.C $(RES_DEBU_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /Fo$@ /FR$(@R).sbr /c RES_DEBU.C

$(INTDIR)\res_init.obj:	RES_INIT.C $(RES_INIT_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /Fo$@ /FR$(@R).sbr /c RES_INIT.C

$(INTDIR)\res_mkqu.obj:	RES_MKQU.C $(RES_MKQU_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /Fo$@ /FR$(@R).sbr /c RES_MKQU.C

$(INTDIR)\res_quer.obj:	RES_QUER.C $(RES_QUER_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /Fo$@ /FR$(@R).sbr /c RES_QUER.C

$(INTDIR)\res_send.obj:	RES_SEND.C $(RES_SEND_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /Fo$@ /FR$(@R).sbr /c RES_SEND.C

$(INTDIR)\resolve.obj:	RESOLVE.C $(RESOLVE_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /Fo$@ /FR$(@R).sbr /c RESOLVE.C

$(INTDIR)\resource.res:	RESOURCE.RC $(RESOURCE_RCDEP)
	$(RC) $(RCFLAGS) $(RCDEFINES) -fo $@ -r RESOURCE.RC


$(OUTDIR)\$(PROJ).dll::	$(INTDIR)\resource.res

$(OUTDIR)\$(PROJ).dll::	$(INTDIR)\gethna.obj $(INTDIR)\herror.obj $(INTDIR)\hesiod.obj $(INTDIR)\hesmailh.obj $(INTDIR)\hespwnam.obj $(INTDIR)\hesservb.obj \
	$(INTDIR)\inetaton.obj $(INTDIR)\res_comp.obj $(INTDIR)\res_debu.obj $(INTDIR)\res_init.obj $(INTDIR)\res_mkqu.obj $(INTDIR)\res_quer.obj $(INTDIR)\res_send.obj \
	$(INTDIR)\resolve.obj $(OBJS_EXT) $(DEFFILE)
	echo >NUL @<<$(OUTDIR)\$(PROJ).crf
$(INTDIR)\gethna.obj +
$(INTDIR)\herror.obj +
$(INTDIR)\hesiod.obj +
$(INTDIR)\hesmailh.obj +
$(INTDIR)\hespwnam.obj +
$(INTDIR)\hesservb.obj +
$(INTDIR)\inetaton.obj +
$(INTDIR)\res_comp.obj +
$(INTDIR)\res_debu.obj +
$(INTDIR)\res_init.obj +
$(INTDIR)\res_mkqu.obj +
$(INTDIR)\res_quer.obj +
$(INTDIR)\res_send.obj +
$(INTDIR)\resolve.obj +
$(OBJS_EXT)
$(OUTDIR)\$(PROJ).dll
$(MAPFILE)
c:\msvc\lib\+
c:\msvc\mfc\lib\+
c:\locallib\+
$(LIBS)
$(DEFFILE);
<<
	link $(LFLAGS) @$(OUTDIR)\$(PROJ).crf
	$(RC) $(RESFLAGS) $(INTDIR)\resource.res $@
	@copy $(OUTDIR)\$(PROJ).crf $(INTDIR)\msvc.bnd
	implib /nowep $(OUTDIR)\$(PROJ).lib $(OUTDIR)\$(PROJ).dll

$(OUTDIR)\$(PROJ).dll::	$(INTDIR)\resource.res
	if not exist $(INTDIR)\msvc.bnd 	$(RC) $(RESFLAGS) $(INTDIR)\resource.res $@

run: $(OUTDIR)\$(PROJ).dll
	$(PROJ) $(RUNFLAGS)


$(OUTDIR)\$(PROJ).bsc: $(SBRS)
	bscmake @<<
/o$@ $(SBRS)
<<
