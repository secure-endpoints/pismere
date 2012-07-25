@echo off
if "%1" == "" goto usage
if "%2" == "release" goto perform_install
if "%2" == "debug" goto perform_install
if NOT "%2" == "" goto usage

:perform_install
if exist %1\bin\i386 rd /s /q %1\bin\i386
if exist %1\lib\i386 rd /s /q %1\lib\i386
mkdir  %1
mkdir  %1\doc
copy ..\doc\kerberos\relnotes.html      %1\doc\relnotes.html
copy auth\leash\help\leash_userdoc.pdf  %1\doc\leash_userdoc.pdf
copy auth\krb5\src\windows\identity\doc\NetIdMgr.pdf %1\doc\netidmgr_userdoc.pdf
copy auth\krb4\kclient\doc\kclient.rtf  %1\doc\kclient.rtf
copy auth\krb5\src\windows\installer\wix\msi-deployment-guide.txt %1\doc\msi-deployment-guide.txt
mkdir %1\sample
mkdir %1\sample\templates
mkdir %1\sample\templates\credprov
copy auth\krb5\src\windows\identity\sample\templates\credprov\* %1\sample\templates\credprov
mkdir %1\sample\templates\credprov\images
copy auth\krb5\src\windows\identity\sample\templates\credprov\images\* %1\sample\templates\credprov\images
mkdir %1\sample\templates\credprov\lang
copy auth\krb5\src\windows\identity\sample\templates\credprov\lang\* %1\sample\templates\credprov\lang
mkdir %1\sample\templates\credprov\lang\en_us
copy auth\krb5\src\windows\identity\sample\templates\credprov\lang\en_us\* %1\sample\templates\credprov\lang\en_us
mkdir  %1\bin
mkdir  %1\bin\i386
if "%2" == "debug" goto debug
copy auth\krb5\src\windows\identity\obj\i386\rel\doc\NetIddev.chm %1\doc\netiddev.chm

copy ..\target\bin\i386\rel\comerr32.dll    %1\bin\i386\comerr32.dll
copy ..\target\bin\i386\rel\gss.exe         %1\bin\i386\gss.exe
copy ..\target\bin\i386\rel\gss-client.exe  %1\bin\i386\gss-client.exe
copy ..\target\bin\i386\rel\gss-server.exe  %1\bin\i386\gss-server.exe
copy ..\target\bin\i386\rel\gssapi32.dll    %1\bin\i386\gssapi32.dll
copy ..\target\bin\i386\rel\k524init.exe    %1\bin\i386\k524init.exe
copy ..\target\bin\i386\rel\kclnt32.dll     %1\bin\i386\kclnt32.dll
copy ..\target\bin\i386\rel\kdestroy.exe    %1\bin\i386\kdestroy.exe
copy ..\target\bin\i386\rel\kinit.exe       %1\bin\i386\kinit.exe
copy ..\target\bin\i386\rel\klist.exe       %1\bin\i386\klist.exe
copy ..\target\bin\i386\rel\krb524.dll      %1\bin\i386\krb524.dll
copy ..\target\bin\i386\rel\krb5_32.dll     %1\bin\i386\krb5_32.dll
copy ..\target\bin\i386\rel\k5sprt32.dll    %1\bin\i386\k5sprt32.dll
copy ..\target\bin\i386\rel\krbcc32.dll     %1\bin\i386\krbcc32.dll
copy ..\target\bin\i386\rel\krbcc32s.exe    %1\bin\i386\krbcc32s.exe
copy ..\target\bin\i386\rel\krbv4w32.dll    %1\bin\i386\krbv4w32.dll
copy ..\target\bin\i386\rel\kvno.exe        %1\bin\i386\kvno.exe
copy ..\target\bin\i386\rel\kpasswd.exe     %1\bin\i386\kpasswd.exe
copy ..\target\bin\i386\rel\leash32.chm     %1\bin\i386\leash32.chm
copy ..\target\bin\i386\rel\leash32.exe     %1\bin\i386\leash32.exe
copy ..\target\bin\i386\rel\leashw32.dll    %1\bin\i386\leashw32.dll
copy ..\target\bin\i386\rel\ms2mit.exe      %1\bin\i386\ms2mit.exe
copy ..\target\bin\i386\rel\wshelp32.dll    %1\bin\i386\wshelp32.dll
copy ..\target\bin\i386\rel\xpprof32.dll    %1\bin\i386\xpprof32.dll
copy ..\target\bin\i386\rel\kcpytkt.exe     %1\bin\i386\kcpytkt.exe
copy ..\target\bin\i386\rel\kdeltkt.exe     %1\bin\i386\kdeltkt.exe
copy ..\target\bin\i386\rel\mit2ms.exe      %1\bin\i386\mit2ms.exe
copy ..\target\bin\i386\rel\krb5cred.dll       %1\bin\i386\krb5cred.dll        
copy ..\target\bin\i386\rel\krb5cred_en_us.dll %1\bin\i386\krb5cred_en_us.dll  
copy ..\target\bin\i386\rel\krb4cred.dll       %1\bin\i386\krb4cred.dll        
copy ..\target\bin\i386\rel\krb4cred_en_us.dll %1\bin\i386\krb4cred_en_us.dll  
copy ..\target\bin\i386\rel\nidmgr32.dll       %1\bin\i386\nidmgr32.dll        
copy ..\target\bin\i386\rel\netidmgr.exe       %1\bin\i386\netidmgr.exe        
copy ..\target\bin\i386\rel\netidmgr.chm       %1\bin\i386\netidmgr.chm
copy ..\target\bin\i386\rel\netiddev.chm       %1\doc\netiddev.chm
copy ..\target\bin\i386\rel\kfwlogon.dll       %1\bin\i386\kfwlogon.dll
copy ..\target\bin\i386\rel\kfwcpcc.exe        %1\bin\i386\kfwcpcc.exe
mkdir %1\bin\i386\w2k
copy auth\krb5\src\windows\identity\obj\i386\rel\w2k\nidmgr32.dll %1\bin\i386\w2k\nidmgr32.dll
copy auth\krb5\src\windows\identity\obj\i386\rel\w2k\netidmgr.exe %1\bin\i386\w2k\netidmgr.exe

copy ..\target\bin\i386\rel\comerr32.pdb    %1\bin\i386\comerr32.pdb
copy ..\target\bin\i386\rel\gss.pdb         %1\bin\i386\gss.pdb
copy ..\target\bin\i386\rel\gss-client.pdb  %1\bin\i386\gss-client.pdb
copy ..\target\bin\i386\rel\gss-server.pdb  %1\bin\i386\gss-server.pdb
copy ..\target\bin\i386\rel\gssapi32.pdb    %1\bin\i386\gssapi32.pdb
copy ..\target\bin\i386\rel\k524init.pdb    %1\bin\i386\k524init.pdb
copy ..\target\bin\i386\rel\kclnt32.pdb     %1\bin\i386\kclnt32.pdb
copy ..\target\bin\i386\rel\kdestroy.pdb    %1\bin\i386\kdestroy.pdb
copy ..\target\bin\i386\rel\kinit.pdb       %1\bin\i386\kinit.pdb
copy ..\target\bin\i386\rel\klist.pdb       %1\bin\i386\klist.pdb
copy ..\target\bin\i386\rel\krb524.pdb      %1\bin\i386\krb524.pdb
copy ..\target\bin\i386\rel\krb5_32.pdb     %1\bin\i386\krb5_32.pdb
copy ..\target\bin\i386\rel\k5sprt32.pdb    %1\bin\i386\k5sprt32.pdb
copy ..\target\bin\i386\rel\krbcc32.pdb     %1\bin\i386\krbcc32.pdb
copy ..\target\bin\i386\rel\krbcc32s.pdb    %1\bin\i386\krbcc32s.pdb
copy ..\target\bin\i386\rel\krbv4w32.pdb    %1\bin\i386\krbv4w32.pdb
copy ..\target\bin\i386\rel\kvno.pdb        %1\bin\i386\kvno.pdb
copy ..\target\bin\i386\rel\kpasswd.pdb     %1\bin\i386\kpasswd.pdb
copy ..\target\bin\i386\rel\leash32.chm     %1\bin\i386\leash32.chm
copy ..\target\bin\i386\rel\leash32.pdb     %1\bin\i386\leash32.pdb
copy ..\target\bin\i386\rel\leashw32.pdb    %1\bin\i386\leashw32.pdb
copy ..\target\bin\i386\rel\ms2mit.pdb      %1\bin\i386\ms2mit.pdb
copy ..\target\bin\i386\rel\wshelp32.pdb    %1\bin\i386\wshelp32.pdb
copy ..\target\bin\i386\rel\xpprof32.pdb    %1\bin\i386\xpprof32.pdb
copy ..\target\bin\i386\rel\kcpytkt.pdb     %1\bin\i386\kcpytkt.pdb
copy ..\target\bin\i386\rel\kdeltkt.pdb     %1\bin\i386\kdeltkt.pdb
copy ..\target\bin\i386\rel\mit2ms.pdb      %1\bin\i386\mit2ms.pdb
copy ..\target\bin\i386\rel\krb5cred.pdb       %1\bin\i386\krb5cred.pdb        
copy ..\target\bin\i386\rel\krb4cred.pdb       %1\bin\i386\krb4cred.pdb        
copy ..\target\bin\i386\rel\nidmgr32.pdb       %1\bin\i386\nidmgr32.pdb
copy ..\target\bin\i386\rel\netidmgr.pdb       %1\bin\i386\netidmgr.pdb        
copy ..\target\bin\i386\rel\kfwlogon.pdb       %1\bin\i386\kfwlogon.pdb
copy ..\target\bin\i386\rel\kfwcpcc.pdb        %1\bin\i386\kfwcpcc.pdb        
copy auth\krb5\src\windows\identity\obj\i386\rel\w2k\nidmgr32.pdb %1\bin\i386\w2k\nidmgr32.pdb
copy auth\krb5\src\windows\identity\obj\i386\rel\w2k\netidmgr.pdb %1\bin\i386\w2k\netidmgr.pdb

mkdir  %1\lib
mkdir  %1\lib\i386
copy ..\target\lib\i386\rel\comerr32.lib    %1\lib\i386\comerr32.lib
copy ..\target\lib\i386\rel\comerr32.a      %1\lib\i386\comerr32.a
copy ..\target\lib\i386\rel\delaydlls.lib   %1\lib\i386\delaydlls.lib
copy ..\target\lib\i386\rel\getopt.lib      %1\lib\i386\getopt.lib
copy ..\target\lib\i386\rel\gssapi32.lib    %1\lib\i386\gssapi32.lib
copy ..\target\lib\i386\rel\gssapi32.a      %1\lib\i386\gssapi32.a
copy ..\target\lib\i386\rel\kclnt32.lib     %1\lib\i386\kclnt32.lib
copy ..\target\lib\i386\rel\krb524.lib      %1\lib\i386\krb524.lib
copy ..\target\lib\i386\rel\krb5_32.lib     %1\lib\i386\krb5_32.lib
copy ..\target\lib\i386\rel\krb5_32.a       %1\lib\i386\krb5_32.a
copy ..\target\lib\i386\rel\krbcc32.lib     %1\lib\i386\krbcc32.lib
copy ..\target\lib\i386\rel\krbv4w32.lib    %1\lib\i386\krbv4w32.lib
copy ..\target\lib\i386\rel\leashw32.lib    %1\lib\i386\leashw32.lib
copy ..\target\lib\i386\rel\loadfuncs.lib   %1\lib\i386\loadfuncs.lib
copy ..\target\lib\i386\rel\wshelp32.lib    %1\lib\i386\wshelp32.lib
copy ..\target\lib\i386\rel\xpprof32.lib    %1\lib\i386\xpprof32.lib
copy ..\target\lib\i386\rel\xpprof32.a      %1\lib\i386\xpprof32.a
copy ..\target\lib\i386\rel\nidmgr32.lib    %1\lib\i386\nidmgr32.lib
goto headers

:debug
copy auth\krb5\src\windows\identity\obj\i386\dbg\doc\NetIddev.chm %1\doc\netiddev.chm

copy ..\target\bin\i386\dbg\comerr32.dll    %1\bin\i386\comerr32.dll
copy ..\target\bin\i386\dbg\gss.exe         %1\bin\i386\gss.exe
copy ..\target\bin\i386\dbg\gss-client.exe  %1\bin\i386\gss-client.exe
copy ..\target\bin\i386\dbg\gss-server.exe  %1\bin\i386\gss-server.exe
copy ..\target\bin\i386\dbg\gssapi32.dll    %1\bin\i386\gssapi32.dll
copy ..\target\bin\i386\dbg\k524init.exe    %1\bin\i386\k524init.exe
copy ..\target\bin\i386\dbg\kclnt32.dll     %1\bin\i386\kclnt32.dll
copy ..\target\bin\i386\dbg\kdestroy.exe    %1\bin\i386\kdestroy.exe
copy ..\target\bin\i386\dbg\kinit.exe       %1\bin\i386\kinit.exe
copy ..\target\bin\i386\dbg\klist.exe       %1\bin\i386\klist.exe
copy ..\target\bin\i386\dbg\krb524.dll      %1\bin\i386\krb524.dll
copy ..\target\bin\i386\dbg\krb5_32.dll     %1\bin\i386\krb5_32.dll
copy ..\target\bin\i386\dbg\k5sprt32.dll    %1\bin\i386\k5sprt32.dll
copy ..\target\bin\i386\dbg\krbcc32.dll     %1\bin\i386\krbcc32.dll
copy ..\target\bin\i386\dbg\krbcc32s.exe    %1\bin\i386\krbcc32s.exe
copy ..\target\bin\i386\dbg\krbv4w32.dll    %1\bin\i386\krbv4w32.dll
copy ..\target\bin\i386\dbg\kvno.exe        %1\bin\i386\kvno.exe
copy ..\target\bin\i386\dbg\kpasswd.exe     %1\bin\i386\kpasswd.exe
copy ..\target\bin\i386\dbg\leash32.chm     %1\bin\i386\leash32.chm
copy ..\target\bin\i386\dbg\leash32.exe     %1\bin\i386\leash32.exe
copy ..\target\bin\i386\dbg\leashw32.dll    %1\bin\i386\leashw32.dll
copy ..\target\bin\i386\dbg\ms2mit.exe      %1\bin\i386\ms2mit.exe
copy ..\target\bin\i386\dbg\wshelp32.dll    %1\bin\i386\wshelp32.dll
copy ..\target\bin\i386\dbg\xpprof32.dll    %1\bin\i386\xpprof32.dll
copy ..\target\bin\i386\dbg\kcpytkt.exe     %1\bin\i386\kcpytkt.exe
copy ..\target\bin\i386\dbg\kdeltkt.exe     %1\bin\i386\kdeltkt.exe
copy ..\target\bin\i386\dbg\mit2ms.exe      %1\bin\i386\mit2ms.exe
copy ..\target\bin\i386\dbg\krb5cred.dll    %1\bin\i386\krb5cred.dll        
copy ..\target\bin\i386\dbg\krb5cred_en_us.dll  %1\bin\i386\krb5cred_en_us.dll  
copy ..\target\bin\i386\dbg\krb4cred.dll    %1\bin\i386\krb4cred.dll        
copy ..\target\bin\i386\dbg\krb4cred_en_us.dll  %1\bin\i386\krb4cred_en_us.dll  
copy ..\target\bin\i386\dbg\nidmgr32.dll    %1\bin\i386\nidmgr32.dll
copy ..\target\bin\i386\dbg\netidmgr.exe    %1\bin\i386\netidmgr.exe        
copy ..\target\bin\i386\dbg\netidmgr.chm    %1\bin\i386\netidmgr.chm
copy ..\target\bin\i386\dbg\netiddev.chm    %1\doc\netiddev.chm
copy ..\target\bin\i386\dbg\kfwlogon.dll    %1\bin\i386\kfwlogon.dll
copy ..\target\bin\i386\dbg\kfwcpcc.exe     %1\bin\i386\kfwcpcc.exe
mkdir %1\bin\i386\w2k
copy auth\krb5\src\windows\identity\obj\i386\dbg\w2k\nidmgr32.dll %1\bin\i386\w2k\nidmgr32.dll
copy auth\krb5\src\windows\identity\obj\i386\dbg\w2k\netidmgr.exe %1\bin\i386\w2k\netidmgr.exe

copy ..\target\bin\i386\dbg\comerr32.pdb    %1\bin\i386\comerr32.pdb
copy ..\target\bin\i386\dbg\gss.pdb         %1\bin\i386\gss.pdb
copy ..\target\bin\i386\dbg\gss-client.pdb  %1\bin\i386\gss-client.pdb
copy ..\target\bin\i386\dbg\gss-server.pdb  %1\bin\i386\gss-server.pdb
copy ..\target\bin\i386\dbg\gssapi32.pdb    %1\bin\i386\gssapi32.pdb
copy ..\target\bin\i386\dbg\k524init.pdb    %1\bin\i386\k524init.pdb
copy ..\target\bin\i386\dbg\kclnt32.pdb     %1\bin\i386\kclnt32.pdb
copy ..\target\bin\i386\dbg\kdestroy.pdb    %1\bin\i386\kdestroy.pdb
copy ..\target\bin\i386\dbg\kinit.pdb       %1\bin\i386\kinit.pdb
copy ..\target\bin\i386\dbg\klist.pdb       %1\bin\i386\klist.pdb
copy ..\target\bin\i386\dbg\krb524.pdb      %1\bin\i386\krb524.pdb
copy ..\target\bin\i386\dbg\krb5_32.pdb     %1\bin\i386\krb5_32.pdb
copy ..\target\bin\i386\dbg\k5sprt32.pdb    %1\bin\i386\k5sprt32.pdb
copy ..\target\bin\i386\dbg\krbcc32.pdb     %1\bin\i386\krbcc32.pdb
copy ..\target\bin\i386\dbg\krbcc32s.pdb    %1\bin\i386\krbcc32s.pdb
copy ..\target\bin\i386\dbg\krbv4w32.pdb    %1\bin\i386\krbv4w32.pdb
copy ..\target\bin\i386\dbg\kvno.pdb        %1\bin\i386\kvno.pdb
copy ..\target\bin\i386\dbg\kpasswd.pdb     %1\bin\i386\kpasswd.pdb
copy ..\target\bin\i386\dbg\leash32.chm     %1\bin\i386\leash32.chm
copy ..\target\bin\i386\dbg\leash32.pdb     %1\bin\i386\leash32.pdb
copy ..\target\bin\i386\dbg\leashw32.pdb    %1\bin\i386\leashw32.pdb
copy ..\target\bin\i386\dbg\ms2mit.pdb      %1\bin\i386\ms2mit.pdb
copy ..\target\bin\i386\dbg\wshelp32.pdb    %1\bin\i386\wshelp32.pdb
copy ..\target\bin\i386\dbg\xpprof32.pdb    %1\bin\i386\xpprof32.pdb
copy ..\target\bin\i386\dbg\kcpytkt.pdb     %1\bin\i386\kcpytkt.pdb
copy ..\target\bin\i386\dbg\kdeltkt.pdb     %1\bin\i386\kdeltkt.pdb
copy ..\target\bin\i386\dbg\mit2ms.pdb      %1\bin\i386\mit2ms.pdb
copy ..\target\bin\i386\dbg\krb5cred.pdb    %1\bin\i386\krb5cred.pdb        
copy ..\target\bin\i386\dbg\krb4cred.pdb    %1\bin\i386\krb4cred.pdb        
copy ..\target\bin\i386\dbg\nidmgr32.pdb    %1\bin\i386\nidmgr32.pdb
copy ..\target\bin\i386\dbg\netidmgr.pdb    %1\bin\i386\netidmgr.pdb        
copy ..\target\bin\i386\dbg\kfwlogon.pdb    %1\bin\i386\kfwlogon.pdb
copy ..\target\bin\i386\dbg\kfwcpcc.pdb     %1\bin\i386\kfwcpcc.pdb
copy auth\krb5\src\windows\identity\obj\i386\dbg\w2k\nidmgr32.pdb %1\bin\i386\w2k\nidmgr32.pdb
copy auth\krb5\src\windows\identity\obj\i386\dbg\w2k\netidmgr.pdb %1\bin\i386\w2k\netidmgr.pdb

mkdir  %1\lib
mkdir  %1\lib\i386
copy ..\target\lib\i386\dbg\comerr32.lib    %1\lib\i386\comerr32.lib
copy ..\target\lib\i386\dbg\delaydlls.lib   %1\lib\i386\delaydlls.lib
copy ..\target\lib\i386\dbg\getopt.lib      %1\lib\i386\getopt.lib
copy ..\target\lib\i386\dbg\gssapi32.lib    %1\lib\i386\gssapi32.lib
copy ..\target\lib\i386\dbg\kclnt32.lib     %1\lib\i386\kclnt32.lib
copy ..\target\lib\i386\dbg\krb524.lib      %1\lib\i386\krb524.lib
copy ..\target\lib\i386\dbg\krb5_32.lib     %1\lib\i386\krb5_32.lib
copy ..\target\lib\i386\dbg\krbcc32.lib     %1\lib\i386\krbcc32.lib
copy ..\target\lib\i386\dbg\krbv4w32.lib    %1\lib\i386\krbv4w32.lib
copy ..\target\lib\i386\dbg\leashw32.lib    %1\lib\i386\leashw32.lib
copy ..\target\lib\i386\dbg\loadfuncs.lib   %1\lib\i386\loadfuncs.lib
copy ..\target\lib\i386\dbg\wshelp32.lib    %1\lib\i386\wshelp32.lib
copy ..\target\lib\i386\dbg\xpprof32.lib    %1\lib\i386\xpprof32.lib
copy ..\target\lib\i386\dbg\nidmgr32.lib    %1\lib\i386\nidmgr32.lib

:headers
mkdir  %1\inc
mkdir  %1\inc\kclient
copy auth\krb4\kclient\include\kclient.h  %1\inc\kclient\kclient.h
copy auth\krb4\kclient\include\kcmacerr.h  %1\inc\kclient\kcmacerr.h
mkdir  %1\inc\krb4\
copy auth\krb4\include\com_err.h   %1\inc\krb4\com_err.h
copy auth\krb4\include\conf-pc.h   %1\inc\krb4\conf-pc.h
copy auth\krb4\include\conf.h      %1\inc\krb4\conf.h
copy auth\krb4\include\des.h       %1\inc\krb4\des.h
copy auth\krb4\include\kadm_err.h  %1\inc\krb4\kadm_err.h
copy auth\krb4\include\krb.h       %1\inc\krb4\krb.h
copy auth\krb4\include\krberr.h    %1\inc\krb4\krberr.h
copy auth\krb4\include\mit_copy.h  %1\inc\krb4\mit_copy.h
copy auth\krb4\include\osconf.h    %1\inc\krb4\osconf.h
mkdir  %1\inc\krb5\
copy auth\krb5\src\include\com_err.h  %1\inc\krb5\com_err.h
mkdir  %1\inc\krb5\gssapi\
copy auth\krb5\src\include\gssapi\gssapi.h          %1\inc\krb5\gssapi\gssapi.h
copy auth\krb5\src\include\gssapi\gssapi_generic.h  %1\inc\krb5\gssapi\gssapi_generic.h
copy auth\krb5\src\include\gssapi\gssapi_krb5.h     %1\inc\krb5\gssapi\gssapi_krb5.h
mkdir %1\inc\krb5\KerberosIV\
copy auth\krb5\src\include\KerberosIV\des.h            %1\inc\krb5\KerberosIV\des.h
copy auth\krb5\src\include\KerberosIV\kadm_err.h       %1\inc\krb5\KerberosIV\kadm_err.h
copy auth\krb5\src\include\KerberosIV\krb_err.h        %1\inc\krb5\KerberosIV\krb_err.h
copy auth\krb5\src\include\KerberosIV\krb.h            %1\inc\krb5\KerberosIV\krb.h
copy auth\krb5\src\include\KerberosIV\mit-copyright.h  %1\inc\krb5\KerberosIV\mit-copyright.h
copy auth\krb5\src\include\krb5.h      %1\inc\krb5\krb5.h
copy auth\krb5\src\include\profile.h   %1\inc\krb5\profile.h
copy auth\krb5\src\include\win-mac.h   %1\inc\krb5\win-mac.h
mkdir  %1\inc\krb5\krb5
copy auth\krb5\src\include\krb5\krb5.h %1\inc\krb5\krb5\krb5.h
mkdir  %1\inc\krbcc\
copy auth\krbcc\include\cacheapi.h  %1\inc\krbcc\cacheapi.h
mkdir  %1\inc\leash\
copy auth\leash\include\leasherr.h    %1\inc\leash\leasherr.h
copy auth\leash\include\leashinfo.h   %1\inc\leash\leashinfo.h
copy auth\leash\include\leashwin.h    %1\inc\leash\leashwin.h
mkdir  %1\inc\loadfuncs\
copy util\loadfuncs\loadfuncs-afs.h       %1\inc\loadfuncs\loadfuncs-afs.h
copy util\loadfuncs\loadfuncs-afs36.h     %1\inc\loadfuncs\loadfuncs-afs36.h
copy util\loadfuncs\loadfuncs-com_err.h   %1\inc\loadfuncs\loadfuncs-com_err.h
copy util\loadfuncs\loadfuncs-krb.h       %1\inc\loadfuncs\loadfuncs-krb.h
copy util\loadfuncs\loadfuncs-krb5.h      %1\inc\loadfuncs\loadfuncs-krb5.h
copy util\loadfuncs\loadfuncs-krb524.h    %1\inc\loadfuncs\loadfuncs-krb524.h
copy util\loadfuncs\loadfuncs-leash.h     %1\inc\loadfuncs\loadfuncs-leash.h
copy util\loadfuncs\loadfuncs-lsa.h       %1\inc\loadfuncs\loadfuncs-lsa.h
copy util\loadfuncs\loadfuncs-profile.h   %1\inc\loadfuncs\loadfuncs-profile.h
copy util\loadfuncs\loadfuncs-wshelper.h  %1\inc\loadfuncs\loadfuncs-wshelper.h
copy util\loadfuncs\loadfuncs.c           %1\inc\loadfuncs\loadfuncs.c
copy util\loadfuncs\loadfuncs.h           %1\inc\loadfuncs\loadfuncs.h
mkdir  %1\inc\wshelper\
mkdir  %1\inc\wshelper\arpa\
copy wshelper\include\arpa\nameser.h  %1\inc\wshelper\arpa\nameser.h
copy wshelper\include\hesiod.h        %1\inc\wshelper\hesiod.h
copy wshelper\include\mitwhich.h      %1\inc\wshelper\mitwhich.h
copy wshelper\include\resolv.h        %1\inc\wshelper\resolv.h
copy wshelper\include\wshelper.h      %1\inc\wshelper\wshelper.h

mkdir  %1\inc\netidmgr
if "%2" == "debug" goto debug_netidmgr_h
copy auth\krb5\src\windows\identity\obj\i386\rel\inc\hashtable.h     %1\inc\netidmgr\hashtable.h     
copy auth\krb5\src\windows\identity\obj\i386\rel\inc\kconfig.h       %1\inc\netidmgr\kconfig.h       
copy auth\krb5\src\windows\identity\obj\i386\rel\inc\kcreddb.h 	     %1\inc\netidmgr\kcreddb.h       
copy auth\krb5\src\windows\identity\obj\i386\rel\inc\khaction.h      %1\inc\netidmgr\khaction.h      
copy auth\krb5\src\windows\identity\obj\i386\rel\inc\khactiondef.h   %1\inc\netidmgr\khactiondef.h   
copy auth\krb5\src\windows\identity\obj\i386\rel\inc\khalerts.h      %1\inc\netidmgr\khalerts.h      
copy auth\krb5\src\windows\identity\obj\i386\rel\inc\khconfigui.h    %1\inc\netidmgr\khconfigui.h    
copy auth\krb5\src\windows\identity\obj\i386\rel\inc\khdefs.h 	     %1\inc\netidmgr\khdefs.h        
copy auth\krb5\src\windows\identity\obj\i386\rel\inc\kherr.h 	     %1\inc\netidmgr\kherr.h         
copy auth\krb5\src\windows\identity\obj\i386\rel\inc\kherror.h 	     %1\inc\netidmgr\kherror.h       
copy auth\krb5\src\windows\identity\obj\i386\rel\inc\khhtlink.h      %1\inc\netidmgr\khhtlink.h      
copy auth\krb5\src\windows\identity\obj\i386\rel\inc\khlist.h 	     %1\inc\netidmgr\khlist.h        
copy auth\krb5\src\windows\identity\obj\i386\rel\inc\khmsgtypes.h    %1\inc\netidmgr\khmsgtypes.h    
copy auth\krb5\src\windows\identity\obj\i386\rel\inc\khnewcred.h     %1\inc\netidmgr\khnewcred.h     
copy auth\krb5\src\windows\identity\obj\i386\rel\inc\khprops.h       %1\inc\netidmgr\khprops.h       
copy auth\krb5\src\windows\identity\obj\i386\rel\inc\khremote.h      %1\inc\netidmgr\khremote.h      
copy auth\krb5\src\windows\identity\obj\i386\rel\inc\khrescache.h    %1\inc\netidmgr\khrescache.h    
copy auth\krb5\src\windows\identity\obj\i386\rel\inc\khtracker.h     %1\inc\netidmgr\khtracker.h     
copy auth\krb5\src\windows\identity\obj\i386\rel\inc\khuidefs.h      %1\inc\netidmgr\khuidefs.h      
copy auth\krb5\src\windows\identity\obj\i386\rel\inc\kmm.h 	     %1\inc\netidmgr\kmm.h           
copy auth\krb5\src\windows\identity\obj\i386\rel\inc\kmq.h 	     %1\inc\netidmgr\kmq.h           
copy auth\krb5\src\windows\identity\obj\i386\rel\inc\kplugin.h 	     %1\inc\netidmgr\kplugin.h       
copy auth\krb5\src\windows\identity\obj\i386\rel\inc\mstring.h 	     %1\inc\netidmgr\mstring.h       
copy auth\krb5\src\windows\identity\obj\i386\rel\inc\sync.h 	     %1\inc\netidmgr\sync.h          
copy auth\krb5\src\windows\identity\obj\i386\rel\inc\utils.h 	     %1\inc\netidmgr\utils.h
copy auth\krb5\src\windows\identity\obj\i386\rel\inc\perfstat.h 	     %1\inc\netidmgr\perfstat.h
copy auth\krb5\src\windows\identity\obj\i386\rel\inc\netidmgr_version.h 	     %1\inc\netidmgr\netidmgr_version.h
copy auth\krb5\src\windows\identity\obj\i386\rel\inc\netidmgr.h      %1\inc\netidmgr\netidmgr.h
goto netidmgr_h_done
:debug_netidmgr_h
copy auth\krb5\src\windows\identity\obj\i386\dbg\inc\hashtable.h     %1\inc\netidmgr\hashtable.h     
copy auth\krb5\src\windows\identity\obj\i386\dbg\inc\kconfig.h       %1\inc\netidmgr\kconfig.h       
copy auth\krb5\src\windows\identity\obj\i386\dbg\inc\kcreddb.h 	     %1\inc\netidmgr\kcreddb.h       
copy auth\krb5\src\windows\identity\obj\i386\dbg\inc\khaction.h      %1\inc\netidmgr\khaction.h      
copy auth\krb5\src\windows\identity\obj\i386\dbg\inc\khactiondef.h   %1\inc\netidmgr\khactiondef.h   
copy auth\krb5\src\windows\identity\obj\i386\dbg\inc\khalerts.h      %1\inc\netidmgr\khalerts.h      
copy auth\krb5\src\windows\identity\obj\i386\dbg\inc\khconfigui.h    %1\inc\netidmgr\khconfigui.h    
copy auth\krb5\src\windows\identity\obj\i386\dbg\inc\khdefs.h 	     %1\inc\netidmgr\khdefs.h        
copy auth\krb5\src\windows\identity\obj\i386\dbg\inc\kherr.h 	     %1\inc\netidmgr\kherr.h         
copy auth\krb5\src\windows\identity\obj\i386\dbg\inc\kherror.h 	     %1\inc\netidmgr\kherror.h       
copy auth\krb5\src\windows\identity\obj\i386\dbg\inc\khhtlink.h      %1\inc\netidmgr\khhtlink.h      
copy auth\krb5\src\windows\identity\obj\i386\dbg\inc\khlist.h 	     %1\inc\netidmgr\khlist.h        
copy auth\krb5\src\windows\identity\obj\i386\dbg\inc\khmsgtypes.h    %1\inc\netidmgr\khmsgtypes.h    
copy auth\krb5\src\windows\identity\obj\i386\dbg\inc\khnewcred.h     %1\inc\netidmgr\khnewcred.h     
copy auth\krb5\src\windows\identity\obj\i386\dbg\inc\khprops.h       %1\inc\netidmgr\khprops.h       
copy auth\krb5\src\windows\identity\obj\i386\dbg\inc\khremote.h      %1\inc\netidmgr\khremote.h      
copy auth\krb5\src\windows\identity\obj\i386\dbg\inc\khrescache.h    %1\inc\netidmgr\khrescache.h    
copy auth\krb5\src\windows\identity\obj\i386\dbg\inc\khtracker.h     %1\inc\netidmgr\khtracker.h     
copy auth\krb5\src\windows\identity\obj\i386\dbg\inc\khuidefs.h      %1\inc\netidmgr\khuidefs.h      
copy auth\krb5\src\windows\identity\obj\i386\dbg\inc\kmm.h 	     %1\inc\netidmgr\kmm.h           
copy auth\krb5\src\windows\identity\obj\i386\dbg\inc\kmq.h 	     %1\inc\netidmgr\kmq.h           
copy auth\krb5\src\windows\identity\obj\i386\dbg\inc\kplugin.h 	     %1\inc\netidmgr\kplugin.h       
copy auth\krb5\src\windows\identity\obj\i386\dbg\inc\mstring.h 	     %1\inc\netidmgr\mstring.h       
copy auth\krb5\src\windows\identity\obj\i386\dbg\inc\sync.h 	     %1\inc\netidmgr\sync.h          
copy auth\krb5\src\windows\identity\obj\i386\dbg\inc\utils.h 	     %1\inc\netidmgr\utils.h         
copy auth\krb5\src\windows\identity\obj\i386\dbg\inc\perfstat.h 	     %1\inc\netidmgr\perfstat.h
copy auth\krb5\src\windows\identity\obj\i386\dbg\inc\netidmgr_version.h 	     %1\inc\netidmgr\netidmgr_version.h
copy auth\krb5\src\windows\identity\obj\i386\dbg\inc\netidmgr.h      %1\inc\netidmgr\netidmgr.h
:netidmgr_h_done

mkdir %1\install
mkdir %1\install\nsis
copy auth\krb5\src\windows\installer\nsis\kfw-fixed.nsi       %1\install\nsis\kfw-fixed.nsi
copy auth\krb5\src\windows\installer\nsis\kfw.ico             %1\install\nsis\kfw.ico
copy auth\krb5\src\windows\installer\nsis\kfw.nsi             %1\install\nsis\kfw.nsi
copy auth\krb5\src\windows\installer\nsis\KfWConfigPage.ini   %1\install\nsis\KfWConfigPage.ini
copy auth\krb5\src\windows\installer\nsis\KfWConfigPage2.ini  %1\install\nsis\KfWConfigPage2.ini
copy auth\krb5\src\windows\installer\nsis\killer.cpp          %1\install\nsis\killer.cpp
copy auth\krb5\src\windows\installer\nsis\licenses.rtf        %1\install\nsis\licenses.rtf
copy auth\krb5\src\windows\installer\nsis\site-local-tagged.nsi      %1\install\nsis\site-local-tagged.nsi
copy auth\krb5\src\windows\installer\nsis\nsi-includes-tagged.nsi      %1\install\nsis\nsi-includes-tagged.nsi
copy auth\krb5\src\windows\installer\nsis\utils.nsi           %1\install\nsis\utils.nsi
mkdir %1\install\wix
mkdir %1\install\wix\Binary
mkdir %1\install\wix\custom
mkdir %1\install\wix\lang
copy auth\krb5\src\windows\installer\wix\msi-deployment-guide.txt %1\install\wix\msi-deployment-guide.txt
copy auth\krb5\src\windows\installer\wix\config.wxi           %1\install\wix\config.wxi
copy auth\krb5\src\windows\installer\wix\features.wxi         %1\install\wix\features.wxi
copy auth\krb5\src\windows\installer\wix\files.wxi            %1\install\wix\files.wxi
copy auth\krb5\src\windows\installer\wix\kfw.wxs              %1\install\wix\kfw.wxs
copy auth\krb5\src\windows\installer\wix\Makefile             %1\install\wix\Makefile
copy auth\krb5\src\windows\installer\wix\property.wxi         %1\install\wix\property.wxi
copy auth\krb5\src\windows\installer\wix\platform.wxi         %1\install\wix\platform.wxi
copy auth\krb5\src\windows\installer\wix\runtime.wxi          %1\install\wix\runtime.wxi
copy auth\krb5\src\windows\installer\wix\runtime_debug.wxi    %1\install\wix\runtime_debug.wxi
copy auth\krb5\src\windows\installer\wix\site-local-tagged.wxi  %1\install\wix\site-local-tagged.wxi
copy auth\krb5\src\windows\installer\wix\Binary\bannrbmp.bmp  %1\install\wix\Binary\bannrbmp.bmp
copy auth\krb5\src\windows\installer\wix\Binary\completi.ico  %1\install\wix\Binary\completi.ico
copy auth\krb5\src\windows\installer\wix\Binary\custicon.ico  %1\install\wix\Binary\custicon.ico
copy auth\krb5\src\windows\installer\wix\Binary\dlgbmp.bmp    %1\install\wix\Binary\dlgbmp.bmp
copy auth\krb5\src\windows\installer\wix\Binary\exclamic.ico  %1\install\wix\Binary\exclamic.ico
copy auth\krb5\src\windows\installer\wix\Binary\info.bmp      %1\install\wix\Binary\info.bmp
copy auth\krb5\src\windows\installer\wix\Binary\insticon.ico  %1\install\wix\Binary\insticon.ico
copy auth\krb5\src\windows\installer\wix\Binary\new.bmp       %1\install\wix\Binary\new.bmp
copy auth\krb5\src\windows\installer\wix\Binary\removico.ico  %1\install\wix\Binary\removico.ico
copy auth\krb5\src\windows\installer\wix\Binary\repairic.ico  %1\install\wix\Binary\repairic.ico
copy auth\krb5\src\windows\installer\wix\Binary\up.bmp        %1\install\wix\Binary\up.bmp
copy auth\krb5\src\windows\installer\wix\custom\custom.cpp    %1\install\wix\custom\custom.cpp
copy auth\krb5\src\windows\installer\wix\custom\custom.h      %1\install\wix\custom\custom.h
copy auth\krb5\src\windows\installer\wix\lang\config_1033.wxi %1\install\wix\lang\config_1033.wxi
copy auth\krb5\src\windows\installer\wix\lang\strings_1033.wxl %1\install\wix\lang\strings_1033.wxl
copy auth\krb5\src\windows\installer\wix\lang\ui_1033.wxi     %1\install\wix\lang\ui_1033.wxi
copy auth\krb5\src\windows\installer\wix\lang\license.rtf     %1\install\wix\lang\license.rtf

pushd %1\bin\i386
c:\src\sign_sei.cmd *.exe *.dll 
popd

exit /b 0

:usage
ECHO COPYSDK.BAT destination-directory [debug]
exit /b 1