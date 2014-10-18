##############################################################################
##
##  Makefile for Detours Test Programs.
##
##  Microsoft Research Detours Package, Version 3.0.
##
##  Copyright (c) Microsoft Corporation.  All rights reserved.
##
##  Note:
##    syelog, setdll, and withdll must be built first because a number of the
##    other samples depend on them.
##

ROOT=..
!include .\common.mak

##############################################################################

all:
    cd "$(MAKEDIR)\simple"
    @$(MAKE)  /$(MAKEFLAGS)
    cd "$(MAKEDIR)\setdll"
    @$(MAKE)  /$(MAKEFLAGS)
    cd "$(MAKEDIR)\withdll"
    @$(MAKE)  /$(MAKEFLAGS)

clean:
    cd "$(MAKEDIR)\simple"
    @$(MAKE)  /$(MAKEFLAGS) clean
    cd "$(MAKEDIR)\slept"
    @$(MAKE)  /$(MAKEFLAGS) clean
    cd "$(MAKEDIR)\setdll"
    @$(MAKE)  /$(MAKEFLAGS) clean
    cd "$(MAKEDIR)\withdll"
    @$(MAKE)  /$(MAKEFLAGS) clean
    -rmdir lib32 2>nul
    -rmdir lib64 2>nul
    -rmdir include 2>nul

realclean:
    cd "$(MAKEDIR)\simple"
    @$(MAKE)  /$(MAKEFLAGS) realclean
    cd "$(MAKEDIR)\slept"
    @$(MAKE)  /$(MAKEFLAGS) realclean
    cd "$(MAKEDIR)\setdll"
    @$(MAKE)  /$(MAKEFLAGS) realclean
    cd "$(MAKEDIR)\withdll"
    @$(MAKE)  /$(MAKEFLAGS) realclean
    -rmdir lib32 2>nul
    -rmdir lib64 2>nul
    -rmdir include 2>nul

test:
    cd "$(MAKEDIR)\simple"
    @$(MAKE)  /$(MAKEFLAGS) test
    cd "$(MAKEDIR)\slept"
    @$(MAKE)  /$(MAKEFLAGS) test
    cd "$(MAKEDIR)\setdll"
    @$(MAKE)  /$(MAKEFLAGS) test
    cd "$(MAKEDIR)\withdll"
    @$(MAKE)  /$(MAKEFLAGS) test
##
################################################################# End of File.
