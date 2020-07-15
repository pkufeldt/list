Version 2.1

This package provides generic linked-list manipulation routines.

In addition, queue and stack abstractions are provided by single header files.
To use the package as a list, queue, or stack, simply include the respective
header file and compile with the list library.  Thus, a single set of linked
list primitives can be used to implement stacks, queues, or any other
list-based data structure (such as caches, etc.).

The paper directory contains LaTeX source and DVI and Postscript output from
a paper entitled "The list(3) Package: Software Design and Reusability",
that describes the design and evolution of the package.  This paper appears
in the proceedings of the ACM 30th Annual Southeast Conference, Raleigh,
North Carolina, April 8-10, 1992.

The man page and install targets in the Makefile assume the library, man
pages, and header files will be installed as a piece of system software.
Specifically, the man pages refer to the header file with <>'s instead of
quotes, and the library is referred to with the -l compiler switch.

A set of example programs are provided to illustrate the package.  They
can be compiled without installing (see above) the library.

By default, some of the routines are implemented as macros.  You can
override this by commenting out the -DUSE_MACROS from the Makefile and
recompiling.  Note that this #define also updates the man pages.

I have tested this package on Sun3, Sun4, DecStation 5000, PC/RT, HP
9000-400, and Sony(68030, News 3.3) architectures, and it performs
well.  Please notify me of bug reports, comments, or suggestions.

This package is constrained by the GNU Library General Public License as 
published by the Free Software Foundation.  See the file COPYING for details.

Enjoy.

28Jul92
--
Bradley C. Spatz                                        Internet:  bcs@ufl.edu
Computer and Information Sciences                    UUCP:  uunet!uflorida!bcs 
College of Engineering
University of Florida	                     "School IS hell" -- Matt Groening
