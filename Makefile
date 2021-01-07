# Makefile for generic list package.
#
# Last edited: Tue Jul 28 15:40:05 1992 by bcs (Bradley C. Spatz) on wasp
#
# Copyright (C) 1992 Bradley C. Spatz, bcs@ufl.edu
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Library General Public
# License as published by the Free Software Foundation; either
# version 2 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Library General Public License for more details.
#
# You should have received a copy of the GNU Library General Public
# License along with this library; if not, write to the
# Free Software Foundation, Inc., 59 Temple Place - Suite 330,
# Boston, MA  02111-1307, USA.
#
#
CC	= gcc
CFLAGS	= -I. -O2 -g
LIBS	= -L. -llist
MANDIR	= /usr/local/man
LIBDIR	= /usr/local/lib
INCDIR	= /usr/local/include

all:		liblist.a list.3 queue.3 stack.3
		(cd examples; make)

liblist.a:	list.o
		ar rc liblist.a list.o
		ranlib liblist.a

list.o:		list.h Makefile

list.3:		list.man list.c Makefile
		cp list.man list.3

queue.3:	queue.man list.c Makefile
		cp queue.man queue.3

stack.3:	stack.man list.c Makefile
		cp stack.man stack.3

install:
		install -c liblist.a $(LIBDIR)
		ranlib $(LIBDIR)/liblist.a
		install -c list.h $(INCDIR)
		install -c queue.h $(INCDIR)
		install -c stack.h $(INCDIR)

install.examples:
		(cd examples/cache; make install)

install.man:
		install -c list.3 $(MANDIR)/man3
		install -c queue.3 $(MANDIR)/man3
		install -c stack.3 $(MANDIR)/man3

clean:
		rm -f *.o *.a *.3 core
		(cd examples; make clean)
