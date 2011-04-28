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

prefix=@prefix@
bindir=@bindir@
datadir=@datadir@
mydatadir=$(datadir)/et
thisconfigdir=.
myfulldir=util/et
mydir=.
BUILDTOP=$(REL)..$(S)..
RELDIR=../util/et
SED = sed

BUILDTOP = ..\..
LIBNAME=$(OUTPRE)comerr.lib
XTRA=
OBJFILE=$(OUTPRE)comerr.lst

STLIBOBJS=error_message.o et_name.o init_et.o com_err.o
STOBJLISTS=OBJS.ST
LIB=com_err
LIBMAJOR=3
LIBMINOR=0

all-unix:: all-liblinks
clean-unix:: clean-liblinks clean-libs clean-libobjs clean-lclint
install-unix:: install-libs

LINTFLAGS=-uhvb 
LINTFILES= error_message.c et_name.c init_et.c com_err.c
LIBOBJS=$(OUTPRE)com_err.$(OBJEXT) \
	$(OUTPRE)error_message.$(OBJEXT) \
	$(OUTPRE)et_name.$(OBJEXT) \
	$(OUTPRE)init_et.$(OBJEXT)
# for et_lex.lex.c include in error_table.y
LOCALINCLUDES=-I. -I$(srcdir)

FILES=	Makefile et_name.c error_message.c compile_et.c \
		et_lex.lex.l error_table.y init_et.c \
		com_err.c com_err.h \
		error_table.h mit-sipb-copyright.h \
		test_et.c test1.et test2.et \
		t_com_err.c et1.et et2.et \
		compiler.h internal.h \
		com_err.texinfo texinfo.tex
#SRCS=	compile_et.c error_table.c error_message.c et_name.c \
#	init_et.c com_err.c
SRCS= $(srcdir)/error_message.c \
	$(srcdir)/et_name.c \
	$(srcdir)/init_et.c \
	$(srcdir)/com_err.c

#
# Warning flags
#
# Uncomment WFLAGS if you want really anal GCC warning messages
#
#
WFLAGS=		-ansi -D_POSIX_SOURCE -pedantic \
			-Wall -Wwrite-strings -Wpointer-arith \
			-Wcast-qual -Wcast-align -Wtraditional \
			-Wstrict-prototypes -Wmissing-prototypes \
			-Wnested-externs -Winline -DNO_INLINE_FUNCS -Wshadow 

DEPLIBS=
SHLIB_LIBS=
SHLIB_LDFLAGS= $(LDFLAGS) @SHLIB_RPATH_DIRS@	
SHLIB_LIBDIRS= @SHLIB_LIBDIRS@

COM_ERR_HDR=$(BUILDTOP)$(S)include$(S)com_err.h

all-windows:: $(COM_ERR_HDR)

$(COM_ERR_HDR): com_err.h
	$(CP) com_err.h "$@"

error_table.c: et_lex.lex.c 
error_table.c: $(srcdir)/error_table.y

error_table.o: error_table.c et_lex.lex.c
	$(CC) $(ALL_CFLAGS) -c error_table.c

com_err.o : com_err.c

et_lex.lex.o: et_lex.lex.c
test1.o: test1.c
test2.o: test2.c
test_et.o: test1.h test2.h
et1.o: et1.c
et2.o: et2.c
test1.c test2.c et1.c et2.c test1.h test2.h et1.h et2.h: \
	compile_et et_c.awk et_h.awk
t_com_err.o: et1.h et2.h t_com_err.c

# /u1/kr/lclint-2.5q/bin/lclint -warnposix -D__sparc
LCLINT=lclint
# +posixlib	gets more complete errno list than ansilib
# -usedef	turns off bogus warnings from poor dataflow analysis (should be
#		redundant with gcc warnings anyways)
# -warnposix
# +charintliteral
# +ignoresigns
# -predboolint
# -exportlocal
# -retvalint	allow ignoring of int return values (e.g., fputs)
LCLINTOPTS=+posixlib \
	-usedef -warnposix +charintliteral +ignoresigns -predboolint +boolint \
	-exportlocal -retvalint \
	+mod-uncon +modinternalstrict +modfilesys
com_err.lcd: error_table.c error_table.h et_lex.lex.c com_err.h
	$(LCLINT) $(LCLINTOPTS) $(LOCALINCLUDES) $(DEFS) $(SRCS) \
		-dump com_err.new -expect 1
	mv -f com_err.new com_err.lcd
do-lclint: com_err.lcd t_com_err.c et1.c et2.c et1.h et2.h
	$(LCLINT) $(LCLINTOPTS) $(LOCALINCLUDES) \
		$(srcdir)/t_com_err.c et1.c et2.c
# "-load com_err.lcd" -> lclint brokenness
clean-lclint:
	$(RM) com_err.new com_err.lcd
ITS4=its4
ITS4OPTS=
do-its4: error_table.y et_lex.lex.c
	$(ITS4) $(ITS4OPTS) $(SRCS)

test_et: test_et.o test1.o test2.o $(LIBOBJS)
	$(CC) -o test_et test_et.o test1.o test2.o $(LIBOBJS)
t_com_err: t_com_err.o et1.o et2.o $(LIBOBJS)
	$(CC) -o t_com_err t_com_err.o et1.o et2.o $(LIBOBJS)

all-unix:: compile_et includes

includes:: com_err.h
	if cmp $(srcdir)/com_err.h \
	$(BUILDTOP)/include/com_err.h >/dev/null 2>&1; then :; \
	else \
		(set -x; $(RM) $(BUILDTOP)/include/com_err.h; \
		 $(CP) $(srcdir)/com_err.h \
			$(BUILDTOP)/include/com_err.h) ; \
	fi

clean-unix::
	$(RM) $(BUILDTOP)/include/com_err.h

# test_et doesn't have an interesting exit status, but it'll exercise
# some cases that t_com_err doesn't, so let's see if it crashes.
check-unix:: t_com_err test_et
	./test_et
	./t_com_err

# The real compile_et just isn't portable.  (But then again, anything using 
# lex and yacc isn't portable by definition.  :-(  )
#
#compile_et: compile_et.o error_table.o
#	$(CC) $(CFLAGS) -o $@ compile_et.o error_table.o $(LEXLIB) $(BSDLIB)
#
install-unix:: compile_et
	$(INSTALL) compile_et $(DESTDIR)$(bindir)/compile_et
	test -d $(DESTDIR)$(mydatadir) || mkdir $(DESTDIR)$(mydatadir)
	$(INSTALL_DATA) $(srcdir)/et_c.awk $(DESTDIR)$(mydatadir)
	$(INSTALL_DATA) $(srcdir)/et_h.awk $(DESTDIR)$(mydatadir)


install-headers:: compile_et

compile_et: $(srcdir)/compile_et.sh $(srcdir)/config_script
	$(SHELL) $(srcdir)/config_script $(srcdir)/compile_et.sh \
		"$(mydatadir)" $(AWK) $(SED) > compile_et
	chmod 755 compile_et	

rebuild: rebuild-c rebuild-h
rebuild-c:
	a2p < $(srcdir)/et_c.awk | awk 'NR != 1 || $$0 !~ /^\043!/{print;}' > $(srcdir)/et_c.tmp
	mv -f $(srcdir)/et_c.tmp $(srcdir)/et_c.pl
rebuild-h:
	a2p < $(srcdir)/et_h.awk | awk 'NR != 1 || $$0 !~ /^\043!/{print;}' > $(srcdir)/et_h.tmp
	mv -f $(srcdir)/et_h.tmp $(srcdir)/et_h.pl

clean-unix::
	$(RM) compile_et

depend:: 

install:: com_err.h 
	$(INSTALL_DATA) $(srcdir)/com_err.h  $(DESTDIR)$(KRB5_INCDIR)/com_err.h

install-unix:: compile_et.1
	$(INSTALL_DATA) $(srcdir)/compile_et.1 $(DESTDIR)$(CLIENT_MANDIR)/compile_et.1


## install_library_target(com_err,$(LIBOBJS),$(LINTFILES),)

clean-unix:: clean-files

clean-files::
	rm -f *~ \#* *.bak \
		*.otl *.aux *.toc *.PS *.dvi *.x9700 *.ps \
		*.cp *.fn *.ky *.log *.pg *.tp *.vr \
		*.o profiled/?*.o libcom_err.a libcom_err_p.a \
		com_err.o compile_et \
		et.ar TAGS y.tab.c lex.yy.c error_table.c \
		et_lex.lex.c \
		test1.h test1.c test2.h test2.c test_et \
		et1.c et1.h et2.c et2.h t_com_err \
		eddep makedep *.ln

clean-windows::
	$(RM) $(COM_ERR_HDR)

com_err.ps : com_err.dvi
com_err.dvi: com_err.texinfo

libcom_err.o:	$(LIBOBJS)
	ld -r -s -o libcom_err.o $(LIBOBJS)
	chmod -x libcom_err.o

archive:	et.tar

TAGS:	et_name.c error_message.c compile_et.c error_table.c \
		lex.yy.c init_et.c
	etags et_name.c error_message.c compile_et.c \
		error_table.c init_et.c

depend::  et_lex.lex.c


# @lib_frag@
# @libobj_frag@

# +++ Dependency line eater +++
# 
# Makefile dependencies follow.  This must be the last section in
# the Makefile.in file
#
error_message.so error_message.po $(OUTPRE)error_message.$(OBJEXT): error_message.c com_err.h \
  error_table.h
et_name.so et_name.po $(OUTPRE)et_name.$(OBJEXT): et_name.c com_err.h error_table.h
init_et.so init_et.po $(OUTPRE)init_et.$(OBJEXT): init_et.c com_err.h error_table.h
com_err.so com_err.po $(OUTPRE)com_err.$(OBJEXT): com_err.c com_err.h error_table.h

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
