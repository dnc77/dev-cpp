# History of changes:
#

# Root path
ROOTDIR                    := /home/duncan/cpp/tcplib/

# Main build directories
INCDIR                     := $(ROOTDIR)inc/
SRCDIR                     := $(ROOTDIR)src/
OBJDIR                     := $(ROOTDIR)obj/
LIBDIR                     := $(ROOTDIR)lib/
BINDIR                     := $(ROOTDIR)bin/

# Build configurations
RASPDEBUG                  := dbg
RASPREL                    := rel

# Main output directories
OBJDIR_RDBG                := $(OBJDIR)$(RASPDEBUG)/
OBJDIR_RREL                := $(OBJDIR)$(RASPREL)/
BINDIR_RDBG                := $(BINDIR)$(RASPDEBUG)/
BINDIR_RREL                := $(BINDIR)$(RASPREL)/

# Compilers and tools
MV                         := mv
MKDIR                      := mkdir -p
RMDIR                      := rm -Rf
AR                         := ar
GCC                        := gcc
TOUCH                      := touch
ECHO                       := echo
VALGRIND							:= valgrind
VALGRINDOPTFULL				:= --leak-check=full --track-origins=yes \
                              --track-fds=yes
VALGRINDOUTPUT					:=

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

# Project names
NETCOMMON                  := netcommon
SERVER                     := server
TEST                       := test

# Individual project include locations
NETCOMMONINCDIR            := $(INCDIR)
SERVERINCDIR               := $(INCDIR)

# Individual project source locations
NETCOMMONSRCDIR            := $(SRCDIR)$(NETCOMMON)/
SERVERSRCDIR               := $(SRCDIR)$(SERVER)/
TESTSRCDIR                 := $(SRCDIR)$(TEST)/

# Individual project include files
NETCOMMONINC               := $(NETCOMMONINCDIR)helpers.h \
                              $(NETCOMMONINCDIR)netnode.h \
                              $(NETCOMMONINCDIR)netaddress.h
SERVERINC                  := $(SERVERINCDIR)server.h \
                              $(SERVERINCDIR)serversync.h \
                              $(SERVERINCDIR)serverasync.h

# Individual project source files
NETCOMMONSRC               := $(NETCOMMONSRCDIR)netnode.cpp \
                              $(NETCOMMONSRCDIR)netaddress.cpp
SERVERSRC                  := $(SERVERSRCDIR)server.cpp \
                              $(SERVERSRCDIR)serversync.cpp \
                              $(SERVERSRCDIR)serverasync.cpp
TESTSRC                    := $(TESTSRCDIR)main.cpp

# Project object files
NETCOMMONOBJ_RDBG          := $(OBJDIR_RDBG)$(NETCOMMON).o
NETCOMMONOBJ_RREL          := $(OBJDIR_RDBG)$(NETCOMMON).o
SERVEROBJ_RDBG             := $(OBGDIR_RDBG)$(SERVER).o
SERVEROBJ_RREL             := $(OBGDIR_RREL)$(SERVER).o

# Project library link options
# Libraries:
# stdc++ - c++ library
# m - math library
# dl - dynamic loading library
TESTLIB_RDBG               := -pthread $(GCCLIB)stdc++
TESTLIB_RREL               := -pthread $(GCCLIB)stdc++

# Project output files
NETCOMMONBIN_RDBG          := $(BINDIR_RDBG)$(NETCOMMON).a
NETCOMMONBIN_RREL          := $(BINDIR_RREL)$(NETCOMMON).a
SERVERBIN_RDBG             := $(BINDIR_RDBG)$(SERVER).a
SERVERBIN_RREL             := $(BINDIR_RREL)$(SERVER).a
TESTBIN_RDBG               := $(BINDIR_RDBG)$(TEST)
TESTBIN_RREL               := $(BINDIR_RREL)$(TEST)

# Project dependencies
TESTDEP_RDBG               := $(SERVERBIN_RDBG) $(NETCOMMONBIN_RDBG)
TESTDEP_RREL               := $(SERVERBIN_RREL) $(NETCOMMONBIN_RREL)

# Individual project type compiler options
OBJGCCOPT_RDBG             := $(GCCDEBUG) $(GCCCOMPILEONLY) $(GCCSTD17) \
                              $(GCCINCDIR)$(INCDIR)
OBJGCCOPT_RREL             := $(GCCCOMPILEONLY) $(GCCSTD17) \
                              $(GCCINCDIR)$(INCDIR)
BINGCCOPT_RDBG             := $(GCCDEBUG) $(GCCSTD17) $(GCCINCDIR)$(INCDIR)
BINGCCOPT_RREL             := $(GCCSTD17) $(GCCINCDIR)$(INCDIR)

rules :
	@echo '   all:    all projects (debug and release)'
	@echo '   dbg:    all the debug projects'
	@echo '   rel:    all the release projects'
	@echo '   clean:  remove all'

# All builds
all : dbg rel

dbg : mkdbgdirs \
      $(NETCOMMONBIN_RDBG) \
      $(SERVERBIN_RDBG) \
      $(TESTBIN_RDBG)

rel : mkreldirs \
      $(NETCOMMONBIN_RREL) \
      $(SERVERBIN_RREL) \
      $(TESTBIN_RREL)

# Create required directories
mkdbgdirs :
	$(MKDIR) $(BINDIR_RDBG)
	$(MKDIR) $(OBJDIR_RDBG)

mkreldirs :
	$(MKDIR) $(BINDIR_RREL)
	$(MKDIR) $(OBJDIR_RREL)

clean :
	$(RMDIR) $(BINDIR)*
	$(RMDIR) $(OBJDIR)*

memchk :
	$(VALGRIND) $(VALGRINDOPTFULL) $(TESTBIN_RDBG)

# netnode debug build
$(NETCOMMONBIN_RDBG) : $(NETCOMMONSRC) $(NETCOMMONINC)
	$(GCC) $(OBJGCCOPT_RDBG) $(NETCOMMONSRC)
	$(AR) rc $(NETCOMMONBIN_RDBG) *.o
	$(MV) *.o $(OBJDIR_RDBG)

# netnode release build
$(NETCOMMONBIN_RREL) : $(NETCOMMONSRC) $(NETCOMMONINC)
	$(GCC) $(OBJGCCOPT_RREL) $(NETCOMMONSRC)
	$(AR) rc $(NETCOMMONBIN_RREL) *.o
	$(MV) *.o $(OBJDIR_RDBG)

# server debug build
$(SERVERBIN_RDBG) : $(SERVERSRC)
	$(GCC) $(OBJGCCOPT_RDBG) $(SERVERSRC)
	$(AR) rc $(SERVERBIN_RDBG) *.o
	$(MV) *.o $(OBJDIR_RDBG)

# server release build
$(SERVERBIN_RREL) : $(SERVERSRC)
	$(GCC) $(OBJGCCOPT_RREL) $(SERVERSRC)
	$(AR) rc $(SERVERBIN_RREL) *.o
	$(MV) *.o $(OBJDIR_RDBG)

# test debug build
$(TESTBIN_RDBG) : $(TESTSRC) $(TESTDEP_RDBG)
	$(GCC) $(BINGCCOPT_RDBG) $(TESTLIB_RDBG) $(GCCOUTFILE) $@ $^

# test release build
$(TESTBIN_RREL) : $(TESTSRC) $(TESTDEP_RREL)
	$(GCC) $(BINGCCOPT_RREL) $(TESTLIB_RREL) $(GCCOUTFILE) $@ $^

