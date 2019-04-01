# History of changes:
#
# 24 Mar 2019              created

# Get global definitions makefile.
MKPATH                     := $(shell dirname\
                                 $(realpath $(lastword $(MAKEFILE_LIST)))\
                              )/
include $(MKPATH)../../makefile.def

# Project root path
PRJROOTDIR                 := $(MKPATH)

# All library directories
LIBDATASTRUCTDIR           := $(PRJROOTDIR)$(LIBCAT_DATASTRUCT)/
LIBENCODEDIR               := $(PRJROOTDIR)$(LIBCAT_ENCODE)/
LIBNETDIR                  := $(PRJROOTDIR)$(LIBCAT_NET)/
LIBALLDIR                  := $(LIBDATASTRUCTDIR) $(LIBENCODEDIR) $(LIBNETDIR)

# All make files
CYCBUFMAKE                 := $(LIBDATASTRUCTDIR)$(LIBDAT_CYCBUF)/makefile
OCTREEMAKE                 := $(LIBDATASTRUCTDIR)$(LIBDAT_OCTREE)/makefile
BECODEMAKE                 := $(LIBENCODEDIR)$(LIBENC_BECODE)/makefile
ELGAMALMAKE                := $(LIBENCODEDIR)$(LIBENC_ELGAMAL)/makefile
TCPLIBMAKE                 := $(LIBNETDIR)$(LIBNET_TCPLIB)/makefile
ALLMAKE                    := $(CYCBUFMAKE) $(OCTREEMAKE) \
                              $(BECODEMAKE) $(ELGAMALMAKE) \
                              $(TCPLIBMAKE)

# Compilers and tools
CD                         := cd
MV                         := mv
MKDIR                      := mkdir -p
RMDIR                      := rm -Rf
AR                         := ar
FIND                       := find
GCC                        := gcc
TOUCH                      := touch
ECHO                       := echo

#
# First rule (parameters)
#

rules :
	@$(ECHO) '   all:    all projects (debug and release)'
	@$(ECHO) '   dbg:    all the debug projects'
	@$(ECHO) '   rel:    all the release projects'
	@$(ECHO) '   clean:  remove all'
	@$(ECHO) ""

# All builds
all : clean dbg rel

clean dbg rel :
	@for mk in $(ALLMAKE); do\
		$(ECHO) "Making " $$mk;\
		$(MAKE) -s -f $$mk $@;\
	done
	@if [ $@ = clean ];\
	then\
		$(RMDIR) $(TOPLIBDIR);\
	fi\
	

