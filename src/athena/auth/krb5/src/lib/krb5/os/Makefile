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
myfulldir=lib/krb5/os
mydir=os
BUILDTOP=$(REL)..$(S)..$(S)..
KRB5_RUN_ENV = @KRB5_RUN_ENV@
PROG_LIBPATH=-L$(TOPLIBD)
PROG_RPATH=$(KRB5_LIBDIR)

BUILDTOP = ..\..\..
PREFIXDIR=os
OBJFILE=..\$(OUTPRE)$(PREFIXDIR).lst
##WIN16##LIBNAME=..\krb5.lib

STLIBOBJS= \
	accessor.o	\
	an_to_ln.o	\
	c_ustime.o	\
	def_realm.o	\
	ccdefname.o	\
	changepw.o	\
	free_krbhs.o	\
	free_hstrl.o	\
	full_ipadr.o	\
	get_krbhst.o	\
	gen_port.o	\
	genaddrs.o	\
	gen_rname.o	\
	gmt_mktime.o	\
	hostaddr.o	\
	hst_realm.o	\
	init_os_ctx.o	\
	krbfileio.o	\
	ktdefname.o	\
	kuserok.o	\
	mk_faddr.o	\
	localaddr.o	\
	locate_kdc.o	\
	lock_file.o	\
	net_read.o	\
	net_write.o	\
	osconfig.o	\
	port2ip.o	\
	prompter.o	\
	promptusr.o	\
	read_msg.o	\
	read_pwd.o	\
	realm_dom.o	\
	realm_iter.o	\
	send524.o	\
	sendto_kdc.o	\
	sn2princ.o	\
	timeofday.o	\
	toffset.o	\
	unlck_file.o	\
	ustime.o	\
	write_msg.o

OBJS= \
	$(OUTPRE)accessor.$(OBJEXT)	\
	$(OUTPRE)an_to_ln.$(OBJEXT)	\
	$(OUTPRE)c_ustime.$(OBJEXT)	\
	$(OUTPRE)def_realm.$(OBJEXT)	\
	$(OUTPRE)ccdefname.$(OBJEXT)	\
	$(OUTPRE)changepw.$(OBJEXT)	\
	$(OUTPRE)free_krbhs.$(OBJEXT)	\
	$(OUTPRE)free_hstrl.$(OBJEXT)	\
	$(OUTPRE)full_ipadr.$(OBJEXT)	\
	$(OUTPRE)get_krbhst.$(OBJEXT)	\
	$(OUTPRE)gen_port.$(OBJEXT)	\
	$(OUTPRE)genaddrs.$(OBJEXT)	\
	$(OUTPRE)gen_rname.$(OBJEXT)	\
	$(OUTPRE)gmt_mktime.$(OBJEXT)	\
	$(OUTPRE)hostaddr.$(OBJEXT)	\
	$(OUTPRE)hst_realm.$(OBJEXT)	\
	$(OUTPRE)init_os_ctx.$(OBJEXT)	\
	$(OUTPRE)krbfileio.$(OBJEXT)	\
	$(OUTPRE)ktdefname.$(OBJEXT)	\
	$(OUTPRE)kuserok.$(OBJEXT)	\
	$(OUTPRE)mk_faddr.$(OBJEXT)	\
	$(OUTPRE)localaddr.$(OBJEXT)	\
	$(OUTPRE)locate_kdc.$(OBJEXT)	\
	$(OUTPRE)lock_file.$(OBJEXT)	\
	$(OUTPRE)net_read.$(OBJEXT)	\
	$(OUTPRE)net_write.$(OBJEXT)	\
	$(OUTPRE)osconfig.$(OBJEXT)	\
	$(OUTPRE)port2ip.$(OBJEXT)	\
	$(OUTPRE)prompter.$(OBJEXT)	\
	$(OUTPRE)promptusr.$(OBJEXT)	\
	$(OUTPRE)read_msg.$(OBJEXT)	\
	$(OUTPRE)read_pwd.$(OBJEXT)	\
	$(OUTPRE)realm_dom.$(OBJEXT)	\
	$(OUTPRE)realm_iter.$(OBJEXT)	\
	$(OUTPRE)send524.$(OBJEXT)	\
	$(OUTPRE)sendto_kdc.$(OBJEXT)	\
	$(OUTPRE)sn2princ.$(OBJEXT)	\
	$(OUTPRE)timeofday.$(OBJEXT)	\
	$(OUTPRE)toffset.$(OBJEXT)	\
	$(OUTPRE)unlck_file.$(OBJEXT)	\
	$(OUTPRE)ustime.$(OBJEXT)	\
	$(OUTPRE)write_msg.$(OBJEXT)

SRCS= \
	$(srcdir)/accessor.c    \
	$(srcdir)/an_to_ln.c	\
	$(srcdir)/c_ustime.c	\
	$(srcdir)/def_realm.c	\
	$(srcdir)/ccdefname.c	\
	$(srcdir)/changepw.c	\
	$(srcdir)/free_krbhs.c	\
	$(srcdir)/free_hstrl.c	\
	$(srcdir)/full_ipadr.c	\
	$(srcdir)/get_krbhst.c	\
	$(srcdir)/gen_port.c	\
	$(srcdir)/genaddrs.c	\
	$(srcdir)/gen_rname.c	\
	$(srcdir)/gmt_mktime.c	\
	$(srcdir)/hostaddr.c	\
	$(srcdir)/hst_realm.c	\
	$(srcdir)/init_os_ctx.c	\
	$(srcdir)/krbfileio.c	\
	$(srcdir)/ktdefname.c	\
	$(srcdir)/kuserok.c	\
	$(srcdir)/mk_faddr.c	\
	$(srcdir)/localaddr.c	\
	$(srcdir)/locate_kdc.c	\
	$(srcdir)/lock_file.c	\
	$(srcdir)/net_read.c	\
	$(srcdir)/net_write.c	\
	$(srcdir)/osconfig.c	\
	$(srcdir)/prompter.c	\
	$(srcdir)/promptusr.c	\
	$(srcdir)/read_msg.c	\
	$(srcdir)/read_pwd.c	\
	$(srcdir)/realm_dom.c	\
	$(srcdir)/realm_iter.c	\
	$(srcdir)/port2ip.c	\
	$(srcdir)/send524.c	\
	$(srcdir)/sendto_kdc.c	\
	$(srcdir)/sn2princ.c	\
	$(srcdir)/timeofday.c	\
	$(srcdir)/toffset.c	\
	$(srcdir)/unlck_file.c	\
	$(srcdir)/ustime.c	\
	$(srcdir)/write_msg.c

LIBOBJS = $(OBJS)

all-unix:: all-libobjs
clean-unix:: clean-libobjs

shared:
	mkdir shared

TEST_PROGS= t_std_conf t_an_to_ln

T_STD_CONF_OBJS= t_std_conf.o def_realm.o get_krbhst.o realm_dom.o \
		hst_realm.o init_os_ctx.o locate_kdc.o 

T_AN_TO_LN_OBJS = t_an_to_ln.o an_to_ln.o 

T_REALM_ITER_OBJS = t_realm_iter.o realm_iter.o

t_std_conf: $(T_STD_CONF_OBJS) $(KRB5_BASE_DEPLIBS)
	$(CC_LINK) -o t_std_conf $(T_STD_CONF_OBJS) $(KRB5_BASE_LIBS)

t_an_to_ln: $(T_AN_TO_LN_OBJS) $(KRB5_BASE_DEPLIBS)
	$(CC_LINK) -o t_an_to_ln $(T_AN_TO_LN_OBJS) $(KRB5_BASE_LIBS)

t_realm_iter: $(T_REALM_ITER_OBJS) $(KRB5_BASE_DEPLIBS)
	$(CC_LINK) -o t_realm_iter $(T_REALM_ITER_OBJS) $(KRB5_BASE_LIBS)

t_mktime: gmt_mktime.c
	$(CC) $(ALL_CFLAGS) -DTEST_LEAP -o t_mktime $(srcdir)/gmt_mktime.c

t_localaddr: localaddr.c
	$(CC) $(ALL_CFLAGS) -DTEST -o t_localaddr $(srcdir)/localaddr.c $(LIBS)

t_locate_kdc: t_locate_kdc.o
	$(CC_LINK) $(ALL_CFLAGS) -o t_locate_kdc t_locate_kdc.o \
		$(KRB5_BASE_LIBS)
t_locate_kdc.o: t_locate_kdc.c locate_kdc.c

LCLINT=lclint
LCLINTOPTS= -warnposix \
	-usedef +charintliteral +ignoresigns -predboolint +boolint \
	-exportlocal -retvalint \
	+mod-uncon +modinternalstrict +modfilesys
lclint-localaddr: localaddr.c
	$(LCLINT) $(LCLINTOPTS) $(CPPFLAGS) $(LOCALINCLUDES) $(DEFS) \
		-DTEST $(srcdir)/localaddr.c

check-unix:: $(TEST_PROGS)
	KRB5_CONFIG=$(srcdir)/td_krb5.conf ; export KRB5_CONFIG ;\
	$(KRB5_RUN_ENV) ./t_std_conf  -d -s NEW.DEFAULT.REALM -d \
		-k IGGY.ORG -k DEFAULT_REALM.TST \
		-D DEFAULT_REALM.TST -r bad.idea -r itar.bad.idea \
		-r really.BAD.IDEA. -r clipper.bad.idea -r KeYEsCrOW.BaD.IDea \
		-r pgp.good.idea -r no_domain > test.out
	cmp test.out $(srcdir)/ref_std_conf.out
	$(RM) test.out

#
# Do some aname-to-lname testing.
#
check-unix::
	echo '[libdefaults]' > ./t_an.conf
	echo '	default_realm = r' >> ./t_an.conf
	echo '[realms]' >> ./t_an.conf
	echo 'r = {' >> ./t_an.conf
#	if test -r ../../../admin/aname/kdb5_anadd ; then \
#		$(KRB5_RUN_ENV) ../../../admin/aname/kdb5_anadd -a -n ./t_an p/i/i/i@r piii; \
#		../../../admin/aname/kdb5_anadd -a -n ./t_an p/a/b/c@r pabc; \
#		echo 'auth_to_local = DB:./t_an' >> ./t_an.conf; \
#	fi
	echo 'auth_to_local = RULE:[3:$$1$$3$$2](rule.*)s/rule//g' \
		>> ./t_an.conf
	echo 'auth_to_local = DEFAULT' >> ./t_an.conf
	echo '}' >> ./t_an.conf
#	if test -r ../../../admin/aname/kdb5_anadd ; then \
#		KRB5_CONFIG=./t_an.conf ; export KRB5_CONFIG ; \
#		$(KRB5_RUN_ENV) ./t_an_to_ln p/i/i/i@r p/a/b/c@r; \
#	fi
	KRB5_CONFIG=./t_an.conf ; export KRB5_CONFIG ; \
	$(KRB5_RUN_ENV) ./t_an_to_ln rul/helpme/e@r ru/123/le@r
	KRB5_CONFIG=./t_an.conf ; export KRB5_CONFIG ; \
	$(KRB5_RUN_ENV) ./t_an_to_ln fred/r@r barney/r@r
	$(RM) ./t_an.*

clean:: 
	$(RM) $(TEST_PROGS) test.out t_std_conf.o t_an_to_ln.o

# @libobj_frag@

# +++ Dependency line eater +++
# 
# Makefile dependencies follow.  This must be the last section in
# the Makefile.in file
#
accessor.so accessor.po $(OUTPRE)accessor.$(OBJEXT): accessor.c $(SRCTOP)/include/k5-int.h \
  $(BUILDTOP)/include/krb5/osconf.h $(BUILDTOP)/include/krb5/autoconf.h \
  $(BUILDTOP)/include/krb5.h $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h \
  $(SRCTOP)/include/port-sockets.h $(SRCTOP)/include/socket-utils.h \
  $(SRCTOP)/include/krb5/kdb.h os-proto.h
an_to_ln.so an_to_ln.po $(OUTPRE)an_to_ln.$(OBJEXT): an_to_ln.c $(SRCTOP)/include/k5-int.h \
  $(BUILDTOP)/include/krb5/osconf.h $(BUILDTOP)/include/krb5/autoconf.h \
  $(BUILDTOP)/include/krb5.h $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h \
  $(SRCTOP)/include/port-sockets.h $(SRCTOP)/include/socket-utils.h \
  $(SRCTOP)/include/krb5/kdb.h
c_ustime.so c_ustime.po $(OUTPRE)c_ustime.$(OBJEXT): c_ustime.c $(SRCTOP)/include/k5-int.h \
  $(BUILDTOP)/include/krb5/osconf.h $(BUILDTOP)/include/krb5/autoconf.h \
  $(BUILDTOP)/include/krb5.h $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h \
  $(SRCTOP)/include/port-sockets.h $(SRCTOP)/include/socket-utils.h \
  $(SRCTOP)/include/krb5/kdb.h
def_realm.so def_realm.po $(OUTPRE)def_realm.$(OBJEXT): def_realm.c $(SRCTOP)/include/k5-int.h \
  $(BUILDTOP)/include/krb5/osconf.h $(BUILDTOP)/include/krb5/autoconf.h \
  $(BUILDTOP)/include/krb5.h $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h \
  $(SRCTOP)/include/port-sockets.h $(SRCTOP)/include/socket-utils.h \
  $(SRCTOP)/include/krb5/kdb.h os-proto.h
ccdefname.so ccdefname.po $(OUTPRE)ccdefname.$(OBJEXT): ccdefname.c $(SRCTOP)/include/k5-int.h \
  $(BUILDTOP)/include/krb5/osconf.h $(BUILDTOP)/include/krb5/autoconf.h \
  $(BUILDTOP)/include/krb5.h $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h \
  $(SRCTOP)/include/port-sockets.h $(SRCTOP)/include/socket-utils.h \
  $(SRCTOP)/include/krb5/kdb.h
changepw.so changepw.po $(OUTPRE)changepw.$(OBJEXT): changepw.c $(SRCTOP)/include/fake-addrinfo.h \
  $(SRCTOP)/include/port-sockets.h $(BUILDTOP)/include/krb5/autoconf.h \
  $(SRCTOP)/include/socket-utils.h $(SRCTOP)/include/k5-int.h \
  $(BUILDTOP)/include/krb5/osconf.h $(BUILDTOP)/include/krb5.h \
  $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h $(SRCTOP)/include/krb5/kdb.h \
  os-proto.h
free_krbhs.so free_krbhs.po $(OUTPRE)free_krbhs.$(OBJEXT): free_krbhs.c $(SRCTOP)/include/k5-int.h \
  $(BUILDTOP)/include/krb5/osconf.h $(BUILDTOP)/include/krb5/autoconf.h \
  $(BUILDTOP)/include/krb5.h $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h \
  $(SRCTOP)/include/port-sockets.h $(SRCTOP)/include/socket-utils.h \
  $(SRCTOP)/include/krb5/kdb.h
free_hstrl.so free_hstrl.po $(OUTPRE)free_hstrl.$(OBJEXT): free_hstrl.c $(SRCTOP)/include/k5-int.h \
  $(BUILDTOP)/include/krb5/osconf.h $(BUILDTOP)/include/krb5/autoconf.h \
  $(BUILDTOP)/include/krb5.h $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h \
  $(SRCTOP)/include/port-sockets.h $(SRCTOP)/include/socket-utils.h \
  $(SRCTOP)/include/krb5/kdb.h
full_ipadr.so full_ipadr.po $(OUTPRE)full_ipadr.$(OBJEXT): full_ipadr.c $(SRCTOP)/include/k5-int.h \
  $(BUILDTOP)/include/krb5/osconf.h $(BUILDTOP)/include/krb5/autoconf.h \
  $(BUILDTOP)/include/krb5.h $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h \
  $(SRCTOP)/include/port-sockets.h $(SRCTOP)/include/socket-utils.h \
  $(SRCTOP)/include/krb5/kdb.h os-proto.h
get_krbhst.so get_krbhst.po $(OUTPRE)get_krbhst.$(OBJEXT): get_krbhst.c $(SRCTOP)/include/k5-int.h \
  $(BUILDTOP)/include/krb5/osconf.h $(BUILDTOP)/include/krb5/autoconf.h \
  $(BUILDTOP)/include/krb5.h $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h \
  $(SRCTOP)/include/port-sockets.h $(SRCTOP)/include/socket-utils.h \
  $(SRCTOP)/include/krb5/kdb.h
gen_port.so gen_port.po $(OUTPRE)gen_port.$(OBJEXT): gen_port.c $(SRCTOP)/include/k5-int.h \
  $(BUILDTOP)/include/krb5/osconf.h $(BUILDTOP)/include/krb5/autoconf.h \
  $(BUILDTOP)/include/krb5.h $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h \
  $(SRCTOP)/include/port-sockets.h $(SRCTOP)/include/socket-utils.h \
  $(SRCTOP)/include/krb5/kdb.h os-proto.h
genaddrs.so genaddrs.po $(OUTPRE)genaddrs.$(OBJEXT): genaddrs.c $(SRCTOP)/include/k5-int.h \
  $(BUILDTOP)/include/krb5/osconf.h $(BUILDTOP)/include/krb5/autoconf.h \
  $(BUILDTOP)/include/krb5.h $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h \
  $(SRCTOP)/include/port-sockets.h $(SRCTOP)/include/socket-utils.h \
  $(SRCTOP)/include/krb5/kdb.h os-proto.h
gen_rname.so gen_rname.po $(OUTPRE)gen_rname.$(OBJEXT): gen_rname.c $(SRCTOP)/include/k5-int.h \
  $(BUILDTOP)/include/krb5/osconf.h $(BUILDTOP)/include/krb5/autoconf.h \
  $(BUILDTOP)/include/krb5.h $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h \
  $(SRCTOP)/include/port-sockets.h $(SRCTOP)/include/socket-utils.h \
  $(SRCTOP)/include/krb5/kdb.h os-proto.h
gmt_mktime.so gmt_mktime.po $(OUTPRE)gmt_mktime.$(OBJEXT): gmt_mktime.c $(SRCTOP)/include/k5-int.h \
  $(BUILDTOP)/include/krb5/osconf.h $(BUILDTOP)/include/krb5/autoconf.h \
  $(BUILDTOP)/include/krb5.h $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h \
  $(SRCTOP)/include/port-sockets.h $(SRCTOP)/include/socket-utils.h \
  $(SRCTOP)/include/krb5/kdb.h
hostaddr.so hostaddr.po $(OUTPRE)hostaddr.$(OBJEXT): hostaddr.c $(SRCTOP)/include/k5-int.h \
  $(BUILDTOP)/include/krb5/osconf.h $(BUILDTOP)/include/krb5/autoconf.h \
  $(BUILDTOP)/include/krb5.h $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h \
  $(SRCTOP)/include/port-sockets.h $(SRCTOP)/include/socket-utils.h \
  $(SRCTOP)/include/krb5/kdb.h $(SRCTOP)/include/fake-addrinfo.h
hst_realm.so hst_realm.po $(OUTPRE)hst_realm.$(OBJEXT): hst_realm.c $(SRCTOP)/include/k5-int.h \
  $(BUILDTOP)/include/krb5/osconf.h $(BUILDTOP)/include/krb5/autoconf.h \
  $(BUILDTOP)/include/krb5.h $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h \
  $(SRCTOP)/include/port-sockets.h $(SRCTOP)/include/socket-utils.h \
  $(SRCTOP)/include/krb5/kdb.h os-proto.h $(SRCTOP)/include/fake-addrinfo.h
init_os_ctx.so init_os_ctx.po $(OUTPRE)init_os_ctx.$(OBJEXT): init_os_ctx.c $(SRCTOP)/include/k5-int.h \
  $(BUILDTOP)/include/krb5/osconf.h $(BUILDTOP)/include/krb5/autoconf.h \
  $(BUILDTOP)/include/krb5.h $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h \
  $(SRCTOP)/include/port-sockets.h $(SRCTOP)/include/socket-utils.h \
  $(SRCTOP)/include/krb5/kdb.h os-proto.h
krbfileio.so krbfileio.po $(OUTPRE)krbfileio.$(OBJEXT): krbfileio.c $(SRCTOP)/include/k5-int.h \
  $(BUILDTOP)/include/krb5/osconf.h $(BUILDTOP)/include/krb5/autoconf.h \
  $(BUILDTOP)/include/krb5.h $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h \
  $(SRCTOP)/include/port-sockets.h $(SRCTOP)/include/socket-utils.h \
  $(SRCTOP)/include/krb5/kdb.h
ktdefname.so ktdefname.po $(OUTPRE)ktdefname.$(OBJEXT): ktdefname.c $(SRCTOP)/include/k5-int.h \
  $(BUILDTOP)/include/krb5/osconf.h $(BUILDTOP)/include/krb5/autoconf.h \
  $(BUILDTOP)/include/krb5.h $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h \
  $(SRCTOP)/include/port-sockets.h $(SRCTOP)/include/socket-utils.h \
  $(SRCTOP)/include/krb5/kdb.h
kuserok.so kuserok.po $(OUTPRE)kuserok.$(OBJEXT): kuserok.c $(SRCTOP)/include/k5-int.h \
  $(BUILDTOP)/include/krb5/osconf.h $(BUILDTOP)/include/krb5/autoconf.h \
  $(BUILDTOP)/include/krb5.h $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h \
  $(SRCTOP)/include/port-sockets.h $(SRCTOP)/include/socket-utils.h \
  $(SRCTOP)/include/krb5/kdb.h
mk_faddr.so mk_faddr.po $(OUTPRE)mk_faddr.$(OBJEXT): mk_faddr.c $(SRCTOP)/include/k5-int.h \
  $(BUILDTOP)/include/krb5/osconf.h $(BUILDTOP)/include/krb5/autoconf.h \
  $(BUILDTOP)/include/krb5.h $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h \
  $(SRCTOP)/include/port-sockets.h $(SRCTOP)/include/socket-utils.h \
  $(SRCTOP)/include/krb5/kdb.h os-proto.h
localaddr.so localaddr.po $(OUTPRE)localaddr.$(OBJEXT): localaddr.c $(SRCTOP)/include/k5-int.h \
  $(BUILDTOP)/include/krb5/osconf.h $(BUILDTOP)/include/krb5/autoconf.h \
  $(BUILDTOP)/include/krb5.h $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h \
  $(SRCTOP)/include/port-sockets.h $(SRCTOP)/include/socket-utils.h \
  $(SRCTOP)/include/krb5/kdb.h $(SRCTOP)/include/foreachaddr.c
locate_kdc.so locate_kdc.po $(OUTPRE)locate_kdc.$(OBJEXT): locate_kdc.c $(SRCTOP)/include/fake-addrinfo.h \
  $(SRCTOP)/include/port-sockets.h $(BUILDTOP)/include/krb5/autoconf.h \
  $(SRCTOP)/include/socket-utils.h $(SRCTOP)/include/k5-int.h \
  $(BUILDTOP)/include/krb5/osconf.h $(BUILDTOP)/include/krb5.h \
  $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h $(SRCTOP)/include/krb5/kdb.h \
  os-proto.h
lock_file.so lock_file.po $(OUTPRE)lock_file.$(OBJEXT): lock_file.c $(SRCTOP)/include/k5-int.h \
  $(BUILDTOP)/include/krb5/osconf.h $(BUILDTOP)/include/krb5/autoconf.h \
  $(BUILDTOP)/include/krb5.h $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h \
  $(SRCTOP)/include/port-sockets.h $(SRCTOP)/include/socket-utils.h \
  $(SRCTOP)/include/krb5/kdb.h
net_read.so net_read.po $(OUTPRE)net_read.$(OBJEXT): net_read.c $(SRCTOP)/include/k5-int.h \
  $(BUILDTOP)/include/krb5/osconf.h $(BUILDTOP)/include/krb5/autoconf.h \
  $(BUILDTOP)/include/krb5.h $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h \
  $(SRCTOP)/include/port-sockets.h $(SRCTOP)/include/socket-utils.h \
  $(SRCTOP)/include/krb5/kdb.h
net_write.so net_write.po $(OUTPRE)net_write.$(OBJEXT): net_write.c $(SRCTOP)/include/k5-int.h \
  $(BUILDTOP)/include/krb5/osconf.h $(BUILDTOP)/include/krb5/autoconf.h \
  $(BUILDTOP)/include/krb5.h $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h \
  $(SRCTOP)/include/port-sockets.h $(SRCTOP)/include/socket-utils.h \
  $(SRCTOP)/include/krb5/kdb.h
osconfig.so osconfig.po $(OUTPRE)osconfig.$(OBJEXT): osconfig.c $(SRCTOP)/include/k5-int.h \
  $(BUILDTOP)/include/krb5/osconf.h $(BUILDTOP)/include/krb5/autoconf.h \
  $(BUILDTOP)/include/krb5.h $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h \
  $(SRCTOP)/include/port-sockets.h $(SRCTOP)/include/socket-utils.h \
  $(SRCTOP)/include/krb5/kdb.h
prompter.so prompter.po $(OUTPRE)prompter.$(OBJEXT): prompter.c $(SRCTOP)/include/k5-int.h \
  $(BUILDTOP)/include/krb5/osconf.h $(BUILDTOP)/include/krb5/autoconf.h \
  $(BUILDTOP)/include/krb5.h $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h \
  $(SRCTOP)/include/port-sockets.h $(SRCTOP)/include/socket-utils.h \
  $(SRCTOP)/include/krb5/kdb.h
promptusr.so promptusr.po $(OUTPRE)promptusr.$(OBJEXT): promptusr.c $(SRCTOP)/include/k5-int.h \
  $(BUILDTOP)/include/krb5/osconf.h $(BUILDTOP)/include/krb5/autoconf.h \
  $(BUILDTOP)/include/krb5.h $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h \
  $(SRCTOP)/include/port-sockets.h $(SRCTOP)/include/socket-utils.h \
  $(SRCTOP)/include/krb5/kdb.h
read_msg.so read_msg.po $(OUTPRE)read_msg.$(OBJEXT): read_msg.c $(SRCTOP)/include/k5-int.h \
  $(BUILDTOP)/include/krb5/osconf.h $(BUILDTOP)/include/krb5/autoconf.h \
  $(BUILDTOP)/include/krb5.h $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h \
  $(SRCTOP)/include/port-sockets.h $(SRCTOP)/include/socket-utils.h \
  $(SRCTOP)/include/krb5/kdb.h
read_pwd.so read_pwd.po $(OUTPRE)read_pwd.$(OBJEXT): read_pwd.c $(SRCTOP)/include/k5-int.h \
  $(BUILDTOP)/include/krb5/osconf.h $(BUILDTOP)/include/krb5/autoconf.h \
  $(BUILDTOP)/include/krb5.h $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h \
  $(SRCTOP)/include/port-sockets.h $(SRCTOP)/include/socket-utils.h \
  $(SRCTOP)/include/krb5/kdb.h
realm_dom.so realm_dom.po $(OUTPRE)realm_dom.$(OBJEXT): realm_dom.c $(SRCTOP)/include/k5-int.h \
  $(BUILDTOP)/include/krb5/osconf.h $(BUILDTOP)/include/krb5/autoconf.h \
  $(BUILDTOP)/include/krb5.h $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h \
  $(SRCTOP)/include/port-sockets.h $(SRCTOP)/include/socket-utils.h \
  $(SRCTOP)/include/krb5/kdb.h
realm_iter.so realm_iter.po $(OUTPRE)realm_iter.$(OBJEXT): realm_iter.c $(SRCTOP)/include/k5-int.h \
  $(BUILDTOP)/include/krb5/osconf.h $(BUILDTOP)/include/krb5/autoconf.h \
  $(BUILDTOP)/include/krb5.h $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h \
  $(SRCTOP)/include/port-sockets.h $(SRCTOP)/include/socket-utils.h \
  $(SRCTOP)/include/krb5/kdb.h
port2ip.so port2ip.po $(OUTPRE)port2ip.$(OBJEXT): port2ip.c $(SRCTOP)/include/k5-int.h \
  $(BUILDTOP)/include/krb5/osconf.h $(BUILDTOP)/include/krb5/autoconf.h \
  $(BUILDTOP)/include/krb5.h $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h \
  $(SRCTOP)/include/port-sockets.h $(SRCTOP)/include/socket-utils.h \
  $(SRCTOP)/include/krb5/kdb.h os-proto.h
send524.so send524.po $(OUTPRE)send524.$(OBJEXT): send524.c $(SRCTOP)/include/fake-addrinfo.h \
  $(SRCTOP)/include/port-sockets.h $(BUILDTOP)/include/krb5/autoconf.h \
  $(SRCTOP)/include/socket-utils.h $(SRCTOP)/include/k5-int.h \
  $(BUILDTOP)/include/krb5/osconf.h $(BUILDTOP)/include/krb5.h \
  $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h $(SRCTOP)/include/krb5/kdb.h \
  os-proto.h
sendto_kdc.so sendto_kdc.po $(OUTPRE)sendto_kdc.$(OBJEXT): sendto_kdc.c $(SRCTOP)/include/fake-addrinfo.h \
  $(SRCTOP)/include/port-sockets.h $(BUILDTOP)/include/krb5/autoconf.h \
  $(SRCTOP)/include/socket-utils.h $(SRCTOP)/include/k5-int.h \
  $(BUILDTOP)/include/krb5/osconf.h $(BUILDTOP)/include/krb5.h \
  $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h $(SRCTOP)/include/krb5/kdb.h \
  os-proto.h $(SRCTOP)/include/cm.h
sn2princ.so sn2princ.po $(OUTPRE)sn2princ.$(OBJEXT): sn2princ.c $(SRCTOP)/include/k5-int.h \
  $(BUILDTOP)/include/krb5/osconf.h $(BUILDTOP)/include/krb5/autoconf.h \
  $(BUILDTOP)/include/krb5.h $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h \
  $(SRCTOP)/include/port-sockets.h $(SRCTOP)/include/socket-utils.h \
  $(SRCTOP)/include/krb5/kdb.h $(SRCTOP)/include/fake-addrinfo.h
timeofday.so timeofday.po $(OUTPRE)timeofday.$(OBJEXT): timeofday.c $(SRCTOP)/include/k5-int.h \
  $(BUILDTOP)/include/krb5/osconf.h $(BUILDTOP)/include/krb5/autoconf.h \
  $(BUILDTOP)/include/krb5.h $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h \
  $(SRCTOP)/include/port-sockets.h $(SRCTOP)/include/socket-utils.h \
  $(SRCTOP)/include/krb5/kdb.h
toffset.so toffset.po $(OUTPRE)toffset.$(OBJEXT): toffset.c $(SRCTOP)/include/k5-int.h \
  $(BUILDTOP)/include/krb5/osconf.h $(BUILDTOP)/include/krb5/autoconf.h \
  $(BUILDTOP)/include/krb5.h $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h \
  $(SRCTOP)/include/port-sockets.h $(SRCTOP)/include/socket-utils.h \
  $(SRCTOP)/include/krb5/kdb.h
unlck_file.so unlck_file.po $(OUTPRE)unlck_file.$(OBJEXT): unlck_file.c $(SRCTOP)/include/k5-int.h \
  $(BUILDTOP)/include/krb5/osconf.h $(BUILDTOP)/include/krb5/autoconf.h \
  $(BUILDTOP)/include/krb5.h $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h \
  $(SRCTOP)/include/port-sockets.h $(SRCTOP)/include/socket-utils.h \
  $(SRCTOP)/include/krb5/kdb.h
ustime.so ustime.po $(OUTPRE)ustime.$(OBJEXT): ustime.c $(SRCTOP)/include/k5-int.h \
  $(BUILDTOP)/include/krb5/osconf.h $(BUILDTOP)/include/krb5/autoconf.h \
  $(BUILDTOP)/include/krb5.h $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h \
  $(SRCTOP)/include/port-sockets.h $(SRCTOP)/include/socket-utils.h \
  $(SRCTOP)/include/krb5/kdb.h
write_msg.so write_msg.po $(OUTPRE)write_msg.$(OBJEXT): write_msg.c $(SRCTOP)/include/k5-int.h \
  $(BUILDTOP)/include/krb5/osconf.h $(BUILDTOP)/include/krb5/autoconf.h \
  $(BUILDTOP)/include/krb5.h $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h \
  $(SRCTOP)/include/port-sockets.h $(SRCTOP)/include/socket-utils.h \
  $(SRCTOP)/include/krb5/kdb.h

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
