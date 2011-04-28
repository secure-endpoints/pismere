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
myfulldir=lib
mydir=lib
MY_SUBDIRS=crypto krb5 des425 @KRB4@ kdb gssapi rpc kadm5
BUILDTOP=$(REL)..

MAC_SUBDIRS = crypto krb5 gssapi

all-unix::

all-mac::

CLEANLIBS = libkrb5.a libkdb5.a libcrypto.a libgssapi_krb5.a libdes425.a \
	libkrb425.a libkadm.a libkrb4.a libcom_err.a libpty.a \
	libss.a libgssapi.a \
	libkrb5.so libcrypto.so libkrb4.so libdes425.so

clean-mac:: clean-unix
	$(RM) $(CLEANLIBS)
clean-unix::

clean-windows::

# Windows stuff to make krb5 and gssapi DLLs.

##MIT##!if !defined(VS_INC)
##MIT##!message Must define VS_INC to point to version server include dir!
##MIT##!error
##MIT##!endif
##MIT##!if !defined(VS_LIB)
##MIT##!message Must define VS_LIB to point to version server library!
##MIT##!error
##MIT##!endif
##MIT##MITLIBS=$(VS_LIB)
##MIT##MITFLAGS=-I$(VS_INC) /DVERSERV=1

CLIBS = $(BUILDTOP)\util\et\$(OUTPRE)comerr.lib
PLIBS = $(BUILDTOP)\util\profile\$(OUTPRE)profile.lib
KLIBS = krb5\$(OUTPRE)krb5.lib crypto\$(OUTPRE)crypto.lib \
	$(BUILDTOP)\util\profile\$(OUTPRE)profile.lib \
	des425\$(OUTPRE)des425.lib
GLIBS = gssapi\$(OUTPRE)gssapi.lib
K4LIBS = krb4\$(OUTPRE)krb4.lib

CDEF = comerr32.def
PDEF = xpprof32.def
KDEF = krb5_32.def
GDEF = gssapi32.def
K4DEF = krb4_32.def

KRB5RC = krb5.rc
VERSIONRC = $(BUILDTOP)\windows\version.rc

WINLIBS = kernel32.lib ws2_32.lib user32.lib shell32.lib oldnames.lib \
	version.lib secur32.lib advapi32.lib gdi32.lib
WINDLLFLAGS = $(DLL_LINKOPTS) -base:0x1c000000

NO_GLUE=$(OUTPRE)no_glue.obj
K5_GLUE=$(OUTPRE)k5_glue.obj
K4_GLUE=$(OUTPRE)k4_glue.obj
GSS_GLUE=$(OUTPRE)gss_glue.obj

CGLUE=$(NO_GLUE)
PGLUE=$(NO_GLUE)
KGLUE=$(K5_GLUE)
GGLUE=$(GSS_GLUE)
K4GLUE=$(K4_GLUE)

RCFLAGS=$(CPPFLAGS) -D_WIN32 -DRES_ONLY

CRES=$(CLIB:.lib=.res)
PRES=$(PLIB:.lib=.res)
KRES=$(KLIB:.lib=.res)
GRES=$(GLIB:.lib=.res)
K4RES=$(K4LIB:.lib=.res)

$(CRES): $(VERSIONRC)
	$(RC) $(RCFLAGS) -DCE_LIB -fo $@ -r $**
$(PRES): $(VERSIONRC)
	$(RC) $(RCFLAGS) -DPROF_LIB -fo $@ -r $**
$(KRES): $(KRB5RC)
	$(RC) $(RCFLAGS) -DKRB5_LIB -fo $@ -r $**
$(K4RES): $(VERSIONRC)
	$(RC) $(RCFLAGS) -DKRB4_LIB -fo $@ -r $**
$(GRES): $(VERSIONRC)
	$(RC) $(RCFLAGS) -DGSSAPI_LIB -fo $@ -r $**
$(KRB5RC): $(VERSIONRC)

$(CLIB): $(CDEF) $(CLIBS) $(CGLUE) $(CRES)
	link $(WINDLLFLAGS) -def:$(CDEF) -out:$*.dll \
	  $(CLIBS) $(CGLUE) $(CRES) $(WINLIBS)

$(PLIB): $(PDEF) $(PLIBS) $(PGLUE) $(PRES) $(CLIB)
	link $(WINDLLFLAGS) -def:$(PDEF) -out:$*.dll \
	  $(PLIBS) $(PGLUE) $(PRES) $(CLIB) $(WINLIBS)

$(KLIB): $(KDEF) $(KLIBS) $(KGLUE) $(KRES) $(CLIB) $(MITLIBS) $(DNSLIBS)
	link $(WINDLLFLAGS) -def:$(KDEF) -out:$*.dll \
	  $(KLIBS) $(KGLUE) $(KRES) $(CLIB) $(MITLIBS) $(DNSLIBS) $(WINLIBS)

$(GLIB): $(GDEF) $(GLIBS) $(GGLUE) $(GRES) $(KLIB) $(CLIB)
	link $(WINDLLFLAGS) -def:$(GDEF) -out:$*.dll \
	  $(GLIBS) $(GGLUE) $(GRES) $(KLIB) $(CLIB) $(WINLIBS)

$(K4LIB): $(K4DEF) $(K4LIBS) $(K4GLUE) $(K4RES) $(KLIB) $(CLIB) $(PLIB)
	link $(WINDLLFLAGS) -def:$(K4DEF) -out:$*.dll \
	  $(K4LIBS) $(K4GLUE) $(K4RES) $(KLIB) $(CLIB) $(PLIB) $(WINLIBS)

$(K5_GLUE): win_glue.c
	$(CC) $(ALL_CFLAGS) $(MITFLAGS) /c /DKRB5=1 /Fo$@ $**
$(K4_GLUE): win_glue.c
	$(CC) $(ALL_CFLAGS) /c /DKRB4=1 /Fo$@ $**
$(GSS_GLUE): win_glue.c
	$(CC) $(ALL_CFLAGS) /c /DGSSAPI=1 /Fo$@ $**
$(NO_GLUE): win_glue.c
	$(CC) $(ALL_CFLAGS) /c /Fo$@ $**

# Build Convenience
comerr.lib: $(CLIB)
krb4.lib:   $(K4LIB)
krb5.lib:   $(KLIB)
gssapi.lib: $(GLIB)
profile.lib: $(PLIB)

all-windows:: 
	@echo Making in lib\crypto
	cd crypto
	$(MAKE) -$(MFLAGS)
	@echo Making in lib\krb5
	cd ..\krb5
	$(MAKE) -$(MFLAGS)
	@echo Making in lib\des425
	cd ..\des425
	$(MAKE) -$(MFLAGS) 
	@echo Making in lib\krb4
	cd ..\krb4
	$(MAKE) -$(MFLAGS) 
	@echo Making in lib\gssapi
	cd ..\gssapi
	$(MAKE) -$(MFLAGS) 
	@echo Making in lib
	cd ..

all-windows:: lib-windows
lib-windows:: krb5.lib gssapi.lib krb4.lib 

clean-windows::
	@echo Making clean in lib\crypto
	cd crypto
	$(MAKE) -$(MFLAGS) clean
	@echo Making clean in lib\krb5
	cd ..\krb5
	$(MAKE) -$(MFLAGS) clean
	@echo Making clean in lib\des425
	cd ..\des425
	$(MAKE) -$(MFLAGS) clean
	@echo Making clean in lib\krb4
	cd ..\krb4
	$(MAKE) -$(MFLAGS) clean
	@echo Making clean in lib\gssapi
	cd ..\gssapi
	$(MAKE) -$(MFLAGS) clean
	@echo Making clean in lib
	cd ..
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
