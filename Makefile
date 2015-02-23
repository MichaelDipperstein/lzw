############################################################################
#
# Makefile for lzw encode/decode library and sample program
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
	DEL = rm -f
endif

all:		sample$(EXE) liblzw.a liboptlist.a

sample$(EXE):	sample.o liblzw.a liboptlist.a
		$(LD) $^ $(LIBS) $(LDFLAGS) $@

sample.o:	sample.c lzw.h optlist.h
		$(CC) $(CFLAGS) $<

liblzw.a:	lzwencode.o lzwdecode.o bitfile.o
		ar crv liblzw.a lzwencode.o lzwdecode.o bitfile.o
		ranlib liblzw.a

lzwencode.o:	lzwencode.c lzw.h lzwlocal.h bitfile.h
		$(CC) $(CFLAGS) $<

lzwdecode.o:	lzwdecode.c lzw.h lzwlocal.h bitfile.h
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
