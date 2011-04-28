# Autoduck makefile

OUTDIR 	   = docs
TARGET 	   = wshelper
AD         = autoduck.exe

SOURCE     = *.h *.c ..\include\mitwhich.h
ADTOC      = help\contents.d
ADFMT      = help\autoduck.fmt
ADFMT_HTML = help\html.fmt
VERBOSE    = /v

# You should not need to edit below...

TITLE      = $(TARGET) Help
DOCHDR 	   = $(TARGET) API Reference

!ifdef ADFMT
ADFMT_OPTS =  /f $(ADFMT)
!else
ADFMT_OPTS =
!endif

ADTAB      = 8
ADOPTS     = /t$(ADTAB)

ADHLP      = $(ADOPTS) $(ADFMT_OPTS) /RH /D "title=$(TITLE)" $(VERBOSE)
ADDOC      = $(ADOPTS) $(ADFMT_OPTS) /RD /D "doc_header=$(DOCHDR)" $(VERBOSE)
ADHTML     = $(ADOPTS) /f $(ADFMT_HTML) /r html /D "title=$(TITLE)" $(VERBOSE)
HC         = hcw /a /e /c

target ::
!if !EXIST("$(OUTDIR)")
	md $(OUTDIR)
!endif

target :: $(OUTDIR)\$(TARGET).hlp $(OUTDIR)\$(TARGET).doc $(OUTDIR)\$(TARGET).html

clean:
	if exist $(OUTDIR)\*.rtf del $(OUTDIR)\*.rtf
	if exist $(OUTDIR)\*.hpj del $(OUTDIR)\*.hpj
	if exist $(OUTDIR)\$(TARGET).doc del $(OUTDIR)\$(TARGET).doc
	if exist $(OUTDIR)\$(TARGET).hlp del $(OUTDIR)\$(TARGET).hlp
	if exist $(OUTDIR)\$(TARGET).html del $(OUTDIR)\$(TARGET).html

# Generate a Help file
$(OUTDIR)\$(TARGET).rtf : $(ADFMT) $(ADTOC) $(SOURCE)
	$(AD) $(ADHLP) /O$@ $(ADTOC) $(SOURCE)

$(OUTDIR)\$(TARGET).hlp : $(OUTDIR)\$(TARGET).rtf
	$(HC) $(OUTDIR)\$(TARGET).hpj

# Generate a print documentation file
$(OUTDIR)\$(TARGET).doc : $(ADFMT) $(SOURCE)
	$(AD) $(ADDOC) /O$@ $(SOURCE)

# Generate an HTML file
$(OUTDIR)\$(TARGET).html : $(ADFMT_HTML) $(ADTOC) $(SOURCE)
	$(AD) $(ADHTML) /O$@ $(ADTOC) $(SOURCE)
