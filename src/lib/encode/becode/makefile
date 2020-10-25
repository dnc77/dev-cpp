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
PRJMAIN                    := $(LIBENC_BECODE)
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
BECODE_INCDIR              := $(INCDIR)$(LIBCATEGORY)/
TEST_INCDIR                := $(INCDIR)$(LIBCATEGORY)/

# Individual project source locations
BECODE_SRCDIR              := $(SRCDIR)
TEST_SRCDIR                := $(SRCDIR)

# Individual project include files
BECODEINC                  := $(BECODE_INCDIR)$(PRJMAIN).h
TESTINC                    := $(BECODEINC)

# Individual project source files
BECODESRC                  := $(BECODE_SRCDIR)$(PRJMAIN).cpp
TESTSRC                    := $(TEST_SRCDIR)main.cpp

# Project object files
BECODE_OBJ_RDBG            := $(OBJDIR_RDBG)$(BECODE).o
BECODE_OBJ_RREL            := $(OBJDIR_RREL)$(BECODE).o
TEST_OBJ_RDBG              := $(OBJDIR_RDBG)$(TEST).o
TEST_OBJ_RREL              := $(OBJDIR_RREL)$(TEST).o

# Project library link options
# Libraries:
# stdc++ - c++ library
# m - math library
# dl - dynamic loading library
BECODE_LNKLIB_RDBG         := $(GCCLIB)m -pthread
BECODE_LNKLIB_RREL         := $(GCCLIB)m -pthread
TEST_LNKLIB_RDBG           := $(BECODE_LNKLIB_RDBG) $(GCCLIB)stdc++
TEST_LNKLIB_RREL           := $(BECODE_LNKLIB_RREL) $(GCCLIB)stdc++

# Project output files
BECODE_RDBG                := $(LIBDIR_RDBG)$(PRJMAIN).a
BECODE_RREL                := $(LIBDIR_RREL)$(PRJMAIN).a
TEST_RDBG                  := $(LIBDIR_RDBG)$(PRJMAIN)$(PRJTEST)
TEST_RREL                  := $(LIBDIR_RREL)$(PRJMAIN)$(PRJTEST)

# Project dependencies
BECODEDEP_RDBG             := 
BECODEDEP_RREL             := 
TESTDEP_RDBG               := $(BECODE_RDBG)
TESTDEP_RREL               := $(BECODE_RREL)

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

dbg : mkdbgdirs $(BECODE_RDBG) $(TEST_RDBG)

rel : mkreldirs $(BECODE_RREL) $(TEST_RREL)

# Create required directories
mkdbgdirs : roottest
	@$(MKDIR) $(LIBDIR_RDBG)
	@$(MKDIR) $(OBJDIR_RDBG)

mkreldirs : roottest
	@$(MKDIR) $(LIBDIR_RREL)
	@$(MKDIR) $(OBJDIR_RREL)

clean : roottest
	@$(RMDIR) $(BECODE_RDBG) $(TEST_RDBG)
	@$(RMDIR) $(BECODE_RREL) $(TEST_RREL)
	@$(RMDIR) $(OBJDIR)

memchk :
	$(VALGRIND) $(VALGRINDOPTFULL) $(BECODE_RDBG)

# becode debug build
$(BECODE_RDBG) : $(BECODEDEP_RDBG) $(BECODEINC) $(BECODESRC)
	@$(ECHO) "dbg: Compiling and linking to $@"
	@$(GCC) $(OBJGCCOPT_RDBG) $(BECODESRC) $(BECODEDEP_RDBG)
	@$(MV) *.o $(OBJDIR_RDBG)
	@$(AR) rc $(BECODE_RDBG) $(OBJDIR_RDBG)*.o

# becode release build
$(BECODE_RREL) : $(BECODEDEP_RREL) $(BECODEINC) $(BECODESRC)
	@$(ECHO) "rel: Compiling and linking to $@"
	@$(GCC) $(OBJGCCOPT_RREL) $(BECODESRC) $(BECODEDEP_RREL)
	@$(MV) *.o $(OBJDIR_RREL)
	@$(AR) rc $(BECODE_RREL) $(OBJDIR_RREL)*.o

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
