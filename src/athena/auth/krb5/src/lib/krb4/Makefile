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
myfulldir=lib/krb4
mydir=.
BUILDTOP=$(REL)..$(S)..
LOCALINCLUDES = -I$(BUILDTOP)/include/kerberosIV -I$(srcdir)/../../include/kerberosIV -I.
DEFINES= -DKRB4_USE_KEYTAB

BUILDTOP = ..\..
LIBNAME=$(OUTPRE)krb4.lib
OBJFILE=$(OUTPRE)krb4.lst

LIB=krb4
LIBMAJOR=2
LIBMINOR=0
RELDIR=krb4

# Depends on libk5crypto, libkrb5, KRB4_CRYPTO_LIB and _et_list...
# Depends on libkrb5, expect to find
#	krb5_init_context, krb5_free_context, profile_get_values
#
KRB4_CRYPTO_LIBS=-ldes425

SHLIB_EXPDEPS = \
	$(TOPLIBD)/libdes425$(SHLIBEXT) \
	$(TOPLIBD)/libk5crypto$(SHLIBEXT) \
	$(TOPLIBD)/libkrb5$(SHLIBEXT)
SHLIB_EXPLIBS=-lkrb5 -lcom_err -ldes425 -lk5crypto
SHLIB_DIRS=-L$(TOPLIBD)
SHLIB_RDIRS=$(KRB5_LIBDIR)

EHDRDIR=$(BUILDTOP)$(S)include$(S)kerberosIV
KRB_ERR=@KRB_ERR@
KRB_ERR=$(OUTPRE)krb_err.$(OBJEXT)

# Name of generated krb_err.c, needed for err_txt.* dependency on Darwin.
KRB_ERR_C=@KRB_ERR_C@
KRB_ERR_C=

OBJS	= \
	$(OUTPRE)change_password.$(OBJEXT) \
	$(OUTPRE)cr_auth_repl.$(OBJEXT) \
	$(OUTPRE)cr_ciph.$(OBJEXT) \
	$(OUTPRE)cr_tkt.$(OBJEXT) \
	$(OUTPRE)debug.$(OBJEXT) \
	$(OUTPRE)decomp_tkt.$(OBJEXT) \
	$(OUTPRE)err_txt.$(OBJEXT) \
	$(OUTPRE)g_ad_tkt.$(OBJEXT) \
	$(OUTPRE)g_in_tkt.$(OBJEXT) \
	$(OUTPRE)g_phost.$(OBJEXT) \
	$(OUTPRE)g_pw_in_tkt.$(OBJEXT) \
	$(OUTPRE)g_pw_tkt.$(OBJEXT) \
	$(OUTPRE)g_tkt_svc.$(OBJEXT) \
	$(OUTPRE)gethostname.$(OBJEXT) \
	$(OUTPRE)getst.$(OBJEXT) \
	$(OUTPRE)kadm_err.$(OBJEXT) \
	$(OUTPRE)kadm_net.$(OBJEXT) \
	$(OUTPRE)kadm_stream.$(OBJEXT) \
	$(OUTPRE)kname_parse.$(OBJEXT) \
	$(OUTPRE)lifetime.$(OBJEXT) \
	$(OUTPRE)mk_auth.$(OBJEXT) \
	$(OUTPRE)mk_err.$(OBJEXT) \
	$(OUTPRE)mk_priv.$(OBJEXT) \
	$(OUTPRE)mk_req.$(OBJEXT) \
	$(OUTPRE)mk_safe.$(OBJEXT) \
	$(OUTPRE)month_sname.$(OBJEXT) \
	$(OUTPRE)password_to_key.$(OBJEXT) \
	$(OUTPRE)prot_client.$(OBJEXT) \
	$(OUTPRE)prot_common.$(OBJEXT) \
	$(OUTPRE)prot_kdc.$(OBJEXT) \
	$(OUTPRE)pkt_cipher.$(OBJEXT) \
	$(OUTPRE)pkt_clen.$(OBJEXT) \
	$(OUTPRE)rd_err.$(OBJEXT) \
	$(OUTPRE)rd_priv.$(OBJEXT) \
	$(OUTPRE)rd_safe.$(OBJEXT) \
	$(OUTPRE)send_to_kdc.$(OBJEXT) \
	$(OUTPRE)stime.$(OBJEXT) \
	$(OUTPRE)strnlen.$(OBJEXT) \
	$(OUTPRE)rd_preauth.$(OBJEXT) \
	$(OUTPRE)mk_preauth.$(OBJEXT) \
	$(OSOBJS) $(CACHEOBJS) $(SETENVOBJS) $(STRCASEOBJS) $(SHMOBJS) \
	$(LIB_KRB_HOSTOBJS) $(SERVER_KRB_OBJS) $(NETIO_OBJS) $(REALMDBOBJS) $(KRB_ERR)

SRCS = \
	change_password.c \
	cr_auth_repl.c \
	cr_ciph.c \
	cr_tkt.c \
	debug.c \
	decomp_tkt.c \
	g_ad_tkt.c \
	g_pw_in_tkt.c \
	g_phost.c \
	g_pw_tkt.c \
	g_tkt_svc.c \
	getst.c \
	gethostname.c \
	kadm_err.c \
	kadm_net.c \
	kadm_stream.c \
	kname_parse.c \
	err_txt.c \
	lifetime.c \
	g_in_tkt.c \
	mk_auth.c \
	mk_err.c \
	mk_priv.c \
	mk_req.c \
	mk_safe.c \
	month_sname.c \
	password_to_key.c \
	pkt_cipher.c \
	pkt_clen.c \
	prot_client.c \
	prot_common.c \
	prot_kdc.c \
	rd_err.c \
	rd_priv.c \
	rd_safe.c \
	send_to_kdc.c \
	stime.c \
	strnlen.c \
	rd_preauth.c \
	mk_preauth.c \
	unix_time.c \
	$(OSSRCS) $(CACHESRCS) $(SETENVSRCS) $(STRCASESRCS) $(SHMSRCS) \
	$(LIB_KRB_HOSTSRCS) $(SERVER_KRB_SRCS) $(NETIO_SRCS) $(REALMDBSRCS)

STLIBOBJS  = $(OBJS)
STOBJLISTS=OBJS.ST

#
# These objects implement the time computation routines.
#
OSOBJS = $(OUTPRE)unix_time.$(OBJEXT)
OSSRCS = unix_time.c

OSOBJS = $(OUTPRE)win_time.obj

#
# These objects implement ticket cacheing for Unix.  They are
# replaced by other files when compiling for Windows or Mac.
#
CACHESRCS = \
	tf_util.c	dest_tkt.c	in_tkt.c \
	tkt_string.c	g_tf_fname.c	g_tf_realm.c \
	g_cred.c	save_creds.c
CACHEOBJS = \
	$(OUTPRE)tf_util.$(OBJEXT) $(OUTPRE)dest_tkt.$(OBJEXT) $(OUTPRE)in_tkt.$(OBJEXT) \
	$(OUTPRE)tkt_string.$(OBJEXT) $(OUTPRE)g_tf_fname.$(OBJEXT) $(OUTPRE)g_tf_realm.$(OBJEXT) \
	$(OUTPRE)g_cred.$(OBJEXT) $(OUTPRE)save_creds.$(OBJEXT)

CACHEOBJS = $(OUTPRE)memcache.$(OBJEXT)

#
# These objects implement Kerberos realm<->host database lookup.
# They read config files and/or network databases in various ways
# on various platforms.
#

CNFFILE = g_cnffile
CNFFILE = win_store

REALMDBSRCS=$(CNFFILE).c RealmsConfig-glue.c
REALMDBOBJS=$(OUTPRE)$(CNFFILE).$(OBJEXT) $(OUTPRE)RealmsConfig-glue.$(OBJEXT)

#
# These objects are only used on server or debug implementations of Kerberos,
# and they cause some major or minor sort of trouble for some
# client-only platform (Mac or Windows).
#
SERVER_KRB_SRCS = \
	klog.c		kuserok.c	log.c \
	kntoln.c \
	fgetst.c	rd_svc_key.c	cr_err_repl.c \
	rd_req.c	g_svc_in_tkt.c	recvauth.c \
	ad_print.c	cr_death_pkt.c \
	kparse.c	put_svc_key.c	sendauth.c
SERVER_KRB_OBJS = \
	$(OUTPRE)klog.$(OBJEXT) $(OUTPRE)kuserok.$(OBJEXT) $(OUTPRE)log.$(OBJEXT) \
	$(OUTPRE)kntoln.$(OBJEXT) \
	$(OUTPRE)fgetst.$(OBJEXT) $(OUTPRE)rd_svc_key.$(OBJEXT) $(OUTPRE)cr_err_repl.$(OBJEXT) \
	$(OUTPRE)rd_req.$(OBJEXT) $(OUTPRE)g_svc_in_tkt.$(OBJEXT) $(OUTPRE)recvauth.$(OBJEXT) \
	$(OUTPRE)ad_print.$(OBJEXT) $(OUTPRE)cr_death_pkt.$(OBJEXT) \
	$(OUTPRE)kparse.$(OBJEXT) $(OUTPRE)put_svc_key.$(OBJEXT) $(OUTPRE)sendauth.$(OBJEXT)
#
# These objects are included on Unix and Windows (for kstream and kadm)
# but not under Mac (there are no file descriptors).
#
NETIO_SRCS=netread.c netwrite.c
NETIO_OBJS=$(OUTPRE)netread.$(OBJEXT) $(OUTPRE)netwrite.$(OBJEXT)

#
# These objects glue the Kerberos library to the operating system
# (time-of-day access, etc).  They are replaced in Mac and Windows
# by other _glue.* routines.
#
LIB_KRB_HOSTSRCS=unix_glue.c
LIB_KRB_HOSTOBJS=$(OUTPRE)unix_glue.$(OBJEXT)

LIB_KRB_HOSTOBJS=$(OUTPRE)win_glue.obj

ARCHIVEARGS= $@ $(OBJS)

# We want *library* compiler options...
DBG=$(DBG_LIB)

all-unix:: includes all-liblinks

LIBOBJS = $(OBJS)

# comp_et_depend(krb_err)
krb_err.h: krb_err.et
krb_err.c: krb_err.et

kadm_err.h: kadm_err.et
kadm_err.c: kadm_err.et

GEN_ERRTXT=$(AWK) -f $(srcdir)$(S)et_errtxt.awk outfile=$@

krb_err_txt.c: krb_err.et $(srcdir)$(S)et_errtxt.awk
	$(GEN_ERRTXT) $(srcdir)/krb_err.et

# Will be empty on Darwin, krb_err_txt.c elsewhere.
KRB_ERR_TXT=@KRB_ERR_TXT@
KRB_ERR_TXT=krb_err_txt.c
err_txt.so err_txt.po $(OUTPRE)err_txt.$(OBJEXT): err_txt.c $(KRB_ERR_C) $(KRB_ERR_TXT)

depend-dependencies: krb_err.h $(EHDRDIR)$(S)krb_err.h \
	kadm_err.h $(EHDRDIR)$(S)kadm_err.h \
	krb_err.c

includes: $(EHDRDIR)$(S)krb_err.h $(EHDRDIR)$(S)kadm_err.h

$(EHDRDIR)$(S)krb_err.h: krb_err.h
	$(CP) krb_err.h $@
$(EHDRDIR)$(S)kadm_err.h: kadm_err.h
	$(CP) kadm_err.h $@

clean-unix::
	$(RM) $(EHDRDIR)/krb_err.h
	$(RM) $(EHDRDIR)/kadm_err.h
	$(RM) krb_err_txt.c

clean::
	-$(RM) $(OBJS)

clean-:: clean-unix

clean-unix::
	-$(RM) krb_err.c
	-$(RM) krb_err.h
	-$(RM) kadm_err.c
	-$(RM) kadm_err.h
	-$(RM) ../../include/kerberosIV/krb_err.h
	-$(RM) ../../include/kerberosIV/kadm_err.h

clean-unix:: clean-liblinks clean-libs clean-libobjs


check-unix:: $(TEST_PROGS)
check-mac:: $(TEST_PROGS)
check-windows::


install-unix:: install-libs

# @lib_frag@
# @libobj_frag@

# +++ Dependency line eater +++
# 
# Makefile dependencies follow.  This must be the last section in
# the Makefile.in file
#
change_password.so change_password.po $(OUTPRE)change_password.$(OBJEXT): change_password.c \
  $(SRCTOP)/include/kerberosIV/krb.h $(SRCTOP)/include/kerberosIV/des.h \
  $(KRB_ERR_H_DEP) $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h \
  krb4int.h $(SRCTOP)/include/port-sockets.h $(BUILDTOP)/include/krb5/autoconf.h \
  $(SRCTOP)/include/kerberosIV/kadm.h $(SRCTOP)/include/kerberosIV/prot.h
cr_auth_repl.so cr_auth_repl.po $(OUTPRE)cr_auth_repl.$(OBJEXT): cr_auth_repl.c $(SRCTOP)/include/kerberosIV/krb.h \
  $(SRCTOP)/include/kerberosIV/des.h $(KRB_ERR_H_DEP) \
  $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h $(SRCTOP)/include/kerberosIV/prot.h
cr_ciph.so cr_ciph.po $(OUTPRE)cr_ciph.$(OBJEXT): cr_ciph.c $(SRCTOP)/include/kerberosIV/krb.h \
  $(SRCTOP)/include/kerberosIV/des.h $(KRB_ERR_H_DEP) \
  $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h $(SRCTOP)/include/kerberosIV/prot.h
cr_tkt.so cr_tkt.po $(OUTPRE)cr_tkt.$(OBJEXT): cr_tkt.c $(BUILDTOP)/include/krb5.h \
  $(COM_ERR_DEPS) $(SRCTOP)/include/kerberosIV/des.h \
  $(SRCTOP)/include/kerberosIV/krb.h $(KRB_ERR_H_DEP) \
  $(BUILDTOP)/include/profile.h $(SRCTOP)/include/kerberosIV/prot.h \
  $(SRCTOP)/include/port-sockets.h $(BUILDTOP)/include/krb5/autoconf.h
debug.so debug.po $(OUTPRE)debug.$(OBJEXT): debug.c $(SRCTOP)/include/kerberosIV/mit-copyright.h
decomp_tkt.so decomp_tkt.po $(OUTPRE)decomp_tkt.$(OBJEXT): decomp_tkt.c $(SRCTOP)/include/kerberosIV/des.h \
  $(SRCTOP)/include/kerberosIV/krb.h $(KRB_ERR_H_DEP) \
  $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h $(SRCTOP)/include/kerberosIV/prot.h \
  $(BUILDTOP)/include/krb5.h $(SRCTOP)/include/krb54proto.h \
  $(SRCTOP)/include/port-sockets.h $(BUILDTOP)/include/krb5/autoconf.h
g_ad_tkt.so g_ad_tkt.po $(OUTPRE)g_ad_tkt.$(OBJEXT): g_ad_tkt.c $(SRCTOP)/include/kerberosIV/krb.h \
  $(SRCTOP)/include/kerberosIV/des.h $(KRB_ERR_H_DEP) \
  $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h krb4int.h \
  $(SRCTOP)/include/port-sockets.h $(BUILDTOP)/include/krb5/autoconf.h \
  $(SRCTOP)/include/kerberosIV/prot.h
g_pw_in_tkt.so g_pw_in_tkt.po $(OUTPRE)g_pw_in_tkt.$(OBJEXT): g_pw_in_tkt.c $(BUILDTOP)/include/krb5.h \
  $(COM_ERR_DEPS) $(SRCTOP)/include/kerberosIV/krb.h \
  $(SRCTOP)/include/kerberosIV/des.h $(KRB_ERR_H_DEP) \
  $(BUILDTOP)/include/profile.h krb4int.h $(SRCTOP)/include/port-sockets.h \
  $(BUILDTOP)/include/krb5/autoconf.h $(SRCTOP)/include/kerberosIV/prot.h
g_phost.so g_phost.po $(OUTPRE)g_phost.$(OBJEXT): g_phost.c $(SRCTOP)/include/kerberosIV/krb.h \
  $(SRCTOP)/include/kerberosIV/des.h $(KRB_ERR_H_DEP) \
  $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h $(SRCTOP)/include/port-sockets.h \
  $(BUILDTOP)/include/krb5/autoconf.h
g_pw_tkt.so g_pw_tkt.po $(OUTPRE)g_pw_tkt.$(OBJEXT): g_pw_tkt.c $(SRCTOP)/include/kerberosIV/mit-copyright.h \
  $(SRCTOP)/include/kerberosIV/krb.h $(SRCTOP)/include/kerberosIV/des.h \
  $(KRB_ERR_H_DEP) $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h
g_tkt_svc.so g_tkt_svc.po $(OUTPRE)g_tkt_svc.$(OBJEXT): g_tkt_svc.c $(SRCTOP)/include/kerberosIV/krb.h \
  $(SRCTOP)/include/kerberosIV/des.h $(KRB_ERR_H_DEP) \
  $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h $(SRCTOP)/include/port-sockets.h \
  $(BUILDTOP)/include/krb5/autoconf.h
getst.so getst.po $(OUTPRE)getst.$(OBJEXT): getst.c $(SRCTOP)/include/kerberosIV/mit-copyright.h \
  $(SRCTOP)/include/kerberosIV/krb.h $(SRCTOP)/include/kerberosIV/des.h \
  $(KRB_ERR_H_DEP) $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h \
  krb4int.h $(SRCTOP)/include/port-sockets.h $(BUILDTOP)/include/krb5/autoconf.h
gethostname.so gethostname.po $(OUTPRE)gethostname.$(OBJEXT): gethostname.c $(SRCTOP)/include/kerberosIV/mit-copyright.h \
  $(SRCTOP)/include/kerberosIV/krb.h $(SRCTOP)/include/kerberosIV/des.h \
  $(KRB_ERR_H_DEP) $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h \
  krb4int.h $(SRCTOP)/include/port-sockets.h $(BUILDTOP)/include/krb5/autoconf.h
kadm_err.so kadm_err.po $(OUTPRE)kadm_err.$(OBJEXT): kadm_err.c $(COM_ERR_DEPS)
kadm_net.so kadm_net.po $(OUTPRE)kadm_net.$(OBJEXT): kadm_net.c $(BUILDTOP)/include/krb5/autoconf.h \
  $(SRCTOP)/include/port-sockets.h $(SRCTOP)/include/kerberosIV/krb.h \
  $(SRCTOP)/include/kerberosIV/des.h $(KRB_ERR_H_DEP) \
  $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h $(SRCTOP)/include/kerberosIV/krbports.h \
  $(SRCTOP)/include/kerberosIV/kadm.h $(BUILDTOP)/include/kerberosIV/kadm_err.h \
  $(SRCTOP)/include/kerberosIV/prot.h
kadm_stream.so kadm_stream.po $(OUTPRE)kadm_stream.$(OBJEXT): kadm_stream.c $(SRCTOP)/include/kerberosIV/kadm.h \
  $(SRCTOP)/include/port-sockets.h $(BUILDTOP)/include/krb5/autoconf.h \
  $(SRCTOP)/include/kerberosIV/krb.h $(SRCTOP)/include/kerberosIV/des.h \
  $(KRB_ERR_H_DEP) $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h \
  $(BUILDTOP)/include/kerberosIV/kadm_err.h $(SRCTOP)/include/kerberosIV/prot.h
kname_parse.so kname_parse.po $(OUTPRE)kname_parse.$(OBJEXT): kname_parse.c $(SRCTOP)/include/kerberosIV/krb.h \
  $(SRCTOP)/include/kerberosIV/des.h $(KRB_ERR_H_DEP) \
  $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h
err_txt.so err_txt.po $(OUTPRE)err_txt.$(OBJEXT): err_txt.c $(SRCTOP)/include/kerberosIV/krb.h \
  $(SRCTOP)/include/kerberosIV/des.h $(KRB_ERR_H_DEP) \
  $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h krb4int.h \
  $(SRCTOP)/include/port-sockets.h $(BUILDTOP)/include/krb5/autoconf.h
lifetime.so lifetime.po $(OUTPRE)lifetime.$(OBJEXT): lifetime.c $(SRCTOP)/include/kerberosIV/krb.h \
  $(SRCTOP)/include/kerberosIV/des.h $(KRB_ERR_H_DEP) \
  $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h $(SRCTOP)/include/k5-int.h \
  $(BUILDTOP)/include/krb5/osconf.h $(BUILDTOP)/include/krb5/autoconf.h \
  $(SRCTOP)/include/k5-platform.h $(BUILDTOP)/include/krb5.h \
  $(SRCTOP)/include/port-sockets.h $(SRCTOP)/include/socket-utils.h \
  $(SRCTOP)/include/krb5/kdb.h
g_in_tkt.so g_in_tkt.po $(OUTPRE)g_in_tkt.$(OBJEXT): g_in_tkt.c $(SRCTOP)/include/kerberosIV/krb.h \
  $(SRCTOP)/include/kerberosIV/des.h $(KRB_ERR_H_DEP) \
  $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h krb4int.h \
  $(SRCTOP)/include/port-sockets.h $(BUILDTOP)/include/krb5/autoconf.h \
  $(SRCTOP)/include/kerberosIV/prot.h
mk_auth.so mk_auth.po $(OUTPRE)mk_auth.$(OBJEXT): mk_auth.c $(SRCTOP)/include/kerberosIV/krb.h \
  $(SRCTOP)/include/kerberosIV/des.h $(KRB_ERR_H_DEP) \
  $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h $(SRCTOP)/include/kerberosIV/prot.h
mk_err.so mk_err.po $(OUTPRE)mk_err.$(OBJEXT): mk_err.c $(SRCTOP)/include/kerberosIV/krb.h \
  $(SRCTOP)/include/kerberosIV/des.h $(KRB_ERR_H_DEP) \
  $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h $(SRCTOP)/include/kerberosIV/prot.h
mk_priv.so mk_priv.po $(OUTPRE)mk_priv.$(OBJEXT): mk_priv.c $(SRCTOP)/include/kerberosIV/krb.h \
  $(SRCTOP)/include/kerberosIV/des.h $(KRB_ERR_H_DEP) \
  $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h $(SRCTOP)/include/kerberosIV/prot.h \
  $(SRCTOP)/include/kerberosIV/lsb_addr_cmp.h $(SRCTOP)/include/kerberosIV/mit-copyright.h \
  $(SRCTOP)/include/port-sockets.h $(BUILDTOP)/include/krb5/autoconf.h
mk_req.so mk_req.po $(OUTPRE)mk_req.$(OBJEXT): mk_req.c $(SRCTOP)/include/kerberosIV/krb.h \
  $(SRCTOP)/include/kerberosIV/des.h $(KRB_ERR_H_DEP) \
  $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h $(SRCTOP)/include/kerberosIV/prot.h \
  krb4int.h $(SRCTOP)/include/port-sockets.h $(BUILDTOP)/include/krb5/autoconf.h
mk_safe.so mk_safe.po $(OUTPRE)mk_safe.$(OBJEXT): mk_safe.c $(SRCTOP)/include/kerberosIV/krb.h \
  $(SRCTOP)/include/kerberosIV/des.h $(KRB_ERR_H_DEP) \
  $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h $(SRCTOP)/include/kerberosIV/prot.h \
  $(SRCTOP)/include/kerberosIV/lsb_addr_cmp.h $(SRCTOP)/include/kerberosIV/mit-copyright.h \
  $(SRCTOP)/include/port-sockets.h $(BUILDTOP)/include/krb5/autoconf.h
month_sname.so month_sname.po $(OUTPRE)month_sname.$(OBJEXT): month_sname.c $(SRCTOP)/include/kerberosIV/krb.h \
  $(SRCTOP)/include/kerberosIV/des.h $(KRB_ERR_H_DEP) \
  $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h krb4int.h \
  $(SRCTOP)/include/port-sockets.h $(BUILDTOP)/include/krb5/autoconf.h
password_to_key.so password_to_key.po $(OUTPRE)password_to_key.$(OBJEXT): password_to_key.c \
  $(SRCTOP)/include/kerberosIV/krb.h $(SRCTOP)/include/kerberosIV/des.h \
  $(KRB_ERR_H_DEP) $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h \
  krb4int.h $(SRCTOP)/include/port-sockets.h $(BUILDTOP)/include/krb5/autoconf.h
pkt_cipher.so pkt_cipher.po $(OUTPRE)pkt_cipher.$(OBJEXT): pkt_cipher.c $(SRCTOP)/include/kerberosIV/mit-copyright.h \
  $(SRCTOP)/include/kerberosIV/krb.h $(SRCTOP)/include/kerberosIV/des.h \
  $(KRB_ERR_H_DEP) $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h \
  $(SRCTOP)/include/kerberosIV/prot.h
pkt_clen.so pkt_clen.po $(OUTPRE)pkt_clen.$(OBJEXT): pkt_clen.c $(SRCTOP)/include/kerberosIV/mit-copyright.h \
  $(SRCTOP)/include/kerberosIV/krb.h $(SRCTOP)/include/kerberosIV/des.h \
  $(KRB_ERR_H_DEP) $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h \
  $(SRCTOP)/include/kerberosIV/prot.h
prot_client.so prot_client.po $(OUTPRE)prot_client.$(OBJEXT): prot_client.c $(SRCTOP)/include/kerberosIV/krb.h \
  $(SRCTOP)/include/kerberosIV/des.h $(KRB_ERR_H_DEP) \
  $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h $(SRCTOP)/include/kerberosIV/prot.h
prot_common.so prot_common.po $(OUTPRE)prot_common.$(OBJEXT): prot_common.c $(SRCTOP)/include/kerberosIV/krb.h \
  $(SRCTOP)/include/kerberosIV/des.h $(KRB_ERR_H_DEP) \
  $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h $(SRCTOP)/include/kerberosIV/prot.h
prot_kdc.so prot_kdc.po $(OUTPRE)prot_kdc.$(OBJEXT): prot_kdc.c $(SRCTOP)/include/kerberosIV/krb.h \
  $(SRCTOP)/include/kerberosIV/des.h $(KRB_ERR_H_DEP) \
  $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h $(SRCTOP)/include/kerberosIV/prot.h \
  $(SRCTOP)/include/port-sockets.h $(BUILDTOP)/include/krb5/autoconf.h
rd_err.so rd_err.po $(OUTPRE)rd_err.$(OBJEXT): rd_err.c $(SRCTOP)/include/kerberosIV/krb.h \
  $(SRCTOP)/include/kerberosIV/des.h $(KRB_ERR_H_DEP) \
  $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h $(SRCTOP)/include/kerberosIV/prot.h
rd_priv.so rd_priv.po $(OUTPRE)rd_priv.$(OBJEXT): rd_priv.c $(SRCTOP)/include/kerberosIV/krb.h \
  $(SRCTOP)/include/kerberosIV/des.h $(KRB_ERR_H_DEP) \
  $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h $(SRCTOP)/include/kerberosIV/prot.h \
  $(SRCTOP)/include/kerberosIV/lsb_addr_cmp.h $(SRCTOP)/include/kerberosIV/mit-copyright.h \
  $(SRCTOP)/include/port-sockets.h $(BUILDTOP)/include/krb5/autoconf.h
rd_safe.so rd_safe.po $(OUTPRE)rd_safe.$(OBJEXT): rd_safe.c $(SRCTOP)/include/kerberosIV/krb.h \
  $(SRCTOP)/include/kerberosIV/des.h $(KRB_ERR_H_DEP) \
  $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h $(SRCTOP)/include/kerberosIV/prot.h \
  $(SRCTOP)/include/kerberosIV/lsb_addr_cmp.h $(SRCTOP)/include/kerberosIV/mit-copyright.h \
  $(SRCTOP)/include/port-sockets.h $(BUILDTOP)/include/krb5/autoconf.h
send_to_kdc.so send_to_kdc.po $(OUTPRE)send_to_kdc.$(OBJEXT): send_to_kdc.c $(SRCTOP)/include/kerberosIV/krb.h \
  $(SRCTOP)/include/kerberosIV/des.h $(KRB_ERR_H_DEP) \
  $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h $(SRCTOP)/include/kerberosIV/krbports.h \
  $(SRCTOP)/include/kerberosIV/prot.h $(BUILDTOP)/include/krb5/autoconf.h \
  $(SRCTOP)/include/port-sockets.h $(SRCTOP)/include/fake-addrinfo.h \
  $(SRCTOP)/include/socket-utils.h $(SRCTOP)/include/k5-platform.h \
  $(SRCTOP)/include/k5-int.h $(BUILDTOP)/include/krb5/osconf.h \
  $(BUILDTOP)/include/krb5.h $(SRCTOP)/include/krb5/kdb.h \
  krb4int.h
stime.so stime.po $(OUTPRE)stime.$(OBJEXT): stime.c $(SRCTOP)/include/kerberosIV/krb.h \
  $(SRCTOP)/include/kerberosIV/des.h $(KRB_ERR_H_DEP) \
  $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h krb4int.h \
  $(SRCTOP)/include/port-sockets.h $(BUILDTOP)/include/krb5/autoconf.h
strnlen.so strnlen.po $(OUTPRE)strnlen.$(OBJEXT): strnlen.c $(SRCTOP)/include/kerberosIV/krb.h \
  $(SRCTOP)/include/kerberosIV/des.h $(KRB_ERR_H_DEP) \
  $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h $(SRCTOP)/include/kerberosIV/prot.h
rd_preauth.so rd_preauth.po $(OUTPRE)rd_preauth.$(OBJEXT): rd_preauth.c $(SRCTOP)/include/kerberosIV/krb.h \
  $(SRCTOP)/include/kerberosIV/des.h $(KRB_ERR_H_DEP) \
  $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h $(SRCTOP)/include/kerberosIV/krb_db.h \
  $(SRCTOP)/include/kerberosIV/prot.h krb4int.h $(SRCTOP)/include/port-sockets.h \
  $(BUILDTOP)/include/krb5/autoconf.h
mk_preauth.so mk_preauth.po $(OUTPRE)mk_preauth.$(OBJEXT): mk_preauth.c $(SRCTOP)/include/kerberosIV/krb.h \
  $(SRCTOP)/include/kerberosIV/des.h $(KRB_ERR_H_DEP) \
  $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h $(BUILDTOP)/include/krb5/autoconf.h
unix_time.so unix_time.po $(OUTPRE)unix_time.$(OBJEXT): unix_time.c $(SRCTOP)/include/kerberosIV/krb.h \
  $(SRCTOP)/include/kerberosIV/des.h $(KRB_ERR_H_DEP) \
  $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h
tf_util.so tf_util.po $(OUTPRE)tf_util.$(OBJEXT): tf_util.c $(SRCTOP)/include/kerberosIV/krb.h \
  $(SRCTOP)/include/kerberosIV/des.h $(KRB_ERR_H_DEP) \
  $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h $(SRCTOP)/include/k5-int.h \
  $(BUILDTOP)/include/krb5/osconf.h $(BUILDTOP)/include/krb5/autoconf.h \
  $(SRCTOP)/include/k5-platform.h $(BUILDTOP)/include/krb5.h \
  $(SRCTOP)/include/port-sockets.h $(SRCTOP)/include/socket-utils.h \
  $(SRCTOP)/include/krb5/kdb.h krb4int.h
dest_tkt.so dest_tkt.po $(OUTPRE)dest_tkt.$(OBJEXT): dest_tkt.c $(SRCTOP)/include/kerberosIV/krb.h \
  $(SRCTOP)/include/kerberosIV/des.h $(KRB_ERR_H_DEP) \
  $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h $(SRCTOP)/include/k5-util.h \
  $(BUILDTOP)/include/krb5/autoconf.h
in_tkt.so in_tkt.po $(OUTPRE)in_tkt.$(OBJEXT): in_tkt.c $(SRCTOP)/include/kerberosIV/krb.h \
  $(SRCTOP)/include/kerberosIV/des.h $(KRB_ERR_H_DEP) \
  $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h $(SRCTOP)/include/k5-util.h \
  $(BUILDTOP)/include/krb5/autoconf.h
tkt_string.so tkt_string.po $(OUTPRE)tkt_string.$(OBJEXT): tkt_string.c $(SRCTOP)/include/kerberosIV/krb.h \
  $(SRCTOP)/include/kerberosIV/des.h $(KRB_ERR_H_DEP) \
  $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h $(BUILDTOP)/include/krb5/autoconf.h \
  $(SRCTOP)/include/port-sockets.h
g_tf_fname.so g_tf_fname.po $(OUTPRE)g_tf_fname.$(OBJEXT): g_tf_fname.c $(SRCTOP)/include/kerberosIV/mit-copyright.h \
  $(SRCTOP)/include/kerberosIV/krb.h $(SRCTOP)/include/kerberosIV/des.h \
  $(KRB_ERR_H_DEP) $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h
g_tf_realm.so g_tf_realm.po $(OUTPRE)g_tf_realm.$(OBJEXT): g_tf_realm.c $(SRCTOP)/include/kerberosIV/krb.h \
  $(SRCTOP)/include/kerberosIV/des.h $(KRB_ERR_H_DEP) \
  $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h
g_cred.so g_cred.po $(OUTPRE)g_cred.$(OBJEXT): g_cred.c $(SRCTOP)/include/kerberosIV/mit-copyright.h \
  $(SRCTOP)/include/kerberosIV/krb.h $(SRCTOP)/include/kerberosIV/des.h \
  $(KRB_ERR_H_DEP) $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h
save_creds.so save_creds.po $(OUTPRE)save_creds.$(OBJEXT): save_creds.c $(SRCTOP)/include/kerberosIV/krb.h \
  $(SRCTOP)/include/kerberosIV/des.h $(KRB_ERR_H_DEP) \
  $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h krb4int.h \
  $(SRCTOP)/include/port-sockets.h $(BUILDTOP)/include/krb5/autoconf.h
unix_glue.so unix_glue.po $(OUTPRE)unix_glue.$(OBJEXT): unix_glue.c $(SRCTOP)/include/kerberosIV/krb.h \
  $(SRCTOP)/include/kerberosIV/des.h $(KRB_ERR_H_DEP) \
  $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h krb4int.h \
  $(SRCTOP)/include/port-sockets.h $(BUILDTOP)/include/krb5/autoconf.h
klog.so klog.po $(OUTPRE)klog.$(OBJEXT): klog.c $(SRCTOP)/include/kerberosIV/krb.h \
  $(SRCTOP)/include/kerberosIV/des.h $(KRB_ERR_H_DEP) \
  $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h $(BUILDTOP)/include/krb5/autoconf.h \
  krb4int.h $(SRCTOP)/include/port-sockets.h $(SRCTOP)/include/kerberosIV/klog.h
kuserok.so kuserok.po $(OUTPRE)kuserok.$(OBJEXT): kuserok.c $(SRCTOP)/include/kerberosIV/krb.h \
  $(SRCTOP)/include/kerberosIV/des.h $(KRB_ERR_H_DEP) \
  $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h $(BUILDTOP)/include/krb5/autoconf.h
log.so log.po $(OUTPRE)log.$(OBJEXT): log.c $(SRCTOP)/include/kerberosIV/krb.h \
  $(SRCTOP)/include/kerberosIV/des.h $(KRB_ERR_H_DEP) \
  $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h $(BUILDTOP)/include/krb5/autoconf.h \
  krb4int.h $(SRCTOP)/include/port-sockets.h $(SRCTOP)/include/kerberosIV/klog.h
kntoln.so kntoln.po $(OUTPRE)kntoln.$(OBJEXT): kntoln.c $(SRCTOP)/include/kerberosIV/mit-copyright.h \
  $(SRCTOP)/include/kerberosIV/krb.h $(SRCTOP)/include/kerberosIV/des.h \
  $(KRB_ERR_H_DEP) $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h
fgetst.so fgetst.po $(OUTPRE)fgetst.$(OBJEXT): fgetst.c $(SRCTOP)/include/kerberosIV/mit-copyright.h \
  $(SRCTOP)/include/kerberosIV/krb.h $(SRCTOP)/include/kerberosIV/des.h \
  $(KRB_ERR_H_DEP) $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h \
  krb4int.h $(SRCTOP)/include/port-sockets.h $(BUILDTOP)/include/krb5/autoconf.h
rd_svc_key.so rd_svc_key.po $(OUTPRE)rd_svc_key.$(OBJEXT): rd_svc_key.c $(SRCTOP)/include/kerberosIV/mit-copyright.h \
  $(SRCTOP)/include/kerberosIV/krb.h $(SRCTOP)/include/kerberosIV/des.h \
  $(KRB_ERR_H_DEP) $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h \
  krb4int.h $(SRCTOP)/include/port-sockets.h $(BUILDTOP)/include/krb5/autoconf.h \
  $(SRCTOP)/include/k5-int.h $(BUILDTOP)/include/krb5/osconf.h \
  $(SRCTOP)/include/k5-platform.h $(BUILDTOP)/include/krb5.h \
  $(SRCTOP)/include/socket-utils.h $(SRCTOP)/include/krb5/kdb.h \
  $(SRCTOP)/include/krb54proto.h $(SRCTOP)/include/kerberosIV/prot.h
cr_err_repl.so cr_err_repl.po $(OUTPRE)cr_err_repl.$(OBJEXT): cr_err_repl.c $(SRCTOP)/include/kerberosIV/krb.h \
  $(SRCTOP)/include/kerberosIV/des.h $(KRB_ERR_H_DEP) \
  $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h $(SRCTOP)/include/kerberosIV/prot.h
rd_req.so rd_req.po $(OUTPRE)rd_req.$(OBJEXT): rd_req.c $(SRCTOP)/include/kerberosIV/des.h \
  $(SRCTOP)/include/kerberosIV/krb.h $(KRB_ERR_H_DEP) \
  $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h $(SRCTOP)/include/kerberosIV/prot.h \
  $(BUILDTOP)/include/krb5.h $(SRCTOP)/include/krb54proto.h
g_svc_in_tkt.so g_svc_in_tkt.po $(OUTPRE)g_svc_in_tkt.$(OBJEXT): g_svc_in_tkt.c $(SRCTOP)/include/kerberosIV/krb.h \
  $(SRCTOP)/include/kerberosIV/des.h $(KRB_ERR_H_DEP) \
  $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h $(SRCTOP)/include/kerberosIV/prot.h \
  krb4int.h $(SRCTOP)/include/port-sockets.h $(BUILDTOP)/include/krb5/autoconf.h
recvauth.so recvauth.po $(OUTPRE)recvauth.$(OBJEXT): recvauth.c $(SRCTOP)/include/kerberosIV/krb.h \
  $(SRCTOP)/include/kerberosIV/des.h $(KRB_ERR_H_DEP) \
  $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h $(BUILDTOP)/include/krb5/autoconf.h \
  $(SRCTOP)/include/port-sockets.h
ad_print.so ad_print.po $(OUTPRE)ad_print.$(OBJEXT): ad_print.c $(SRCTOP)/include/kerberosIV/krb.h \
  $(SRCTOP)/include/kerberosIV/des.h $(KRB_ERR_H_DEP) \
  $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h krb4int.h \
  $(SRCTOP)/include/port-sockets.h $(BUILDTOP)/include/krb5/autoconf.h
cr_death_pkt.so cr_death_pkt.po $(OUTPRE)cr_death_pkt.$(OBJEXT): cr_death_pkt.c $(SRCTOP)/include/kerberosIV/krb.h \
  $(SRCTOP)/include/kerberosIV/des.h $(KRB_ERR_H_DEP) \
  $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h $(SRCTOP)/include/kerberosIV/prot.h
kparse.so kparse.po $(OUTPRE)kparse.$(OBJEXT): kparse.c $(SRCTOP)/include/kerberosIV/mit-copyright.h \
  $(SRCTOP)/include/kerberosIV/krb.h $(SRCTOP)/include/kerberosIV/des.h \
  $(KRB_ERR_H_DEP) $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h \
  $(SRCTOP)/include/kerberosIV/kparse.h $(BUILDTOP)/include/krb5/autoconf.h
put_svc_key.so put_svc_key.po $(OUTPRE)put_svc_key.$(OBJEXT): put_svc_key.c $(SRCTOP)/include/kerberosIV/krb.h \
  $(SRCTOP)/include/kerberosIV/des.h $(KRB_ERR_H_DEP) \
  $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h krb4int.h \
  $(SRCTOP)/include/port-sockets.h $(BUILDTOP)/include/krb5/autoconf.h
sendauth.so sendauth.po $(OUTPRE)sendauth.$(OBJEXT): sendauth.c $(SRCTOP)/include/kerberosIV/mit-copyright.h \
  $(SRCTOP)/include/kerberosIV/krb.h $(SRCTOP)/include/kerberosIV/des.h \
  $(KRB_ERR_H_DEP) $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h \
  krb4int.h $(SRCTOP)/include/port-sockets.h $(BUILDTOP)/include/krb5/autoconf.h
netread.so netread.po $(OUTPRE)netread.$(OBJEXT): netread.c $(SRCTOP)/include/kerberosIV/krb.h \
  $(SRCTOP)/include/kerberosIV/des.h $(KRB_ERR_H_DEP) \
  $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h $(BUILDTOP)/include/krb5/autoconf.h \
  $(SRCTOP)/include/port-sockets.h
netwrite.so netwrite.po $(OUTPRE)netwrite.$(OBJEXT): netwrite.c $(SRCTOP)/include/kerberosIV/krb.h \
  $(SRCTOP)/include/kerberosIV/des.h $(KRB_ERR_H_DEP) \
  $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h $(BUILDTOP)/include/krb5/autoconf.h \
  $(SRCTOP)/include/port-sockets.h
g_cnffile.so g_cnffile.po $(OUTPRE)g_cnffile.$(OBJEXT): g_cnffile.c $(SRCTOP)/include/kerberosIV/krb.h \
  $(SRCTOP)/include/kerberosIV/des.h $(KRB_ERR_H_DEP) \
  $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h $(SRCTOP)/include/k5-int.h \
  $(BUILDTOP)/include/krb5/osconf.h $(BUILDTOP)/include/krb5/autoconf.h \
  $(SRCTOP)/include/k5-platform.h $(BUILDTOP)/include/krb5.h \
  $(SRCTOP)/include/port-sockets.h $(SRCTOP)/include/socket-utils.h \
  $(SRCTOP)/include/krb5/kdb.h krb4int.h
RealmsConfig-glue.so RealmsConfig-glue.po $(OUTPRE)RealmsConfig-glue.$(OBJEXT): RealmsConfig-glue.c \
  $(BUILDTOP)/include/profile.h $(COM_ERR_DEPS) $(SRCTOP)/include/kerberosIV/krb.h \
  $(SRCTOP)/include/kerberosIV/des.h $(KRB_ERR_H_DEP) \
  krb4int.h $(SRCTOP)/include/port-sockets.h $(BUILDTOP)/include/krb5/autoconf.h \
  $(SRCTOP)/include/k5-int.h $(BUILDTOP)/include/krb5/osconf.h \
  $(SRCTOP)/include/k5-platform.h $(BUILDTOP)/include/krb5.h \
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
