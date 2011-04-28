/*
@doc

@contents1 Contents | WinSock Helper 2.0 MIT Information Services<nl> <nl>
To display a list of topics by category, click any 
of the contents entries below. To display an alphabetical list of 
topics, choose the Index button.




@head3 An Introduction to WSHELPER and WSHELP32 |

@subindex	    Authors and Copyright
@subindex		Introduction
@subindex		End User Configuration
@subindex		Using WSHELPER, for the programmer



@head3 C Elements |

@subindex		BIND-Compatible DNS Resolver API
@subindex		MIT Hesiod API
@subindex       Other useful functions
@subindex       All exported functions
@subindex Structures and Enums


@head3 Appendices |

@subindex Modules

@normal Help file built: <date>

@subindex About Autoduck



***********************************************************************
@contents2 All exported functions |
@index func,cb |

***********************************************************************
@contents2 Messages |
@index msg |

***********************************************************************
@contents2 Structures and Enums |
@index struct,enum |

***********************************************************************
@contents2 Overviews |
@index topic |

***********************************************************************
@contents2 Modules |
@index module |

***********************************************************************
@contents2 Constants |
@index const |

***********************************************************************
@contents2 About Autoduck | The sources for this Help file were generated
by Autoduck, the source code documentation tool that generates Print or
Help files from tagged comments in C, C++, Assembly, and Basic source
files.<nl><nl>

For more information, contact Eric Artzt (erica@microsoft.com).<nl><nl>

Current source and binaries can be found at http://www.accessone.com/~ericartzt/

***********************************************************************
@contents2 Authors and Copyright |

This software is being provided to you, the LICENSEE, by the Massachusetts Institute of Technology (M.I.T.) under the following license.  By obtaining, using and/or copying this software, you agree that you have read, understood, and will comply with these terms and conditions.  Permission to use, copy, modify and distribute this software and its documentation for any purpose and without fee or royalty is hereby granted, provided that you agree to comply with the following copyright notice and statements, including the disclaimer, and that the same appear on ALL copies of the software and documentation, including modifications that you make for internal use or for distribution:
Copyright 1994, 1995, 1996 by the Massachusetts Institute of Technology.  All rights reserved.
<nl>
<nl>
THIS SOFTWARE IS PROVIDED "AS IS", AND M.I.T. MAKES NO REPRESENTATIONS OR WARRANTIES, EXPRESS OR IMPLIED.  By way of example, but not limitation, M.I.T. MAKES NO REPRESENTATIONS OR WARRANTIES OF MERCHANTABILITY OR FITNESS FOR ANY PARTICULAR PURPOSE OR THAT THE USE OF THE LICENSED SOFTWARE OR DOCUMENTATION WILL NOT INFRINGE ANY THIRD PARTY PATENTS, COPYRIGHTS, TRADEMARKS OR OTHER RIGHTS.
<nl>
<nl>
The name of the Massachusetts Institute of Technology or M.I.T. may NOT be used in advertising or publicity pertaining to distribution of the software.  Title to copyright in this software and any associated documentation shall at all times remain with M.I.T., and USER agrees to preserve same.
<nl>
<nl>
This product includes software developed by the University of California, Berkeley and its contributors.


***********************************************************************
@contents2 Introduction | 

 The WSHelper (Windows Sockets Helper) library was created in an effort to fill a gap in the current WinSock specification (1.1).  This specification, while requiring implementors to provide getXbyY() style "database" functions which retrieve host address, service port, and protocol information, does not explicitly state how the functions should go about this.  At the simple level, implementors could have the database functions search local files, which is fine for finding standard service and protocol information, but impractical for modern applications which could possibly require a canonical Internet host table, and thus this approach is suitable only for PC's that use TCP/IP on a LAN basis.  More reasonably, implementors could search a local host table, then, failing that, go on to make a resolver query to a known nameserver, or vice versa.  This is precisely what many implementors do, but since this functionality is abstracted at a fairly high level (the actual resolver functions are inaccessible to developers), and because the specification is as vague as it is, a need exists for true WinSock-based BIND resolver functions, which WSHelper provides.  In addition, WSHelper complements its BIND support with MIT Hesiod Name Service support, for queries on sites which support the Hesiod query class.
<nl>
<nl>
We have also found that in many Winsock 1.1 implementations the getXbyY() functions are poorly implemented. By provding the alternate implementation for which full source code is available we hope to improve this situation. At this time we have not attempted to implement non-blocking versions of these function nor have we implemented thread safe versions. We may provide this functionality in the future.
<nl>




***********************************************************************
@contents2 Using WSHELPER, for the programmer |

 First and foremost, it is the responsibility of the module calling WSHelper to take measures to ensure that a valid WINSOCK.DLL exists on the system and has been loaded (either automatically or dynamically), and as well that WSAStartup() has been called and successfully.  The WinSock documentation describes how this is done.
 <nl><nl>
 The wshelper and wshelp32 DLLs consist of a set of C functions that may be used by any 
 application developer. The exported functions which are accessible to applications are
 described individually in the C elements section of this document.
 <nl><nl>
 The wshelper.dll is a 16 bit DLL. It has been tested on Windows 3.11, WFWG 3.11, 
 Windows 95 and Windows NT 3.x and 4.0. The wshelp32.dll is the 32 bit version of the
 same library. Currently we are only distributing the i386 build. This has been tested
 on Windows 95, and Windows 4.0 with the Microsoft supplied TCP/IP stacks. Please see 
 the end user configuration section for more installation/configuration information.
 <nl><nl>
 Bugs may be reported to bugs@mit.edu.




***********************************************************************
@contents2 End User Configuration |


This version of wshelper and wshelp32 tries to make the end user configuration as simple 
as possible. By default the libraries should support the following Winsock TCP/IP
implementations: Microsoft's TCP/IP as provided on Windows 95 and NT, Novell's LAN 
WorkPlace, FTP, Inc's PCTCP (and some other their other products), Trumpet Winsock,
CORE Winsock. By using environment variables, simple text files, or a resource editor,
you should also be able to support your own local configuration if these defaults do
not suffice.<nl>

When running under Windows 95 or Windows NT both the 16 bit and 32 DLL 
will attempt to read the IP addresses of the nameservers, also known as the BIND servers,
from the registry. The libraries will also attempt to read the default domain from 
the registry.<nl>

This version assumes the registry values are those that Microsoft currently uses
for Windows 95 and NT 4.0. However these values may be configured differently on
other stacks or changed by Microsoft in the future. The actual values used are
defined in the header file mitwhich.h. These values are used to build string resources
in the DLL. So in most cases the string resource is what is actually used. This means
that you may use a resource editor to change the default registry values without
needing to recompile the source code.<nl>

When running on Windows 3.x, WFWG, or if the libraries are unable to find the correct
registry values, the library will try to find the configuration information in a file.
The file will be looked for in a variety of places. First it will check 4 different
environment variables, ETC, EXCELAN, NDIR, and PCTCP. If the ETC environment variable 
is found the library will look for a file named resolv.cfg in the directory named by
the ETC variable. If the EXCELAN environment variable is found then the library will 
use this value as a base directory name and then search the subdirectories etc and tcp
for the resolv.cfg file. If the PCTCP environment variable is found the library
expects this to point to a fully qualified pctcp.ini file as supported by products
from FTP, Inc.<nl>

The library goes to even more extreme lengths to get the users configuration data. It 
will also look on the user's path for resolv.cfg, trumpwsk.ini, core.ini, and 
pctcp.ini. The library does understand the format of the trumpwsk.ini, core.ini, and
pctcp.ini files. The library will also search for the file defined by the string
resource IDS_DEF_RESCONF_PATH, which is initially controlled by the _PATH_RESCONF
defined in the resolv.h header file of the source code.<nl>

If all else fails the user will be notified that the machine is not properly 
configured but they will be given a chance to continue. In this case some hard coded
defaults will be used. These are defined as string resources. Site administrators
please use a resource editor to change these to your defaults rather than MIT's. It 
will provide your users and ours with better performance.<nl>

Here is a sample resolv.cfg file:<nl>

domain mit.edu<nl>
nameserver 18.70.0.160<nl>
nameserver 18.71.0.151<nl>
nameserver 18.72.0.3<nl><nl>

If you have to create a resolv.cfg file for your machine please determine the correct 
IP address for your domain and change mit.edu to your own local domain.<nl>

@xref <f res_init>

For Hesiod support we do not go to quite such elaborate lengths to find the 
configuration information. The library will check for a HES_DOMAIN environment 
variable. If it is present then this will be used as the RHS or the Hesiod domain, 
e.g. ATHENA.MIT.EDU. The LHS and RHS defaults are stringtable entries of 
IDS_DEF_HES_LHS and IDS_DEF_HES_RHS. So these may be changed by modifying the source
or editing the resource. The string resource IDS_DEF_HES_CONFIG_FILE is also checked.
In this case the libraries will look to see if a Hesiod config file is present and if
so it will use the RHS and LHS defined in the file. Note that the environment variable
will take precedence over the config file, the config file takes precedence over the
default LHS and RHS string table entries.<nl>

@xref <f hes_init>

Here is a sample hesiod.cfg file:<nl>
lhs     .ns<nl>
rhs     .ATHENA.MIT.EDU<nl><nl>

At Stanford the proper values would be:<nl>
lhs .ns<nl>
rhs .STANFORD.EDU<nl><nl>


A more formal description of the resolv.cfg and hesiod.cfg syntax:<nl>

To use either the DNS or Hesiod resolution functions, a resolver configuration 
file (by default, C:\NET\TCP\RESOLV.CFG - change  _PATH_RESCONF  in resolv.h) must 
be set up. This should contain at least one entry of the form:
<nl>
<nl>
	nameserver	<lt> ip-address <gt> 
<nl>
<nl>
, where <lt> ip-address <gt> is the dotted-decimal IP address of a known BIND 
server.  If no nameserver is specified, the resolver code will assume that the 
local machine is capable of  handling resolver queries.  If no BIND server is 
running locally, the resolver routines will search a local hosts file (by 
default, C:\TCP\HOSTS, change _PATH_HOSTS in gethna.c).  If you wish the resolver 
functions to search a default domain, add an entry of the form:
<nl>
<nl>
	domain		<lt> default.domain <gt> 
<nl>
<nl>
to the resolver configuration file.  Note that 
<nl>
<nl>
	domain		MIT.EDU 
<nl>
<nl>
would be valid whereas 
<nl>
<nl>
	domain		.MIT.EDU 
<nl>
<nl>
includes an unncecessary first dot. 

<nl>
<nl>
 To configure Hesiod name service lookup, create a Hesiod configuration file (by 
 default C:\NET\HESIOD.CFG) with two entries of the form:
<nl>
<nl>
	lhs		.ns <nl>
	rhs		.your.hesiod.domain
<nl>
<nl>
, noting that here a first dot is needed.




***********************************************************************
@contents2 BIND-Compatible DNS Resolver API |
 While the standard WinSock GetXbyY function calls suit many development needs, some applications require true nameserver resolution capability to function.  WSHelper provides true resolver functions and resolver-based replacements to WinSock's gethostbyname() and gethostbyaddr() functions.
<nl>
<nl>     
@index func | RESOLVE

***********************************************************************
@contents2 MIT Hesiod API |
 
     WSHelper also provides an interface to MIT Hesiod Name Service resolution functions.
<nl>
<nl>

@index func | HESIOD

***********************************************************************
@contents2 Other useful functions |
 
     WSHelper also provides an interface to other functions, most are related to name resolution.
<nl>
<nl>

@index func | MISC

*/

