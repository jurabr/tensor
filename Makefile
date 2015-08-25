#  File name: Makefile
#  Date: Wed Nov 12 19:27:14 CET 2014
#  Author:    Jiri Brozovsky
#
#  Copyright (C) 2014 Jiri Brozovsky
#
#  This program is free software; you can redistribute it and/or
#  modify it under the terms of the GNU General Public License as
#  published by the Free Software Foundation; either version 2 of the
#  License, or (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful, but
#  WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  in a file called COPYING along with this program; if not, write to
#  the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA
#  02139, USA.
#
#  Tensor scale evaluator
#

ifeq ($(SYS_TYPE),IRIX)
CC=/usr/nekoware/gcc-4.7/bin/gcc
else
CC=gcc
endif

AR=ar r
RANLIB=-ranlib
LD=ld

#DEBUG=-g -O0 
DEBUG=-O3 
CFLAGS=$(FEATFLAGS) $(DEBUG) -Wall -ansi -pedantic 

LIBS=$(FEATLIBS) -lm
GLLIBS=-L/usr/freeware/lib32 -L/usr/lib32 -L/usr/X11R6/lib -lglut -lGLU -lGL -lX11 -lXmu

OBJECTS=tensor.o tensor_io.o
GLOBJECTS=gltensor.o tensor_io.o 
GENOBJECTS=gen.o 

# targets:
all: tensor gltensor

tensor.o: tensor.h tensor.c tensor_io.c
gltensor.o: tensor.h gltensor.c tensor_io.c
gen.o: gen.c

# main()
tensor: $(OBJECTS)
	$(CC) -o $(@) $(CFLAGS) $(OBJECTS) $(LIBS)  

# GL/GLUT visualizer
gltensor: $(GLOBJECTS)
	$(CC) -o $(@) $(CFLAGS) $(GLOBJECTS) $(GLLIBS) $(LIBS)  

# data generator (rectangle):
gen: $(GENOBJECTS)
	$(CC) -o $(@) $(CFLAGS) $(GENOBJECTS) $(LIBS)  

# cleaning:
clean:
	-rm -f tensor gltensor gen *.o *.a *.gcda *.gcno *.exe core
