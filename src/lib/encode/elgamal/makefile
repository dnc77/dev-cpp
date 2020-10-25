# History of changes:
#
# 27 Mar 2019              introducing globalized compilation
# 16 Oct 2020              introducing global compilers and tools

# Get global definitions makefile.
MKPATH                     := $(shell dirname\
                                 $(realpath $(lastword $(MAKEFILE_LIST)))\
                              )
include                    $(MKPATH)/../../../../makefile.def

LIBCATEGORY                := $(LIBCAT_ENCODE)
PRJMAIN                    := $(LIBENC_ELGAMAL)
PRJTEST                    := test

# Project root path
PRJROOTDIR                 := $(TOPSRCDIR)$(TOPLIB)/$(LIBCATEGORY)/$(PRJMAIN)/

# Main build directories
INCDIR                     := $(TOPINCDIR)
BINDIR                     := $(TOPBINDIR)
LIBDIR                     := $(TOPLIBDIR)$(LIBCATEGORY)/
SRCDIR                     := $(PRJROOTDIR)
OBJDIR                     := $(PRJROOTDIR)$(TOPOBJ)/

# Main output directories
BINDIR_RDBG                := $(BINDIR)$(RASPDEBUG)/
BINDIR_RREL                := $(BINDIR)$(RASPREL)/
LIBDIR_RDBG                := $(LIBDIR)$(RASPDEBUG)/
LIBDIR_RREL                := $(LIBDIR)$(RASPREL)/
OBJDIR_RDBG                := $(OBJDIR)$(RASPDEBUG)/
OBJDIR_RREL                := $(OBJDIR)$(RASPREL)/

# Compilers and tools (uncomment to override makefile.def)
# CD                         := cd
# MV                         := mv
# MKDIR                      := mkdir -p
# RMDIR                      := rm -Rf
# AR                         := ar
# GCC                        := gcc
# TOUCH                      := touch
# ECHO                       := echo
# VALGRIND                   := valgrind
# VALGRINDOPTFULL            := --leak-check=full --track-origins=yes \
#                               --track-fds=yes
# VALGRINDOUTPUT             :=

# Compiler flags
GCCSTD98                   := -std=c++98
GCCSTD11                   := -std=c++11
GCCSTD14                   := -std=c++14
GCCSTD17                   := -std=c++17
GCCPIC                     := -fPIC
GCCDEBUG                   := -g
GCCCOMPILEONLY             := -c
GCCOUTFILE                 := -o
GCCLIB                     := -l
GCCINCDIR                  := -I
GCCLIBDIR                  := -L

# External libraries include locations

# External libraries library dirs

# External libraries

# Individual project include locations
ELGAMAL_INCDIR             := $(INCDIR)$(LIBCATEGORY)/
TEST_INCDIR                := $(INCDIR)$(LIBCATEGORY)/

# Individual project source locations
ELGAMAL_SRCDIR             := $(SRCDIR)
TEST_SRCDIR                := $(SRCDIR)

# Individual project include files
ELGAMALINC                 := $(ELGAMAL_INCDIR)CElGamal.h
TESTINC                    := $(ELGAMALINC)

# Individual project source files
ELGAMALSRC                 := $(ELGAMAL_SRCDIR)CElGamal.cpp
TESTSRC                    := $(TEST_SRCDIR)main.cpp

# Project object files
ELGAMAL_OBJ_RDBG           := $(OBJDIR_RDBG)$(ELGAMAL).o
ELGAMAL_OBJ_RREL           := $(OBJDIR_RREL)$(ELGAMAL).o
TEST_OBJ_RDBG              := $(OBJDIR_RDBG)$(TEST).o
TEST_OBJ_RREL              := $(OBJDIR_RREL)$(TEST).o

# Project library link options
# Libraries:
# stdc++ - c++ library
# m - math library
# dl - dynamic loading library
ELGAMAL_LNKLIB_RDBG        :=
ELGAMAL_LNKLIB_RREL        :=
TEST_LNKLIB_RDBG           := $(ELGAMAL_LNKLIB_RDBG) $(GCCLIB)stdc++
TEST_LNKLIB_RREL           := $(ELGAMAL_LNKLIB_RREL) $(GCCLIB)stdc++

# Project output files
ELGAMAL_RDBG               := $(LIBDIR_RDBG)$(PRJMAIN).a
ELGAMAL_RREL               := $(LIBDIR_RREL)$(PRJMAIN).a
TEST_RDBG                  := $(LIBDIR_RDBG)$(PRJMAIN)$(PRJTEST)
TEST_RREL                  := $(LIBDIR_RREL)$(PRJMAIN)$(PRJTEST)

# Project dependencies
ELGAMALDEP_RDBG            := 
ELGAMALDEP_RREL            := 
TESTDEP_RDBG               := $(ELGAMAL_RDBG)
TESTDEP_RREL               := $(ELGAMAL_RREL)

# Individual project type compiler options
OBJCOPT_RDBG               := $(GCCDEBUG) $(GCCCOMPILEONLY) \
                              $(GCCINCDIR)$(INCDIR)
OBJCOPT_RREL               := $(GCCCOMPILEONLY) \
                              $(GCCINCDIR)$(INCDIR)
OBJGCCOPT_RDBG             := $(GCCDEBUG) $(GCCCOMPILEONLY) $(GCCSTD14) \
                              $(GCCINCDIR)$(INCDIR)
OBJGCCOPT_RREL             := $(GCCCOMPILEONLY) $(GCCSTD14) \
                              $(GCCINCDIR)$(INCDIR)
BINGCCOPT_RDBG             := $(GCCDEBUG) $(GCCSTD14) $(GCCINCDIR)$(INCDIR)
BINGCCOPT_RREL             := $(GCCSTD14) $(GCCINCDIR)$(INCDIR)

rules : roottest
	@$(ECHO) '   all:    all projects (debug and release)'
	@$(ECHO) '   dbg:    all the debug projects'
	@$(ECHO) '   rel:    all the release projects'
	@$(ECHO) '   clean:  remove all'
	@$(ECHO) ""

roottest :
	@$(ECHO) 'Checking for ' $(GLOBALROOTDIR)
	@[ -d $(GLOBALROOTDIR) ]
	@$(ECHO) 'Checking for ' $(GLOBALROOTDIR)makefile.def
	@[ -f $(GLOBALROOTDIR)makefile.def ]
	@$(ECHO) 'Checking for ' $(PRJROOTDIR)makefile
	@[ -f $(PRJROOTDIR)makefile ]
	@$(ECHO) ""

# All builds
all : dbg rel

dbg : mkdbgdirs $(ELGAMAL_RDBG) $(TEST_RDBG)

rel : mkreldirs $(ELGAMAL_RREL) $(TEST_RREL)

# Create required directories
mkdbgdirs : roottest
	@$(MKDIR) $(LIBDIR_RDBG)
	@$(MKDIR) $(OBJDIR_RDBG)

mkreldirs : roottest
	@$(MKDIR) $(LIBDIR_RREL)
	@$(MKDIR) $(OBJDIR_RREL)

clean : roottest
	@$(RMDIR) $(ELGAMAL_RDBG) $(TEST_RDBG)
	@$(RMDIR) $(ELGAMAL_RREL) $(TEST_RREL)
	@$(RMDIR) $(OBJDIR)

memchk :
	$(VALGRIND) $(VALGRINDOPTFULL) $(ELGAMAL_RDBG)

# elgamal debug build
$(ELGAMAL_RDBG) : $(ELGAMALDEP_RDBG) $(ELGAMALINC) $(ELGAMALSRC)
	@$(ECHO) "dbg: Compiling and linking to $@"
	@$(GCC) $(OBJGCCOPT_RDBG) $(ELGAMALSRC) $(ELGAMALDEP_RDBG)
	@$(MV) *.o $(OBJDIR_RDBG)
	@$(AR) rc $(ELGAMAL_RDBG) $(OBJDIR_RDBG)*.o

# elgamal release build
$(ELGAMAL_RREL) : $(BELGAMALDEP_RREL) $(ELGAMALINC) $(ELGAMALSRC)
	@$(ECHO) "rel: Compiling and linking to $@"
	@$(GCC) $(OBJGCCOPT_RREL) $(ELGAMALSRC) $(ELGAMALDEP_RREL)
	@$(MV) *.o $(OBJDIR_RREL)
	@$(AR) rc $(ELGAMAL_RREL) $(OBJDIR_RREL)*.o

# test debug build
$(TEST_RDBG) : $(TESTSRC) $(TESTDEP_RDBG)
	@$(ECHO) "dbg: Compiling and linking to $@"
	@$(GCC) $(GCCOUTFILE) $(TEST_RDBG) $(TEST_LNKLIB_RDBG) $(BINGCCOPT_RDBG)\
		$(TESTSRC) $(TESTDEP_RDBG)

# test release build
$(TEST_RREL) : $(TESTSRC) $(TESTDEP_RREL)
	@$(ECHO) "rel: Compiling and linking to $@"
	@$(GCC) $(GCCOUTFILE) $(TEST_RREL) $(TEST_LNKLIB_RREL) $(BINGCCOPT_RREL)\
		$(TESTSRC) $(TESTDEP_RREL)
