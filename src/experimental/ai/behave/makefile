# History of changes:
#
# 20 Aug 2019              created

# Get global definitions makefile.
MKPATH                     := $(shell dirname\
                                 $(realpath $(lastword $(MAKEFILE_LIST)))\
                              )/
include                    $(MKPATH)../../../../makefile.def

BINCATEGORY                := $(BINCAT_AI)
PRJMAIN                    := $(BINAI_BEHAVE)

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
GCCDISABLE71WARN           := -Wno-psabi
GCCPIC                     := -fPIC
GCCDEBUG                   := -g
GCCCOMPILEONLY             := -c
GCCOUTFILE                 := -o
GCCLIB                     := -l
GCCINCDIR                  := -I
GCCLIBDIR                  := -L

# External libraries include locations
CYCBUF_INCDIR              := $(INCDIR)$(LIBCAT_DATASTRUCT)/

# External libraries library dirs
#LIBDATDIR_RDBG             := $(TOPLIBDIR)$(LIBCAT_DATASTRUCT)/$(RASPDEBUG)/
#LIBDATDIR_RREL             := $(TOPLIBDIR)$(LIBCAT_DATASTRUCT)/$(RASPREL)/
#LIBENCDIR_RDBG             := $(TOPLIBDIR)$(LIBCAT_ENCODE)/$(RASPDEBUG)/
#LIBENCDIR_RREL             := $(TOPLIBDIR)$(LIBCAT_ENCODE)/$(RASPREL)/
#LIBNETDIR_RDBG             := $(TOPLIBDIR)$(LIBCAT_NET)/$(RASPDEBUG)/
#LIBNETDIR_RREL             := $(TOPLIBDIR)$(LIBCAT_NET)/$(RASPREL)/

# External libraries
#LIBDAT_CYCBUF_RDBG         := $(LIBDATDIR_RDBG)$(LIBDAT_CYCBUF).a
#LIBDAT_CYCBUF_RREL         := $(LIBDATDIR_RREL)$(LIBDAT_CYCBUF).a
#LIBENC_BECODE_RDBG         := $(LIBENCDIR_RDBG)$(LIBENC_BECODE).a
#LIBENC_BECODE_RREL         := $(LIBENCDIR_RREL)$(LIBENC_BECODE).a
#LIBNET_TCPLIB_RDBG         := $(LIBNETDIR_RDBG)$(LIBNET_TCPLIB).a
#LIBNET_TCPLIB_RREL         := $(LIBNETDIR_RREL)$(LIBNET_TCPLIB).a

# Individual project include locations
BEHAVE_INCDIR              := $(INCDIR)

# Individual project source locations
BEHAVE_SRCDIR              := $(SRCDIR)

# Individual project include files
BEHAVEINC                  := $(BEHAVE_SRCDIR)stringlist.h \
                                 $(BEHAVE_SRCDIR)objref.h \
                                 $(BEHAVE_SRCDIR)serializer.h \
                                 $(BEHAVE_SRCDIR)weightedbinary.h \
                                 $(BEHAVE_SRCDIR)mood.h \
                                 $(BEHAVE_SRCDIR)action.h \
                                 $(BEHAVE_SRCDIR)being.h \
                                 $(BEHAVE_SRCDIR)gathering.h \
                                 $(BEHAVE_SRCDIR)behavefactory.h \

# Individual project source files
BEHAVESRC                  := $(BEHAVE_SRCDIR)stringlist.cpp \
                                 $(BEHAVE_SRCDIR)serializer.cpp \
                                 $(BEHAVE_SRCDIR)weightedbinary.cpp \
                                 $(BEHAVE_SRCDIR)mood.cpp \
                                 $(BEHAVE_SRCDIR)action.cpp \
                                 $(BEHAVE_SRCDIR)being.cpp \
                                 $(BEHAVE_SRCDIR)gathering.cpp \
                                 $(BEHAVE_SRCDIR)behavefactory.cpp \
                                 $(BEHAVE_SRCDIR)objrefinstances.cpp \
                                 $(BEHAVE_SRCDIR)main.cpp

# Project object files
BEHAVE_OBJ_RDBG            := $(OBJDIR_RDBG)$(PRJMAIN).o
BEHAVE_OBJ_RREL            := $(OBJDIR_RREL)$(PRJMAIN).o

# Project library link options
# Libraries:
# stdc++ - c++ library
# m - math library
# dl - dynamic loading library
BEHAVE_LNKLIB_RDBG         := $(GCCLIB)stdc++ $(GCCLIB)m
BEHAVE_LNKLIB_RREL         := $(GCCLIB)stdc++ $(GCCLIB)m

# Project output files
BEHAVE_RDBG                := $(BINDIR_RDBG)$(PRJMAIN)
BEHAVE_RREL                := $(BINDIR_RREL)$(PRJMAIN)

# Project dependencies
BEHAVEDEP_RDBG             :=
BEHAVEDEP_RREL             :=

# Individual project type compiler options
OBJCOPT_RDBG               := $(GCCDEBUG) $(GCCCOMPILEONLY) \
                              $(GCCINCDIR)$(INCDIR) \
                              $(GCCINCDIR)$(CYCBUF_INCDIR)
OBJCOPT_RREL               := $(GCCCOMPILEONLY) \
                              $(GCCINCDIR)$(INCDIR) \
                              $(GCCINCDIR)$(CYCBUF_INCDIR)
OBJGCCOPT_RDBG             := $(GCCDEBUG) $(GCCCOMPILEONLY) $(GCCSTD14) \
                              $(GCCINCDIR)$(INCDIR) \
                              $(GCCINCDIR)$(CYCBUF_INCDIR)
OBJGCCOPT_RREL             := $(GCCCOMPILEONLY) $(GCCSTD14) \
                              $(GCCINCDIR)$(INCDIR) \
                              $(GCCINCDIR)$(CYCBUF_INCDIR)
BINGCCOPT_RDBG             := $(GCCDEBUG) $(GCCSTD14) \
                              $(GCCDISABLE71WARN) \
                              $(GCCINCDIR)$(INCDIR) \
                              $(GCCINCDIR)$(CYCBUF_INCDIR)
BINGCCOPT_RREL             := $(GCCSTD14) $(GCCDISABLE71WARN) \
                              $(GCCINCDIR)$(INCDIR) \
                              $(GCCINCDIR)$(CYCBUF_INCDIR)

rules : roottest
	@$(ECHO) '   all:    all projects (debug and release)'
	@$(ECHO) '   dbg:    all the debug projects'
	@$(ECHO) '   rel:    all the release projects'
	@$(ECHO) '   memchk: leak checks'
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
all : dbg rel $(BEHAVE_RDBG)
	$(VALGRIND) 

dbg : mkdbgdirs $(BEHAVE_RDBG)

rel : mkreldirs $(BEHAVE_RREL)

# Create required directories
mkdbgdirs : roottest
	@$(MKDIR) $(OBJDIR_RDBG)
	@$(MKDIR) $(BINDIR_RDBG)

mkreldirs : roottest
	@$(MKDIR) $(OBJDIR_RREL)
	@$(MKDIR) $(BINDIR_RREL)

clean : roottest
	@$(RMDIR) $(BINDIR) $(OBJDIR)

memchk : $(BEHAVE_RDBG)
	@$(VALGRIND) $(VALGRINDOPTFULL) $(BEHAVE_RDBG)

# behave debug build
$(BEHAVE_RDBG) : $(BEHAVEDEP_RDBG) $(BEHAVEINC) $(BEHAVESRC)
	@$(ECHO) "dbg: Compiling and linking to $@"
	@$(GCC) $(GCCOUTFILE) $(BEHAVE_RDBG) $(BEHAVE_LNKLIB_RDBG)\
		$(BINGCCOPT_RDBG)\
		$(BEHAVESRC) $(BEHAVEDEP_RDBG)

# behave release build
$(BEHAVE_RREL) : $(BEHAVEDEP_RREL) $(BEHAVEINC) $(BEHAVESRC)
	@$(ECHO) "rel: Compiling and linking to $@"
	@$(GCC) $(GCCOUTFILE) $(BEHAVE_RREL) $(BEHAVE_LNKLIB_RREL)\
		$(BINGCCOPT_RREL)\
		$(BEHAVESRC) $(BEHAVEDEP_RREL)

