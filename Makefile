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
#CFLAGS	= $(DEFINES) -fstrength-reduce -finline-functions -I.
CFLAGS	= $(DEFINES) -I. -g
DEFINES = -DUSE_MACROS
LIBS	= -L. -llist
#LIBS	= -L . -llist   # Use this for HP-UX; great loader guys!
CPP	= cpp -E -P -I. -w
MANDIR	= /usr/local/man
LIBDIR	= /usr/local/lib
INCDIR	= /usr/local/include

# We specify some goofy dependencies between the man pages and the source,
# because the man page reflects whether USE_MACROS was specified.  Thus,
# the dependencies that follow insure the library and the man page
# are always in sync.  A similar argument goes for the header file and it's
# prototype.  To be sure, you can always make clean and make.

all:		liblist.a #list.3 queue.3 stack.3
		(cd examples; make)

liblist.a:	list.o
		ar rc liblist.a list.o
		ranlib liblist.a

list.o:		list.h list.3 Makefile

list.h:		list.h.proto Makefile
		$(CPP) $(DEFINES) list.h.proto > list.h

list.3:		list.man list.c Makefile
		$(CPP) $(DEFINES) list.man > list.3

queue.3:	queue.man list.c Makefile
		$(CPP) $(DEFINES) queue.man > queue.3

stack.3:	stack.man list.c Makefile
		$(CPP) $(DEFINES) stack.man > stack.3

install:
		install -c liblist.a $(LIBDIR)
		ranlib $(LIBDIR)/liblist.a
		install -c list.h $(INCDIR)
		install -c queue.h $(INCDIR)
		install -c stack.h $(INCDIR)
		(cd examples/cache; make install)

install.man:
		install -c list.3 $(MANDIR)/man3
		install -c queue.3 $(MANDIR)/man3
		install -c stack.3 $(MANDIR)/man3

clean:
		rm -f *.o *.a list.h *.3 core
		(cd examples; make clean)
