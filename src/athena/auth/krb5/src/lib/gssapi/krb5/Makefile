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
myfulldir=lib/gssapi/krb5
mydir=krb5
BUILDTOP=$(REL)..$(S)..$(S)..
LOCALINCLUDES = -I. -I$(srcdir) -I../generic -I$(srcdir)/../generic

BUILDTOP = ..\..\..
PREFIXDIR=krb5
OBJFILE = ..\$(OUTPRE)krb5.lst
##WIN16##LIBNAME=..\gssapi.lib

DLL_EXP_TYPE=GSS

ETSRCS= gssapi_err_krb5.c
ETOBJS= $(OUTPRE)gssapi_err_krb5.$(OBJEXT)
ETHDRS= gssapi_err_krb5.h

$(OUTPRE)gssapi_err_krb5.$(OBJEXT): gssapi_err_krb5.c
gssapi_err_krb5.h: gssapi_err_krb5.et
gssapi_err_krb5.c: gssapi_err_krb5.et

SRCS = \
	$(srcdir)/accept_sec_context.c \
	$(srcdir)/acquire_cred.c \
	$(srcdir)/add_cred.c \
	$(srcdir)/canon_name.c \
	$(srcdir)/compare_name.c \
	$(srcdir)/context_time.c \
	$(srcdir)/copy_ccache.c \
	$(srcdir)/delete_sec_context.c \
	$(srcdir)/disp_name.c \
	$(srcdir)/disp_status.c \
	$(srcdir)/duplicate_name.c \
	$(srcdir)/export_name.c \
	$(srcdir)/export_sec_context.c \
	$(srcdir)/get_tkt_flags.c \
	$(srcdir)/gssapi_krb5.c \
	$(srcdir)/import_name.c \
	$(srcdir)/import_sec_context.c \
	$(srcdir)/indicate_mechs.c \
	$(srcdir)/init_sec_context.c \
	$(srcdir)/inq_context.c \
	$(srcdir)/inq_cred.c \
	$(srcdir)/inq_names.c \
	$(srcdir)/k5seal.c \
	$(srcdir)/k5unseal.c \
	$(srcdir)/krb5_gss_glue.c \
	$(srcdir)/process_context_token.c \
	$(srcdir)/rel_cred.c \
	$(srcdir)/rel_oid.c \
	$(srcdir)/rel_name.c \
	$(srcdir)/seal.c \
	$(srcdir)/ser_sctx.c \
	$(srcdir)/set_ccache.c \
	$(srcdir)/sign.c \
	$(srcdir)/unseal.c \
	$(srcdir)/util_cksum.c \
	$(srcdir)/util_crypt.c \
	$(srcdir)/util_seed.c \
	$(srcdir)/util_seqnum.c \
	$(srcdir)/val_cred.c \
	$(srcdir)/verify.c \
	$(srcdir)/wrap_size_limit.c \
	gssapi_err_krb5.c

#	$(srcdir)/pname_to_uid.c \
#	$(srcdir)/k5mech.c

OBJS = \
	$(OUTPRE)accept_sec_context.$(OBJEXT) \
	$(OUTPRE)acquire_cred.$(OBJEXT) \
	$(OUTPRE)add_cred.$(OBJEXT) \
	$(OUTPRE)canon_name.$(OBJEXT) \
	$(OUTPRE)compare_name.$(OBJEXT) \
	$(OUTPRE)context_time.$(OBJEXT) \
	$(OUTPRE)copy_ccache.$(OBJEXT) \
	$(OUTPRE)delete_sec_context.$(OBJEXT) \
	$(OUTPRE)disp_name.$(OBJEXT) \
	$(OUTPRE)disp_status.$(OBJEXT) \
	$(OUTPRE)duplicate_name.$(OBJEXT) \
	$(OUTPRE)export_name.$(OBJEXT) \
	$(OUTPRE)export_sec_context.$(OBJEXT) \
	$(OUTPRE)get_tkt_flags.$(OBJEXT) \
	$(OUTPRE)gssapi_krb5.$(OBJEXT) \
	$(OUTPRE)import_name.$(OBJEXT) \
	$(OUTPRE)import_sec_context.$(OBJEXT) \
	$(OUTPRE)indicate_mechs.$(OBJEXT) \
	$(OUTPRE)init_sec_context.$(OBJEXT) \
	$(OUTPRE)inq_context.$(OBJEXT) \
	$(OUTPRE)inq_cred.$(OBJEXT) \
	$(OUTPRE)inq_names.$(OBJEXT) \
	$(OUTPRE)k5seal.$(OBJEXT) \
	$(OUTPRE)k5unseal.$(OBJEXT) \
	$(OUTPRE)krb5_gss_glue.$(OBJEXT) \
	$(OUTPRE)process_context_token.$(OBJEXT) \
	$(OUTPRE)rel_cred.$(OBJEXT) \
	$(OUTPRE)rel_oid.$(OBJEXT) \
	$(OUTPRE)rel_name.$(OBJEXT) \
	$(OUTPRE)seal.$(OBJEXT) \
	$(OUTPRE)ser_sctx.$(OBJEXT) \
	$(OUTPRE)set_ccache.$(OBJEXT) \
	$(OUTPRE)sign.$(OBJEXT) \
	$(OUTPRE)unseal.$(OBJEXT) \
	$(OUTPRE)util_cksum.$(OBJEXT) \
	$(OUTPRE)util_crypt.$(OBJEXT) \
	$(OUTPRE)util_seed.$(OBJEXT) \
	$(OUTPRE)util_seqnum.$(OBJEXT) \
	$(OUTPRE)val_cred.$(OBJEXT) \
	$(OUTPRE)verify.$(OBJEXT) \
	$(OUTPRE)wrap_size_limit.$(OBJEXT) \
	$(OUTPRE)gssapi_err_krb5.$(OBJEXT)

#	k5mech.$(OBJEXT) \
#	pname_to_uid.$(OBJEXT)

STLIBOBJS = \
	accept_sec_context.o \
	acquire_cred.o \
	add_cred.o \
	canon_name.o \
	compare_name.o \
	context_time.o \
	copy_ccache.o \
	delete_sec_context.o \
	disp_name.o \
	disp_status.o \
	duplicate_name.o \
	export_name.o \
	export_sec_context.o \
	get_tkt_flags.o \
	gssapi_krb5.o \
	import_name.o \
	import_sec_context.o \
	indicate_mechs.o \
	init_sec_context.o \
	inq_context.o \
	inq_cred.o \
	inq_names.o \
	k5seal.o \
	k5unseal.o \
	krb5_gss_glue.o \
	process_context_token.o \
	rel_cred.o \
	rel_oid.o \
	rel_name.o \
	seal.o \
	ser_sctx.o \
	set_ccache.o \
	sign.o \
	unseal.o \
	util_cksum.o \
	util_crypt.o \
	util_seed.o \
	util_seqnum.o \
	val_cred.o \
	verify.o \
	wrap_size_limit.o \
	gssapi_err_krb5.o

#	k5mech.o \
#	pname_to_uid.o

HDRS= $(ETHDRS)

EHDRDIR=$(BUILDTOP)$(S)include$(S)gssapi
EXPORTED_HEADERS= gssapi_krb5.h

LIBOBJS = $(OBJS)

GSSAPI_KRB5_HDR=$(EHDRDIR)$(S)gssapi_krb5.h

all-windows:: $(EHDRDIR) $(GSSAPI_KRB5_HDR) $(SRCS) $(HDRS)

$(EHDRDIR):
	mkdir $(EHDRDIR)

MK_EHDRDIR=if test -d $(EHDRDIR); then :; else (set -x; mkdir $(EHDRDIR)); fi
MK_EHDRDIR=rem

$(GSSAPI_KRB5_HDR): gssapi_krb5.h
	@$(MK_EHDRDIR)
	$(CP) $(srcdir)$(S)gssapi_krb5.h "$@"

all-unix:: $(SRCS) $(HDRS) $(GSSAPI_KRB5_HDR) includes
all-unix:: all-libobjs

clean-unix::
	$(RM) $(BUILDTOP)/include/gssapi/gssapi_krb5.h

clean-unix:: clean-libobjs
	$(RM) $(ETHDRS) $(ETSRCS)

clean-windows::
	$(RM) $(EHDRDIR)\gssapi_krb5.h
	-if exist $(EHDRDIR)\nul rmdir $(EHDRDIR)

install-headers-unix install::
	@set -x; for f in $(EXPORTED_HEADERS) ; \
	do $(INSTALL_DATA) $(srcdir)/$$f	\
		$(DESTDIR)$(KRB5_INCDIR)/gssapi/$$f ; \
	done

depend:: $(ETSRCS)

includes:: 

install::

# @libobj_frag@

# +++ Dependency line eater +++
# 
# Makefile dependencies follow.  This must be the last section in
# the Makefile.in file
#
accept_sec_context.so accept_sec_context.po $(OUTPRE)accept_sec_context.$(OBJEXT): accept_sec_context.c \
  $(SRCTOP)/include/k5-int.h $(BUILDTOP)/include/krb5/osconf.h \
  $(BUILDTOP)/include/krb5/autoconf.h $(BUILDTOP)/include/krb5.h \
  $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h $(SRCTOP)/include/port-sockets.h \
  $(SRCTOP)/include/socket-utils.h $(SRCTOP)/include/krb5/kdb.h \
  gssapiP_krb5.h $(srcdir)/../generic/gssapiP_generic.h \
  $(srcdir)/../generic/gssapi_generic.h $(BUILDTOP)/include/gssapi/gssapi.h \
  ../generic/gssapi_err_generic.h gssapi_krb5.h gssapi_err_krb5.h
acquire_cred.so acquire_cred.po $(OUTPRE)acquire_cred.$(OBJEXT): acquire_cred.c $(SRCTOP)/include/k5-int.h \
  $(BUILDTOP)/include/krb5/osconf.h $(BUILDTOP)/include/krb5/autoconf.h \
  $(BUILDTOP)/include/krb5.h $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h \
  $(SRCTOP)/include/port-sockets.h $(SRCTOP)/include/socket-utils.h \
  $(SRCTOP)/include/krb5/kdb.h gssapiP_krb5.h $(srcdir)/../generic/gssapiP_generic.h \
  $(srcdir)/../generic/gssapi_generic.h $(BUILDTOP)/include/gssapi/gssapi.h \
  ../generic/gssapi_err_generic.h gssapi_krb5.h gssapi_err_krb5.h
add_cred.so add_cred.po $(OUTPRE)add_cred.$(OBJEXT): add_cred.c gssapiP_krb5.h \
  $(BUILDTOP)/include/krb5.h $(COM_ERR_DEPS) $(srcdir)/../generic/gssapiP_generic.h \
  $(srcdir)/../generic/gssapi_generic.h $(BUILDTOP)/include/gssapi/gssapi.h \
  ../generic/gssapi_err_generic.h gssapi_krb5.h gssapi_err_krb5.h
canon_name.so canon_name.po $(OUTPRE)canon_name.$(OBJEXT): canon_name.c gssapiP_krb5.h \
  $(BUILDTOP)/include/krb5.h $(COM_ERR_DEPS) $(srcdir)/../generic/gssapiP_generic.h \
  $(srcdir)/../generic/gssapi_generic.h $(BUILDTOP)/include/gssapi/gssapi.h \
  ../generic/gssapi_err_generic.h gssapi_krb5.h gssapi_err_krb5.h
compare_name.so compare_name.po $(OUTPRE)compare_name.$(OBJEXT): compare_name.c gssapiP_krb5.h \
  $(BUILDTOP)/include/krb5.h $(COM_ERR_DEPS) $(srcdir)/../generic/gssapiP_generic.h \
  $(srcdir)/../generic/gssapi_generic.h $(BUILDTOP)/include/gssapi/gssapi.h \
  ../generic/gssapi_err_generic.h gssapi_krb5.h gssapi_err_krb5.h
context_time.so context_time.po $(OUTPRE)context_time.$(OBJEXT): context_time.c gssapiP_krb5.h \
  $(BUILDTOP)/include/krb5.h $(COM_ERR_DEPS) $(srcdir)/../generic/gssapiP_generic.h \
  $(srcdir)/../generic/gssapi_generic.h $(BUILDTOP)/include/gssapi/gssapi.h \
  ../generic/gssapi_err_generic.h gssapi_krb5.h gssapi_err_krb5.h
copy_ccache.so copy_ccache.po $(OUTPRE)copy_ccache.$(OBJEXT): copy_ccache.c gssapiP_krb5.h \
  $(BUILDTOP)/include/krb5.h $(COM_ERR_DEPS) $(srcdir)/../generic/gssapiP_generic.h \
  $(srcdir)/../generic/gssapi_generic.h $(BUILDTOP)/include/gssapi/gssapi.h \
  ../generic/gssapi_err_generic.h gssapi_krb5.h gssapi_err_krb5.h
delete_sec_context.so delete_sec_context.po $(OUTPRE)delete_sec_context.$(OBJEXT): delete_sec_context.c \
  gssapiP_krb5.h $(BUILDTOP)/include/krb5.h $(COM_ERR_DEPS) \
  $(srcdir)/../generic/gssapiP_generic.h $(srcdir)/../generic/gssapi_generic.h \
  $(BUILDTOP)/include/gssapi/gssapi.h ../generic/gssapi_err_generic.h \
  gssapi_krb5.h gssapi_err_krb5.h
disp_name.so disp_name.po $(OUTPRE)disp_name.$(OBJEXT): disp_name.c gssapiP_krb5.h \
  $(BUILDTOP)/include/krb5.h $(COM_ERR_DEPS) $(srcdir)/../generic/gssapiP_generic.h \
  $(srcdir)/../generic/gssapi_generic.h $(BUILDTOP)/include/gssapi/gssapi.h \
  ../generic/gssapi_err_generic.h gssapi_krb5.h gssapi_err_krb5.h
disp_status.so disp_status.po $(OUTPRE)disp_status.$(OBJEXT): disp_status.c gssapiP_krb5.h \
  $(BUILDTOP)/include/krb5.h $(COM_ERR_DEPS) $(srcdir)/../generic/gssapiP_generic.h \
  $(srcdir)/../generic/gssapi_generic.h $(BUILDTOP)/include/gssapi/gssapi.h \
  ../generic/gssapi_err_generic.h gssapi_krb5.h gssapi_err_krb5.h
duplicate_name.so duplicate_name.po $(OUTPRE)duplicate_name.$(OBJEXT): duplicate_name.c \
  gssapiP_krb5.h $(BUILDTOP)/include/krb5.h $(COM_ERR_DEPS) \
  $(srcdir)/../generic/gssapiP_generic.h $(srcdir)/../generic/gssapi_generic.h \
  $(BUILDTOP)/include/gssapi/gssapi.h ../generic/gssapi_err_generic.h \
  gssapi_krb5.h gssapi_err_krb5.h
export_name.so export_name.po $(OUTPRE)export_name.$(OBJEXT): export_name.c gssapiP_krb5.h \
  $(BUILDTOP)/include/krb5.h $(COM_ERR_DEPS) $(srcdir)/../generic/gssapiP_generic.h \
  $(srcdir)/../generic/gssapi_generic.h $(BUILDTOP)/include/gssapi/gssapi.h \
  ../generic/gssapi_err_generic.h gssapi_krb5.h gssapi_err_krb5.h
export_sec_context.so export_sec_context.po $(OUTPRE)export_sec_context.$(OBJEXT): export_sec_context.c \
  gssapiP_krb5.h $(BUILDTOP)/include/krb5.h $(COM_ERR_DEPS) \
  $(srcdir)/../generic/gssapiP_generic.h $(srcdir)/../generic/gssapi_generic.h \
  $(BUILDTOP)/include/gssapi/gssapi.h ../generic/gssapi_err_generic.h \
  gssapi_krb5.h gssapi_err_krb5.h
get_tkt_flags.so get_tkt_flags.po $(OUTPRE)get_tkt_flags.$(OBJEXT): get_tkt_flags.c gssapiP_krb5.h \
  $(BUILDTOP)/include/krb5.h $(COM_ERR_DEPS) $(srcdir)/../generic/gssapiP_generic.h \
  $(srcdir)/../generic/gssapi_generic.h $(BUILDTOP)/include/gssapi/gssapi.h \
  ../generic/gssapi_err_generic.h gssapi_krb5.h gssapi_err_krb5.h
gssapi_krb5.so gssapi_krb5.po $(OUTPRE)gssapi_krb5.$(OBJEXT): gssapi_krb5.c $(SRCTOP)/include/k5-int.h \
  $(BUILDTOP)/include/krb5/osconf.h $(BUILDTOP)/include/krb5/autoconf.h \
  $(BUILDTOP)/include/krb5.h $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h \
  $(SRCTOP)/include/port-sockets.h $(SRCTOP)/include/socket-utils.h \
  $(SRCTOP)/include/krb5/kdb.h gssapiP_krb5.h $(srcdir)/../generic/gssapiP_generic.h \
  $(srcdir)/../generic/gssapi_generic.h $(BUILDTOP)/include/gssapi/gssapi.h \
  ../generic/gssapi_err_generic.h gssapi_krb5.h gssapi_err_krb5.h
import_name.so import_name.po $(OUTPRE)import_name.$(OBJEXT): import_name.c gssapiP_krb5.h \
  $(BUILDTOP)/include/krb5.h $(COM_ERR_DEPS) $(srcdir)/../generic/gssapiP_generic.h \
  $(srcdir)/../generic/gssapi_generic.h $(BUILDTOP)/include/gssapi/gssapi.h \
  ../generic/gssapi_err_generic.h gssapi_krb5.h gssapi_err_krb5.h
import_sec_context.so import_sec_context.po $(OUTPRE)import_sec_context.$(OBJEXT): import_sec_context.c \
  gssapiP_krb5.h $(BUILDTOP)/include/krb5.h $(COM_ERR_DEPS) \
  $(srcdir)/../generic/gssapiP_generic.h $(srcdir)/../generic/gssapi_generic.h \
  $(BUILDTOP)/include/gssapi/gssapi.h ../generic/gssapi_err_generic.h \
  gssapi_krb5.h gssapi_err_krb5.h
indicate_mechs.so indicate_mechs.po $(OUTPRE)indicate_mechs.$(OBJEXT): indicate_mechs.c \
  gssapiP_krb5.h $(BUILDTOP)/include/krb5.h $(COM_ERR_DEPS) \
  $(srcdir)/../generic/gssapiP_generic.h $(srcdir)/../generic/gssapi_generic.h \
  $(BUILDTOP)/include/gssapi/gssapi.h ../generic/gssapi_err_generic.h \
  gssapi_krb5.h gssapi_err_krb5.h
init_sec_context.so init_sec_context.po $(OUTPRE)init_sec_context.$(OBJEXT): init_sec_context.c \
  gssapiP_krb5.h $(BUILDTOP)/include/krb5.h $(COM_ERR_DEPS) \
  $(srcdir)/../generic/gssapiP_generic.h $(srcdir)/../generic/gssapi_generic.h \
  $(BUILDTOP)/include/gssapi/gssapi.h ../generic/gssapi_err_generic.h \
  gssapi_krb5.h gssapi_err_krb5.h
inq_context.so inq_context.po $(OUTPRE)inq_context.$(OBJEXT): inq_context.c gssapiP_krb5.h \
  $(BUILDTOP)/include/krb5.h $(COM_ERR_DEPS) $(srcdir)/../generic/gssapiP_generic.h \
  $(srcdir)/../generic/gssapi_generic.h $(BUILDTOP)/include/gssapi/gssapi.h \
  ../generic/gssapi_err_generic.h gssapi_krb5.h gssapi_err_krb5.h
inq_cred.so inq_cred.po $(OUTPRE)inq_cred.$(OBJEXT): inq_cred.c gssapiP_krb5.h \
  $(BUILDTOP)/include/krb5.h $(COM_ERR_DEPS) $(srcdir)/../generic/gssapiP_generic.h \
  $(srcdir)/../generic/gssapi_generic.h $(BUILDTOP)/include/gssapi/gssapi.h \
  ../generic/gssapi_err_generic.h gssapi_krb5.h gssapi_err_krb5.h
inq_names.so inq_names.po $(OUTPRE)inq_names.$(OBJEXT): inq_names.c gssapiP_krb5.h \
  $(BUILDTOP)/include/krb5.h $(COM_ERR_DEPS) $(srcdir)/../generic/gssapiP_generic.h \
  $(srcdir)/../generic/gssapi_generic.h $(BUILDTOP)/include/gssapi/gssapi.h \
  ../generic/gssapi_err_generic.h gssapi_krb5.h gssapi_err_krb5.h
k5seal.so k5seal.po $(OUTPRE)k5seal.$(OBJEXT): k5seal.c gssapiP_krb5.h \
  $(BUILDTOP)/include/krb5.h $(COM_ERR_DEPS) $(srcdir)/../generic/gssapiP_generic.h \
  $(srcdir)/../generic/gssapi_generic.h $(BUILDTOP)/include/gssapi/gssapi.h \
  ../generic/gssapi_err_generic.h gssapi_krb5.h gssapi_err_krb5.h
k5unseal.so k5unseal.po $(OUTPRE)k5unseal.$(OBJEXT): k5unseal.c gssapiP_krb5.h \
  $(BUILDTOP)/include/krb5.h $(COM_ERR_DEPS) $(srcdir)/../generic/gssapiP_generic.h \
  $(srcdir)/../generic/gssapi_generic.h $(BUILDTOP)/include/gssapi/gssapi.h \
  ../generic/gssapi_err_generic.h gssapi_krb5.h gssapi_err_krb5.h
krb5_gss_glue.so krb5_gss_glue.po $(OUTPRE)krb5_gss_glue.$(OBJEXT): krb5_gss_glue.c gssapiP_krb5.h \
  $(BUILDTOP)/include/krb5.h $(COM_ERR_DEPS) $(srcdir)/../generic/gssapiP_generic.h \
  $(srcdir)/../generic/gssapi_generic.h $(BUILDTOP)/include/gssapi/gssapi.h \
  ../generic/gssapi_err_generic.h gssapi_krb5.h gssapi_err_krb5.h
process_context_token.so process_context_token.po $(OUTPRE)process_context_token.$(OBJEXT): process_context_token.c \
  gssapiP_krb5.h $(BUILDTOP)/include/krb5.h $(COM_ERR_DEPS) \
  $(srcdir)/../generic/gssapiP_generic.h $(srcdir)/../generic/gssapi_generic.h \
  $(BUILDTOP)/include/gssapi/gssapi.h ../generic/gssapi_err_generic.h \
  gssapi_krb5.h gssapi_err_krb5.h
rel_cred.so rel_cred.po $(OUTPRE)rel_cred.$(OBJEXT): rel_cred.c gssapiP_krb5.h \
  $(BUILDTOP)/include/krb5.h $(COM_ERR_DEPS) $(srcdir)/../generic/gssapiP_generic.h \
  $(srcdir)/../generic/gssapi_generic.h $(BUILDTOP)/include/gssapi/gssapi.h \
  ../generic/gssapi_err_generic.h gssapi_krb5.h gssapi_err_krb5.h
rel_oid.so rel_oid.po $(OUTPRE)rel_oid.$(OBJEXT): rel_oid.c gssapiP_krb5.h \
  $(BUILDTOP)/include/krb5.h $(COM_ERR_DEPS) $(srcdir)/../generic/gssapiP_generic.h \
  $(srcdir)/../generic/gssapi_generic.h $(BUILDTOP)/include/gssapi/gssapi.h \
  ../generic/gssapi_err_generic.h gssapi_krb5.h gssapi_err_krb5.h
rel_name.so rel_name.po $(OUTPRE)rel_name.$(OBJEXT): rel_name.c gssapiP_krb5.h \
  $(BUILDTOP)/include/krb5.h $(COM_ERR_DEPS) $(srcdir)/../generic/gssapiP_generic.h \
  $(srcdir)/../generic/gssapi_generic.h $(BUILDTOP)/include/gssapi/gssapi.h \
  ../generic/gssapi_err_generic.h gssapi_krb5.h gssapi_err_krb5.h
seal.so seal.po $(OUTPRE)seal.$(OBJEXT): seal.c gssapiP_krb5.h $(BUILDTOP)/include/krb5.h \
  $(COM_ERR_DEPS) $(srcdir)/../generic/gssapiP_generic.h \
  $(srcdir)/../generic/gssapi_generic.h $(BUILDTOP)/include/gssapi/gssapi.h \
  ../generic/gssapi_err_generic.h gssapi_krb5.h gssapi_err_krb5.h
ser_sctx.so ser_sctx.po $(OUTPRE)ser_sctx.$(OBJEXT): ser_sctx.c $(SRCTOP)/include/k5-int.h \
  $(BUILDTOP)/include/krb5/osconf.h $(BUILDTOP)/include/krb5/autoconf.h \
  $(BUILDTOP)/include/krb5.h $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h \
  $(SRCTOP)/include/port-sockets.h $(SRCTOP)/include/socket-utils.h \
  $(SRCTOP)/include/krb5/kdb.h gssapiP_krb5.h $(srcdir)/../generic/gssapiP_generic.h \
  $(srcdir)/../generic/gssapi_generic.h $(BUILDTOP)/include/gssapi/gssapi.h \
  ../generic/gssapi_err_generic.h gssapi_krb5.h gssapi_err_krb5.h
set_ccache.so set_ccache.po $(OUTPRE)set_ccache.$(OBJEXT): set_ccache.c gssapiP_krb5.h \
  $(BUILDTOP)/include/krb5.h $(COM_ERR_DEPS) $(srcdir)/../generic/gssapiP_generic.h \
  $(srcdir)/../generic/gssapi_generic.h $(BUILDTOP)/include/gssapi/gssapi.h \
  ../generic/gssapi_err_generic.h gssapi_krb5.h gssapi_err_krb5.h
sign.so sign.po $(OUTPRE)sign.$(OBJEXT): sign.c gssapiP_krb5.h $(BUILDTOP)/include/krb5.h \
  $(COM_ERR_DEPS) $(srcdir)/../generic/gssapiP_generic.h \
  $(srcdir)/../generic/gssapi_generic.h $(BUILDTOP)/include/gssapi/gssapi.h \
  ../generic/gssapi_err_generic.h gssapi_krb5.h gssapi_err_krb5.h
unseal.so unseal.po $(OUTPRE)unseal.$(OBJEXT): unseal.c gssapiP_krb5.h \
  $(BUILDTOP)/include/krb5.h $(COM_ERR_DEPS) $(srcdir)/../generic/gssapiP_generic.h \
  $(srcdir)/../generic/gssapi_generic.h $(BUILDTOP)/include/gssapi/gssapi.h \
  ../generic/gssapi_err_generic.h gssapi_krb5.h gssapi_err_krb5.h
util_cksum.so util_cksum.po $(OUTPRE)util_cksum.$(OBJEXT): util_cksum.c gssapiP_krb5.h \
  $(BUILDTOP)/include/krb5.h $(COM_ERR_DEPS) $(srcdir)/../generic/gssapiP_generic.h \
  $(srcdir)/../generic/gssapi_generic.h $(BUILDTOP)/include/gssapi/gssapi.h \
  ../generic/gssapi_err_generic.h gssapi_krb5.h gssapi_err_krb5.h
util_crypt.so util_crypt.po $(OUTPRE)util_crypt.$(OBJEXT): util_crypt.c $(SRCTOP)/include/k5-int.h \
  $(BUILDTOP)/include/krb5/osconf.h $(BUILDTOP)/include/krb5/autoconf.h \
  $(BUILDTOP)/include/krb5.h $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h \
  $(SRCTOP)/include/port-sockets.h $(SRCTOP)/include/socket-utils.h \
  $(SRCTOP)/include/krb5/kdb.h gssapiP_krb5.h $(srcdir)/../generic/gssapiP_generic.h \
  $(srcdir)/../generic/gssapi_generic.h $(BUILDTOP)/include/gssapi/gssapi.h \
  ../generic/gssapi_err_generic.h gssapi_krb5.h gssapi_err_krb5.h
util_seed.so util_seed.po $(OUTPRE)util_seed.$(OBJEXT): util_seed.c gssapiP_krb5.h \
  $(BUILDTOP)/include/krb5.h $(COM_ERR_DEPS) $(srcdir)/../generic/gssapiP_generic.h \
  $(srcdir)/../generic/gssapi_generic.h $(BUILDTOP)/include/gssapi/gssapi.h \
  ../generic/gssapi_err_generic.h gssapi_krb5.h gssapi_err_krb5.h
util_seqnum.so util_seqnum.po $(OUTPRE)util_seqnum.$(OBJEXT): util_seqnum.c gssapiP_krb5.h \
  $(BUILDTOP)/include/krb5.h $(COM_ERR_DEPS) $(srcdir)/../generic/gssapiP_generic.h \
  $(srcdir)/../generic/gssapi_generic.h $(BUILDTOP)/include/gssapi/gssapi.h \
  ../generic/gssapi_err_generic.h gssapi_krb5.h gssapi_err_krb5.h \
  $(SRCTOP)/include/k5-int.h $(BUILDTOP)/include/krb5/osconf.h \
  $(BUILDTOP)/include/krb5/autoconf.h $(BUILDTOP)/include/profile.h \
  $(SRCTOP)/include/port-sockets.h $(SRCTOP)/include/socket-utils.h \
  $(SRCTOP)/include/krb5/kdb.h
val_cred.so val_cred.po $(OUTPRE)val_cred.$(OBJEXT): val_cred.c gssapiP_krb5.h \
  $(BUILDTOP)/include/krb5.h $(COM_ERR_DEPS) $(srcdir)/../generic/gssapiP_generic.h \
  $(srcdir)/../generic/gssapi_generic.h $(BUILDTOP)/include/gssapi/gssapi.h \
  ../generic/gssapi_err_generic.h gssapi_krb5.h gssapi_err_krb5.h
verify.so verify.po $(OUTPRE)verify.$(OBJEXT): verify.c gssapiP_krb5.h \
  $(BUILDTOP)/include/krb5.h $(COM_ERR_DEPS) $(srcdir)/../generic/gssapiP_generic.h \
  $(srcdir)/../generic/gssapi_generic.h $(BUILDTOP)/include/gssapi/gssapi.h \
  ../generic/gssapi_err_generic.h gssapi_krb5.h gssapi_err_krb5.h
wrap_size_limit.so wrap_size_limit.po $(OUTPRE)wrap_size_limit.$(OBJEXT): wrap_size_limit.c \
  gssapiP_krb5.h $(BUILDTOP)/include/krb5.h $(COM_ERR_DEPS) \
  $(srcdir)/../generic/gssapiP_generic.h $(srcdir)/../generic/gssapi_generic.h \
  $(BUILDTOP)/include/gssapi/gssapi.h ../generic/gssapi_err_generic.h \
  gssapi_krb5.h gssapi_err_krb5.h
gssapi_err_krb5.so gssapi_err_krb5.po $(OUTPRE)gssapi_err_krb5.$(OBJEXT): gssapi_err_krb5.c \
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
