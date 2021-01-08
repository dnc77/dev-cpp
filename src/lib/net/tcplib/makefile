# History of changes:
#
# 28 Jan 2019              added client
# 03 Feb 2019              added logger
# 23 Mar 2019              added root directory notice and check
# 28 Mar 2019              start getting dependent libs from global locations
# 16 Oct 2020              introducing global compilers and tools

# Get global definitions makefile.
MKPATH                     := $(shell dirname\
                                 $(realpath $(lastword $(MAKEFILE_LIST)))\
                              )/
include                    $(MKPATH)../../../../makefile.def

LIBCATEGORY                := $(LIBCAT_NET)
PRJMAIN                    := $(LIBNET_TCPLIB)
PRJLOGGER                  := logger
PRJNETCOMMON               := netcommon
PRJCLIENT                  := client
PRJSERVER                  := server
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
GC                         := gcc -x c
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
GCCNOFORMATWRN             := -Wformat=0
GCCDEBUG                   := -g
GCCCOMPILEONLY             := -c
GCCOUTFILE                 := -o
GCCLIB                     := -l
GCCINCDIR                  := -I
GCCLIBDIR                  := -L

# External libraries include locations

# External libraries library dirs
LIBDATDIR_RDBG             := $(TOPLIBDIR)$(LIBCAT_DATASTRUCT)/$(RASPDEBUG)/
LIBDATDIR_RREL             := $(TOPLIBDIR)$(LIBCAT_DATASTRUCT)/$(RASPREL)/
LIBENCDIR_RDBG             := $(TOPLIBDIR)$(LIBCAT_ENCODE)/$(RASPDEBUG)/
LIBENCDIR_RREL             := $(TOPLIBDIR)$(LIBCAT_ENCODE)/$(RASPREL)/

# External libraries
LIBDAT_CYCBUF_RDBG         := $(LIBDATDIR_RDBG)$(LIBDAT_CYCBUF).a
LIBDAT_CYCBUF_RREL         := $(LIBDATDIR_RREL)$(LIBDAT_CYCBUF).a
LIBENC_BECODE_RDBG         := $(LIBENCDIR_RDBG)$(LIBENC_BECODE).a
LIBENC_BECODE_RREL         := $(LIBENCDIR_RREL)$(LIBENC_BECODE).a

# Individual project include locations
TCPLIB_INCDIR              := $(INCDIR)$(LIBCATEGORY)/
TEST_INCDIR                := $(INCDIR)$(LIBCATEGORY)/

# Individual project source locations
LOGGER_SRCDIR              := $(SRCDIR)$(PRJLOGGER)/
NETCOMMON_SRCDIR           := $(SRCDIR)$(PRJNETCOMMON)/
CLIENT_SRCDIR              := $(SRCDIR)$(PRJCLIENT)/
SERVER_SRCDIR              := $(SRCDIR)$(PRJSERVER)/
TEST_SRCDIR                := $(SRCDIR)$(PRJTEST)/

# Individual project include files
LOGGERINC                  := $(TCPLIB_INCDIR)$(PRJLOGGER).h
NETCOMMONINC               := $(TCPLIB_INCDIR)netaddress.h \
                              $(TCPLIB_INCDIR)netnode.h \
                              $(TCPLIB_INCDIR)netdataraw.h
CLIENTINC                  := $(TCPLIB_INCDIR)$(PRJCLIENT).h
SERVERINC                  := $(TCPLIB_INCDIR)$(PRJSERVER).h \
                              $(TCPLIB_INCDIR)$(PRJSERVER)async.h \
                              $(TCPLIB_INCDIR)$(PRJSERVER)sync.h
TCPLIBINC                  := $(NETCOMMONINC) \
                              $(CLIENTINC) $(SERVERINC)
TESTINC                    :=

# Individual project source files
LOGGERSRC                  := $(LOGGER_SRCDIR)$(PRJLOGGER).c
NETCOMMONSRC               := $(NETCOMMON_SRCDIR)netaddress.cpp \
                              $(NETCOMMON_SRCDIR)netnode.cpp \
                              $(NETCOMMON_SRCDIR)netdataraw.cpp
CLIENTSRC                  := $(CLIENT_SRCDIR)$(PRJCLIENT).cpp
SERVERSRC                  := $(SERVER_SRCDIR)$(PRJSERVER).cpp \
                              $(SERVER_SRCDIR)$(PRJSERVER)async.cpp \
                              $(SERVER_SRCDIR)$(PRJSERVER)sync.cpp
TCPLIBSRC                  := $(NETCOMMONSRC) \
                              $(CLIENTSRC) $(SERVERSRC)
TESTSRC                    := $(TEST_SRCDIR)main.cpp

# Project object files
LOGGER_OBJ_RDBG            := $(OBJDIR_RDBG)$(PRJLOGGER).o
LOGGER_OBJ_RREL            := $(OBJDIR_RREL)$(PRJLOGGER).o
NETCOMMON_OBJ_RDBG         := $(OBJDIR_RDBG)netaddress.o \
                              $(OBJDIR_RDBG)netnode.o \
                              $(OBJDIR_RDBG)netdataraw.o
NETCOMMON_OBJ_RREL         := $(OBJDIR_RREL)netaddress.o \
                              $(OBJDIR_RREL)netnode.o \
                              $(OBJDIR_RREL)netdataraw.o
CLIENT_OBJ_RDBG            := $(OBJDIR_RDBG)$(PRJCLIENT).o
CLIENT_OBJ_RREL            := $(OBJDIR_RREL)$(PRJCLIENT).o
SERVER_OBJ_RDBG            := $(OBJDIR_RDBG)$(PRJSERVER).o \
                              $(OBJDIR_RDBG)$(PRJSERVER)async.o \
                              $(OBJDIR_RDBG)$(PRJSERVER)aync.o 
SERVER_OBJ_RREL            := $(OBJDIR_RREL)$(PRJSERVER).o \
                              $(OBJDIR_RREL)$(PRJSERVER)async.o \
                              $(OBJDIR_RREL)$(PRJSERVER)aync.o 
TCPLIB_OBJ_RDBG            := $(LOGGER_OBJ_RDBG) $(NETCOMMON_OBJ_RDBG) \
                              $(CLIENT_OBJ_RDBG) $(SERVER_OBJ_RDBG)
TCPLIB_OBJ_RREL            := $(LOGGER_OBJ_RREL) $(NETCOMMON_OBJ_RREL) \
                              $(CLIENT_OBJ_RREL) $(SERVER_OBJ_RREL)
TEST_OBJ_RDBG              := $(OBJDIR_RDBG)$(TEST).o
TEST_OBJ_RREL              := $(OBJDIR_RREL)$(TEST).o

# Project library link options
# Libraries:
# stdc++ - c++ library
# m - math library
# dl - dynamic loading library
LOGGER_LNKLIB_RDBG         :=
LOGGER_LNKLIB_RREL         :=
TCPLIB_LNKLIB_RDBG         := $(GCCLIB)m -pthread
TCPLIB_LNKLIB_RREL         := $(GCCLIB)m -pthread
TEST_LNKLIB_RDBG           := $(TCPLIB_LNKLIB_RDBG) $(GCCLIB)stdc++
TEST_LNKLIB_RREL           := $(TCPLIB_LNKLIB_RREL) $(GCCLIB)stdc++

# Project output files
LOGGER_RDBG                := $(LIBDIR_RDBG)$(PRJLOGGER).a
LOGGER_RREL                := $(LIBDIR_RREL)$(PRJLOGGER).a
TCPLIB_RDBG                := $(LIBDIR_RDBG)$(PRJMAIN).a
TCPLIB_RREL                := $(LIBDIR_RREL)$(PRJMAIN).a
TEST_RDBG                  := $(LIBDIR_RDBG)$(PRJMAIN)$(PRJTEST)
TEST_RREL                  := $(LIBDIR_RREL)$(PRJMAIN)$(PRJTEST)

# Project dependencies
LOGGERDEP_RDBG             := 
LOGGERDEP_RREL             := 
TCPLIBDEP_RDBG             := $(LIBDAT_CYCBUF_RDBG) $(LIBENC_BECODE_RDBG)\
                              $(LOGGER_RDBG)
TCPLIBDEP_RREL             := $(LIBDAT_CYCBUF_RDBG) $(LIBENC_BECODE_RREL)\
                              $(LOGGER_RREL)
TESTDEP_RDBG               := $(TCPLIB_RDBG)
TESTDEP_RREL               := $(TCPLIB_RREL)

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

dbg : mkdbgdirs $(LOGGER_RDBG) $(TCPLIB_RDBG) $(TEST_RDBG)

rel : mkreldirs $(LOGGER_RREL) $(TCPLIB_RREL) $(TEST_RREL)

# Create required directories
mkdbgdirs : roottest
	@$(MKDIR) $(LIBDIR_RDBG)
	@$(MKDIR) $(OBJDIR_RDBG)

mkreldirs : roottest
	@$(MKDIR) $(LIBDIR_RREL)
	@$(MKDIR) $(OBJDIR_RREL)

clean : roottest
	@$(RMDIR) $(LOGGER_RDBG) $(TCPLIB_RDBG) $(TEST_RDBG)
	@$(RMDIR) $(LOGGER_RREL) $(TCPLIB_RREL) $(TEST_RREL)
	@$(RMDIR) $(OBJDIR)

memchk :
	$(VALGRIND) $(VALGRINDOPTFULL) $(BECODE_RDBG)

# logger debug build
$(LOGGER_RDBG) : $(LOGGERDEP_RDBG) $(LOGGERINC) $(LOGGERSRC)
	@$(ECHO) "dbg: Compiling and linking to $@"
	@$(GC) $(OBJCOPT_RDBG) $(LOGGERSRC) $(LOGGERDEP_RDBG)
	@$(MV) *.o $(OBJDIR_RDBG)
	@$(AR) rc $(LOGGER_RDBG) $(OBJDIR_RDBG)*.o

# logger release build
$(LOGGER_RREL) : $(LOGGERDEP_RREL) $(LOGGERINC) $(LOGGERSRC)
	@$(ECHO) "rel: Compiling and linking to $@"
	@$(GCC) $(OBJCOPT_RREL) $(LOGGERSRC) $(LOGGERDEP_RREL)
	@$(MV) *.o $(OBJDIR_RREL)
	@$(AR) rc $(LOGGER_RREL) $(OBJDIR_RREL)*.o

# tcplib debug build
$(TCPLIB_RDBG) : $(TCPLIBDEP_RDBG) $(TCPLIBINC) $(TCPLIBSRC)
	@$(ECHO) "dbg: Compiling and linking to $@"
	@$(GCC) $(OBJGCCOPT_RDBG) $(TCPLIBSRC)
	@$(MV) *.o $(OBJDIR_RDBG)
	@$(AR) rc $(TCPLIB_RDBG) $(OBJDIR_RDBG)*.o

# tcplib release build
$(TCPLIB_RREL) : $(TCPLIBDEP_RREL) $(TCPLIBINC) $(TCPLIBSRC)
	@$(ECHO) "rel: Compiling and linking to $@"
	@$(GCC) $(OBJGCCOPT_RREL) $(TCPLIBSRC)
	@$(MV) *.o $(OBJDIR_RREL)
	@$(AR) rc $(TCPLIB_RREL) $(OBJDIR_RREL)*.o

# test debug build
$(TEST_RDBG) : $(TESTSRC) $(TESTDEP_RDBG)
	@$(ECHO) "dbg: Compiling and linking to $@"
	@$(GCC) $(GCCOUTFILE) $(TEST_RDBG) $(TEST_LNKLIB_RDBG) $(BINGCCOPT_RDBG)\
		$(TESTSRC) $(TESTDEP_RDBG) $(TCPLIBDEP_RDBG)

# test release build
$(TEST_RREL) : $(TESTSRC) $(TESTDEP_RREL)
	@$(ECHO) "rel: Compiling and linking to $@"
	@$(GCC) $(GCCOUTFILE) $(TEST_RREL) $(TEST_LNKLIB_RREL) $(BINGCCOPT_RREL)\
		$(TESTSRC) $(TESTDEP_RREL) $(TCPLIBDEP_RREL)
