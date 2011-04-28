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

thisconfigdir=.
myfulldir=lib/des425
mydir=.
BUILDTOP=$(REL)..$(S)..
LOCALINCLUDES = -I$(srcdir)/../crypto/des -I$(srcdir)/../../include/kerberosIV

BUILDTOP = ..\..
LIBNAME=$(OUTPRE)des425.lib
OBJFILE=$(OUTPRE)des425.lst
OBJFILEDEP=$(OUTPRE)des425.lst
OBJFILELIST=@$(OUTPRE)des425.lst

PROG_LIBPATH=-L$(TOPLIBD)
PROG_RPATH=$(KRB5_LIBDIR)

RUN_SETUP=@KRB5_RUN_ENV@

LIB=des425
LIBMAJOR=3
LIBMINOR=0
RELDIR=des425
# Depends on libk5crypto and libkrb5
SHLIB_EXPDEPS = \
	$(TOPLIBD)/libk5crypto$(SHLIBEXT) \
	$(TOPLIBD)/libkrb5$(SHLIBEXT)
SHLIB_EXPLIBS=-lkrb5 -lcom_err -lk5crypto
SHLIB_DIRS=-L$(TOPLIBD)
SHLIB_RDIRS=$(KRB5_LIBDIR)

STOBJLISTS=OBJS.ST
STLIBOBJS=cksum.o	\
	des.o		\
	enc_dec.o	\
	key_parity.o	\
	key_sched.o	\
	new_rnd_key.o	\
	pcbc_encrypt.o	\
	quad_cksum.o	\
	random_key.o	\
	read_passwd.o  \
	str_to_key.o	\
	unix_time.o     \
	util.o		\
	weak_key.o


OBJS=	$(OUTPRE)cksum.$(OBJEXT)	\
	$(OUTPRE)des.$(OBJEXT)		\
	$(OUTPRE)enc_dec.$(OBJEXT)	\
	$(OUTPRE)key_parity.$(OBJEXT)	\
	$(OUTPRE)key_sched.$(OBJEXT)	\
	$(OUTPRE)new_rnd_key.$(OBJEXT)	\
	$(OUTPRE)pcbc_encrypt.$(OBJEXT)	\
	$(OUTPRE)quad_cksum.$(OBJEXT)	\
	$(OUTPRE)random_key.$(OBJEXT)	\
	$(OUTPRE)read_passwd.$(OBJEXT)	\
	$(OUTPRE)str_to_key.$(OBJEXT)	\
	$(OUTPRE)unix_time.$(OBJEXT)	\
	$(OUTPRE)util.$(OBJEXT)		\
	$(OUTPRE)weak_key.$(OBJEXT)

SRCS=	$(srcdir)/cksum.c	\
	$(srcdir)/des.c		\
	$(srcdir)/enc_dec.c	\
	$(srcdir)/key_parity.c	\
	$(srcdir)/key_sched.c	\
	$(srcdir)/new_rnd_key.c	\
	$(srcdir)/pcbc_encrypt.c	\
	$(srcdir)/quad_cksum.c	\
	$(srcdir)/random_key.c	\
	$(srcdir)/read_passwd.c \
	$(srcdir)/str_to_key.c	\
	$(srcdir)/unix_time.c   \
	$(srcdir)/util.c	\
	$(srcdir)/weak_key.c

all-unix:: all-liblinks
all-mac:: $(OBJS)

LIBOBJS = $(OBJS)

shared:
	mkdir shared

verify: verify.o $(DES425_DEPLIB) $(KRB5_BASE_DEPLIBS)
	$(CC_LINK) -o $@ verify.o $(DES425_LIB) $(KRB5_BASE_LIBS)

t_quad: t_quad.o quad_cksum.o
	$(CC_LINK) -o $@ t_quad.o quad_cksum.o

t_pcbc: t_pcbc.o pcbc_encrypt.o key_sched.o $(KRB5_BASE_DEPLIBS)
	$(CC_LINK) -o $@ t_pcbc.o pcbc_encrypt.o key_sched.o $(KRB5_BASE_LIBS)

check-unix:: verify t_quad t_pcbc
	$(RUN_SETUP) ./verify -z
	$(RUN_SETUP) ./verify -m
	$(RUN_SETUP) ./verify
	$(RUN_SETUP) ./t_quad
	$(RUN_SETUP) ./t_pcbc

check-windows::

clean:: 
	$(RM) $(OUTPRE)verify$(EXEEXT) $(OUTPRE)verify.$(OBJEXT) \
		$(OUTPRE)t_quad$(EXEEXT) $(OUTPRE)t_quad.$(OBJEXT) \
		$(OUTPRE)t_pcbc$(EXEEXT) $(OUTPRE)t_pcbc.$(OBJEXT)

clean-unix:: clean-liblinks clean-libs clean-libobjs

install-unix:: install-libs

# @lib_frag@
# @libobj_frag@

# +++ Dependency line eater +++
# 
# Makefile dependencies follow.  This must be the last section in
# the Makefile.in file
#
cksum.so cksum.po $(OUTPRE)cksum.$(OBJEXT): cksum.c $(srcdir)/../crypto/des/des_int.h \
  $(SRCTOP)/include/k5-int.h $(BUILDTOP)/include/krb5/osconf.h \
  $(BUILDTOP)/include/krb5/autoconf.h $(SRCTOP)/include/k5-platform.h \
  $(BUILDTOP)/include/krb5.h $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h \
  $(SRCTOP)/include/port-sockets.h $(SRCTOP)/include/socket-utils.h \
  $(SRCTOP)/include/krb5/kdb.h $(SRCTOP)/include/kerberosIV/des.h
des.so des.po $(OUTPRE)des.$(OBJEXT): des.c $(srcdir)/../crypto/des/des_int.h \
  $(SRCTOP)/include/k5-int.h $(BUILDTOP)/include/krb5/osconf.h \
  $(BUILDTOP)/include/krb5/autoconf.h $(SRCTOP)/include/k5-platform.h \
  $(BUILDTOP)/include/krb5.h $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h \
  $(SRCTOP)/include/port-sockets.h $(SRCTOP)/include/socket-utils.h \
  $(SRCTOP)/include/krb5/kdb.h $(SRCTOP)/include/kerberosIV/des.h
enc_dec.so enc_dec.po $(OUTPRE)enc_dec.$(OBJEXT): enc_dec.c $(srcdir)/../crypto/des/des_int.h \
  $(SRCTOP)/include/k5-int.h $(BUILDTOP)/include/krb5/osconf.h \
  $(BUILDTOP)/include/krb5/autoconf.h $(SRCTOP)/include/k5-platform.h \
  $(BUILDTOP)/include/krb5.h $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h \
  $(SRCTOP)/include/port-sockets.h $(SRCTOP)/include/socket-utils.h \
  $(SRCTOP)/include/krb5/kdb.h $(SRCTOP)/include/kerberosIV/des.h
key_parity.so key_parity.po $(OUTPRE)key_parity.$(OBJEXT): key_parity.c $(srcdir)/../crypto/des/des_int.h \
  $(SRCTOP)/include/k5-int.h $(BUILDTOP)/include/krb5/osconf.h \
  $(BUILDTOP)/include/krb5/autoconf.h $(SRCTOP)/include/k5-platform.h \
  $(BUILDTOP)/include/krb5.h $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h \
  $(SRCTOP)/include/port-sockets.h $(SRCTOP)/include/socket-utils.h \
  $(SRCTOP)/include/krb5/kdb.h $(SRCTOP)/include/kerberosIV/des.h
key_sched.so key_sched.po $(OUTPRE)key_sched.$(OBJEXT): key_sched.c $(srcdir)/../crypto/des/des_int.h \
  $(SRCTOP)/include/k5-int.h $(BUILDTOP)/include/krb5/osconf.h \
  $(BUILDTOP)/include/krb5/autoconf.h $(SRCTOP)/include/k5-platform.h \
  $(BUILDTOP)/include/krb5.h $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h \
  $(SRCTOP)/include/port-sockets.h $(SRCTOP)/include/socket-utils.h \
  $(SRCTOP)/include/krb5/kdb.h $(SRCTOP)/include/kerberosIV/des.h
new_rnd_key.so new_rnd_key.po $(OUTPRE)new_rnd_key.$(OBJEXT): new_rnd_key.c $(srcdir)/../crypto/des/des_int.h \
  $(SRCTOP)/include/k5-int.h $(BUILDTOP)/include/krb5/osconf.h \
  $(BUILDTOP)/include/krb5/autoconf.h $(SRCTOP)/include/k5-platform.h \
  $(BUILDTOP)/include/krb5.h $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h \
  $(SRCTOP)/include/port-sockets.h $(SRCTOP)/include/socket-utils.h \
  $(SRCTOP)/include/krb5/kdb.h $(SRCTOP)/include/kerberosIV/des.h
pcbc_encrypt.so pcbc_encrypt.po $(OUTPRE)pcbc_encrypt.$(OBJEXT): pcbc_encrypt.c $(srcdir)/../crypto/des/des_int.h \
  $(SRCTOP)/include/k5-int.h $(BUILDTOP)/include/krb5/osconf.h \
  $(BUILDTOP)/include/krb5/autoconf.h $(SRCTOP)/include/k5-platform.h \
  $(BUILDTOP)/include/krb5.h $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h \
  $(SRCTOP)/include/port-sockets.h $(SRCTOP)/include/socket-utils.h \
  $(SRCTOP)/include/krb5/kdb.h $(SRCTOP)/include/kerberosIV/des.h \
  $(srcdir)/../crypto/des/f_tables.h
quad_cksum.so quad_cksum.po $(OUTPRE)quad_cksum.$(OBJEXT): quad_cksum.c $(srcdir)/../crypto/des/des_int.h \
  $(SRCTOP)/include/k5-int.h $(BUILDTOP)/include/krb5/osconf.h \
  $(BUILDTOP)/include/krb5/autoconf.h $(SRCTOP)/include/k5-platform.h \
  $(BUILDTOP)/include/krb5.h $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h \
  $(SRCTOP)/include/port-sockets.h $(SRCTOP)/include/socket-utils.h \
  $(SRCTOP)/include/krb5/kdb.h $(SRCTOP)/include/kerberosIV/des.h
random_key.so random_key.po $(OUTPRE)random_key.$(OBJEXT): random_key.c $(srcdir)/../crypto/des/des_int.h \
  $(SRCTOP)/include/k5-int.h $(BUILDTOP)/include/krb5/osconf.h \
  $(BUILDTOP)/include/krb5/autoconf.h $(SRCTOP)/include/k5-platform.h \
  $(BUILDTOP)/include/krb5.h $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h \
  $(SRCTOP)/include/port-sockets.h $(SRCTOP)/include/socket-utils.h \
  $(SRCTOP)/include/krb5/kdb.h $(SRCTOP)/include/kerberosIV/des.h
read_passwd.so read_passwd.po $(OUTPRE)read_passwd.$(OBJEXT): read_passwd.c $(srcdir)/../crypto/des/des_int.h \
  $(SRCTOP)/include/k5-int.h $(BUILDTOP)/include/krb5/osconf.h \
  $(BUILDTOP)/include/krb5/autoconf.h $(SRCTOP)/include/k5-platform.h \
  $(BUILDTOP)/include/krb5.h $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h \
  $(SRCTOP)/include/port-sockets.h $(SRCTOP)/include/socket-utils.h \
  $(SRCTOP)/include/krb5/kdb.h $(SRCTOP)/include/kerberosIV/des.h
str_to_key.so str_to_key.po $(OUTPRE)str_to_key.$(OBJEXT): str_to_key.c $(srcdir)/../crypto/des/des_int.h \
  $(SRCTOP)/include/k5-int.h $(BUILDTOP)/include/krb5/osconf.h \
  $(BUILDTOP)/include/krb5/autoconf.h $(SRCTOP)/include/k5-platform.h \
  $(BUILDTOP)/include/krb5.h $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h \
  $(SRCTOP)/include/port-sockets.h $(SRCTOP)/include/socket-utils.h \
  $(SRCTOP)/include/krb5/kdb.h $(SRCTOP)/include/kerberosIV/des.h
unix_time.so unix_time.po $(OUTPRE)unix_time.$(OBJEXT): unix_time.c $(SRCTOP)/include/k5-int.h \
  $(BUILDTOP)/include/krb5/osconf.h $(BUILDTOP)/include/krb5/autoconf.h \
  $(SRCTOP)/include/k5-platform.h $(BUILDTOP)/include/krb5.h \
  $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h $(SRCTOP)/include/port-sockets.h \
  $(SRCTOP)/include/socket-utils.h $(SRCTOP)/include/krb5/kdb.h
util.so util.po $(OUTPRE)util.$(OBJEXT): util.c $(SRCTOP)/include/k5-int.h \
  $(BUILDTOP)/include/krb5/osconf.h $(BUILDTOP)/include/krb5/autoconf.h \
  $(SRCTOP)/include/k5-platform.h $(BUILDTOP)/include/krb5.h \
  $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h $(SRCTOP)/include/port-sockets.h \
  $(SRCTOP)/include/socket-utils.h $(SRCTOP)/include/krb5/kdb.h \
  $(srcdir)/../crypto/des/des_int.h $(SRCTOP)/include/kerberosIV/des.h
weak_key.so weak_key.po $(OUTPRE)weak_key.$(OBJEXT): weak_key.c $(srcdir)/../crypto/des/des_int.h \
  $(SRCTOP)/include/k5-int.h $(BUILDTOP)/include/krb5/osconf.h \
  $(BUILDTOP)/include/krb5/autoconf.h $(SRCTOP)/include/k5-platform.h \
  $(BUILDTOP)/include/krb5.h $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h \
  $(SRCTOP)/include/port-sockets.h $(SRCTOP)/include/socket-utils.h \
  $(SRCTOP)/include/krb5/kdb.h $(SRCTOP)/include/kerberosIV/des.h

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
