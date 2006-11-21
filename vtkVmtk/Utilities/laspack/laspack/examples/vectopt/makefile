#
# set appropriate the following variables in your environment (if necessary):
#
# CC		for the C compiler
# CPP		for the C++ compiler
# CFLAGS	for options of the C compiler
# CPPFLAGS	for options of the C++ compiler
# PFLAGS	for options of the Pascal compiler
# FFLAGS	for options of the FORTRAN compiler
# LDFLAGS	for linker options
#

#
# ARCH_EXT can be used in order to install binaries in different directories
# depending on the computer architecture,
# e.g. $(HOME)/lib/sunos for ARCH_EXT = '/sunos'
#
#ARCH_EXT	=

#
# set the path for the root of the inlude directories here,
# e.g. /usr/local/include
#
INCROOT		= $(HOME)/include
#
# set the path for the root of the library directories here,
# e.g. /usr/local/lib
#
LIBROOT		= $(HOME)/lib$(ARCH_EXT)
#
# set the destination directory for the executable
#
DEST		= $(HOME)/bin$(ARCH_EXT)

#
# the following text was created automaticaly. You should change it carefully.
#

SHELL		= /bin/sh

PROGRAM		= vectopt

HDRS		= testproc.h

EXTHDRS		=

SRCS		= testproc.c \
		vectopt.c

OBJS		= testproc.o \
		vectopt.o

LIBS		=

EXTLIBS		= -llaspack -lxc

COMPFLAGS	=  

# compiler options will be passed throuth enviroment variables
#CFLAGS		=
#PFLAGS		=
#FFLAGS		=
#CXXFLAGS	=

BINLOCAL	= /usr/local/bin

INSTALL		= mv

LD		= $(CC)

# linker options passed throuth enviroment variables
#LDFLAGS	=

LINTLIBS	=

LINTFLAGS	= -I$(INCROOT) $(CFLAGS)

MAKEFILE	= makefile

PRINT		= pr

PRINTFLAGS	=

LP		= lp

LPFLAGS		= 

all:		$(PROGRAM)

clean:;		@rm -f $(OBJS) core

clobber:;	@rm -f $(OBJS) $(PROGRAM) core tags
		@if [ -f compllist ]; then rm -f compllist; fi
		@if [ -f cleanlist ]; then rm -f cleanlist; fi
		@find . -type f -print > compllist
		@sed -n \
			-e "/~/ w cleanlist" \
			-e '/%/ w cleanlist' \
			-e '/.bak/ w cleanlist' \
			-e '/.obj/ w cleanlist' \
			-e '/.exe/ w cleanlist' \
			-e '/.aux/ w cleanlist' \
			-e '/.blg/ w cleanlist' \
			-e '/.dvi/ w cleanlist' \
			-e '/.glo/ w cleanlist' \
			-e '/.idx/ w cleanlist' \
			-e '/.ilg/ w cleanlist' \
			-e '/.ind/ w cleanlist' \
			-e '/.lof/ w cleanlist' \
			-e '/.log/ w cleanlist' \
			-e '/.lot/ w cleanlist' \
			-e '/.toc/ w cleanlist' \
			compllist
		@rm -f `cat cleanlist`
		@rm -f compllist
		@rm -f cleanlist
		
depend:;	@mkmf -f $(MAKEFILE)

echo:;		@echo $(HDRS) $(SRCS)

index:;		@ctags -wx $(HDRS) $(SRCS)

install:	$(PROGRAM)
		@echo Installing $(PROGRAM) in $(DEST)
		@strip $(PROGRAM)
		@if [ $(DEST) != . ]; then rm -f $(DEST)/$(PROGRAM); fi
		@if [ $(DEST) != . ]; then $(INSTALL) -f $(PROGRAM) $(DEST); fi

install-local:	$(PROGRAM)
		@echo Installing $(PROGRAM) in $(BINLOCAL)
		@strip $(PROGRAM)
		@rm -f $(BINLOCAL)/$(PROGRAM).old
		@if [ -f $(BINLOCAL)/$(PROGRAM) ]; then \
			mv $(BINLOCAL)/$(PROGRAM) $(BINLOCAL)/$(PROGRAM).old; \
		fi
		@$(INSTALL) -f $(PROGRAM) $(BINLOCAL)
		@chmod 755 $(BINLOCAL)/$(PROGRAM)

lint:		$(LINTLIBS) $(HDRS) $(EXTHDRS) $(SRCS)
		@$(LINT) $(LINTFLAGS) $(LINTLIBS) $(SRCS)

print:;		@$(PRINT) $(PRINTFLAGS) $(HDRS) $(SRCS) | $(LP) $(LPFLAGS)

tags:           $(HDRS) $(SRCS) 
		@ctags $(HDRS) $(SRCS)

touch:;		@touch $(HDRS) $(SRCS) $(MAKEFILE)

update:		$(DEST)/$(PROGRAM)

d2u:;		@d2u $(HDRS) $(SRCS)
 
c:;		@$(MAKE) -f $(MAKEFILE) clean
cl:;		@$(MAKE) -f $(MAKEFILE) clobber
i:;             @$(MAKE) -f $(MAKEFILE) install
il:;		@$(MAKE) -f $(MAKEFILE) install-local
l:;		@$(MAKE) -f $(MAKEFILE) lint
t:;		@$(MAKE) -f $(MAKEFILE) touch
u:;		@$(MAKE) -f $(MAKEFILE) update 

$(PROGRAM):     $(OBJS) $(LIBS) $(MAKEFILE)
		@echo "Linking $(PROGRAM) ..."
		@$(LD) $(COMPFLAGS) $(OBJS) $(LIBS) -o $(PROGRAM) \
			-L$(LIBROOT) $(EXTLIBS) $(LDFLAGS) -lm

$(DEST)/$(PROGRAM):  $(HDRS) $(EXTHDRS) $(SRCS) $(LIBS) 
		@$(MAKE) -f $(MAKEFILE) install:

.SUFFIXES: .c .cc  .p .f .o

.c.o:;		$(CC) -I$(INCROOT) $(CFLAGS) $(COMPFLAGS) -c $<
.cc.o:;		$(CPP) -I$(INCROOT) $(CPPFLAGS) $(COMPFLAGS) -c $<
.p.o:;		pc $(PFLAGS) $(COMPFLAGS) -c $<
.f.o:;		f77 $(FFLAGS) $(COMPFLAGS) -c $<
###
testproc.o: testproc.h
vectopt.o: testproc.h
