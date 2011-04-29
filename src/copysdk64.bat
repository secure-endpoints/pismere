@echo off
if "%1" == "" goto usage
if "%2" == "release" goto perform_install
if "%2" == "debug" goto perform_install
if NOT "%2" == "" goto usage

:perform_install
mkdir  %1
mkdir  %1\doc
copy ..\doc\kerberos\relnotes.html      %1\doc\relnotes.html
copy auth\leash\help\leash_userdoc.pdf  %1\doc\leash_userdoc.pdf
copy auth\krb5\src\windows\identity\doc\NetIdMgr.pdf %1\doc\netidmgr_userdoc.pdf
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
mkdir  %1\bin\amd64
if "%2" == "debug" goto debug
copy auth\krb5\src\windows\identity\obj\amd64\rel\doc\NetIddev.chm %1\doc\netiddev.chm

copy ..\target\bin\amd64\rel\comerr64.dll    %1\bin\amd64\comerr64.dll
copy ..\target\bin\amd64\rel\gss.exe         %1\bin\amd64\gss.exe
copy ..\target\bin\amd64\rel\gss-client.exe  %1\bin\amd64\gss-client.exe
copy ..\target\bin\amd64\rel\gss-server.exe  %1\bin\amd64\gss-server.exe
copy ..\target\bin\amd64\rel\gssapi64.dll    %1\bin\amd64\gssapi64.dll
copy ..\target\bin\amd64\rel\kdestroy.exe    %1\bin\amd64\kdestroy.exe
copy ..\target\bin\amd64\rel\kinit.exe       %1\bin\amd64\kinit.exe
copy ..\target\bin\amd64\rel\klist.exe       %1\bin\amd64\klist.exe
copy ..\target\bin\amd64\rel\krb5_64.dll     %1\bin\amd64\krb5_64.dll
copy ..\target\bin\amd64\rel\k5sprt64.dll    %1\bin\amd64\k5sprt64.dll
copy ..\target\bin\amd64\rel\krbcc64.dll     %1\bin\amd64\krbcc64.dll
copy ..\target\bin\amd64\rel\krbcc64s.exe    %1\bin\amd64\krbcc64s.exe
copy ..\target\bin\amd64\rel\kvno.exe        %1\bin\amd64\kvno.exe
copy ..\target\bin\amd64\rel\kpasswd.exe     %1\bin\amd64\kpasswd.exe
copy ..\target\bin\amd64\rel\leashw64.dll    %1\bin\amd64\leashw64.dll
copy ..\target\bin\amd64\rel\ms2mit.exe      %1\bin\amd64\ms2mit.exe
copy ..\target\bin\amd64\rel\wshelp64.dll    %1\bin\amd64\wshelp64.dll
copy ..\target\bin\amd64\rel\xpprof64.dll    %1\bin\amd64\xpprof64.dll
copy ..\target\bin\amd64\rel\kcpytkt.exe     %1\bin\amd64\kcpytkt.exe
copy ..\target\bin\amd64\rel\kdeltkt.exe     %1\bin\amd64\kdeltkt.exe
copy ..\target\bin\amd64\rel\mit2ms.exe      %1\bin\amd64\mit2ms.exe
copy ..\target\bin\amd64\rel\krb5cred.dll       %1\bin\amd64\krb5cred.dll        
copy ..\target\bin\amd64\rel\krb5cred_en_us.dll %1\bin\amd64\krb5cred_en_us.dll  
copy ..\target\bin\amd64\rel\nidmgr64.dll       %1\bin\amd64\nidmgr64.dll        
copy ..\target\bin\amd64\rel\netidmgr.exe       %1\bin\amd64\netidmgr.exe        
copy ..\target\bin\amd64\rel\netidmgr.chm       %1\bin\amd64\netidmgr.chm
copy ..\target\bin\amd64\rel\netiddev.chm       %1\doc\netiddev.chm
copy ..\target\bin\amd64\rel\kfwlogon.dll       %1\bin\amd64\kfwlogon.dll
copy ..\target\bin\amd64\rel\kfwcpcc.exe        %1\bin\amd64\kfwcpcc.exe
mkdir %1\bin\amd64\w2k
copy auth\krb5\src\windows\identity\obj\amd64\rel\w2k\nidmgr64.dll %1\bin\amd64\w2k\nidmgr64.dll
copy auth\krb5\src\windows\identity\obj\amd64\rel\w2k\netidmgr.exe %1\bin\amd64\w2k\netidmgr.exe

copy ..\target\bin\amd64\rel\comerr64.pdb    %1\bin\amd64\comerr64.pdb
copy ..\target\bin\amd64\rel\gss.pdb         %1\bin\amd64\gss.pdb
copy ..\target\bin\amd64\rel\gss-client.pdb  %1\bin\amd64\gss-client.pdb
copy ..\target\bin\amd64\rel\gss-server.pdb  %1\bin\amd64\gss-server.pdb
copy ..\target\bin\amd64\rel\gssapi64.pdb    %1\bin\amd64\gssapi64.pdb
copy ..\target\bin\amd64\rel\kdestroy.pdb    %1\bin\amd64\kdestroy.pdb
copy ..\target\bin\amd64\rel\kinit.pdb       %1\bin\amd64\kinit.pdb
copy ..\target\bin\amd64\rel\klist.pdb       %1\bin\amd64\klist.pdb
copy ..\target\bin\amd64\rel\krb5_64.pdb     %1\bin\amd64\krb5_64.pdb
copy ..\target\bin\amd64\rel\k5sprt64.pdb    %1\bin\amd64\k5sprt64.pdb
copy ..\target\bin\amd64\rel\krbcc64.pdb     %1\bin\amd64\krbcc64.pdb
copy ..\target\bin\amd64\rel\krbcc64s.pdb    %1\bin\amd64\krbcc64s.pdb
copy ..\target\bin\amd64\rel\kvno.pdb        %1\bin\amd64\kvno.pdb
copy ..\target\bin\amd64\rel\kpasswd.pdb     %1\bin\amd64\kpasswd.pdb
copy ..\target\bin\amd64\rel\leashw64.pdb    %1\bin\amd64\leashw64.pdb
copy ..\target\bin\amd64\rel\ms2mit.pdb      %1\bin\amd64\ms2mit.pdb
copy ..\target\bin\amd64\rel\wshelp64.pdb    %1\bin\amd64\wshelp64.pdb
copy ..\target\bin\amd64\rel\xpprof64.pdb    %1\bin\amd64\xpprof64.pdb
copy ..\target\bin\amd64\rel\kcpytkt.pdb     %1\bin\amd64\kcpytkt.pdb
copy ..\target\bin\amd64\rel\kdeltkt.pdb     %1\bin\amd64\kdeltkt.pdb
copy ..\target\bin\amd64\rel\mit2ms.pdb      %1\bin\amd64\mit2ms.pdb
copy ..\target\bin\amd64\rel\krb5cred.pdb       %1\bin\amd64\krb5cred.pdb        
copy ..\target\bin\amd64\rel\nidmgr64.pdb       %1\bin\amd64\nidmgr64.pdb
copy ..\target\bin\amd64\rel\netidmgr.pdb       %1\bin\amd64\netidmgr.pdb        
copy ..\target\bin\amd64\rel\kfwlogon.pdb       %1\bin\amd64\kfwlogon.pdb
copy ..\target\bin\amd64\rel\kfwcpcc.pdb        %1\bin\amd64\kfwcpcc.pdb        
copy auth\krb5\src\windows\identity\obj\amd64\rel\w2k\nidmgr64.pdb %1\bin\amd64\w2k\nidmgr64.pdb
copy auth\krb5\src\windows\identity\obj\amd64\rel\w2k\netidmgr.pdb %1\bin\amd64\w2k\netidmgr.pdb

mkdir  %1\lib
mkdir  %1\lib\amd64
copy ..\target\lib\amd64\rel\comerr64.lib    %1\lib\amd64\comerr64.lib
copy ..\target\lib\amd64\rel\comerr64.a      %1\lib\amd64\comerr64.a
copy ..\target\lib\amd64\rel\delaydlls.lib   %1\lib\amd64\delaydlls.lib
copy ..\target\lib\amd64\rel\getopt.lib      %1\lib\amd64\getopt.lib
copy ..\target\lib\amd64\rel\gssapi64.lib    %1\lib\amd64\gssapi64.lib
copy ..\target\lib\amd64\rel\gssapi64.a      %1\lib\amd64\gssapi64.a
copy ..\target\lib\amd64\rel\krb5_64.lib     %1\lib\amd64\krb5_64.lib
copy ..\target\lib\amd64\rel\krb5_64.a       %1\lib\amd64\krb5_64.a
copy ..\target\lib\amd64\rel\krbcc64.lib     %1\lib\amd64\krbcc64.lib
copy ..\target\lib\amd64\rel\leashw64.lib    %1\lib\amd64\leashw64.lib
copy ..\target\lib\amd64\rel\loadfuncs.lib   %1\lib\amd64\loadfuncs.lib
copy ..\target\lib\amd64\rel\wshelp64.lib    %1\lib\amd64\wshelp64.lib
copy ..\target\lib\amd64\rel\xpprof64.lib    %1\lib\amd64\xpprof64.lib
copy ..\target\lib\amd64\rel\xpprof64.a    %1\lib\amd64\xpprof64.a
copy ..\target\lib\amd64\rel\nidmgr64.lib    %1\lib\amd64\nidmgr64.lib
goto headers

:debug
copy auth\krb5\src\windows\identity\obj\amd64\dbg\doc\NetIddev.chm %1\doc\netiddev.chm

copy ..\target\bin\amd64\dbg\comerr64.dll    %1\bin\amd64\comerr64.dll
copy ..\target\bin\amd64\dbg\gss.exe         %1\bin\amd64\gss.exe
copy ..\target\bin\amd64\dbg\gss-client.exe  %1\bin\amd64\gss-client.exe
copy ..\target\bin\amd64\dbg\gss-server.exe  %1\bin\amd64\gss-server.exe
copy ..\target\bin\amd64\dbg\gssapi64.dll    %1\bin\amd64\gssapi64.dll
copy ..\target\bin\amd64\dbg\kdestroy.exe    %1\bin\amd64\kdestroy.exe
copy ..\target\bin\amd64\dbg\kinit.exe       %1\bin\amd64\kinit.exe
copy ..\target\bin\amd64\dbg\klist.exe       %1\bin\amd64\klist.exe
copy ..\target\bin\amd64\dbg\krb5_64.dll     %1\bin\amd64\krb5_64.dll
copy ..\target\bin\amd64\dbg\k5sprt64.dll    %1\bin\amd64\k5sprt64.dll
copy ..\target\bin\amd64\dbg\krbcc64.dll     %1\bin\amd64\krbcc64.dll
copy ..\target\bin\amd64\dbg\krbcc64s.exe    %1\bin\amd64\krbcc64s.exe
copy ..\target\bin\amd64\dbg\kvno.exe        %1\bin\amd64\kvno.exe
copy ..\target\bin\amd64\dbg\kpasswd.exe     %1\bin\amd64\kpasswd.exe
copy ..\target\bin\amd64\dbg\leashw64.dll    %1\bin\amd64\leashw64.dll
copy ..\target\bin\amd64\dbg\ms2mit.exe      %1\bin\amd64\ms2mit.exe
copy ..\target\bin\amd64\dbg\wshelp64.dll    %1\bin\amd64\wshelp64.dll
copy ..\target\bin\amd64\dbg\xpprof64.dll    %1\bin\amd64\xpprof64.dll
copy ..\target\bin\amd64\dbg\kcpytkt.exe     %1\bin\amd64\kcpytkt.exe
copy ..\target\bin\amd64\dbg\kdeltkt.exe     %1\bin\amd64\kdeltkt.exe
copy ..\target\bin\amd64\dbg\mit2ms.exe      %1\bin\amd64\mit2ms.exe
copy ..\target\bin\amd64\dbg\krb5cred.dll    %1\bin\amd64\krb5cred.dll        
copy ..\target\bin\amd64\dbg\krb5cred_en_us.dll  %1\bin\amd64\krb5cred_en_us.dll  
copy ..\target\bin\amd64\dbg\nidmgr64.dll    %1\bin\amd64\nidmgr64.dll
copy ..\target\bin\amd64\dbg\netidmgr.exe    %1\bin\amd64\netidmgr.exe        
copy ..\target\bin\amd64\dbg\netidmgr.chm    %1\bin\amd64\netidmgr.chm
copy ..\target\bin\amd64\dbg\netiddev.chm    %1\doc\netiddev.chm
copy ..\target\bin\amd64\dbg\kfwlogon.dll    %1\bin\amd64\kfwlogon.dll
copy ..\target\bin\amd64\dbg\kfwcpcc.exe     %1\bin\amd64\kfwcpcc.exe
mkdir %1\bin\amd64\w2k
copy auth\krb5\src\windows\identity\obj\amd64\dbg\w2k\nidmgr64.dll %1\bin\amd64\w2k\nidmgr64.dll
copy auth\krb5\src\windows\identity\obj\amd64\dbg\w2k\netidmgr.exe %1\bin\amd64\w2k\netidmgr.exe

copy ..\target\bin\amd64\dbg\comerr64.pdb    %1\bin\amd64\comerr64.pdb
copy ..\target\bin\amd64\dbg\gss.pdb         %1\bin\amd64\gss.pdb
copy ..\target\bin\amd64\dbg\gss-client.pdb  %1\bin\amd64\gss-client.pdb
copy ..\target\bin\amd64\dbg\gss-server.pdb  %1\bin\amd64\gss-server.pdb
copy ..\target\bin\amd64\dbg\gssapi64.pdb    %1\bin\amd64\gssapi64.pdb
copy ..\target\bin\amd64\dbg\kdestroy.pdb    %1\bin\amd64\kdestroy.pdb
copy ..\target\bin\amd64\dbg\kinit.pdb       %1\bin\amd64\kinit.pdb
copy ..\target\bin\amd64\dbg\klist.pdb       %1\bin\amd64\klist.pdb
copy ..\target\bin\amd64\dbg\krb5_64.pdb     %1\bin\amd64\krb5_64.pdb
copy ..\target\bin\amd64\dbg\k5sprt64.pdb    %1\bin\amd64\k5sprt64.pdb
copy ..\target\bin\amd64\dbg\krbcc64.pdb     %1\bin\amd64\krbcc64.pdb
copy ..\target\bin\amd64\dbg\krbcc64s.pdb    %1\bin\amd64\krbcc64s.pdb
copy ..\target\bin\amd64\dbg\kvno.pdb        %1\bin\amd64\kvno.pdb
copy ..\target\bin\amd64\dbg\kpasswd.pdb     %1\bin\amd64\kpasswd.pdb
copy ..\target\bin\amd64\dbg\leashw64.pdb    %1\bin\amd64\leashw64.pdb
copy ..\target\bin\amd64\dbg\ms2mit.pdb      %1\bin\amd64\ms2mit.pdb
copy ..\target\bin\amd64\dbg\wshelp64.pdb    %1\bin\amd64\wshelp64.pdb
copy ..\target\bin\amd64\dbg\xpprof64.pdb    %1\bin\amd64\xpprof64.pdb
copy ..\target\bin\amd64\dbg\kcpytkt.pdb     %1\bin\amd64\kcpytkt.pdb
copy ..\target\bin\amd64\dbg\kdeltkt.pdb     %1\bin\amd64\kdeltkt.pdb
copy ..\target\bin\amd64\dbg\mit2ms.pdb      %1\bin\amd64\mit2ms.pdb
copy ..\target\bin\amd64\dbg\krb5cred.pdb    %1\bin\amd64\krb5cred.pdb        
copy ..\target\bin\amd64\dbg\nidmgr64.pdb    %1\bin\amd64\nidmgr64.pdb
copy ..\target\bin\amd64\dbg\netidmgr.pdb    %1\bin\amd64\netidmgr.pdb        
copy ..\target\bin\amd64\dbg\kfwlogon.pdb    %1\bin\amd64\kfwlogon.pdb
copy ..\target\bin\amd64\dbg\kfwcpcc.pdb     %1\bin\amd64\kfwcpcc.pdb
copy auth\krb5\src\windows\identity\obj\amd64\dbg\w2k\nidmgr64.pdb %1\bin\amd64\w2k\nidmgr64.pdb
copy auth\krb5\src\windows\identity\obj\amd64\dbg\w2k\netidmgr.pdb %1\bin\amd64\w2k\netidmgr.pdb

mkdir  %1\lib
mkdir  %1\lib\amd64
copy ..\target\lib\amd64\dbg\comerr64.lib    %1\lib\amd64\comerr64.lib
copy ..\target\lib\amd64\dbg\comerr64.a      %1\lib\amd64\comerr64.a
copy ..\target\lib\amd64\dbg\delaydlls.lib   %1\lib\amd64\delaydlls.lib
copy ..\target\lib\amd64\dbg\getopt.lib      %1\lib\amd64\getopt.lib
copy ..\target\lib\amd64\dbg\gssapi64.lib    %1\lib\amd64\gssapi64.lib
copy ..\target\lib\amd64\dbg\gssapi64.a      %1\lib\amd64\gssapi64.a
copy ..\target\lib\amd64\dbg\krb5_64.lib     %1\lib\amd64\krb5_64.lib
copy ..\target\lib\amd64\dbg\krb5_64.a       %1\lib\amd64\krb5_64.a
copy ..\target\lib\amd64\dbg\krbcc64.lib     %1\lib\amd64\krbcc64.lib
copy ..\target\lib\amd64\dbg\leashw64.lib    %1\lib\amd64\leashw64.lib
copy ..\target\lib\amd64\dbg\loadfuncs.lib   %1\lib\amd64\loadfuncs.lib
copy ..\target\lib\amd64\dbg\wshelp64.lib    %1\lib\amd64\wshelp64.lib
copy ..\target\lib\amd64\dbg\xpprof64.lib    %1\lib\amd64\xpprof64.lib
copy ..\target\lib\amd64\dbg\xpprof64.a      %1\lib\amd64\xpprof64.a
copy ..\target\lib\amd64\dbg\nidmgr64.lib    %1\lib\amd64\nidmgr64.lib

:headers
mkdir  %1\inc
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
copy auth\krb5\src\windows\identity\obj\amd64\rel\inc\hashtable.h     %1\inc\netidmgr\hashtable.h     
copy auth\krb5\src\windows\identity\obj\amd64\rel\inc\kconfig.h       %1\inc\netidmgr\kconfig.h       
copy auth\krb5\src\windows\identity\obj\amd64\rel\inc\kcreddb.h 	     %1\inc\netidmgr\kcreddb.h       
copy auth\krb5\src\windows\identity\obj\amd64\rel\inc\khaction.h      %1\inc\netidmgr\khaction.h      
copy auth\krb5\src\windows\identity\obj\amd64\rel\inc\khactiondef.h   %1\inc\netidmgr\khactiondef.h   
copy auth\krb5\src\windows\identity\obj\amd64\rel\inc\khalerts.h      %1\inc\netidmgr\khalerts.h      
copy auth\krb5\src\windows\identity\obj\amd64\rel\inc\khconfigui.h    %1\inc\netidmgr\khconfigui.h    
copy auth\krb5\src\windows\identity\obj\amd64\rel\inc\khdefs.h 	     %1\inc\netidmgr\khdefs.h        
copy auth\krb5\src\windows\identity\obj\amd64\rel\inc\kherr.h 	     %1\inc\netidmgr\kherr.h         
copy auth\krb5\src\windows\identity\obj\amd64\rel\inc\kherror.h 	     %1\inc\netidmgr\kherror.h       
copy auth\krb5\src\windows\identity\obj\amd64\rel\inc\khhtlink.h      %1\inc\netidmgr\khhtlink.h      
copy auth\krb5\src\windows\identity\obj\amd64\rel\inc\khlist.h 	     %1\inc\netidmgr\khlist.h        
copy auth\krb5\src\windows\identity\obj\amd64\rel\inc\khmsgtypes.h    %1\inc\netidmgr\khmsgtypes.h    
copy auth\krb5\src\windows\identity\obj\amd64\rel\inc\khnewcred.h     %1\inc\netidmgr\khnewcred.h     
copy auth\krb5\src\windows\identity\obj\amd64\rel\inc\khprops.h       %1\inc\netidmgr\khprops.h       
copy auth\krb5\src\windows\identity\obj\amd64\rel\inc\khremote.h      %1\inc\netidmgr\khremote.h      
copy auth\krb5\src\windows\identity\obj\amd64\rel\inc\khrescache.h    %1\inc\netidmgr\khrescache.h    
copy auth\krb5\src\windows\identity\obj\amd64\rel\inc\khtracker.h     %1\inc\netidmgr\khtracker.h     
copy auth\krb5\src\windows\identity\obj\amd64\rel\inc\khuidefs.h      %1\inc\netidmgr\khuidefs.h      
copy auth\krb5\src\windows\identity\obj\amd64\rel\inc\kmm.h 	     %1\inc\netidmgr\kmm.h           
copy auth\krb5\src\windows\identity\obj\amd64\rel\inc\kmq.h 	     %1\inc\netidmgr\kmq.h           
copy auth\krb5\src\windows\identity\obj\amd64\rel\inc\kplugin.h 	     %1\inc\netidmgr\kplugin.h       
copy auth\krb5\src\windows\identity\obj\amd64\rel\inc\mstring.h 	     %1\inc\netidmgr\mstring.h       
copy auth\krb5\src\windows\identity\obj\amd64\rel\inc\sync.h 	     %1\inc\netidmgr\sync.h          
copy auth\krb5\src\windows\identity\obj\amd64\rel\inc\utils.h 	     %1\inc\netidmgr\utils.h
copy auth\krb5\src\windows\identity\obj\amd64\rel\inc\perfstat.h 	     %1\inc\netidmgr\perfstat.h
copy auth\krb5\src\windows\identity\obj\amd64\rel\inc\netidmgr_version.h 	     %1\inc\netidmgr\netidmgr_version.h
copy auth\krb5\src\windows\identity\obj\amd64\rel\inc\netidmgr.h      %1\inc\netidmgr\netidmgr.h
goto netidmgr_h_done
:debug_netidmgr_h
copy auth\krb5\src\windows\identity\obj\amd64\dbg\inc\hashtable.h     %1\inc\netidmgr\hashtable.h     
copy auth\krb5\src\windows\identity\obj\amd64\dbg\inc\kconfig.h       %1\inc\netidmgr\kconfig.h       
copy auth\krb5\src\windows\identity\obj\amd64\dbg\inc\kcreddb.h 	     %1\inc\netidmgr\kcreddb.h       
copy auth\krb5\src\windows\identity\obj\amd64\dbg\inc\khaction.h      %1\inc\netidmgr\khaction.h      
copy auth\krb5\src\windows\identity\obj\amd64\dbg\inc\khactiondef.h   %1\inc\netidmgr\khactiondef.h   
copy auth\krb5\src\windows\identity\obj\amd64\dbg\inc\khalerts.h      %1\inc\netidmgr\khalerts.h      
copy auth\krb5\src\windows\identity\obj\amd64\dbg\inc\khconfigui.h    %1\inc\netidmgr\khconfigui.h    
copy auth\krb5\src\windows\identity\obj\amd64\dbg\inc\khdefs.h 	     %1\inc\netidmgr\khdefs.h        
copy auth\krb5\src\windows\identity\obj\amd64\dbg\inc\kherr.h 	     %1\inc\netidmgr\kherr.h         
copy auth\krb5\src\windows\identity\obj\amd64\dbg\inc\kherror.h 	     %1\inc\netidmgr\kherror.h       
copy auth\krb5\src\windows\identity\obj\amd64\dbg\inc\khhtlink.h      %1\inc\netidmgr\khhtlink.h      
copy auth\krb5\src\windows\identity\obj\amd64\dbg\inc\khlist.h 	     %1\inc\netidmgr\khlist.h        
copy auth\krb5\src\windows\identity\obj\amd64\dbg\inc\khmsgtypes.h    %1\inc\netidmgr\khmsgtypes.h    
copy auth\krb5\src\windows\identity\obj\amd64\dbg\inc\khnewcred.h     %1\inc\netidmgr\khnewcred.h     
copy auth\krb5\src\windows\identity\obj\amd64\dbg\inc\khprops.h       %1\inc\netidmgr\khprops.h       
copy auth\krb5\src\windows\identity\obj\amd64\dbg\inc\khremote.h      %1\inc\netidmgr\khremote.h      
copy auth\krb5\src\windows\identity\obj\amd64\dbg\inc\khrescache.h    %1\inc\netidmgr\khrescache.h    
copy auth\krb5\src\windows\identity\obj\amd64\dbg\inc\khtracker.h     %1\inc\netidmgr\khtracker.h     
copy auth\krb5\src\windows\identity\obj\amd64\dbg\inc\khuidefs.h      %1\inc\netidmgr\khuidefs.h      
copy auth\krb5\src\windows\identity\obj\amd64\dbg\inc\kmm.h 	     %1\inc\netidmgr\kmm.h           
copy auth\krb5\src\windows\identity\obj\amd64\dbg\inc\kmq.h 	     %1\inc\netidmgr\kmq.h           
copy auth\krb5\src\windows\identity\obj\amd64\dbg\inc\kplugin.h 	     %1\inc\netidmgr\kplugin.h       
copy auth\krb5\src\windows\identity\obj\amd64\dbg\inc\mstring.h 	     %1\inc\netidmgr\mstring.h       
copy auth\krb5\src\windows\identity\obj\amd64\dbg\inc\sync.h 	     %1\inc\netidmgr\sync.h          
copy auth\krb5\src\windows\identity\obj\amd64\dbg\inc\utils.h 	     %1\inc\netidmgr\utils.h         
copy auth\krb5\src\windows\identity\obj\amd64\dbg\inc\perfstat.h 	     %1\inc\netidmgr\perfstat.h
copy auth\krb5\src\windows\identity\obj\amd64\dbg\inc\netidmgr_version.h 	     %1\inc\netidmgr\netidmgr_version.h
copy auth\krb5\src\windows\identity\obj\amd64\dbg\inc\netidmgr.h      %1\inc\netidmgr\netidmgr.h
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

pushd %1\bin\amd64
c:\src\sign_sei.cmd *.exe *.dll 
popd

exit /b 0

:usage
ECHO COPYSDK.BAT destination-directory [debug]
exit /b 1