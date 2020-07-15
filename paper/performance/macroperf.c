/* macroperf.c  --  list cpp macro performance test for list(3) library
 *
 * Bradley C. Spatz, University of Florida, bcs@ufl.edu
 * Last edited: Wed Sep 18 23:19:15 1991 by bcs (Bradley C. Spatz) on cutback
 */

#include <stdio.h>
#include <list.h>

#define ITERATIONS  100000

/* Objects associated with CPU times -- may have to change
 * CLICKSPERSEC is defined as HZ on many systems
 *	Typical values of CLICKSPERSEC: Vax=60  Cray=105296000
 *	For large values, also change %5d in macro loop1, 99999 in minclicks;
 */

#define CLICKSPERSEC 60

#include <sys/types.h>
#include <sys/times.h>

int jobclicks()
{
	struct	tms buffer;

	times(&buffer);
	return (int) (buffer.tms_utime + buffer.tms_stime);
}


main(argc, argv)
int argc;
char *argv[];
{
   LIST *l;
   int iterations, i;
   int start, clicks;

   if (argc > 1) {
      iterations = atoi(argv[1]);
   }
   else {
      iterations = ITERATIONS;
   }

   if ((l = list_init()) == NULL) {
      fprintf(stderr, "%s: unable to allocate list descriptor.\n", argv[0]);
      exit(1);
   }

   start = jobclicks();

   for (i=0; i<iterations; i++) {
      list_mvfront(l);
   }

   clicks = jobclicks() - start;
   printf("%d clicks (%.2f s)\n", clicks, (float) (clicks/CLICKSPERSEC));

   list_free(l, LIST_DEALLOC);
   exit(0);
}
