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
myfulldir=util/profile
mydir=.
BUILDTOP=$(REL)..$(S)..
PROG_LIBPATH=-L$(TOPLIBD)
PROG_RPATH=$(KRB5_LIBDIR)
BUILDTOP = ..\..
OBJFILE=$(OUTPRE)profile.lst
LIBNAME=$(OUTPRE)profile.lib

LOCALINCLUDES=-I.

STLIBOBJS = \
	prof_tree.o \
	prof_file.o \
	prof_parse.o \
	prof_get.o \
	prof_set.o \
	prof_err.o \
	prof_init.o

OBJS = $(OUTPRE)prof_tree.$(OBJEXT) \
	$(OUTPRE)prof_file.$(OBJEXT) \
	$(OUTPRE)prof_parse.$(OBJEXT) \
	$(OUTPRE)prof_get.$(OBJEXT) \
	$(OUTPRE)prof_set.$(OBJEXT) \
	$(OUTPRE)prof_err.$(OBJEXT) \
	$(OUTPRE)prof_init.$(OBJEXT)

SRCS = $(srcdir)/prof_tree.c \
	$(srcdir)/prof_file.c \
	$(srcdir)/prof_parse.c \
	$(srcdir)/prof_get.c \
	$(srcdir)/prof_set.c \
	prof_err.c \
	$(srcdir)/prof_init.c

EXTRADEPSRCS=$(srcdir)/test_parse.c $(srcdir)/test_profile.c

DEPLIBS = $(COM_ERR_DEPLIB)
MLIBS = -lcom_err

LIB=profile
LIBMAJOR=1
LIBMINOR=1
SHLIB_EXPDEPS = $(COM_ERR_DEPLIB)
SHLIB_EXPLIBS = -lcom_err
SHLIB_DIRS = -L$(TOPLIBD)
SHLIB_RDIRS=$(KRB5_LIBDIR)

STOBJLISTS=OBJS.ST

PROFILE_HDR=$(BUILDTOP)$(S)include$(S)profile.h

all-unix:: includes 
all-unix:: all-libs
all-windows:: $(PROFILE_HDR)

install-headers-unix:: includes

$(PROFILE_HDR):: profile.h
	$(CP) profile.h "$@"

includes:: profile.h
	if cmp profile.h \
	$(BUILDTOP)/include/profile.h >/dev/null 2>&1; then :; \
	else \
		(set -x; $(RM) $(BUILDTOP)/include/profile.h; \
		 $(CP) profile.h $(BUILDTOP)/include/profile.h) ; \
	fi

clean-unix::
	$(RM) $(BUILDTOP)/include/profile.h

LIBOBJS = $(OBJS)


awk-windows:
	$(AWK) -f $(BUILDTOP)/util/et/et_h.awk outfile=prof_err.h prof_err.et
	$(AWK) -f $(BUILDTOP)/util/et/et_c.awk outfile=prof_err.c prof_err.et
	if exist prof_err.h copy profile.hin+prof_err.h profile.h
	if exist profile.h copy profile.h $(BUILDTOP)\include\profile.h

test_parse: test_parse.$(OBJEXT) $(OBJS) $(DEPLIBS)
	$(CC_LINK) -o test_parse test_parse.$(OBJEXT) $(OBJS) $(MLIBS)

test_profile: test_profile.$(OBJEXT) argv_parse.$(OBJEXT) $(OBJS) $(DEPLIBS)
	$(CC_LINK) -o test_profile test_profile.$(OBJEXT) \
		argv_parse.$(OBJEXT) $(OBJS) $(MLIBS)

.d: includes

# NEED TO FIX!!
$(OUTPRE)test_parse.exe: 
	$(CC) $(CFLAGS2) -o test_parse.exe test_parse.c \
		prof_parse.c prof_tree.c /link /stack:16384

# NEED TO FIX!!
$(OUTPRE)test_profile.exe: 
	$(CC) $(CFLAGS2) -o test_profile.exe test_profile.c prof_init.c \
		prof_file.c prof_parse.c prof_tree.c /link /stack:16384

profile.h: prof_err.h profile.hin
	cat $(srcdir)/profile.hin prof_err.h > $@

prof_err.h: $(srcdir)/prof_err.et

prof_err.c: $(srcdir)/prof_err.et

prof_err.o: prof_err.c

clean-unix:: clean-libs clean-libobjs
	$(RM) $(PROGS) *.o *~ test_parse core prof_err.h \
		prof_err.c test_profile profile.h

clean-windows::
	$(RM) $(PROFILE_HDR)

check-unix:: test_parse test_profile

check-windows:: $(OUTPRE)test_profile.exe $(OUTPRE)test_parse.exe
	$(RM) $(OUTPRE)*.obj
	$(OUTPRE)test_parse test.ini


# @lib_frag@
# @libobj_frag@

# +++ Dependency line eater +++
# 
# Makefile dependencies follow.  This must be the last section in
# the Makefile.in file
#
prof_tree.so prof_tree.po $(OUTPRE)prof_tree.$(OBJEXT): prof_tree.c prof_int.h \
  $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h
prof_file.so prof_file.po $(OUTPRE)prof_file.$(OBJEXT): prof_file.c prof_int.h \
  $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h
prof_parse.so prof_parse.po $(OUTPRE)prof_parse.$(OBJEXT): prof_parse.c prof_int.h \
  $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h
prof_get.so prof_get.po $(OUTPRE)prof_get.$(OBJEXT): prof_get.c prof_int.h \
  $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h
prof_set.so prof_set.po $(OUTPRE)prof_set.$(OBJEXT): prof_set.c prof_int.h \
  $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h
prof_err.so prof_err.po $(OUTPRE)prof_err.$(OBJEXT): prof_err.c $(COM_ERR_DEPS)
prof_init.so prof_init.po $(OUTPRE)prof_init.$(OBJEXT): prof_init.c prof_int.h \
  $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h
test_parse.so test_parse.po $(OUTPRE)test_parse.$(OBJEXT): test_parse.c prof_int.h \
  $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h
test_profile.so test_profile.po $(OUTPRE)test_profile.$(OBJEXT): test_profile.c prof_int.h \
  $(COM_ERR_DEPS) $(BUILDTOP)/include/profile.h argv_parse.h

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
