/* cachetest.c -- test program for generic cache package
 *
 * Last edited: Tue Jul 28 15:42:57 1992 by bcs (Bradley C. Spatz) on wasp
 *
 * Copyright (C) 1992, Bradley C. Spatz, bcs@ufl.edu
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA  02111-1307, USA.
 */

static char brag[] = "$$Version: cachetest-2.1 Copyright (C) 1992 Bradley C. Spatz";

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include "cache.h"

#define MAX_ELEMENTS 3

list_boolean_t match(void *data, void *curr);
int print_cache(CACHE *cache);

int
main(int argc, char *argv[])
{
   CACHE *cache;
   char cmd[2];
   int val, *pval;

   /* Print some instructions. */
   printf("This program demonstrates the various cache manipulation\n");
   printf("routines supplied by this package.  Although this program\n");
   printf("manipulates a cache of integers, data of any type and size\n");
   printf("(and not just integers) are supported by the routines.  See\n");
   printf("the man page for more information.\n\n");
   printf("We illustrate a cache with a maximum size of 3 entries.\n\n");

   /* Allocate a new cache. */
   cache = cache_init(MAX_ELEMENTS);
   print_cache(cache);

   /* Get some commands. */
   printf("\n(e)nter new element, (c)heck for element; (q)uit: ");
   while (scanf("%1s", cmd) != EOF) {
      switch (cmd[0]) {
         case 'e':
	    printf("Value (int) to enter: ");
	    if (scanf("%d", &val)) {
	       /* We ignore the return code here, but in practice, 
		* we may fail (with a return code of NULL).
		*/
	       cache_enter(cache, &val, sizeof(val), &pval);
	       if (pval != NULL) {
		  printf("%d was removed to make room.\n", *pval);
	       }
	    }
            break;
	 case 'c':
	    printf("Value (int) to check for: ");
	    if (scanf("%d", &val)) {
	       pval = (int *) cache_check(cache, (char *) &val, match);
	       if (pval != NULL) {
		  printf("%d found!\n", *pval);
	       }
	       else {
		  printf("Not found.\n");
	       }
	    }
	    break;
	 case 'q':
	    cache_free(cache, CACHE_DEALLOC);
	    exit(0);
	    break;
         default:
	    printf("'%c' not a recognized command!\n", cmd[0]);
	    break;
      }
      print_cache(cache);
      printf("\n(e)nter new element, (c)heck for element; (q)uit: ");
   }

   exit(0);
}


/* Provide a routine to return FALSE (0) if the current element in the cache,
 * curr, is equal to the element we are searching for, data.  We return
 * FALSE, instead of TRUE, because we will use the list_traverse function.
 * That function continues if the user-supplied function is TRUE.  We want
 * to stop when we've found our element.
 */
list_boolean_t
match(void *data, void *curr)
{
   return(((*(int *) data) == (*(int *) curr)) ? CACHE_FALSE : CACHE_TRUE);
}


/* The following routines make know that the cache is implemented with
 * the list(3) package.  It makes knowledge of the CACHE structure as
 * well.  We do this here for illustration only.
 */
list_boolean_t
print_element(void *input, void *curr)
{
   printf(" %d", *(int *) curr);
   return(CACHE_TRUE);
}

int
print_cache(CACHE *cache)
{
   printf("Cache:");
   if (list_empty(cache->list))
      printf(" (empty).\n");
   else {
      list_traverse(cache->list, (char *) 0, print_element,
		    (LIST_FRNT | LIST_FORW | LIST_SAVE));
      printf(".\n");
   }
}
