#
# Makefile for Kerberos Help File (kerberos.hlp)
#

default:: all

DONT_CREATE_OUTDIR=1
INSTALL_FILES=kerberos.hlp

!INCLUDE <configur.mak>

clean:
	-rm kerberos.hlp kerberos.err

all:: kerberos.hlp rename
	@$(BINPLACE_CMD) kerberos.hlp

install: rename default_install

rename:
	ren kerberos.hlp kerberos.hlp

HC=hcw /a /c /m /e

# We rename the file to get a lower-case file.
# It looks like the silly help compiler gives us uppercase.
kerberos.hlp : leash.rtf
	$(HC) kerberos.hpj
