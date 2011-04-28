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
myfulldir=lib/gssapi/generic
mydir=generic
BUILDTOP=$(REL)..$(S)..$(S)..
LOCALINCLUDES = -I. -I$(srcdir)

BUILDTOP = ..\..\..
PREFIXDIR=generic
OBJFILE=..\$(OUTPRE)generic.lst
##WIN16##LIBNAME=..\gssapi.lib

DLL_EXP_TYPE=GSS

ETSRCS= gssapi_err_generic.c
ETOBJS= $(OUTPRE)gssapi_err_generic.$(OBJEXT)
ETHDRS= gssapi_err_generic.h

EHDRDIR= $(BUILDTOP)$(S)include$(S)gssapi

HDRS=	$(EHDRDIR)$(S)gssapi.h \
	$(EHDRDIR)$(S)gssapi_generic.h

MK_EHDRDIR=if test -d $(EHDRDIR); then :; else (set -x; mkdir $(EHDRDIR)); fi
MK_EHDRDIR=rem

gssapi-include: $(EHDRDIR)$(S)gssapi.h

$(EHDRDIR)$(S)gssapi.h: gssapi.h
	@$(MK_EHDRDIR)
	$(CP) gssapi.h $@
$(EHDRDIR)$(S)gssapi_generic.h: $(srcdir)$(S)gssapi_generic.h
	@$(MK_EHDRDIR)
	$(CP) $(srcdir)$(S)gssapi_generic.h $@

$(OUTPRE)gssapi_err_generic.$(OBJEXT): gssapi_err_generic.c
gssapi_err_generic.h: gssapi_err_generic.et
gssapi_err_generic.c: gssapi_err_generic.et

gssapi.h: gssapi.hin
	@echo "Creating gssapi.h" ; \
	h=gss$$$$; $(RM) $$h; \
	(echo "/* This is the gssapi.h prologue. */"; \
	echo "/* It contains some choice pieces of autoconf.h */"; \
	grep SIZEOF $(BUILDTOP)/include/krb5/autoconf.h; \
	grep 'HAVE_.*_H' $(BUILDTOP)/include/krb5/autoconf.h; \
	grep 'USE_.*_H' $(BUILDTOP)/include/krb5/autoconf.h; \
	echo "/* End of gssapi.h prologue. */"; \
	cat $(srcdir)/gssapi.hin )> $$h && \
	(set -x; $(MV) $$h $@) ; e=$$?; $(RM) $$h; exit $$e

#if HasHashLibrary
# UTIL_VALIDATE_SRC= $(srcdir)/util_validate.c
# UTIL_VALIDATE_OBJ= util_validate.$(OBJEXT)
#else
#UTIL_VALIDATE_SRC= $(srcdir)/utl_nohash_validate.c
#UTIL_VALIDATE_OBJ= utl_nohash_validate.$(OBJEXT)
#endif

SRCS = \
	$(srcdir)/disp_com_err_status.c \
	$(srcdir)/disp_major_status.c \
	$(srcdir)/gssapi_generic.c \
	$(srcdir)/oid_ops.c \
	$(srcdir)/rel_buffer.c \
	$(srcdir)/rel_oid_set.c \
	$(srcdir)/util_buffer.c \
	$(srcdir)/util_oid.c \
	$(srcdir)/util_ordering.c \
	$(srcdir)/util_set.c \
	$(srcdir)/util_token.c \
	$(srcdir)/util_validate.c \
	gssapi_err_generic.c

OBJS = \
	$(OUTPRE)disp_com_err_status.$(OBJEXT) \
	$(OUTPRE)disp_major_status.$(OBJEXT) \
	$(OUTPRE)gssapi_generic.$(OBJEXT) \
	$(OUTPRE)oid_ops.$(OBJEXT) \
	$(OUTPRE)rel_buffer.$(OBJEXT) \
	$(OUTPRE)rel_oid_set.$(OBJEXT) \
	$(OUTPRE)util_buffer.$(OBJEXT) \
	$(OUTPRE)util_oid.$(OBJEXT) \
	$(OUTPRE)util_ordering.$(OBJEXT) \
	$(OUTPRE)util_set.$(OBJEXT) \
	$(OUTPRE)util_token.$(OBJEXT) \
	$(OUTPRE)util_validate.$(OBJEXT) \
	$(OUTPRE)gssapi_err_generic.$(OBJEXT)

STLIBOBJS = \
	disp_com_err_status.o \
	disp_major_status.o \
	gssapi_generic.o \
	oid_ops.o \
	rel_buffer.o \
	rel_oid_set.o \
	util_buffer.o \
	util_oid.o \
	util_ordering.o \
	util_set.o \
	util_token.o \
	util_validate.o \
	gssapi_err_generic.o

EXPORTED_HEADERS= gssapi_generic.h
EXPORTED_BUILT_HEADERS= gssapi.h

$(OBJS): $(EXPORTED_HEADERS) $(ETHDRS)

all-unix:: $(EXPORTED_HEADERS) $(ETHDRS) $(HDRS)
all-unix:: all-libobjs


LIBOBJS = $(OBJS)

all-windows:: win-create-ehdrdir
all-windows:: $(HDRS)

win-create-ehdrdir:
	if not exist $(EHDRDIR)\nul mkdir $(EHDRDIR)

clean-unix:: clean-libobjs
	$(RM) $(ETHDRS) $(ETSRCS) $(HDRS) $(EXPORTED_BUILT_HEADERS)

clean-windows::
	$(RM) $(HDRS)
	-if exist $(EHDRDIR)\nul rmdir $(EHDRDIR)

# Krb5InstallHeaders($(EXPORTED_HEADERS), $(KRB5_INCDIR)/krb5)
install-headers-unix install:: gssapi.h
	@set -x; for f in $(EXPORTED_HEADERS) ; \
	do $(INSTALL_DATA) $(srcdir)/$$f	\
		$(DESTDIR)$(KRB5_INCDIR)/gssapi/$$f ; \
	done
	@set -x; for f in $(EXPORTED_BUILT_HEADERS) ; \
	do $(INSTALL_DATA) $$f	\
		$(DESTDIR)$(KRB5_INCDIR)/gssapi/$$f ; \
	done

depend:: $(ETSRCS)

# @libobj_frag@

# +++ Dependency line eater +++
# 
# Makefile dependencies follow.  This must be the last section in
# the Makefile.in file
#
disp_com_err_status.so disp_com_err_status.po $(OUTPRE)disp_com_err_status.$(OBJEXT): disp_com_err_status.c \
  gssapiP_generic.h gssapi_generic.h $(BUILDTOP)/include/gssapi/gssapi.h \
  gssapi_err_generic.h $(COM_ERR_DEPS) $(SRCTOP)/include/k5-platform.h \
  $(BUILDTOP)/include/krb5/autoconf.h
disp_major_status.so disp_major_status.po $(OUTPRE)disp_major_status.$(OBJEXT): disp_major_status.c \
  gssapiP_generic.h gssapi_generic.h $(BUILDTOP)/include/gssapi/gssapi.h \
  gssapi_err_generic.h $(COM_ERR_DEPS) $(SRCTOP)/include/k5-platform.h \
  $(BUILDTOP)/include/krb5/autoconf.h
gssapi_generic.so gssapi_generic.po $(OUTPRE)gssapi_generic.$(OBJEXT): gssapi_generic.c \
  gssapiP_generic.h gssapi_generic.h $(BUILDTOP)/include/gssapi/gssapi.h \
  gssapi_err_generic.h $(COM_ERR_DEPS) $(SRCTOP)/include/k5-platform.h \
  $(BUILDTOP)/include/krb5/autoconf.h
oid_ops.so oid_ops.po $(OUTPRE)oid_ops.$(OBJEXT): oid_ops.c gssapiP_generic.h \
  gssapi_generic.h $(BUILDTOP)/include/gssapi/gssapi.h \
  gssapi_err_generic.h $(COM_ERR_DEPS) $(SRCTOP)/include/k5-platform.h \
  $(BUILDTOP)/include/krb5/autoconf.h
rel_buffer.so rel_buffer.po $(OUTPRE)rel_buffer.$(OBJEXT): rel_buffer.c gssapiP_generic.h \
  gssapi_generic.h $(BUILDTOP)/include/gssapi/gssapi.h \
  gssapi_err_generic.h $(COM_ERR_DEPS) $(SRCTOP)/include/k5-platform.h \
  $(BUILDTOP)/include/krb5/autoconf.h
rel_oid_set.so rel_oid_set.po $(OUTPRE)rel_oid_set.$(OBJEXT): rel_oid_set.c gssapiP_generic.h \
  gssapi_generic.h $(BUILDTOP)/include/gssapi/gssapi.h \
  gssapi_err_generic.h $(COM_ERR_DEPS) $(SRCTOP)/include/k5-platform.h \
  $(BUILDTOP)/include/krb5/autoconf.h
util_buffer.so util_buffer.po $(OUTPRE)util_buffer.$(OBJEXT): util_buffer.c gssapiP_generic.h \
  gssapi_generic.h $(BUILDTOP)/include/gssapi/gssapi.h \
  gssapi_err_generic.h $(COM_ERR_DEPS) $(SRCTOP)/include/k5-platform.h \
  $(BUILDTOP)/include/krb5/autoconf.h
util_oid.so util_oid.po $(OUTPRE)util_oid.$(OBJEXT): util_oid.c gssapiP_generic.h \
  gssapi_generic.h $(BUILDTOP)/include/gssapi/gssapi.h \
  gssapi_err_generic.h $(COM_ERR_DEPS) $(SRCTOP)/include/k5-platform.h \
  $(BUILDTOP)/include/krb5/autoconf.h
util_ordering.so util_ordering.po $(OUTPRE)util_ordering.$(OBJEXT): util_ordering.c gssapiP_generic.h \
  gssapi_generic.h $(BUILDTOP)/include/gssapi/gssapi.h \
  gssapi_err_generic.h $(COM_ERR_DEPS) $(SRCTOP)/include/k5-platform.h \
  $(BUILDTOP)/include/krb5/autoconf.h
util_set.so util_set.po $(OUTPRE)util_set.$(OBJEXT): util_set.c gssapiP_generic.h \
  gssapi_generic.h $(BUILDTOP)/include/gssapi/gssapi.h \
  gssapi_err_generic.h $(COM_ERR_DEPS) $(SRCTOP)/include/k5-platform.h \
  $(BUILDTOP)/include/krb5/autoconf.h
util_token.so util_token.po $(OUTPRE)util_token.$(OBJEXT): util_token.c gssapiP_generic.h \
  gssapi_generic.h $(BUILDTOP)/include/gssapi/gssapi.h \
  gssapi_err_generic.h $(COM_ERR_DEPS) $(SRCTOP)/include/k5-platform.h \
  $(BUILDTOP)/include/krb5/autoconf.h
util_validate.so util_validate.po $(OUTPRE)util_validate.$(OBJEXT): util_validate.c gssapiP_generic.h \
  gssapi_generic.h $(BUILDTOP)/include/gssapi/gssapi.h \
  gssapi_err_generic.h $(COM_ERR_DEPS) $(SRCTOP)/include/k5-platform.h \
  $(BUILDTOP)/include/krb5/autoconf.h
gssapi_err_generic.so gssapi_err_generic.po $(OUTPRE)gssapi_err_generic.$(OBJEXT): gssapi_err_generic.c \
  $(COM_ERR_DEPS)

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
