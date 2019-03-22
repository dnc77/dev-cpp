# History of changes:
#
# 28 Jan 2019              added client
# 03 Feb 2019              added logger
# 23 Mar 2019              added root directory notice and check

# Root path
ROOTDIR                    := /home/duncan/dev-cpp/lib/net/tcplib/

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
ECHO                       := @echo
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

# Project names
LOGGER                     := logger
CYCBUF                     := cycbuf
BECODE                     := becode
NETCOMMON                  := netcommon
SERVER                     := server
CLIENT                     := client
TEST                       := test

# Individual project include locations
LOGGERINCDIR               := $(INCDIR)
CYCBUFINCDIR               := $(INCDIR)
BECODEINCDIR               := $(INCDIR)
NETCOMMONINCDIR            := $(INCDIR)
SERVERINCDIR               := $(INCDIR)
CLIENTINCDIR               := $(INCDIR)

# Individual project source locations
LOGGERSRCDIR               := $(SRCDIR)$(LOGGER)/
CYCBUFSRCDIR               := $(SRCDIR)$(CYCBUF)/
BECODESRCDIR               := $(SRCDIR)coding/
NETCOMMONSRCDIR            := $(SRCDIR)$(NETCOMMON)/
SERVERSRCDIR               := $(SRCDIR)$(SERVER)/
CLIENTSRCDIR               := $(SRCDIR)$(CLIENT)/
TESTSRCDIR                 := $(SRCDIR)$(TEST)/

# Individual project include files
LOGGERINC                  := $(LOGGERINCDIR)logger.h
CYCBUFINC                  := $(CYCBUFINCDIR)cycbuf.h
BECODEINC                  := $(BECODEINCDIR)becode.h
NETCOMMONINC               := $(NETCOMMONINCDIR)helpers.h \
                              $(NETCOMMONINCDIR)netnode.h \
                              $(NETCOMMONINCDIR)netaddress.h \
                              $(NETCOMMONINCDIR)netdataraw.h
SERVERINC                  := $(SERVERINCDIR)server.h \
                              $(SERVERINCDIR)serversync.h \
                              $(SERVERINCDIR)serverasync.h
CLIENTINC                  := $(CLIENTINCDIR)client.h

# Individual project source files
LOGGERSRC                  := $(LOGGERSRCDIR)logger.c
CYCBUFSRC                  := $(CYCBUFSRCDIR)cycbuf.cpp
BECODESRC                  := $(BECODESRCDIR)becode.cpp
NETCOMMONSRC               := $(NETCOMMONSRCDIR)netnode.cpp \
                              $(NETCOMMONSRCDIR)netaddress.cpp \
                              $(NETCOMMONSRCDIR)netdataraw.cpp
SERVERSRC                  := $(SERVERSRCDIR)server.cpp \
                              $(SERVERSRCDIR)serversync.cpp \
                              $(SERVERSRCDIR)serverasync.cpp
CLIENTSRC                  := $(CLIENTSRCDIR)client.cpp
TESTSRC                    := $(TESTSRCDIR)main.cpp

# Project object files
LOGGER_RDBG                := $(OBJDIR_RDBG)$(LOGGER).o
LOGGER_RREL                := $(OBJDIR_RREL)$(LOGGER).o
CYCBUF_RDBG                := $(OBJDIR_RDBG)$(CYCBUF).o
CYCBUF_RREL                := $(OBJDIR_RREL)$(CYCBUF).o
BECODE_RDBG                := $(OBJDIR_RDBG)$(BECODE).o
BECODE_RREL                := $(OBJDIR_RREL)$(BECODE).o
NETCOMMONOBJ_RDBG          := $(OBJDIR_RDBG)$(NETCOMMON).o
NETCOMMONOBJ_RREL          := $(OBJDIR_RDBG)$(NETCOMMON).o
SERVEROBJ_RDBG             := $(OBGDIR_RDBG)$(SERVER).o
SERVEROBJ_RREL             := $(OBGDIR_RREL)$(SERVER).o
CLIENTOBJ_RDBG             := $(OBGDIR_RDBG)$(CLIENT).o
CLIENTOBJ_RREL             := $(OBGDIR_RREL)$(CLIENT).o

# Project library link options
# Libraries:
# stdc++ - c++ library
# m - math library
# dl - dynamic loading library
TESTLIB_RDBG               := -lm -pthread $(GCCLIB)stdc++
TESTLIB_RREL               := -lm -pthread $(GCCLIB)stdc++

# Project output files
LOGGERBIN_RDBG             := $(BINDIR_RDBG)$(LOGGER).a
LOGGERBIN_RREL             := $(BINDIR_RREL)$(LOGGER).a
CYCBUFBIN_RDBG             := $(BINDIR_RDBG)$(CYCBUF).a
CYCBUFBIN_RREL             := $(BINDIR_RREL)$(CYCBUF).a
BECODEBIN_RDBG             := $(BINDIR_RDBG)$(BECODE).a
BECODEBIN_RREL             := $(BINDIR_RREL)$(BECODE).a
NETCOMMONBIN_RDBG          := $(BINDIR_RDBG)$(NETCOMMON).a
NETCOMMONBIN_RREL          := $(BINDIR_RREL)$(NETCOMMON).a
SERVERBIN_RDBG             := $(BINDIR_RDBG)$(SERVER).a
SERVERBIN_RREL             := $(BINDIR_RREL)$(SERVER).a
CLIENTBIN_RDBG             := $(BINDIR_RDBG)$(CLIENT).a
CLIENTBIN_RREL             := $(BINDIR_RREL)$(CLIENT).a
TESTBIN_RDBG               := $(BINDIR_RDBG)$(TEST)
TESTBIN_RREL               := $(BINDIR_RREL)$(TEST)

# Project dependencies
TESTDEP_RDBG               := $(SERVERBIN_RDBG) $(CLIENTBIN_RDBG) \
                              $(NETCOMMONBIN_RDBG) $(LOGGERBIN_RDBG) \
                              $(CYCBUF_RDBG) $(BECODE_RDBG)
TESTDEP_RREL               := $(SERVERBIN_RREL) $(CLIENTBIN_RREL) \
                              $(NETCOMMONBIN_RREL) $(LOGGERBIN_RREL) \
                              $(CYCBUF_RREL) $(BECODE_RREL)

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

rules : rootnotice
	$(ECHO) '   all:    all projects (debug and release)'
	$(ECHO) '   dbg:    all the debug projects'
	$(ECHO) '   rel:    all the release projects'
	$(ECHO) '   clean:  remove all'
	$(ECHO) ""

rootnotice :
	$(ECHO) '** NOTE: working root directory is $(ROOTDIR) **'
	$(ECHO) '   Is this correct in makefile?'
	@[ -d $(ROOTDIR) ]
	$(ECHO) ""

# All builds
all : rootnotice dbg rel

dbg : mkdbgdirs \
      $(LOGGERBIN_RDBG) \
      $(CYCBUFBIN_RDBG) \
      $(BECODEBIN_RDBG) \
      $(NETCOMMONBIN_RDBG) \
      $(SERVERBIN_RDBG) \
      $(CLIENTBIN_RDBG) \
      $(TESTBIN_RDBG)

rel : mkreldirs \
      $(LOGGERBIN_RREL) \
      $(CYCBUFBIN_RREL) \
      $(BECODEBIN_RREL) \
      $(NETCOMMONBIN_RREL) \
      $(SERVERBIN_RREL) \
      $(CLIENTBIN_RREL) \
      $(TESTBIN_RREL)

# Create required directories
mkdbgdirs : rootnotice
	$(MKDIR) $(BINDIR_RDBG)
	$(MKDIR) $(OBJDIR_RDBG)

mkreldirs : rootnotice
	$(MKDIR) $(BINDIR_RREL)
	$(MKDIR) $(OBJDIR_RREL)

clean : rootnotice
	$(RMDIR) $(BINDIR)*
	$(RMDIR) $(OBJDIR)*

memchk :
	$(VALGRIND) $(VALGRINDOPTFULL) $(TESTBIN_RDBG)

# logger debug build
$(LOGGERBIN_RDBG) : $(LOGGERSRC) $(LOGGERINC)
	$(GCC) $(OBJCOPT_RDBG) $(LOGGERSRC)
	$(AR) rc $(LOGGERBIN_RDBG) *.o
	$(MV) *.o $(OBJDIR_RDBG)

# logger release build
$(LOGGERBIN_RREL) : $(LOGGERSRC) $(LOGGERINC)
	$(GCC) $(OBJCOPT_RREL) $(LOGGERSRC)
	$(AR) rc $(LOGGERBIN_RREL) *.o
	$(MV) *.o $(OBJDIR_RREL)

# cycbuf debug build
$(CYCBUFBIN_RDBG) : $(CYCBUFSRC) $(CYCBUFINC)
	$(GCC) $(OBJGCCOPT_RDBG) $(CYCBUFSRC)
	$(AR) rc $(CYCBUFBIN_RDBG) *.o
	$(MV) *.o $(OBJDIR_RDBG)

# cycbuf release build
$(CYCBUFBIN_RREL) : $(CYCBUFSRC) $(CYCBUFINC)
	$(GCC) $(OBJGCCOPT_RREL) $(CYCBUFSRC)
	$(AR) rc $(CYCBUFBIN_RREL) *.o
	$(MV) *.o $(OBJDIR_RREL)

# becode debug build
$(BECODEBIN_RDBG) : $(BECODESRC) $(BECODEINC)
	$(GCC) $(OBJGCCOPT_RDBG) $(BECODESRC)
	$(AR) rc $(BECODEBIN_RDBG) *.o
	$(MV) *.o $(OBJDIR_RDBG)

# becode release build
$(BECODEBIN_RREL) : $(BECODESRC) $(BECODEINC)
	$(GCC) $(OBJGCCOPT_RREL) $(BECODESRC)
	$(AR) rc $(BECODEBIN_RREL) *.o
	$(MV) *.o $(OBJDIR_RREL)

# netnode debug build
$(NETCOMMONBIN_RDBG) : $(NETCOMMONSRC) $(NETCOMMONINC)
	$(GCC) $(OBJGCCOPT_RDBG) $(NETCOMMONSRC)
	$(AR) rc $(NETCOMMONBIN_RDBG) *.o
	$(MV) *.o $(OBJDIR_RDBG)

# netnode release build
$(NETCOMMONBIN_RREL) : $(NETCOMMONSRC) $(NETCOMMONINC)
	$(GCC) $(OBJGCCOPT_RREL) $(NETCOMMONSRC)
	$(AR) rc $(NETCOMMONBIN_RREL) *.o
	$(MV) *.o $(OBJDIR_RREL)

# server debug build
$(SERVERBIN_RDBG) : $(SERVERSRC)
	$(GCC) $(OBJGCCOPT_RDBG) $(SERVERSRC)
	$(AR) rc $(SERVERBIN_RDBG) *.o
	$(MV) *.o $(OBJDIR_RDBG)

# server release build
$(SERVERBIN_RREL) : $(SERVERSRC)
	$(GCC) $(OBJGCCOPT_RREL) $(SERVERSRC)
	$(AR) rc $(SERVERBIN_RREL) *.o
	$(MV) *.o $(OBJDIR_RREL)

# client debug build
$(CLIENTBIN_RDBG) : $(CLIENTSRC)
	$(GCC) $(OBJGCCOPT_RDBG) $(CLIENTSRC)
	$(AR) rc $(CLIENTBIN_RDBG) *.o
	$(MV) *.o $(OBJDIR_RDBG)

# client release build
$(CLIENTBIN_RREL) : $(CLIENTSRC)
	$(GCC) $(OBJGCCOPT_RREL) $(CLIENTSRC)
	$(AR) rc $(CLIENTBIN_RREL) *.o
	$(MV) *.o $(OBJDIR_RREL)

# test debug build
$(TESTBIN_RDBG) : $(TESTSRC) $(TESTDEP_RDBG)
	$(GCC) $(BINGCCOPT_RDBG) $(TESTLIB_RDBG) $(GCCOUTFILE) $@ $^

# test release build
$(TESTBIN_RREL) : $(TESTSRC) $(TESTDEP_RREL)
	$(GCC) $(BINGCCOPT_RREL) $(TESTLIB_RREL) $(GCCOUTFILE) $@ $^

