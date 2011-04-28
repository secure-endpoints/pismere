WHAT=windows

all:: setup-msg outpre-dir

all:: all-$(WHAT)
clean:: clean-$(WHAT)
install:: install-$(WHAT)
check:: check-$(WHAT)

all-windows::
clean-windows::
install-windows::
check-windows::

all-windows:: Makefile
clean-windows:: Makefile

#
# Figure out the CPU
#
!if !defined(CPU) || "$(CPU)" == ""
CPU=$(PROCESSOR_ARCHITECTURE)
!endif # CPU

!if "$(CPU)" == ""
CPU=i386
!endif

# Change x86 or X86 to i386
!if ( "$(CPU)" == "X86" ) || ( "$(CPU)" == "x86" )
CPU=i386
!endif # CPU == X86

!if ( "$(CPU)" != "i386" ) && ( "$(CPU)" != "ALPHA" )
!error Must specify CPU environment variable ( CPU=i386, CPU=ALPHA)
!endif
#
# End of figuring out CPU
#

!if "$(OS)" == "Windows_NT"
DIRNUL=
!else 
DIRNUL=\nul
!endif

# NOTE: ^ is an escape char for NMAKE.
!ifdef NODEBUG
OUTPRE_DBG=rel
!else
OUTPRE_DBG=dbg
!endif
OUTPRE1=obj
OUTPRE2=$(OUTPRE1)\$(CPU)
OUTPRE3=$(OUTPRE2)\$(OUTPRE_DBG)
OUTPRE=$(OUTPRE3)^\

$(OUTPRE3)$(DIRNUL):
	-@if not exist $(OUTPRE1)$(DIRNUL) mkdir $(OUTPRE1)
	-@if not exist $(OUTPRE2)$(DIRNUL) mkdir $(OUTPRE2)
	-@if not exist $(OUTPRE3)$(DIRNUL) mkdir $(OUTPRE3)
	@if exist $(OUTPRE3)$(DIRNUL) echo Output going into $(OUTPRE3)
	@if not exist $(OUTPRE1)$(DIRNUL) echo The directory $(OUTPRE1) could not be created.
	@if exist $(OUTPRE1)$(DIRNUL) if not exist $(OUTPRE2)$(DIRNUL) echo The directory $(OUTPRE2) could not be created.
	@if exist $(OUTPRE2)$(DIRNUL) if not exist $(OUTPRE3)$(DIRNUL) echo The directory $(OUTPRE3) could not be created.


clean-windows-dir::
	-@if exist $(OUTPRE3)$(DIRNUL) rmdir $(OUTPRE3)
	-@if exist $(OUTPRE2)$(DIRNUL) rmdir $(OUTPRE2)
	-@if exist $(OUTPRE1)$(DIRNUL) rmdir $(OUTPRE1)
	@if exist $(OUTPRE2)$(DIRNUL) echo The directory $(OUTPRE2) is not empty.
	@if not exist $(OUTPRE2)$(DIRNUL) if exist $(OUTPRE1)$(DIRNUL) echo The directory $(OUTPRE1) is not empty.

# Directory syntax:
#
# begin absolute path
ABS=^\
# begin relative path
REL=
# up-directory
U=..
# path separator
S=^\
# this is magic... should only be used for preceding a program invocation
C=.^\

srcdir = .
SRCTOP = $(srcdir)\$(BUILDTOP)

!if defined(KRB5_USE_DNS) || defined(KRB5_USE_DNS_KDC) || defined(KRB5_USE_DNS_REALMS)
!if defined(KRB5_NO_WSHELPER)
DNSMSG=resolver
!else
DNSMSG=wshelper
DNSFLAGS=-DWSHELPER=1
!endif
!if !defined(DNS_INC)
!message Must define DNS_INC to point to $(DNSMSG) includes dir!
!error
!endif
!if !defined(DNS_LIB)
!message Must define DNS_LIB to point to $(DNSMSG) library!
!error
!endif
DNSLIBS=$(DNS_LIB)
DNSFLAGS=-I$(DNS_INC) $(DNSFLAGS) -DKRB5_DNS_LOOKUP=1
!if defined(KRB5_USE_DNS_KDC)
DNSFLAGS=$(DNSFLAGS) -DKRB5_DNS_LOOKUP_KDC=1
!endif
!if defined(KRB5_USE_DNS_REALMS)
DNSFLAGS=$(DNSFLAGS) -DKRB5_DNS_LOOKUP_REALM=1
!endif
!else
DNSLIBS=
DNSFLAGS=
!endif

!if defined(KRB5_KFW_COMPILE)
KFWFLAGS=-DUSE_LEASH=1
!endif

#
# The name of the C compiler for the target
#
CC=cl

PDB_OPTS=-Fd$(OUTPRE)\ -FD
CPPFLAGS=-I$(SRCTOP)\include -I$(SRCTOP)\include\krb5 $(DNSFLAGS) -DKRB5_PRIVATE=1 -DWIN32_LEAN_AND_MEAN -DKRB5_DEPRECATED=1 $(KFWFLAGS)
CCOPTS=-nologo /W3 $(PDB_OPTS) $(DLL_FILE_DEF)
LOPTS=-nologo -incremental:no

# /ZI gives better debug info in each object file (MSVC 6.0 or higher).
# /Zi gives debug info in each object file.
# /Gs Avoid stack probes (they don't seem to work anyway)
# /Os optimize for space.  FIXME:  Do not use /Ox; it miscompiles the DES lib!
# /Od disable optimization (for debugging)
# /MD (Win32) thread safe, ML would be single threaded, don't build with ML

#
# CCOPTS  was for DLL compiles
# CCOPTS2 was for non-DLL compiles (EXEs, for example)
#
!ifdef NODEBUG
CCOPTS=/Os /MD $(CCOPTS)
LOPTS=$(LOPTS)
!else
CCOPTS=/Od /ZI /MDd $(CCOPTS)
LOPTS=$(LOPTS) -debug
!endif

# XXX - NOTE: We should probably use DllMainCRTStartup
DLL_LINKOPTS=$(LOPTS) -dll -entry:DllMain
EXE_LINKOPTS=$(LOPTS)

RM=$(BUILDTOP)\config\rm.bat
LIBECHO=$(BUILDTOP)\util\windows\$(OUTPRE)libecho
CP=copy
MV=ren
LN=copy
LIBCMD=lib
AWK=rem
RC = rc
CVTRES = cvtres

WCONFIG_EXE=$(BUILDTOP)\$(OUTPRE)wconfig.exe
WCONFIG=$(WCONFIG_EXE:.exe=) $(WCONFIG_FLAGS)

CLIB=$(BUILDTOP)\lib\$(OUTPRE)comerr32.lib
PLIB=$(BUILDTOP)\lib\$(OUTPRE)xpprof32.lib
KLIB=$(BUILDTOP)\lib\$(OUTPRE)krb5_32.lib
K4LIB=$(BUILDTOP)\lib\$(OUTPRE)krb4_32.lib

GLIB=$(BUILDTOP)\lib\$(OUTPRE)gssapi32.lib
WLIB=

KRB4_INCLUDES=-I$(BUILDTOP)/include/kerberosIV

COM_ERR_DEPS	= $(BUILDTOP)/include/com_err.h

ARADD=rem
RANLIB=rem
ARCHIVE=rem

LIBEXT=lib
OBJEXT=obj
EXEEXT=.exe

MFLAGS=$(MAKEFLAGS)
!ifdef MIGNORE
MAKE=-$(MAKE)
!endif

CFLAGS = $(CCOPTS)
ALL_CFLAGS = $(DEFS) $(DEFINES) $(LOCALINCLUDES) $(CPPFLAGS) $(CFLAGS)

C_RULE_STUFF=$(CC) $(ALL_CFLAGS) -Fo$(OUTPRE)\ -c
C_RULE_PRINT=$(C_RULE_STUFF)
C_RULE=$(C_RULE_STUFF) $<

{}.rc{$(OUTPRE)}.res:
	$(RC) $(RCFLAGS) -fo $@ -r $<

{}.c{$(OUTPRE)}.obj:
	@if "%DO_C_RULE_PRINT%"=="1" echo %C_RULE_PRINT% ...
	@set DO_C_RULE_PRINT=
	@$(C_RULE)

{}.cxx{$(OUTPRE)}.obj:
	@if "%DO_C_RULE_PRINT%"=="1" echo %C_RULE_PRINT% ...
	@set DO_C_RULE_PRINT=
	@$(C_RULE)

{}.cpp{$(OUTPRE)}.obj:
	@if "%DO_C_RULE_PRINT%"=="1" echo %C_RULE_PRINT% ...
	@set DO_C_RULE_PRINT=
	@$(C_RULE)

#
# End of Win32 pre-config lines (config/win-pre.in)
#

thisconfigdir=./..
myfulldir=lib/krb5/ccache
mydir=ccache
BUILDTOP=$(REL)..$(S)..$(S)..
LOCAL_SUBDIRS =

RUN_SETUP = @KRB5_RUN_ENV@
PROG_LIBPATH=-L$(TOPLIBD)
PROG_RPATH=$(KRB5_LIBDIR)

LOCALINCLUDES = -I$(srcdir)$(S)ccapi $(WIN_INCLUDES)

WIN_INCLUDES = -I$(SRCTOP)\windows\lib

BUILDTOP = ..\..\..
PREFIXDIR=ccache
OBJFILE=..\$(OUTPRE)$(PREFIXDIR).lst
##WIN16##LIBNAME=..\krb5.lib

MSLSA_OBJ = $(OUTPRE)cc_mslsa.$(OBJEXT)
MSLSA_SRC = $(srcdir)/cc_mslsa.c

MAC_SUBDIRS = ccapi

STLIBOBJS= \
	ccbase.o \
	cccopy.o \
	ccdefault.o \
	ccdefops.o \
	cc_retr.o \
	cc_file.o cc_memory.o \
	ccfns.o \
	ser_cc.o

OBJS=	$(OUTPRE)ccbase.$(OBJEXT) \
	$(OUTPRE)cccopy.$(OBJEXT) \
	$(OUTPRE)ccdefault.$(OBJEXT) \
	$(OUTPRE)ccdefops.$(OBJEXT) \
	$(OUTPRE)cc_retr.$(OBJEXT) \
	$(OUTPRE)cc_file.$(OBJEXT) \
	$(OUTPRE)cc_memory.$(OBJEXT) \
	$(OUTPRE)ccfns.$(OBJEXT) \
	$(OUTPRE)ser_cc.$(OBJEXT) $(MSLSA_OBJ)

SRCS=	$(srcdir)/ccbase.c \
	$(srcdir)/cccopy.c \
	$(srcdir)/ccdefault.c \
	$(srcdir)/ccdefops.c \
	$(srcdir)/cc_retr.c \
	$(srcdir)/cc_file.c \
	$(srcdir)/cc_memory.c \
	$(srcdir)/ccfns.c \
	$(srcdir)/ser_cc.c $(MSLSA_SRC)

OBJS=$(OBJS) $(OUTPRE)ccfns.$(OBJEXT)

all-unix:: all-libobjs

all-windows:: subdirs $(OBJFILE) 

subdirs:: ccapi\$(OUTPRE)file.lst

ccapi\$(OUTPRE)file.lst::
	cd ccapi
	@echo Making in krb5\ccache\ccapi
	$(MAKE) -$(MFLAGS)
	cd ..

$(OBJFILE): $(OBJS) ccapi\$(OUTPRE)file.lst
	$(RM) $(OBJFILE)
##WIN16##	$(CP) nul: $(OBJFILE)
	$(LIBECHO) -p $(PREFIXDIR)\ $(OUTPRE)*.obj \
		ccapi\$(OUTPRE)*.obj > $(OBJFILE)


clean-unix:: clean-libobjs

clean-windows::
	cd ccapi
	@echo Making clean in krb5\ccache\ccapi
	$(MAKE) -$(MFLAGS) clean
	cd ..
	@echo Making clean in krb5\ccache
	$(RM) $(OBJFILE)

T_FILE_OBJS=t_file.o

t_file: $(T_FILE_OBJS) $(KRB5_BASE_DEPLIBS)
	$(CC_LINK) -o t_file $(T_FILE_OBJS) $(KRB5_BASE_LIBS)

T_CC_OBJS=t_cc.o

t_cc: $(T_CC_OBJS) $(KRB5_BASE_DEPLIBS)
	$(CC_LINK) -o t_cc $(T_CC_OBJS) $(KRB5_BASE_LIBS)

check-unix:: t_cc
	KRB5_CONFIG=$(srcdir)/t_krb5.conf ; export KRB5_CONFIG ;\
	$(RUN_SETUP) ./t_cc 

clean-unix::
	$(RM) t_cc t_cc.o

 $(OUTPRE)cc_mslsa.$(OBJEXT): cc_mslsa.c $(SRCTOP)/include/k5-int.h $(BUILDTOP)/include/krb5/osconf.h $(BUILDTOP)/include/krb5/autoconf.h $(BUILDTOP)/include/krb5.h $(COM_ERR_DEPS)

# @libobj_frag@

# +++ Dependency line eater +++
# 
# Makefile dependencies follow.  This must be the last section in
# the Makefile.in file
#
ccbase.so ccbase.po $(OUTPRE)ccbase.$(OBJEXT): ccbase.c $(SRCTOP)/include/k5-int.h \
  $(BUILDTOP)/include/krb5/osconf.h $(BUILDTOP)/include/krb5/autoconf.h \
  $(SRCTOP)/include/k5-platform.h $(BUILDTOP)/include/krb5.h \
  $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h $(SRCTOP)/include/port-sockets.h \
  $(SRCTOP)/include/socket-utils.h $(SRCTOP)/include/krb5/kdb.h \
  fcc.h
cccopy.so cccopy.po $(OUTPRE)cccopy.$(OBJEXT): cccopy.c $(SRCTOP)/include/k5-int.h \
  $(BUILDTOP)/include/krb5/osconf.h $(BUILDTOP)/include/krb5/autoconf.h \
  $(SRCTOP)/include/k5-platform.h $(BUILDTOP)/include/krb5.h \
  $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h $(SRCTOP)/include/port-sockets.h \
  $(SRCTOP)/include/socket-utils.h $(SRCTOP)/include/krb5/kdb.h
ccdefault.so ccdefault.po $(OUTPRE)ccdefault.$(OBJEXT): ccdefault.c $(SRCTOP)/include/k5-int.h \
  $(BUILDTOP)/include/krb5/osconf.h $(BUILDTOP)/include/krb5/autoconf.h \
  $(SRCTOP)/include/k5-platform.h $(BUILDTOP)/include/krb5.h \
  $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h $(SRCTOP)/include/port-sockets.h \
  $(SRCTOP)/include/socket-utils.h $(SRCTOP)/include/krb5/kdb.h
ccdefops.so ccdefops.po $(OUTPRE)ccdefops.$(OBJEXT): ccdefops.c $(SRCTOP)/include/k5-int.h \
  $(BUILDTOP)/include/krb5/osconf.h $(BUILDTOP)/include/krb5/autoconf.h \
  $(SRCTOP)/include/k5-platform.h $(BUILDTOP)/include/krb5.h \
  $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h $(SRCTOP)/include/port-sockets.h \
  $(SRCTOP)/include/socket-utils.h $(SRCTOP)/include/krb5/kdb.h \
  fcc.h
cc_retr.so cc_retr.po $(OUTPRE)cc_retr.$(OBJEXT): cc_retr.c $(SRCTOP)/include/k5-int.h \
  $(BUILDTOP)/include/krb5/osconf.h $(BUILDTOP)/include/krb5/autoconf.h \
  $(SRCTOP)/include/k5-platform.h $(BUILDTOP)/include/krb5.h \
  $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h $(SRCTOP)/include/port-sockets.h \
  $(SRCTOP)/include/socket-utils.h $(SRCTOP)/include/krb5/kdb.h
cc_file.so cc_file.po $(OUTPRE)cc_file.$(OBJEXT): cc_file.c $(SRCTOP)/include/k5-int.h \
  $(BUILDTOP)/include/krb5/osconf.h $(BUILDTOP)/include/krb5/autoconf.h \
  $(SRCTOP)/include/k5-platform.h $(BUILDTOP)/include/krb5.h \
  $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h $(SRCTOP)/include/port-sockets.h \
  $(SRCTOP)/include/socket-utils.h $(SRCTOP)/include/krb5/kdb.h
cc_memory.so cc_memory.po $(OUTPRE)cc_memory.$(OBJEXT): cc_memory.c $(SRCTOP)/include/k5-int.h \
  $(BUILDTOP)/include/krb5/osconf.h $(BUILDTOP)/include/krb5/autoconf.h \
  $(SRCTOP)/include/k5-platform.h $(BUILDTOP)/include/krb5.h \
  $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h $(SRCTOP)/include/port-sockets.h \
  $(SRCTOP)/include/socket-utils.h $(SRCTOP)/include/krb5/kdb.h
ccfns.so ccfns.po $(OUTPRE)ccfns.$(OBJEXT): ccfns.c $(SRCTOP)/include/k5-int.h \
  $(BUILDTOP)/include/krb5/osconf.h $(BUILDTOP)/include/krb5/autoconf.h \
  $(SRCTOP)/include/k5-platform.h $(BUILDTOP)/include/krb5.h \
  $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h $(SRCTOP)/include/port-sockets.h \
  $(SRCTOP)/include/socket-utils.h $(SRCTOP)/include/krb5/kdb.h
ser_cc.so ser_cc.po $(OUTPRE)ser_cc.$(OBJEXT): ser_cc.c $(SRCTOP)/include/k5-int.h \
  $(BUILDTOP)/include/krb5/osconf.h $(BUILDTOP)/include/krb5/autoconf.h \
  $(SRCTOP)/include/k5-platform.h $(BUILDTOP)/include/krb5.h \
  $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h $(SRCTOP)/include/port-sockets.h \
  $(SRCTOP)/include/socket-utils.h $(SRCTOP)/include/krb5/kdb.h

#
# Start of Win32 post-config lines (config/win-post.in)
#

setup-msg::
	@set C_RULE_PRINT=	$(C_RULE_PRINT)
	@set DO_C_RULE_PRINT=1

!if defined(NO_OUTPRE) || defined(NO_OUTDIR)
outpre-dir::
!else
outpre-dir:: $(OUTPRE3)$(DIRNUL)
!endif

#
# put all:: first just in case no other rules occur here
#
all::

#
# Set the #define to indicate that we are compiling a DLL.  We default to 
# compiling the Kerberos library
#
!if defined(DLL_EXP_TYPE)
DLL_FILE_DEF=/D$(DLL_EXP_TYPE)_DLL_FILE
!else
DLL_FILE_DEF=/DKRB5_DLL_FILE
!endif

# Build the Makefile unless we are in the top-level
#(where there is already an explicit rule).
!if !defined(ZIP) && !defined(WINFILES)
Makefile: Makefile.in $(BUILDTOP)\config\win-pre.in $(BUILDTOP)\config\win-post.in
	$(WCONFIG) $(BUILDTOP)\config < Makefile.in > Makefile
!endif


!if defined(LIBNAME)

!if !defined(OBJFILELIST)
OBJFILELIST=@$(OBJFILE)
!endif
!if !defined(OBJFILEDEP)
OBJFILEDEP=$(OBJFILE)
!endif

all-windows:: $(LIBNAME)
$(LIBNAME): $(OBJFILEDEP)
	$(LIBCMD) /out:$(LIBNAME) /nologo $(OBJFILELIST)

!endif # LIBNAME


!if defined(OBJFILE)
all-windows:: $(OBJFILE)
!if defined(LIBOBJS)
$(OBJFILE): $(LIBOBJS)
	if exist $(OBJFILE) del $(OBJFILE)
!if defined(PREFIXDIR)
	$(LIBECHO) -p $(PREFIXDIR)\ $** > $(OBJFILE)
!else
	$(LIBECHO) $** > $(OBJFILE)
!endif # !PREFIXDIR
!endif # LIBOBJS
!endif # OBJFILE


check::
check-windows::

!if defined(LIBNAME)
clean-windows::
# NO LONGER NEEDED BECAUSE WE CLEAN OUT THE DIR...
#	if exist $(LIBNAME) del $(LIBNAME)
!endif
!if defined(OBJFILE)
clean-windows::
# NO LONGER NEEDED BECAUSE WE CLEAN OUT THE DIR...
#	if exist $(OBJFILE) del $(OBJFILE)
!endif

clean-windows:: clean-windows-files clean-windows-dir


# This needs to be in the post because we need RM to be defined in terms
# of BUILDTOP
clean-windows-files::
!if "$(OUTPRE3)" == ""
!error ASSERTION FAILURE: OUTPRE3 must be defined!!!
!endif
!if "$(OS)" == "Windows_NT"
	@if exist $(OUTPRE3)$(DIRNUL) rd /s/q $(OUTPRE3)
!else
	@if exist $(OUTPRE3)$(DIRNUL) deltree /y $(OUTPRE3)
!endif
!if 0
	$(RM) .\$(OUTPRE)*.obj .\$(OUTPRE)*.res
	$(RM) .\$(OUTPRE)*.exe .\$(OUTPRE)*.dll
	$(RM) .\$(OUTPRE)*.lib .\$(OUTPRE)*.pdb
	$(RM) .\$(OUTPRE)*.exp .\$(OUTPRE)*.map
	$(RM) .\$(OUTPRE)*.idb .\$(OUTPRE)*.ilk
!endif
