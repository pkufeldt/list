# Makefile for generic cache package based on the list(3) library.
#
# Last edited: Tue Jul 28 15:40:04 1992 by bcs (Bradley C. Spatz) on wasp
#
# Copyright (C) 1992 Bradley C. Spatz, bcs@ufl.edu
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
#
#
CC	= gcc -fstrength-reduce -finline-functions
CFLAGS	= -I../..
LIBS	= -L. -L../.. -lcache -llist
#LIBS	= -L . -L ../.. -lcache -llist      # For HP-UX; great loader guys!
MANDIR	= /usr/local/man
LIBDIR	= /usr/local/lib
INCDIR	= /usr/local/include

cachetest:	cachetest.o libcache.a
		$(CC) $(CFLAGS) -o cachetest cachetest.o $(LIBS)

libcache.a:	cache.o
		ar rc libcache.a cache.o
		ranlib libcache.a

cache.o:	cache.h Makefile

install:
		install -c libcache.a $(LIBDIR)
		ranlib $(LIBDIR)/libcache.a
		install -c cache.h $(INCDIR)

install.man:
		install -c cache.3 $(MANDIR)/man3

clean:
		rm -f cachetest *.o *.a core
