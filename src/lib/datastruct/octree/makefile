# History of changes:
#
# 26 Mar 2019              introducing globalized compilation

# Get global definitions makefile.
MKPATH                     := $(shell dirname\
                                 $(realpath $(lastword $(MAKEFILE_LIST)))\
                              )
include                    $(MKPATH)/../../../../makefile.def

LIBCATEGORY                := $(LIBCAT_DATASTRUCT)
PRJMAIN                    := $(LIBDAT_OCTREE)
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

# Compilers and tools
CD                         := cd
MV                         := mv
MKDIR                      := mkdir -p
RMDIR                      := rm -Rf
AR                         := ar
GCC                        := gcc
TOUCH                      := touch
ECHO                       := echo
VALGRIND                   := valgrind
VALGRINDOPTFULL            := --leak-check=full --track-origins=yes \
                              --track-fds=yes
VALGRINDOUTPUT             :=

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
OCTREE_INCDIR              := $(INCDIR)$(LIBCATEGORY)/
TEST_INCDIR                := $(INCDIR)$(LIBCATEGORY)/

# Individual project source locations
OCTREE_SRCDIR              := $(SRCDIR)
TEST_SRCDIR                := $(SRCDIR)

# Individual project include files
OCTREEINC                  := $(OCTREE_INCDIR)$(PRJMAIN).h
TESTINC                    := $(OCTREEINC)

# Individual project source files
OCTREESRC                  := $(OCTREE_SRCDIR)$(PRJMAIN).cpp
TESTSRC                    := $(TEST_SRCDIR)main.cpp

# Project object files
OCTREE_OBJ_RDBG            := $(OBJDIR_RDBG)$(OCTREE).o
OCTREE_OBJ_RREL            := $(OBJDIR_RREL)$(OCTREE).o
TEST_OBJ_RDBG              := $(OBJDIR_RDBG)$(TEST).o
TEST_OBJ_RREL              := $(OBJDIR_RREL)$(TEST).o

# Project library link options
# Libraries:
# stdc++ - c++ library
# m - math library
# dl - dynamic loading library
OCTREE_LNKLIB_RDBG         :=
OCTREE_LNKLIB_RREL         :=
TEST_LNKLIB_RDBG           := $(OCTREE_LNKLIB_RDBG) $(GCCLIB)stdc++
TEST_LNKLIB_RREL           := $(OCTREE_LNKLIB_RREL) $(GCCLIB)stdc++

# Project output files
OCTREE_RDBG                := $(LIBDIR_RDBG)$(PRJMAIN).a
OCTREE_RREL                := $(LIBDIR_RREL)$(PRJMAIN).a
TEST_RDBG                  := $(LIBDIR_RDBG)$(PRJMAIN)$(PRJTEST)
TEST_RREL                  := $(LIBDIR_RREL)$(PRJMAIN)$(PRJTEST)

# Project dependencies
OCTREEDEP_RDBG             := 
OCTREEDEP_RREL             := 
TESTDEP_RDBG               := $(OCTREE_RDBG)
TESTDEP_RREL               := $(OCTREE_RREL)

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

dbg : mkdbgdirs $(OCTREE_RDBG) $(TEST_RDBG)

rel : mkreldirs $(OCTREE_RREL) $(TEST_RREL)

# Create required directories
mkdbgdirs : roottest
	@$(MKDIR) $(LIBDIR_RDBG)
	@$(MKDIR) $(OBJDIR_RDBG)

mkreldirs : roottest
	@$(MKDIR) $(LIBDIR_RREL)
	@$(MKDIR) $(OBJDIR_RREL)

clean : roottest
	@$(RMDIR) $(OCTREE_RDBG) $(TEST_RDBG)
	@$(RMDIR) $(OCTREE_RREL) $(TEST_RREL)
	@$(RMDIR) $(OBJDIR)

memchk :
	$(VALGRIND) $(VALGRINDOPTFULL) $(OCTREE_RDBG)

# octree debug build
$(OCTREE_RDBG) : $(OCTREEDEP_RDBG) $(OCTREEINC) $(OCTREESRC)
	@$(ECHO) "dbg: Compiling and linking to $@"
	@$(GCC) $(OBJGCCOPT_RDBG) $(OCTREESRC) $(OCTREEDEP_RDBG)
	@$(MV) *.o $(OBJDIR_RDBG)
	@$(AR) rc $(OCTREE_RDBG) $(OBJDIR_RDBG)*.o

# octree release build
$(OCTREE_RREL) : $(OCTREEDEP_RREL) $(OCTREEINC) $(OCTREESRC)
	@$(ECHO) "rel: Compiling and linking to $@"
	@$(GCC) $(OBJGCCOPT_RREL) $(OCTREESRC) $(OCTREEDEP_RREL)
	@$(MV) *.o $(OBJDIR_RREL)
	@$(AR) rc $(OCTREE_RREL) $(OBJDIR_RREL)*.o

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
