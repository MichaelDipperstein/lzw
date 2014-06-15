############################################################################
# Makefile for lzw encode/decode library and sample program
#
#   $Id: Makefile,v 1.5 2007/09/29 01:28:27 michael Exp $
#   $Log: Makefile,v $
#   Revision 1.5  2007/09/29 01:28:27  michael
#   Replace getopt with optlist.
#   Changes required for LGPL v3.
#
#   Revision 1.4  2007/07/16 02:18:15  michael
#   Use -pedantic option when compiling.
#
#   Revision 1.3  2005/04/09 03:11:22  michael
#   Separated encode and decode routines into two different files in order to
#   make future enhancements easier.
#
#   Revision 1.2  2005/02/22 05:41:58  michael
#   Build code using libraries for easier integration into other applications.
#
#   Revision 1.1.1.1  2005/02/21 03:35:34  michael
#   Initial Release
#
############################################################################
CC = gcc
LD = gcc
CFLAGS = -O3 -Wall -Wextra -pedantic -ansi -c
LDFLAGS = -O3 -o

# libraries
LIBS = -L. -llzw -loptlist

# Treat NT and non-NT windows the same
ifeq ($(OS),Windows_NT)
	OS = Windows
endif

ifeq ($(OS),Windows)
	EXE = .exe
	DEL = del
else	#assume Linux/Unix
	EXE =
	DEL = rm
endif

all:		sample$(EXE) liblzw.a liboptlist.a

sample$(EXE):	sample.o liblzw.a liboptlist.a
		$(LD) $^ $(LIBS) $(LDFLAGS) $@

sample.o:	sample.c lzw.h optlist.h
		$(CC) $(CFLAGS) $<

liblzw.a:	lzwencode.o lzwdecode.o bitfile.o
		ar crv liblzw.a lzwencode.o lzwdecode.o bitfile.o
		ranlib liblzw.a

lzwencode.o:	lzwencode.c lzw.h bitfile.h
		$(CC) $(CFLAGS) $<

lzwdecode.o:	lzwdecode.c lzw.h bitfile.h
		$(CC) $(CFLAGS) $<

bitfile.o:	bitfile.c bitfile.h
		$(CC) $(CFLAGS) $<

liboptlist.a:	optlist.o
		ar crv liboptlist.a optlist.o
		ranlib liboptlist.a

optlist.o:	optlist.c optlist.h
		$(CC) $(CFLAGS) $<

clean:
		$(DEL) *.o
		$(DEL) *.a
		$(DEL) sample$(EXE)
