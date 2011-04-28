#
# Makefile for Leash Help File
#

!ifndef BINPLACE_CMD
BINPLACE_CMD=echo
!endif

!ifdef NODEBUG
BINPLACE_CMD=$(BINPLACE_CMD) --nodebug
!else
BINPLACE_CMD=$(BINPLACE_CMD) --debug
!endif

TARGET=leash32.hlp
ERR=$(TARGET:.hlp=.err)
HPJ=$(TARGET:.hlp=.hpj)
RTF=$(TARGET:.hlp=.rtf)

all: $(TARGET) rename
	@$(BINPLACE_CMD) $(TARGET)

clean:
	@if exist $(TARGET) del $(TARGET)
	@if exist $(ERR) del $(ERR)

rename:
	@if exist $(TARGET) ren $(TARGET) $(TARGET)
	@if exist $(ERR) ren $(ERR) $(ERR)

HC=hcw /a /c /m /e

# We rename the file to get a lower-case file.
# It looks like the silly help compiler gives us uppercase.
$(TARGET): $(RTF) $(HPJ)
	$(HC) $(HPJ)
