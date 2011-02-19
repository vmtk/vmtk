# makefile for Stellar
#
# Type "make" to compile Stellar on a Mac system.
# Type "make linux" to compile Stellar on a Linux-like system.
# Type "make showme" to compile Show Me on a system with X Windows.
#
# Type "make clean" to delete all executable and object files.

# location of the Stellar source files
SRC = ./src/
# location of the produced executables
BIN = ./

# CC should be set to the name of your favorite C compiler.
CC = cc

# RM should be set to the name of your favorite rm (file deletion program).
RM = /bin/rm

# Switches for C compiler.  -DNDEBUG suppresses assertions from assert.h
CSWITCHES = -g -pg -DSELF_CHECK -DLINUX -Wall -Wconversion -Wstrict-prototypes -Wno-strict-aliasing -fno-strict-aliasing -I$(SRC) -I/usr/X11R6/include -L/usr/X11R6/lib
CSWITCHESFAST = -DSELF_CHECK -g -DLINUX -O3 -Wall -Wconversion -Wstrict-prototypes -Wno-strict-aliasing -fno-strict-aliasing -I$(SRC) -I/usr/X11R6/include -L/usr/X11R6/lib
CSWITCHESMAC = -arch i386 -arch ppc -g -pg -fast -DSELF_CHECK -Wall -Wno-strict-aliasing -fno-strict-aliasing -Wconversion -Wstrict-prototypes -I$(SRC) -I/usr/X11R6/include -L/usr/X11R6/lib
CSWITCHESSTELLARMAC = -pedantic -g -pg  -DSELF_CHECK -Wall -Wno-strict-aliasing -fno-strict-aliasing -Wconversion -Wstrict-prototypes -I$(SRC)
CSWITCHESSTELLARMACFAST = -arch i386 -arch ppc -pedantic -fast -Wall -Wno-strict-aliasing -fno-strict-aliasing -Wconversion -Wstrict-prototypes -I$(SRC)

# Where to find the basic X Windows (X11) #include files and libraries
SHOWMESWITCHES = -I/usr/X11R6/include -L/usr/X11R6/lib

STARLIBDEFS = -DNOMAIN

# sources for Stellar
STELLARSRCS = $(SRC)Stellar.c $(SRC)arraypoolstack.c $(SRC)classify.c $(SRC)insertion.c $(SRC)interact.c $(SRC)journal.c $(SRC)main.c $(SRC)output.c $(SRC)print.c $(SRC)quadric.c $(SRC)anisotropy.c $(SRC)quality.c $(SRC)size.c $(SRC)smoothing.c $(SRC)top.c $(SRC)topological.c $(SRC)vector.c $(SRC)improve.c $(SRC)Starbase.c

all: mac
mac: $(BIN)Stellarmac
macdebug: $(BIN)Stellarmacdebug
macshowme: $(BIN)showmemac
linux: $(BIN)Stellarlinux
linuxdebug: $(BIN)Stellarlinuxdebug
linuxshowme: $(BIN)showmelinux

$(BIN)Stellarlinux: $(BIN)Starbase.o $(STELLARSRCS)
	$(CC) $(CSWITCHESFAST) -o $(BIN)Stellar $(SRC)Stellar.c \
		$(BIN)Starbase.o -lm

$(BIN)Stellarlinuxdebug: $(BIN)Starbase.o $(STELLARSRCS)
	$(CC) $(CSWITCHES) -o $(BIN)Stellardebug $(SRC)Stellar.c \
		$(BIN)Starbase.o -lm

$(BIN)Stellarmac: $(BIN)Starbasemac.o $(STELLARSRCS)
	$(CC) $(CSWITCHESSTELLARMACFAST) -o $(BIN)Stellar $(SRC)Stellar.c \
		$(BIN)Starbasemac.o -lm

$(BIN)Stellarmacdebug: $(BIN)Starbasemac.o $(STELLARSRCS)
	$(CC) $(CSWITCHESSTELLARMAC) -o $(BIN)Stellardebug $(SRC)Stellar.c \
		$(BIN)Starbasemac.o -lm

$(BIN)Starbasemac.o: $(SRC)Starbase.c $(SRC)Starbase.h
	$(CC) $(CSWITCHESMAC) $(STARLIBDEFS) -c -o $(BIN)Starbasemac.o \
		$(SRC)Starbase.c

$(BIN)Starbase.o: $(SRC)Starbase.c $(SRC)Starbase.h
	$(CC) $(CSWITCHESFAST) $(STARLIBDEFS) -c -o $(BIN)Starbase.o \
		$(SRC)Starbase.c

$(BIN)showmelinux: $(SRC)showme.c
	$(CC) $(CSWITCHESFAST) $(SHOWMESWITCHES) -o $(BIN)showme \
		$(SRC)showme.c -lX11 -lm

$(BIN)showmemac: $(SRC)showme.c
	$(CC) $(CSWITCHESMACFAST) $(SHOWMESWITCHES) -o $(BIN)showme \
		$(SRC)showme.c -lX11 -lm

clean:
	-$(RM) $(BIN)Starbase.o $(BIN)Starbasemac.o $(BIN)Stellar $(BIN)Stellardebug $(BIN)showme
