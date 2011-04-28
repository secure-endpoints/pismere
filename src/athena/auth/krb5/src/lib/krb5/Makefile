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

thisconfigdir=.
myfulldir=lib/krb5
mydir=.
BUILDTOP=$(REL)..$(S)..
LOCAL_SUBDIRS= error_tables asn.1 ccache keytab krb os rcache posix

BUILDTOP = ..\..
LIBNAME=$(OUTPRE)krb5.lib
OBJFILEDEP=$(OUTPRE)asn1.lst $(OUTPRE)ccache.lst $(OUTPRE)err_tbls.lst $(OUTPRE)keytab.lst $(OUTPRE)krb.lst $(OUTPRE)os.lst $(OUTPRE)posix.lst $(OUTPRE)rcache.lst
OBJFILELIST=@$(OUTPRE)asn1.lst @$(OUTPRE)ccache.lst @$(OUTPRE)err_tbls.lst @$(OUTPRE)keytab.lst @$(OUTPRE)krb.lst @$(OUTPRE)os.lst @$(OUTPRE)posix.lst @$(OUTPRE)rcache.lst

TST=if test -n "`cat DONE`" ; then

MAC_SUBDIRS = error_tables asn.1 ccache keytab krb rcache os posix 

STLIBOBJS=krb5_libinit.o

LIB=krb5
LIBMAJOR=3
LIBMINOR=2

STOBJLISTS= \
	OBJS.ST \
	error_tables/OBJS.ST \
	asn.1/OBJS.ST \
	ccache/OBJS.ST \
	keytab/OBJS.ST \
	krb/OBJS.ST \
	rcache/OBJS.ST \
	os/OBJS.ST \
	posix/OBJS.ST \
	$(BUILDTOP)/util/profile/OBJS.ST

SUBDIROBJLISTS= \
	error_tables/OBJS.ST \
	asn.1/OBJS.ST \
	ccache/OBJS.ST \
	keytab/OBJS.ST \
	krb/OBJS.ST \
	rcache/OBJS.ST \
	os/OBJS.ST \
	posix/OBJS.ST \
	$(BUILDTOP)/util/profile/OBJS.ST

OBJS=\
	$(OUTPRE)krb5_libinit.$(OBJEXT)

SRCS=\
	$(srcdir)/krb5_libinit.c

RELDIR=krb5
SHLIB_EXPDEPS = \
	$(TOPLIBD)/libk5crypto$(SHLIBEXT) \
	$(COM_ERR_DEPLIB)
SHLIB_EXPLIBS=-lk5crypto -lcom_err @GEN_LIB@ $(LIBS)
SHLIB_DIRS=-L$(TOPLIBD)
SHLIB_RDIRS=$(KRB5_LIBDIR)

all-unix:: all-liblinks

all-windows::

clean-unix:: clean-liblinks clean-libs clean-libobjs

clean-windows::
	$(RM) $(OUTPRE)krb5.lib krb5.bak

all-windows::
	cd asn.1
	@echo Making in krb5\asn.1
	$(MAKE) -$(MFLAGS)
	cd ..\ccache
	@echo Making in krb5\ccache
	$(MAKE) -$(MFLAGS)
	cd ..\error_tables
	@echo Making in krb5\error_tables
	$(MAKE) -$(MFLAGS)
	cd ..\keytab
	@echo Making in krb5\keytab
	$(MAKE) -$(MFLAGS)
	cd ..\krb
	@echo Making in krb5\krb
	$(MAKE) -$(MFLAGS)
	cd ..\os
	@echo Making in krb5\os
	$(MAKE) -$(MFLAGS)
	cd ..\posix
	@echo Making in krb5\posix
	$(MAKE) -$(MFLAGS)
	cd ..\rcache
	@echo Making in krb5\rcache
	$(MAKE) -$(MFLAGS)
	cd ..

clean-windows::
	cd asn.1
	@echo Making clean in krb5\asn.1
	$(MAKE) -$(MFLAGS) clean
	cd ..\ccache
	@echo Making clean in krb5\ccache
	$(MAKE) -$(MFLAGS) clean
	cd ..\error_tables
	@echo Making clean in krb5\error_tables
	$(MAKE) -$(MFLAGS) clean
	cd ..\keytab
	@echo Making clean in krb5\keytab
	$(MAKE) -$(MFLAGS) clean
	cd ..\krb
	@echo Making clean in krb5\krb
	$(MAKE) -$(MFLAGS) clean
	cd ..\os
	@echo Making clean in krb5\os
	$(MAKE) -$(MFLAGS) clean
	cd ..\posix
	@echo Making clean in krb5\posix
	$(MAKE) -$(MFLAGS) clean
	cd ..\rcache
	@echo Making clean in krb5\rcache
	$(MAKE) -$(MFLAGS) clean
	cd ..
	@echo Making clean locally

install-unix:: install-libs

# @lib_frag@
# @libobj_frag@

# +++ Dependency line eater +++
# 
# Makefile dependencies follow.  This must be the last section in
# the Makefile.in file
#
krb5_libinit.so krb5_libinit.po $(OUTPRE)krb5_libinit.$(OBJEXT): krb5_libinit.c $(COM_ERR_DEPS) \
  $(BUILDTOP)/include/krb5.h $(BUILDTOP)/include/krb5_err.h \
  $(BUILDTOP)/include/kv5m_err.h $(BUILDTOP)/include/asn1_err.h \
  $(BUILDTOP)/include/kdb5_err.h krb5_libinit.h

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
