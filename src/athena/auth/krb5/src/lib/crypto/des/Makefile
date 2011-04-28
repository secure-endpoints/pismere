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

#
# The name of the C compiler for the target
#
CC=cl

PDB_OPTS=-Fd$(OUTPRE)\ -FD
CPPFLAGS=-I$(SRCTOP)\include -I$(SRCTOP)\include\krb5 $(DNSFLAGS) -DKRB5_PRIVATE=1 -DWIN32_LEAN_AND_MEAN -DKRB5_DEPRECATED=1
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
myfulldir=lib/crypto/des
mydir=des
BUILDTOP=$(REL)..$(S)..$(S)..

BUILDTOP = ..\..\..
PREFIXDIR=des
OBJFILE=..\$(OUTPRE)des.lst
##WIN16##LIBNAME=..\crypto.lib

PROG_LIBPATH=-L$(TOPLIBD)
PROG_RPATH=$(KRB5_LIBDIR)

RUN_SETUP = @KRB5_RUN_ENV@ KRB5_CONFIG=$(SRCTOP)/config-files/krb5.conf

STLIBOBJS=\
	afsstring2key.o	\
	d3_cbc.o	\
	d3_kysched.o	\
	f_cbc.o 	\
	f_cksum.o	\
	f_parity.o 	\
	f_sched.o 	\
	f_tables.o	\
	key_sched.o	\
	string2key.o	\
	weak_key.o

OBJS=	$(OUTPRE)afsstring2key.$(OBJEXT)	\
	$(OUTPRE)d3_cbc.$(OBJEXT)	\
	$(OUTPRE)d3_kysched.$(OBJEXT)	\
	$(OUTPRE)f_cbc.$(OBJEXT) 	\
	$(OUTPRE)f_cksum.$(OBJEXT)	\
	$(OUTPRE)f_parity.$(OBJEXT) 	\
	$(OUTPRE)f_sched.$(OBJEXT) 	\
	$(OUTPRE)f_tables.$(OBJEXT)	\
	$(OUTPRE)key_sched.$(OBJEXT)	\
	$(OUTPRE)string2key.$(OBJEXT)	\
	$(OUTPRE)weak_key.$(OBJEXT)

SRCS=	$(srcdir)/afsstring2key.c	\
	$(srcdir)/d3_cbc.c	\
	$(srcdir)/d3_kysched.c	\
	$(srcdir)/f_cbc.c	\
	$(srcdir)/f_cksum.c	\
	$(srcdir)/f_parity.c	\
	$(srcdir)/f_sched.c	\
	$(srcdir)/f_tables.c	\
	$(srcdir)/key_sched.c	\
	$(srcdir)/weak_key.c	\
	$(srcdir)/string2key.c

LIBOBJS = $(OBJS)

all-unix:: all-libobjs

includes:: depend

depend:: $(SRCS)

TOBJS = $(OUTPRE)key_sched.$(OBJEXT) $(OUTPRE)f_sched.$(OBJEXT) $(OUTPRE)f_cbc.$(OBJEXT) \
	$(OUTPRE)f_tables.$(OBJEXT) $(OUTPRE)f_cksum.$(OBJEXT)

verify$(EXEEXT): t_verify.$(OBJEXT) $(TOBJS) f_parity.$(OBJEXT) \
	$(COM_ERR_DEPLIB)
	$(CC_LINK) -o $@ t_verify.$(OBJEXT) $(TOBJS) f_parity.$(OBJEXT) \
		-lcom_err

destest$(EXEEXT): destest.$(OBJEXT) $(TOBJS)
	$(CC_LINK) -o $@ destest.$(OBJEXT) $(TOBJS)

TAFSS2KOBJS =								\
	t_afss2k.$(OBJEXT) $(TOBJS)					\
	afsstring2key.$(OBJEXT) f_parity.$(OBJEXT) weak_key.$(OBJEXT)

t_afss2k$(EXEEXT): $(TAFSS2KOBJS) $(COM_ERR_DEPLIB)
	$(CC_LINK) -o $@ $(TAFSS2KOBJS) -lcom_err

check-unix:: verify destest t_afss2k
	$(RUN_SETUP) ./verify -z
	$(RUN_SETUP) ./verify -m
	$(RUN_SETUP) ./verify
	$(RUN_SETUP) ./destest < $(srcdir)/keytest.data
	$(RUN_SETUP) ./t_afss2k

check-windows::

clean:: 
	$(RM) destest$(EXEEXT) verify$(EXEEXT) destest.$(OBJEXT) \
	t_verify.$(OBJEXT) t_afss2k.$(OBJEXT) t_afss2k$(EXEEXT)

clean-unix:: clean-libobjs

# @libobj_frag@

# +++ Dependency line eater +++
# 
# Makefile dependencies follow.  This must be the last section in
# the Makefile.in file
#
afsstring2key.so afsstring2key.po $(OUTPRE)afsstring2key.$(OBJEXT): afsstring2key.c $(SRCTOP)/include/k5-int.h \
  $(BUILDTOP)/include/krb5/osconf.h $(BUILDTOP)/include/krb5/autoconf.h \
  $(BUILDTOP)/include/krb5.h $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h \
  $(SRCTOP)/include/port-sockets.h $(SRCTOP)/include/socket-utils.h \
  $(SRCTOP)/include/krb5/kdb.h des_int.h $(SRCTOP)/include/kerberosIV/des.h
d3_cbc.so d3_cbc.po $(OUTPRE)d3_cbc.$(OBJEXT): d3_cbc.c des_int.h $(SRCTOP)/include/k5-int.h \
  $(BUILDTOP)/include/krb5/osconf.h $(BUILDTOP)/include/krb5/autoconf.h \
  $(BUILDTOP)/include/krb5.h $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h \
  $(SRCTOP)/include/port-sockets.h $(SRCTOP)/include/socket-utils.h \
  $(SRCTOP)/include/krb5/kdb.h $(SRCTOP)/include/kerberosIV/des.h \
  f_tables.h
d3_kysched.so d3_kysched.po $(OUTPRE)d3_kysched.$(OBJEXT): d3_kysched.c $(SRCTOP)/include/k5-int.h \
  $(BUILDTOP)/include/krb5/osconf.h $(BUILDTOP)/include/krb5/autoconf.h \
  $(BUILDTOP)/include/krb5.h $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h \
  $(SRCTOP)/include/port-sockets.h $(SRCTOP)/include/socket-utils.h \
  $(SRCTOP)/include/krb5/kdb.h des_int.h $(SRCTOP)/include/kerberosIV/des.h
f_cbc.so f_cbc.po $(OUTPRE)f_cbc.$(OBJEXT): f_cbc.c des_int.h $(SRCTOP)/include/k5-int.h \
  $(BUILDTOP)/include/krb5/osconf.h $(BUILDTOP)/include/krb5/autoconf.h \
  $(BUILDTOP)/include/krb5.h $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h \
  $(SRCTOP)/include/port-sockets.h $(SRCTOP)/include/socket-utils.h \
  $(SRCTOP)/include/krb5/kdb.h $(SRCTOP)/include/kerberosIV/des.h \
  f_tables.h
f_cksum.so f_cksum.po $(OUTPRE)f_cksum.$(OBJEXT): f_cksum.c des_int.h $(SRCTOP)/include/k5-int.h \
  $(BUILDTOP)/include/krb5/osconf.h $(BUILDTOP)/include/krb5/autoconf.h \
  $(BUILDTOP)/include/krb5.h $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h \
  $(SRCTOP)/include/port-sockets.h $(SRCTOP)/include/socket-utils.h \
  $(SRCTOP)/include/krb5/kdb.h $(SRCTOP)/include/kerberosIV/des.h \
  f_tables.h
f_parity.so f_parity.po $(OUTPRE)f_parity.$(OBJEXT): f_parity.c des_int.h $(SRCTOP)/include/k5-int.h \
  $(BUILDTOP)/include/krb5/osconf.h $(BUILDTOP)/include/krb5/autoconf.h \
  $(BUILDTOP)/include/krb5.h $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h \
  $(SRCTOP)/include/port-sockets.h $(SRCTOP)/include/socket-utils.h \
  $(SRCTOP)/include/krb5/kdb.h $(SRCTOP)/include/kerberosIV/des.h
f_sched.so f_sched.po $(OUTPRE)f_sched.$(OBJEXT): f_sched.c $(SRCTOP)/include/k5-int.h \
  $(BUILDTOP)/include/krb5/osconf.h $(BUILDTOP)/include/krb5/autoconf.h \
  $(BUILDTOP)/include/krb5.h $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h \
  $(SRCTOP)/include/port-sockets.h $(SRCTOP)/include/socket-utils.h \
  $(SRCTOP)/include/krb5/kdb.h des_int.h $(SRCTOP)/include/kerberosIV/des.h
f_tables.so f_tables.po $(OUTPRE)f_tables.$(OBJEXT): f_tables.c des_int.h $(SRCTOP)/include/k5-int.h \
  $(BUILDTOP)/include/krb5/osconf.h $(BUILDTOP)/include/krb5/autoconf.h \
  $(BUILDTOP)/include/krb5.h $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h \
  $(SRCTOP)/include/port-sockets.h $(SRCTOP)/include/socket-utils.h \
  $(SRCTOP)/include/krb5/kdb.h $(SRCTOP)/include/kerberosIV/des.h \
  f_tables.h
key_sched.so key_sched.po $(OUTPRE)key_sched.$(OBJEXT): key_sched.c $(SRCTOP)/include/k5-int.h \
  $(BUILDTOP)/include/krb5/osconf.h $(BUILDTOP)/include/krb5/autoconf.h \
  $(BUILDTOP)/include/krb5.h $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h \
  $(SRCTOP)/include/port-sockets.h $(SRCTOP)/include/socket-utils.h \
  $(SRCTOP)/include/krb5/kdb.h des_int.h $(SRCTOP)/include/kerberosIV/des.h
weak_key.so weak_key.po $(OUTPRE)weak_key.$(OBJEXT): weak_key.c $(SRCTOP)/include/k5-int.h \
  $(BUILDTOP)/include/krb5/osconf.h $(BUILDTOP)/include/krb5/autoconf.h \
  $(BUILDTOP)/include/krb5.h $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h \
  $(SRCTOP)/include/port-sockets.h $(SRCTOP)/include/socket-utils.h \
  $(SRCTOP)/include/krb5/kdb.h des_int.h $(SRCTOP)/include/kerberosIV/des.h
string2key.so string2key.po $(OUTPRE)string2key.$(OBJEXT): string2key.c $(SRCTOP)/include/k5-int.h \
  $(BUILDTOP)/include/krb5/osconf.h $(BUILDTOP)/include/krb5/autoconf.h \
  $(BUILDTOP)/include/krb5.h $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h \
  $(SRCTOP)/include/port-sockets.h $(SRCTOP)/include/socket-utils.h \
  $(SRCTOP)/include/krb5/kdb.h des_int.h $(SRCTOP)/include/kerberosIV/des.h

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
