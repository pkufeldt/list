# Makefile for generic linked list examples.
#
# Last edited: Tue Jul 28 15:36:13 1992 by bcs (Bradley C. Spatz) on wasp
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
CC	= gcc
CFLAGS	= $(INCLUDES) -g -O2
INCLUDES= -I..
LIBS	= -L.. -llist

all:		listtest queuetest stacktest
		(cd cache; make)

listtest:	listtest.o ../liblist.a
		$(CC) $(CFLAGS) -o listtest listtest.o $(LIBS)

queuetest:	queuetest.o ../liblist.a
		$(CC) $(CFLAGS) -o queuetest queuetest.o $(LIBS)

stacktest:	stacktest.o ../liblist.a
		$(CC) $(CFLAGS) -o stacktest stacktest.o $(LIBS)

listtest.o:	../list.h
queuetest.o:	../queue.h
stacktest.o:	../stack.h
clean:
		rm -f listtest queuetest stacktest *.o core
		(cd cache; make clean)
