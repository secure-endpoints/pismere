
Building Kerberos for Windows
=============================================


1. Prerequisites
----------------

The following are the pre-requisites for building Kerberos for Windows:

* The Pismere repository with the krb5 repository grafted into
  src/athena/auth/krb5 and the file src/scripts/site/graft/krb5/Makefile.src
  copied into the src/athena/auth/krb5 directory

* __Visual C++ Compiler__: Kerberos for Windows has been tested
  with Microsoft Visual C/C++ compiler version 13.x and 14.x.
  These correspond to Microsoft Visual Studio versions 2003 and 2005,
  although some of these compilers are also shipped with Visual
  Studio express editions and newer versions of Windows Platform SDKs.

* __Windows Platform SDK__: Tested with Microsoft Platform SDK version
  XP SP2, Server 2003 SP1, and 6.1.  Later SDKs should also work though
  they are not required.

* __Perl__: A recent version of Perl.  Tested with ActiveState
  ActivePerl.

* __NSIS__: The Nullsoft Installer toolkit 2.4.6.  Used to build an
  EXE installer appropriate for individual machine installers.  NSIS
  is capable of only producing installers for 32-bit versions of
  Windows.

* __WiX__: The Windows [Installer XML toolkit (WiX)][1] Version 3.x is
  used to build the installers.  Not needed if you don't need to build
  installers.

[1]: http://wix.sourceforge.net/

[2]: http://github.com/secure-endpoints/pismere

[3]: http://github.com/secure-endpoints/mit-krb5


The pismere documentation including build directions
is located in src/doc/kerberos/relnotes.html



