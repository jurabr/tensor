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

DEBUG=-g -O0 
CFLAGS=$(FEATFLAGS) $(DEBUG) -Wall -ansi -pedantic 

LIBS=$(FEATLIBS) -lm

OBJECTS=tensor.o

# targets:
all: tensor

# main()
tensor: $(OBJECTS)
	$(CC) -o $(@) $(CFLAGS) $(OBJECTS) $(LIBS)  

# cleaning:
clean:
	-rm -f tensor *.o *.a *.gcda *.gcno *.exe core
