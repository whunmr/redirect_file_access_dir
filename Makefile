##############################################################################
##
##  Makefile for Detours.
##
##  Microsoft Research Detours Package, Version 3.0.
##
##  Copyright (c) Microsoft Corporation.  All rights reserved.
##

ROOT = .
!include "$(ROOT)\system.mak"

all:
    cd "$(MAKEDIR)\src"
    @$(MAKE) /$(MAKEFLAGS)
    cd "$(MAKEDIR)\samples"
    @$(MAKE) /$(MAKEFLAGS)
	@if exist "$(MAKEDIR)\bugs\makefile" cd "$(MAKEDIR)\bugs" && $(MAKE)  /$(MAKEFLAGS)
    cd "$(MAKEDIR)"

clean:
    cd "$(MAKEDIR)\src"
    @$(MAKE) /$(MAKEFLAGS) clean
    cd "$(MAKEDIR)\samples"
    @$(MAKE) /$(MAKEFLAGS) clean
	@if exist "$(MAKEDIR)\bugs\makefile" cd "$(MAKEDIR)\bugs" && $(MAKE)  /$(MAKEFLAGS) clean
    cd "$(MAKEDIR)"

realclean: clean
    cd "$(MAKEDIR)\src"
    @$(MAKE) /$(MAKEFLAGS) realclean
    cd "$(MAKEDIR)\samples"
    @$(MAKE) /$(MAKEFLAGS) realclean
	@if exist "$(MAKEDIR)\bugs\makefile" cd "$(MAKEDIR)\bugs" && $(MAKE)  /$(MAKEFLAGS) realclean
    cd "$(MAKEDIR)"
    -rmdir /q /s $(INCDS) 2> nul
    -rmdir /q /s $(LIBDS) 2> nul
    -rmdir /q /s $(BINDS) 2> nul
    -rmdir /q /s dist 2> nul
    -del docsrc\detours.chm 2> nul
    -del /q *.msi 2>nul

test:
    cd "$(MAKEDIR)\samples"
    @$(MAKE) /$(MAKEFLAGS) test
    cd "$(MAKEDIR)"

################################################################# End of File.
