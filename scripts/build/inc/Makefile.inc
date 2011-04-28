###############################################################################
# In the beginning...

!if "$(USERNAME)"=="" || "$(HOSTNAME)"=="" || "$(DATE)" == ""
!error You must invoke the build script!
!endif

all::

!if !defined(RECURSED_DIR)
RECURSED_DIR=.
!endif

###############################################################################

!if defined(RECURSED_DIRS) && "$(RECURSED_DIRS)" != "."

!if exists("Makefile.dir")
!include .\Makefile.dir
!else
!message Coult not find Makefile.dir
!error
!endif

!if !defined(PREDIRS) && !defined(DIRS)
!message You must define one of PREDIRS or DIRS in Makefile.dir
!error
!endif

!if "$(RECURSED_DIRS)" == "PREDIRS"
DIRS=$(PREDIRS)
!elseif "$(RECURSED_DIRS)" == "DIRS"
DIRS=$(DIRS)
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
	@echo Entering $(RECURSED_DIR)\$@
	@cd $@
	@$(MAKE) -$(MAKEFLAGS) $(RECURSED_TARGET) RECURSED_DIR=$(RECURSED_DIR)\$@ RECURSED_DIRS=.
	@echo Exiting $(RECURSED_DIR)\$@
	@cd $(MAKEDIR)

!else # not recursed
###############################################################################
# Not Recursed...

!if exists("Makefile.dir")
MAKEFILE_FOUND=1
!include .\Makefile.dir

!if !defined(PREDIRS) && !defined(DIRS)
!message You must define one of PREDIRS or DIRS in Makefile.dir
!error
!endif

!endif

###############################################################################
# Pre dirs...
!if defined(PREDIRS)
all clean::
	@$(MAKE) -$(MAKEFLAGS) $@ RECURSED_TARGET=$@ RECURSED_DIRS=PREDIRS
!endif

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

STD_LIBS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib

!include .\Makefile.src

!if defined(NO_PLACE) || !defined(BINPLACE_CMD)
BINPLACE=echo NOT PLACING:
!else
BINPLACE=$(BINPLACE_CMD) $(BINPLACE_FLAGS)
!endif

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

!ifndef NO_EXTERNAL_DEPS
!if exists("$(NAME).dep")
!include "$(NAME).dep"
MESSAGE_DEP=Included "$(NAME).dep"
!elseif defined(USE_EXTERNAL_DEPS)
MESSAGE_DEP=Warning: Cannot find "$(NAME).dep"
!endif
!endif

!if defined(PCH_FILE) || defined(USE_PCH)
C_FLAGS=$(C_FLAGS) -Fp$(O)\$(NAME).pch -YX$(PCH_FILE)
!endif

!ifdef USE_CPP_EH
C_FLAGS=$(C_FLAGS) -GX
!endif

!ifdef NODEBUG
!ifndef FORCE_BSC
NO_BSC=1
!endif
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

!ifdef DEF_FILE
LDEF=-def:$(DEF_FILE)
!endif

!ifdef GEN_MAP
LMAP=-map
!endif

!if "$(TARGETTYPE)"=="DLL"
!ifndef NO_LIBC
LTAR_XTRA=-entry:_DllMainCRTStartup$(DLLENTRY)
!else
LTAR_XTRA=
!endif
LTAR=-dll $(LTAR_XTRA) -implib:$(O)\$(NAME).lib
OUTFILE=$(O)\$(NAME).dll
VER_TARGET=VFT_DLL
!endif

!if "$(TARGETTYPE)"=="EXE"
OUTFILE=$(O)\$(NAME).exe
VER_TARGET=VFT_APP
!endif

!if "$(TARGETTYPE)"=="LIB"
OUTFILE=$(O)\$(NAME).lib
VER_TARGET=VFT_STATIC_LIB
!endif

LSS=

!if "$(TARGETSS)"=="console"
LSS=-subsystem:console
!elseif "$(TARGETSS)"=="windows"
LSS=-subsystem:windows
!else
!message Invalid TARGETSS: $(TARGETSS)
!message Valid ones are: console, windows
!error
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

!ifndef NO_ALL
all:: output_dir
!endif

#------------------------------------------------------------------------------

BSCFILE=$(O)\$(NAME).bsc

SBRS=$(OBJS:.obj=.sbr)

$(BSCFILE): $(O) $(SBRS)
	bscmake.exe -nologo $(BSC_FLAGS) -o$(BSCFILE) $(SBRS)

#------------------------------------------------------------------------------

!ifdef NO_BSC
all:: $(OUTFILE)
!else
all:: $(OUTFILE) $(BSCFILE)
!endif
	@$(BINPLACE) $(OUTFILE)

LINK_LINE=-nologo $(ldebug) $(LIBFLAGS) $(LSS) $(LDEF) $(LNDL) $(LTAR) $(LMAP) -out:$(OUTFILE) -pdb:$(O)\$(NAME).pdb -pdbtype:sept -incremental:yes $(LIBC_LIB) $(optlibs) $(LIBS) $(OBJS) $(RES)

LIB_LINE=-nologo -out:$@ $(OBJS) $(LIBS) $(LNDL)

!if ("$(TARGETTYPE)"=="DLL") || ("$(TARGETTYPE)"=="EXE")
$(OUTFILE): $(OBJS) $(RES) $(DEF_FILE)
	$(link) $(LINK_LINE)
#	@echo $(link) $(LINK_LINE)
#	$(link) @<<
#$(LINK_LINE)
#<<
!elseif "$(TARGETTYPE)"=="LIB"
$(OUTFILE): $(OBJS)
	$(implib) $(LIB_LINE)
#	@echo $(implib) $(LIB_LINE)
#	@$(implib) @<<
#$(LIB_LINE)
#<<
!else
!error Invalid TARGETTYPE $(TARGETTYPE) (must be one of DLL EXE LIB)
!endif

#------------------------------------------------------------------------------

!ifdef NO_OUT
output_dir:
!else
output_dir: $(OUTDIR)$(DIRNUL)
!endif

super_clean: super_clean_files clean_outdir

super_clean_files:
!if "$(OS)" == "Windows_NT"
	@if exist $(O)$(DIRNUL) rd /s /q $(O)
!else
	@if exist $(O)$(DIRNUL) deltree /y $(O)
!endif

default_clean: clean_files clean_outdir

CLEAN_FILES=$(OBJS) $(RES) $(O)\*.idb $(O)\*.pdb $(O)\$(NAME).lib $(SBRS) $(OUTFILE) $(BSCFILE) $(O)\$(NAME).ilk $(O)\$(NAME).exp $(O)\$(NAME).pch

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
	@echo Building $(RECURSED_DIR)
	@set C_RULE_PRINT=	$(C_RULE_PRINT)
	@set DO_C_RULE_PRINT=1

_clean_msg: _pseudo_target
	@echo Cleaning $(RECURSED_DIR)

_pre_msg: _pseudo_target
	@echo.

_post_msg: _pseudo_target
!if defined(MESSAGE_INST)
	@echo $(MESSAGE_INST)
!endif
!if defined(MESSAGE_DEP)
	@echo $(MESSAGE_DEP)
!endif

!endif # Makefile.src

###############################################################################
# Post dirs...
!if defined(DIRS)
all clean::
	@$(MAKE) -$(MAKEFLAGS) $@ RECURSED_TARGET=$@ RECURSED_DIRS=DIRS
!endif

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
