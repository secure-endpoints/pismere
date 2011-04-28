###############################################################################
# In the beginning...

!if "$(USERNAME)"=="" || "$(HOSTNAME)"=="" || "$(DATE)" == "" || "$(BINPLACE_CMD)" == "" || "$(WHICH_CMD)" == "" || "$(BUILD_MAKEFILE_INC)" == "" || "$(BUILD_INCLUDE)" == ""
!error You must invoke the build script!
!endif

ENV_INCLUDE=$(INCLUDE)
INCLUDE=$(BUILD_INCLUDE);$(INCLUDE)

BUILD_MAKE_CMD=$(MAKE) -$(MAKEFLAGS) -f $(BUILD_MAKEFILE_INC)
BUILD_MAKE_CMD_ALT=$(MAKE) -$(MAKEFLAGS)

all::

!if !defined(RECURSED_DIR)
RECURSED_DIR=.
!endif

###############################################################################

!if defined(BUILD_USING_OLDSTYLE_BUILD) && defined(RECURSED_DIRS) && "$(RECURSED_DIRS)" != "." # recursed

!if exists("Makefile.dir")
!include .\Makefile.dir
!else
!message Coult not find Makefile.dir
!error
!endif

!if !defined(PREDIRS) && !defined(DIRS) && !defined(OPTDIRS)
!message You must define one of PREDIRS, DIRS, or OPTDIRS in Makefile.dir
!error
!endif

!if "$(RECURSED_DIRS)" == "PREDIRS"
DIRS=$(PREDIRS)
!elseif "$(RECURSED_DIRS)" == "DIRS"
DIRS=$(DIRS)
!elseif "$(RECURSED_DIRS)" == "OPTDIRS"
DIRS=$(OPTDIRS)
!elseif "$(RECURSED_DIRS)" == "OPTDIRS_DIR" && "$(OPTDIRS_DIR)" != ""
DIRS=$(OPTDIRS_DIR)
!else
!message Invalid RECURSED_DIRS.
!message This should never happen.  Contact the build dude.
!error
!endif

!if "$(RECURSED_TARGET)" != "all" && "$(RECURSED_TARGET)" != "clean"
!message Invalid RECURSED_TARGET.
!message This should never happen.  Contact the build dude.
!error
!endif

$(RECURSED_TARGET):: $(DIRS)

$(DIRS): _pseudo_target
!if "$(RECURSED_DIRS)" == "OPTDIRS"
	@$(BUILD_MAKE_CMD) $(RECURSED_TARGET) RECURSED_TARGET=$(RECURSED_TARGET) RECURSED_DIRS=OPTDIRS_DIR OPTDIRS_DIR=$@
!elseif "$(RECURSED_DIRS)" == "OPTDIRS_DIR" && !exists("$(DIRS)\Makefile") && !exists("$(DIRS)\Makefile.dir") && !exists("$(DIRS)\Makefile.src")
!if "$(OPTDIRS_MESSAGE)" != ""
	@echo MISSING DIR: $(DIRS) $(OPTDIRS_MESSAGE)
!endif
!else
	@echo Entering $(RECURSED_DIR)\$@
	@cd $@
	@if "%CMDEXTVERSION%"=="" echo The build may break.  You need Windows 2000 or newer to build.
	@if not cmdextversion 2 echo The build may break.  You need Windows 2000 or newer to build.
	@if exist Makefile.dir ( \
		$(BUILD_MAKE_CMD) $(RECURSED_TARGET) RECURSED_DIR=$(RECURSED_DIR)\$@ RECURSED_DIRS=. \
	) else ( \
		if exist Makefile.src ( \
			$(BUILD_MAKE_CMD) $(RECURSED_TARGET) RECURSED_DIR=$(RECURSED_DIR)\$@ RECURSED_DIRS=. \
		) else ( \
		$(BUILD_MAKE_CMD_ALT) $(RECURSED_TARGET) RECURSED_DIR=$(RECURSED_DIR)\$@ RECURSED_DIRS=. \
		) \
	)
	@echo Exiting $(RECURSED_DIR)\$@
	@cd $(MAKEDIR)
	@echo.
!endif

!else # not recursed

!if defined(BUILD_USING_OLDSTYLE_BUILD)
###############################################################################
# Not Recursed...

!if exists("Makefile.dir")
MAKEFILE_FOUND=1
!include .\Makefile.dir

!if !defined(PREDIRS) && !defined(DIRS) && !defined(OPTDIRS)
!message You must define one of PREDIRS, DIRS, or OPTDIRS in Makefile.dir
!error
!endif

!endif

###############################################################################
# Pre dirs...
!if defined(PREDIRS)
all clean::
	@$(BUILD_MAKE_CMD) $@ RECURSED_TARGET=$@ RECURSED_DIRS=PREDIRS
!endif
!endif # BUILD_USING_OLDSTYLE_BUILD

###############################################################################
# The main event...
!if exists("Makefile.src")
MAKEFILE_FOUND=1

all:: _pre_msg _all_msg _post_msg

clean:: _pre_msg _clean_msg _post_msg

!ifndef NODEBUG
BINPLACE_FLAGS=--debug
DEBUG_F=1
DEBUG_N=dbg
DEBUG_DEF=-D_DEBUG
!else
BINPLACE_FLAGS=--nodebug
DEBUG_F=0
DEBUG_N=rel
DEBUG_DEF=-DNDEBUG
!endif

#------------------------------------------------------------------------------

!include <Makefile.w32>

OUTDIR1=.\obj
OUTDIR2=$(OUTDIR1)\$(CPU)
OUTDIR=$(OUTDIR2)\$(DEBUG_N)
O=$(OUTDIR)
T=.\$(CPU)\$(DEBUG_N)

STD_LIBS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib 

!include .\Makefile.src

!IF "$(CPU)" == "IA64" || "$(CPU)" == "AMD64" || "$(CPU)" == "ALPHA64"
!IF defined(WIN64NAME)
NAME=$(WIN64NAME)
!ENDIF
!IF defined(WIN64LIBS)
LIBS=$(WIN64LIBS)
!ENDIF
!IF defined(WIN64_DEF_FILE)
DEF_FILE=$(WIN64_DEF_FILE)
!ENDIF
!IF defined(WIN64_PLACE)
PLACE=$(WIN64_PLACE)
!ENDIF
!IF defined(WIN64_PLACE_OPTIONAL)
PLACE_OPTIONAL=$(WIN64_PLACE_OPTIONAL)
!ENDIF
!IF defined(WIN64_DELAY_DLLS)
DELAY_DLLS=$(WIN64_DELAY_DLLS)
!ENDIF
!IF defined(WIN64_DEP_LIBS)
DEP_LIBS=$(WIN64_DEP_LIBS)
!ENDIF
!IF defined(WIN64_C_DEFINE)
C_DEFINE=$(WIN64_C_DEFINE)
!ENDIF
!ENDIF


!IFDEF NODEBUG
!IFDEF DEBUG_SYMBOL
ldebug = /DEBUG
cdebug = -Os -Zi
!ENDIF
!ENDIF

!if defined(NO_PLACE) || !defined(BINPLACE_CMD)
BINPLACE=echo NOT PLACING:
BINPLACE_OPTIONAL=$(BINPLACE)
!else
BINPLACE=$(BINPLACE_CMD) $(BINPLACE_FLAGS)
BINPLACE_OPTIONAL=$(BINPLACE) --optional
!endif


DNS_INC=$(PISMERE)/athena/wshelper/include
DNS_LIB=$(PISMERE)/athena/wshelper/wshload/$(OUTDIR)/wshload.lib
KRB4_USE_DNS=1
KRB5_USE_DNS=1
KRB5_USE_DNS_KDC=1
KRB5_KFW_COMPILE=1


#--------------------------------------------------------
# if we're going to use the NuMega NMCL and NMLINK tools we have to set CC and LINK after
# including win32.mak

CC=$(cc)
LINK=$(link)

!if defined(NMBC) && !defined(NMTC)
MESSAGE_INST=Instrumenting to NuMega BoundsChecker, FinalCheck
CC=NMCL.EXE /NMbcOn
LINK=NMLINK.EXE /NMbcOn
CPP=NMCL.EXE /NMbcOn
LINK32=NMLINK.EXE /NMbcOn

!elseif defined(NMTC) && !defined(NMBC) 
MESSAGE_INST=Instrumenting for NuMega TrueCoverage
CC=NMCL.EXE /NMtcOn
LINK=NMLINK.EXE /NMtcOn
CPP=NMCL.EXE /NMtcOn
LINK32=NMLINK.EXE /NMtcOn

!elseif defined(NMTT)
MESSAGE_INST=Instrumenting for NuMega TrueTime
CC=NMCL.EXE /NMttOn
LINK=NMLINK.EXE /NMttOn
CPP=NMCL.EXE /NMttOn
LINK32=NMLINK.EXE /NMttOn

!elseif ( defined(NMBC) && defined(NMTC) ) && !defined(NMTT) 
MESSAGE_INST=Instrumenting for NuMega BoundsChecker and TrueCoverage
CC=NMCL.EXE /NMbcOn /NMtcOn
LINK=NMLINK.EXE /NMbcOn /NMtcOn
CPP=NMCL.EXE /NMbcOn /NMtcOn
LINK32=NMLINK.EXE /NMbcOn /NMtcOn

!elseif defined(NMTT) && defined(NMBC) && defined(NMTC)
!message Illegal combination of NuMega tool options.
!error
!endif

#---------------------------------------------------------

# $(OUTDIR)\nul:
#	-mkdir $(OUTDIR)


!if "$(OS)" == "Windows_NT"
DIRNUL=
!else
DIRNUL=\nul
!endif

$(OUTDIR)$(DIRNUL):
	-@if not exist $(OUTDIR1)$(DIRNUL) mkdir $(OUTDIR1)
	-@if not exist $(OUTDIR2)$(DIRNUL) mkdir $(OUTDIR2)
	-@if not exist $(OUTDIR)$(DIRNUL) mkdir $(OUTDIR)
	@if exist $(OUTDIR)$(DIRNUL) echo Output going into $(OUTDIR)
	@if not exist $(OUTDIR1)$(DIRNUL) echo The directory $(OUTDIR1) could not be created.
	@if exist $(OUTDIR1)$(DIRNUL) if not exist $(OUTDIR2)$(DIRNUL) echo The directory $(OUTDIR2) could not be created.
	@if exist $(OUTDIR2)$(DIRNUL) if not exist $(OUTDIR)$(DIRNUL) echo The directory $(OUTDIR) could not be created.

clean_outdir:
	-@if exist $(OUTDIR)$(DIRNUL) rmdir $(OUTDIR)
	-@if exist $(OUTDIR2)$(DIRNUL) rmdir $(OUTDIR2)
	-@if exist $(OUTDIR1)$(DIRNUL) rmdir $(OUTDIR1)
	@if exist $(OUTDIR2)$(DIRNUL) echo The directory $(OUTDIR2) is not empty.
	@if not exist $(OUTDIR2)$(DIRNUL) if exist $(OUTDIR1)$(DIRNUL) echo The directory $(OUTDIR1) is not empty.

#------------------------------------------------------------------------------

!ifdef DELAY_DLLS
!ifndef DELAY_HELP_LIB
DELAY_HELP_LIB=delayimp.lib
!endif
LINK_DELAY_OPTS=-delay:unload -delayload:$(DELAY_DLLS:,= -delayload:)
!else
!undef LINK_DELAY_OPTS
!endif

!ifdef C_FLAGS
MESSAGE_C_FLAGS=1
!endif

!ifdef RC_FLAGS
MESSAGE_RC_FLAGS=1
!endif

!ifdef C_DEFINE
C_FLAGS=$(C_DEFINE) $(C_FLAGS)
!endif

!ifdef RC_DEFINE
RC_FLAGS=$(RC_DEFINE) $(RC_FLAGS)
!endif

!if defined(PCH_FILE) || defined(USE_PCH)
C_FLAGS=$(C_FLAGS) -Fp$(O)\$(NAME).pch -YX$(PCH_FILE)
!endif

!ifdef UNICODE
C_FLAGS=$(C_FLAGS) -DUNICODE -D_UNICODE
!endif

!ifndef USE_MINIMAL
NO_MINIMAL=1
!endif

!if !defined(NO_MINIMAL) && !defined(NODEBUG)
C_FLAGS=$(C_FLAGS) -Gm
!endif

!ifdef USE_CPP_RTTI
C_FLAGS=$(C_FLAGS) -GR
!endif

!ifdef DETECT64
C_FLAGS=$(C_FLAGS) -Wp64
!endif

!ifdef USE_CPP_EH
C_FLAGS=$(C_FLAGS) -EHsc
!endif

!ifndef GEN_BSC
NO_BSC=1
!endif

!ifdef KEEP_SBRS
BSC_FLAGS=-n
!else
BSC_FLAGS=
!endif

!ifndef NO_BSC
C_FLAGS=$(C_FLAGS) -FR$(O)^\
!endif

!ifdef USE_MFC
USE_DEFAULTLIB=1
!ifndef USE_MFC_STATIC
RC_FLAGS=$(RC_FLAGS) -D_AFXDLL
C_FLAGS=$(C_FLAGS) -D_AFXDLL
!endif
!endif

!ifdef USE_MFC_USRDLL
USE_DEFAULTLIB=1
C_FLAGS=$(C_FLAGS) -D_USRDLL
!endif

!ifdef NO_LIBC
LIBC_FLAGS=$(cvars)
LIBC_LIB=-nodefaultlib
!else
!ifdef USE_LIBC_STD
!ifdef LIBC_FLAGS
!error Only use one LIBC declaration!
!endif
LIBC_FLAGS=$(cvarsstd)
LIBC_LIB=$(libc)
!endif

!ifdef USE_LIBC_MT
!ifdef LIBC_FLAGS
!error Only use one LIBC declaration!
!endif
LIBC_FLAGS=$(cvarsmt)
LIBC_LIB=$(libcmt)
!endif

!ifdef USE_LIBC_DLL
!ifdef LIBC_FLAGS
!error Only use one LIBC declaration!
!endif
LIBC_FLAGS=$(cvarsdll)
LIBC_LIB=$(libcdll)
!endif

!ifndef LIBC_FLAGS
LIBC_FLAGS=$(cvarsdll)
!endif

!endif # !NO_LIBC

!ifdef NO_LIBC_LINK
LIBC_LIB=
!endif

!ifdef INCLUDES
INCFLAGS=-I$(INCLUDES:;= -I)
!endif

!ifdef LIB_PATHS
LIBFLAGS=-libpath:$(LIB_PATHS:;= -libpath:)
!endif

CFLAGS=$(cflags) $(cdebug) $(LIBC_FLAGS) $(C_FLAGS) $(DEBUG_DEF) -nologo $(INCFLAGS)
RCFLAGS=$(rcflags) $(rcvars) $(RC_FLAGS) $(DEBUG_DEF) $(INCFLAGS) -DVER_USERNAME="stringer($(USERNAME))" -DVER_HOSTNAME="stringer($(HOSTNAME))" -DVER_TARGET=$(VER_TARGET) -DVER_NAME="stringer($(NAME))" -DVER_DATE="stringer($(DATE))"

!ifdef VENDORMOD
RCFLAGS=$(RCFLAGS) -DVENDORMOD="stringer($(VENDORMOD))"
!endif

!ifdef BETA
RCFLAGS=$(RCFLAGS) -DBETA=$(BETA) -DBETA_STR="stringer($(BETA))"
!endif

!include <Makefile.inc.site>

!ifdef DEF_FILE
LDEF=-def:$(DEF_FILE)
!endif

!ifdef GEN_MAP
LMAP=-map
!endif

_VC_MANIFEST_EMBED=
!if ("$(TARGETTYPE)"=="DLL") || ("$(TARGETTYPE)"=="CPL")
!ifndef NO_LIBC
LTAR_XTRA=-entry:_DllMainCRTStartup$(DLLENTRY)
!else
LTAR_XTRA=
!endif
LTAR=-dll $(LTAR_XTRA) -implib:$(O)\$(NAME).lib
!if "$(TARGETTYPE)"=="CPL"
OUTFILE=$(O)\$(NAME).cpl
!else
OUTFILE=$(O)\$(NAME).dll
!if exists("$(VCINSTALLDIR)\..\..\MICROSOFT VISUAL STUDIO 8")
_VC_MANIFEST_EMBED_DLL = if exist $@.manifest mt.exe -manifest $@.manifest -outputresource:$@;2
_VC_MANIFEST_EMBED = $(_VC_MANIFEST_EMBED_DLL)
!else
_VC_MANIFEST_EMBED_DLL =
!endif
!endif
VER_TARGET=VFT_DLL
!endif

!if "$(TARGETTYPE)"=="EXE"
OUTFILE=$(O)\$(NAME).exe
!if exists("$(VCINSTALLDIR)\..\..\MICROSOFT VISUAL STUDIO 8")
_VC_MANIFEST_EMBED_EXE = if exist $@.manifest mt.exe -manifest $@.manifest -outputresource:$@;1
_VC_MANIFEST_EMBED = $(_VC_MANIFEST_EMBED_EXE)
!else
_VC_MANIFEST_EMBED_EXE =
!endif
VER_TARGET=VFT_APP
!if "$(TARGETSS)"=="windows" && defined(UNICODE) && !defined(NO_LIBC) 
LTAR=-entry:wWinMainCRTStartup
!endif
!endif

!if "$(TARGETTYPE)"=="SCR"
OUTFILE=$(O)\$(NAME).scr
VER_TARGET=VFT_APP
!if "$(TARGETSS)"=="windows" && defined(UNICODE) && !defined(NO_LIBC) 
LTAR=-entry:wWinMainCRTStartup
!endif
!endif

!if "$(TARGETTYPE)"=="LIB"
OUTFILE=$(O)\$(NAME).lib
VER_TARGET=VFT_STATIC_LIB
!endif

LSS=

!ifdef OUTFILE
!if "$(TARGETSS)"=="console"
LSS=-subsystem:console
!elseif "$(TARGETSS)"=="windows"
LSS=-subsystem:windows
!else
!message Invalid TARGETSS: $(TARGETSS)
!message Valid ones are: console, windows
!error
!endif
!endif

C_RULE_STUFF=$(cc) $(CFLAGS) -Fo$(O)^\ -Fd$(O)^\ -FD -c 
C_RULE_PRINT=$(C_RULE_STUFF)

C_RULE=$(C_RULE_STUFF) $<

!ifndef USE_DEFAULTLIB
LNDL=-nodefaultlib
!endif

!if "$(RES)"!=""
$(RES): $(OBJS)
!endif

!ifndef NO_EXTERNAL_DEPS
!if exists("$(NAME).dep") && exists("Makefile.dep")
!include "$(NAME).dep"
!include "Makefile.dep"
MESSAGE_DEP=Warning: Included both "$(NAME).dep" and "Makefile.dep"
!elseif exists("$(NAME).dep")
!include "$(NAME).dep"
MESSAGE_DEP=Included "$(NAME).dep"
!elseif exists("Makefile.dep")
!include "Makefile.dep"
MESSAGE_DEP=Included Makefile.dep
!elseif defined(USE_EXTERNAL_DEPS)
MESSAGE_DEP=Warning: Cannot find "$(NAME).dep" nor "Makefile.dep"
!endif
!endif

{}.rc{$O\}.res:
	$(rc) -r $(RCFLAGS) -fo $@ $<

{}.c{$O\}.obj:
	@if "%DO_C_RULE_PRINT%"=="1" echo %C_RULE_PRINT% ...
	@set DO_C_RULE_PRINT=
	@$(C_RULE)

{}.cxx{$O\}.obj:
	@if "%DO_C_RULE_PRINT%"=="1" echo %C_RULE_PRINT% ...
	@set DO_C_RULE_PRINT=
	@$(C_RULE)

{}.cpp{$O\}.obj:
	@if "%DO_C_RULE_PRINT%"=="1" echo %C_RULE_PRINT% ...
	@set DO_C_RULE_PRINT=
	@$(C_RULE)

!ifdef USE_SUFFIX_CP
.SUFFIXES: .cp

{}.cp{$O\}.obj:
	@if "%DO_C_RULE_PRINT%"=="1" echo %C_RULE_PRINT% ...
	@set DO_C_RULE_PRINT=
	@$(C_RULE) -TP
!endif

#------------------------------------------------------------------------------

BSCFILE=$(O)\$(NAME).bsc

SBRS=$(OBJS:.obj=.sbr)

$(BSCFILE): $(O) $(SBRS)
	bscmake.exe -nologo $(BSC_FLAGS) -o$(BSCFILE) $(SBRS)

#------------------------------------------------------------------------------

!ifdef OUTFILE

all:: output_dir

LINK_LINE=-nologo $(ldebug) $(LIBFLAGS) $(LSS) $(LDEF) $(LNDL) $(LTAR) $(LMAP) -out:$(OUTFILE) -pdb:$(O)\$(NAME).pdb  $(LINK_DELAY_OPTS) $(LIBC_LIB) $(optlibs) $(LIBS) $(DELAY_HELP_LIB) $(OBJS) $(RES)

LIB_LINE=-nologo -out:$@ $(LIBFLAGS) $(OBJS) $(LIBS) $(LNDL)

!if ("$(TARGETTYPE)"=="DLL") || ("$(TARGETTYPE)"=="EXE") || ("$(TARGETTYPE)"=="CPL") || ("$(TARGETTYPE)"=="SCR")
$(OUTFILE): $(OBJS) $(RES) $(DEF_FILE) $(DEP_LIBS)
	$(link) $(LINK_LINE)
	$(_VC_MANIFEST_EMBED)
#	@echo $(link) $(LINK_LINE)
#	$(link) @<<
#$(LINK_LINE)
#<<
!elseif "$(TARGETTYPE)"=="LIB"
$(OUTFILE): $(OBJS) $(DEP_LIBS)
	$(implib) $(LIB_LINE)
#	@echo $(implib) $(LIB_LINE)
#	@$(implib) @<<
#$(LIB_LINE)
#<<
!else
!error Invalid TARGETTYPE $(TARGETTYPE) (must be one of DLL EXE LIB CPL SCR NONE)
!endif

!ifdef NO_BSC
all:: $(OUTFILE)
!else
all:: $(OUTFILE) $(BSCFILE)
!endif
	@$(BINPLACE) $(OUTFILE)

!ifdef SELF_REGISTERABLE
HLINE=*****************************************************************************
!ifndef SELF_REGISTER
all:: $(OUTFILE)
	@echo.
	@echo $(HLINE)
	@echo This is a self-registering component.  You can automatically register this 
	@echo component by setting SELF_REGISTER=1 for the build.
	@echo $(HLINE)
	@echo.
!else
all::
	@echo.
	@echo $(HLINE)
	@echo Registering component:
	regsvr32 /s /c $(OUTFILE)
	@echo $(HLINE)
	@echo.
!endif # SELF_REGISTER
!endif # SELF_REGISTERABLE

!else # OUTFILE

!if ("$(TARGETTYPE)"!="NONE")
!error Invalid TARGETTYPE $(TARGETTYPE) (must be one of DLL EXE LIB CPL SCR NONE)
!endif

!endif # OUTFILE

!ifdef PLACE
all::
	@$(BINPLACE) $(PLACE)
!endif

!ifdef PLACE_OPTIONAL
all::
	@$(BINPLACE_OPTIONAL) $(PLACE_OPTIONAL)
!endif

#------------------------------------------------------------------------------

!if exists("Makefile.xtra")
!include .\Makefile.xtra
!endif

#------------------------------------------------------------------------------

output_dir: $(OUTDIR)$(DIRNUL)

super_clean: super_clean_files clean_outdir

super_clean_files:
!if "$(OS)" == "Windows_NT"
	@if exist $(O)$(DIRNUL) rd /s /q $(O)
!else
	@if exist $(O)$(DIRNUL) deltree /y $(O)
!endif

default_clean: clean_files clean_outdir

CLEAN_FILES=$(OBJS) $(RES) $(O)\*.idb $(O)\*.pdb $(O)\$(NAME).lib $(SBRS) $(OUTFILE) $(BSCFILE) $(O)\$(NAME).ilk $(O)\$(NAME).exp $(O)\$(NAME).pch $(O)\*.manifest

clean_files:
	@for /f %a in ( << ) do @if exist %a (echo Deleting %a & del %a)
$(CLEAN_FILES: =
)
<<

clean:: super_clean

#clean:: default_clean

###############################################################################
# Build messagges...

_all_msg: _pseudo_target
!ifdef BUILD_USING_OLDSTYLE_BUILD
	@echo Building $(RECURSED_DIR)
!endif
	@set C_RULE_PRINT=	$(C_RULE_PRINT)
	@set DO_C_RULE_PRINT=1

_clean_msg: _pseudo_target
	@echo Cleaning $(RECURSED_DIR)

_pre_msg: _pseudo_target
!ifdef BUILD_USING_OLDSTYLE_BUILD
	@echo.
!endif

_post_msg: _pseudo_target
!if defined(MESSAGE_INST)
	@echo $(MESSAGE_INST)
!endif
!if defined(MESSAGE_DEP)
	@echo $(MESSAGE_DEP)
!endif
!if defined(MESSAGE_C_FLAGS)
	@echo WARNING: C_FLAGS definition in Makefile.src! (See build docs to fix this.)
!endif
!if defined(MESSAGE_RC_FLAGS)
	@echo WARNING: RC_FLAGS definition in Makefile.src! (See build docs to fix this.)
!endif

!endif # Makefile.src

!ifdef BUILD_USING_OLDSTYLE_BUILD
###############################################################################
# Post dirs...
!if defined(DIRS)
all clean::
	@$(BUILD_MAKE_CMD) $@ RECURSED_TARGET=$@ RECURSED_DIRS=DIRS
!endif

###############################################################################
# Optional dirs...
!if defined(OPTDIRS)
all clean::
	@$(BUILD_MAKE_CMD) $@ RECURSED_TARGET=$@ RECURSED_DIRS=OPTDIRS
!endif

!endif # BUILD_USING_OLDSTYLE_BUILD

###############################################################################
# Sanity check...
!if !defined(MAKEFILE_FOUND)
!message Cannot find Makefile.dir or Makefile.src!
!error
!endif

!endif

_pseudo_target:

# THE END
###############################################################################
