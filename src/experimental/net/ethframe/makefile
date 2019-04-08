# History of changes:
#
# 28 Jan 2019              added client
# 03 Feb 2019              added logger
# 23 Mar 2019              added root directory notice and check
# 28 Mar 2019              start getting dependent libs from global locations

# Get global definitions makefile.
MKPATH                     := $(shell dirname\
                                 $(realpath $(lastword $(MAKEFILE_LIST)))\
                              )/
include                    $(MKPATH)../../../../makefile.def

BINCATEGORY                := $(BINCAT_NET)
PRJMAIN                    := $(BINNET_ETHFRAME)

# Project root path
PRJROOTDIR                 := $(TOPSRCDIR)experimental/$\
                                 $(BINCATEGORY)/$(PRJMAIN)/

# Main build directories
INCDIR                     := $(TOPINCDIR)
BINDIR                     := $(PRJROOTDIR)$(TOPBIN)/
LIBDIR                     := $(TOPLIBDIR)
SRCDIR                     := $(PRJROOTDIR)
OBJDIR                     := $(PRJROOTDIR)$(TOPOBJ)/

# Main output directories
BINDIR_RDBG                := $(BINDIR)$(RASPDEBUG)/
BINDIR_RREL                := $(BINDIR)$(RASPREL)/
LIBDIR_RDBG                := # no libraries here
LIBDIR_RREL                := # no libraries here
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
LIBDATDIR_RDBG             := $(TOPLIBDIR)$(LIBCAT_DATASTRUCT)/$(RASPDEBUG)/
LIBDATDIR_RREL             := $(TOPLIBDIR)$(LIBCAT_DATASTRUCT)/$(RASPREL)/
#LIBENCDIR_RDBG             := $(TOPLIBDIR)$(LIBCAT_ENCODE)/$(RASPDEBUG)/
#LIBENCDIR_RREL             := $(TOPLIBDIR)$(LIBCAT_ENCODE)/$(RASPREL)/
LIBNETDIR_RDBG             := $(TOPLIBDIR)$(LIBCAT_NET)/$(RASPDEBUG)/
LIBNETDIR_RREL             := $(TOPLIBDIR)$(LIBCAT_NET)/$(RASPREL)/

# External libraries
LIBDAT_CYCBUF_RDBG         := $(LIBDATDIR_RDBG)$(LIBDAT_CYCBUF).a
LIBDAT_CYCBUF_RREL         := $(LIBDATDIR_RREL)$(LIBDAT_CYCBUF).a
#LIBENC_BECODE_RDBG         := $(LIBENCDIR_RDBG)$(LIBENC_BECODE).a
#LIBENC_BECODE_RREL         := $(LIBENCDIR_RREL)$(LIBENC_BECODE).a
LIBNET_TCPLIB_RDBG         := $(LIBNETDIR_RDBG)$(LIBNET_TCPLIB).a
LIBNET_TCPLIB_RREL         := $(LIBNETDIR_RREL)$(LIBNET_TCPLIB).a

# Individual project include locations
RAWRECV_INCDIR             := $(INCDIR)

# Individual project source locations
RAWRECV_SRCDIR             := $(SRCDIR)

# Individual project include files
RAWRECVINC                 := $(RAWRECV_SRCDIR)ethframe.h

# Individual project source files
RAWRECVSRC                 := $(RAWRECV_SRCDIR)main.cpp \
                              $(RAWRECV_SRCDIR)ethframe.cpp

# Project object files
RAWRECV_OBJ_RDBG           := $(OBJDIR_RDBG)$(PRJMAIN).o
RAWRECV_OBJ_RREL           := $(OBJDIR_RREL)$(PRJMAIN).o

# Project library link options
# Libraries:
# stdc++ - c++ library
# m - math library
# dl - dynamic loading library
RAWRECV_LNKLIB_RDBG        := $(GCCLIB)stdc++
RAWRECV_LNKLIB_RREL        := $(GCCLIB)stdc++

# Project output files
RAWRECV_RDBG               := $(BINDIR_RDBG)$(PRJMAIN)
RAWRECV_RREL               := $(BINDIR_RREL)$(PRJMAIN)

# Project dependencies
RAWRECVDEP_RDBG            := $(LIBDATDIR_RDBG)$(LIBDAT_CYCBUF).a\
                              $(LIBNETDIR_RDBG)$(LIBNET_TCPLIB).a
RAWRECVDEP_RREL            := $(LIBDATDIR_RREL)$(LIBDAT_CYCBUF).a\
                              $(LIBNETDIR_RREL)$(LIBNET_TCPLIB).a

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

dbg : mkdbgdirs $(RAWRECV_RDBG)

rel : mkreldirs $(RAWRECV_RREL)

# Create required directories
mkdbgdirs : roottest
	@$(MKDIR) $(OBJDIR_RDBG)
	@$(MKDIR) $(BINDIR_RDBG)

mkreldirs : roottest
	@$(MKDIR) $(OBJDIR_RREL)
	@$(MKDIR) $(BINDIR_RREL)

clean : roottest
	@$(RMDIR) $(BINDIR) $(OBJDIR)

memchk :
	$(VALGRIND) $(VALGRINDOPTFULL) $(RAWRECV_RDBG)

# rawrecv debug build
$(RAWRECV_RDBG) : $(RAWRECVDEP_RDBG) $(RAWRECVINC) $(RAWRECVSRC)
	@$(ECHO) "dbg: Compiling and linking to $@"
	@$(GCC) $(GCCOUTFILE) $(RAWRECV_RDBG) $(RAWRECV_LNKLIB_RDBG)\
		$(BINGCCOPT_RDBG)\
		$(RAWRECVSRC) $(RAWRECVDEP_RDBG)

# rawrecv release build
$(RAWRECV_RREL) : $(RAWRECVDEP_RREL) $(RAWRECVINC) $(RAWRECVSRC)
	@$(ECHO) "rel: Compiling and linking to $@"
	@$(GCC) $(GCCOUTFILE) $(RAWRECV_RREL) $(RAWRECV_LNKLIB_RREL)\
		$(BINGCCOPT_RREL)\
		$(RAWRECVSRC) $(RAWRECVDEP_RREL)

