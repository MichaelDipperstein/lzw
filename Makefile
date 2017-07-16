############################################################################
#
# Makefile for lzw encode/decode library and sample program
#
############################################################################
CC = gcc
LD = gcc
CFLAGS = -O3 -Wall -Wextra -pedantic -ansi -c
LDFLAGS = -O3 -o

# Libraries
LIBS = -L. -Lbitfile -Loptlist -llzw -lbitfile -loptlist

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

all:		sample$(EXE)

sample$(EXE):	sample.o liblzw.a optlist/liboptlist.a bitfile/libbitfile.a
		$(LD) $^ $(LIBS) $(LDFLAGS) $@

sample.o:	sample.c lzw.h optlist/optlist.h
		$(CC) $(CFLAGS) $<

liblzw.a:	lzwencode.o lzwdecode.o
		ar crv liblzw.a lzwencode.o lzwdecode.o
		ranlib liblzw.a

lzwencode.o:	lzwencode.c lzw.h lzwlocal.h bitfile/bitfile.h
		$(CC) $(CFLAGS) $<

lzwdecode.o:	lzwdecode.c lzw.h lzwlocal.h bitfile/bitfile.h
		$(CC) $(CFLAGS) $<

bitfile/libbitfile.a:
		cd bitfile && $(MAKE) libbitfile.a

optlist/liboptlist.a:
		cd optlist && $(MAKE) liboptlist.a

clean:
		$(DEL) *.o
		$(DEL) *.a
		$(DEL) sample$(EXE)
		cd optlist && $(MAKE) clean
		cd bitfile && $(MAKE) clean
