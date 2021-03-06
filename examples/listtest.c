/* listtest.c -- test program for generic list package
 *
 * Last edited: Tue Jul 28 15:37:21 1992 by bcs (Bradley C. Spatz) on wasp
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

static char brag[] = "$$Version: listtest-2.1 Copyright (C) 1992 Bradley C. Spatz";

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include "../list.h"

int print_list(LIST *list);
int do_insert(LIST *list);
int do_remove(LIST *list);
int do_move(LIST *list);
int do_setcurr(LIST *list);
int do_traverse(LIST *list);

int
main(int argc, char *argv[])
{
   LIST *list;
   char cmd[2];
   int val, *pval;

   /* Print some instructions. */
   printf("This program demonstrates the various list manipulation\n");
   printf("routines supplied by this package.  Although this program\n");
   printf("manipulates a list of integers, data of any type and size\n");
   printf("(and not just integers) are supported by the routines.  See\n");
   printf("the man page for more information.\n\n");

   /* Allocate a new list. */
   list = list_create();
   printf("Curr = (nil).\n");
   print_list(list);

   /* Get some commands. */
   printf("\n");
   printf("(i)nsert/(r)emove\n(f)ront/r(e)ear\n(m)ove/set(c)urr\n(t)raverse, (s)ize, (q)uit: ");
   while (scanf("%1s", cmd) != EOF) {
      switch (cmd[0]) {
         case 'i':
	     do_insert(list);
             break;
	 case 'r':
	    do_remove(list);
	    break;
         case 'm':
	    do_move(list);
	    break;
         case 'c':
	    do_setcurr(list);
	    break;
         case 'f':
            pval = (int *) list_front(list);
	    if (pval == NULL)
	       printf("Front = (nil)\n");
	    else
	       printf("Front = %d\n", *pval);
	    break;
         case 'e':
            pval = (int *) list_rear(list);
	    if (pval == NULL)
	       printf("Rear = (nil)\n");
	    else
	       printf("Rear = %d\n", *pval);
	    break;
         case 't':
	    do_traverse(list);
	    break;
         case 's':
	    printf("List has %u elements.\n", list_size(list));
	    break;
	 case 'q':
	    list_destroy(list, (void *)LIST_DEALLOC);
	    exit(0);
	    break;
         default:
	    printf("'%c' not a recognized command!\n", cmd[0]);
	    break;
      }
      pval = (int *) list_curr(list);
      if (pval == NULL)
	 printf("Curr = (nil)\n");
      else
	 printf("Curr = %d\n", *pval);
      print_list(list);
      printf("(i)nsert/(r)emove\n(f)ront/r(e)ear\n(m)ove/set(c)urr\n(t)raverse, (s)ize, (q)uit: ");
   }

   exit(0);
}

int
do_insert(LIST *list)
{
   char cmd[2];
   int val, *bogus, rc;

   printf("insert (b)efore or (a)after: ");
   rc = scanf("%1s", cmd);
   switch (cmd[0]) {
      case 'b':
	 printf("Value (int) to insert before: ");
	 if (scanf("%d", &val))
	    /* We ignore the return code here, but in practice, 
	     * we may fail (with a return code of NULL).
	     */
	    list_insert_before(list, &val, sizeof(val));
	 break;
      case 'a':
         printf("Value (int) to insert after: ");
	 if (scanf("%d", &val))
	    /* We ignore the return code here, but in practice,
	     * we may fail (with a return code of NULL).
	     */
	    list_insert_after(list, &val, sizeof(val));
	 break;
      default:
         printf("%c not recognized.  Returning to main menu.\n", cmd[0]);
   }
}


int
do_remove(LIST *list)
{
   char cmd[2];
   int *pval, rc;
   
   printf("remove (f)ront, (c)urr, or (r)ear: ");
   rc = scanf("%1s", cmd);
   switch (cmd[0]) {
      case 'f':
	 pval = (int *) list_remove_front(list);
	 break;
      case 'c':
	 pval = (int *) list_remove_curr(list);
	 break;
      case 'r':
	 pval = (int *) list_remove_rear(list);
	 break;
      default:
         printf("%c not recognized.  Returning to main menu.\n", cmd[0]);
   }

   if (pval == NULL)
      printf("List is empty!\n");
   else
      printf("%d removed.\n", *pval);
}


int
do_setcurr(LIST *list)
{
	LIST_ELEMENT *e;
	char cmd[2];
	int rc;
	
	printf("pickup to (c) curr, (f)ront, or (r)ear: ");
	rc = scanf("%1s", cmd);
	switch (cmd[0]) {
	case 'c':
		/* We ignore the return code here. */
		e = list_element_curr(list);
		break;
	case 'f':
		/* We ignore the return code here. */
		e = list_element_front(list);
		break;
	case 'r':
		/* We ignore the return code here. */
		e = list_element_rear(list);
		break;
	default:
		printf("%c not recognized.  Returning to main menu.\n", cmd[0]);
		return 1;
	}
	
	if (!e) {
		printf("No element.  Returning to main menu.\n");
		return 1;
	}

	list_setcurr(list, e);
}


int
do_move(LIST *list)
{
   char cmd[2];
   int rc;
   
   printf("move to (p)revious, (n)ext, (f)ront, or (r)ear: ");
   rc = scanf("%1s", cmd);
   switch (cmd[0]) {
      case 'p':
	 if (list_mvprev(list) == NULL)
	    printf("No previous element!\n");
	 break;
      case 'n':
	 if (list_mvnext(list) == NULL)
	    printf("No next element!\n");
	 break;
      case 'f':
	 /* We ignore the return code here. */
	 list_mvfront(list);
	 break;
      case 'r':
	 /* We ignore the return code here. */
	 list_mvrear(list);
	 break;
      default:
         printf("%c not recognized.  Returning to main menu.\n", cmd[0]);
   }
}


/* Routine to print the integer stored at each node.  In this example,
 * we ignore the first parameter, which might be useful if we were
 * searching the list or something.  We must return 0 or 1, so we always
 * return 1.
 */
list_boolean_t
print_element(void *input, void *curr)
{
   printf(" %d", *(int *) curr);
   return(LIST_TRUE);
}


int
do_traverse(LIST *list)
{
   char cmd[2];
   int opts=0, rc;

   printf("traverse from (f)ront, (r)ear, or (c)urrent element: ");
   rc = scanf("%1s", cmd);
   switch (cmd[0]) {
      case 'f':
         opts = (opts | LIST_FRNT);
	 break;
      case 'r':
         opts = (opts | LIST_REAR);
	 break;
      case 'c':
         opts = (opts | LIST_CURR);
	 break;
      default:
         printf("%c not recognized.  Returning to main menu.\n", cmd[0]);
   }

   if (cmd[0] == 'c') {
      printf("traverse (f)orwards or (b)ackwards: ");
      rc = scanf("%1s", cmd);
      switch (cmd[0]) {
         case 'f':
            opts = (opts | LIST_FORW);
	    break;
         case 'b':
            opts = (opts | LIST_BACK);
	    break;
         default:
            printf("%c not recognized.  Returning to main menu.\n", cmd[0]);
      }
   }

   printf("(a)lter or (p)reserve the current element pointer: ");
   rc = scanf("%1s", cmd);
   switch (cmd[0]) {
      case 'a':
         opts = (opts | LIST_ALTR);
	 break;
      case 'p':
         opts = (opts | LIST_SAVE);
	 break;
      default:
         printf("%c not recognized.  Returning to main menu.\n", cmd[0]);
   }

   printf("Traversal: ");
   rc = list_traverse(list, (char *) 0, print_element, opts);
   switch (rc) {
      case LIST_EMPTY:
         printf("(empty).\n");
         break;
      case LIST_OK:
         printf(".\n");
         break;
      case LIST_EXTENT:
         printf(". (extent reached)\n");
         break;
      default:
         printf("%c not recognized.  Returning to main menu.\n", cmd[0]);
   }
}


/* Routine to print a list of integers, using the traversal function.
 * In this example, we send NULL for the second parameter, which might be
 * used to specify an element to search for.
 */
int
print_list(LIST *list)
{
   printf("List:");
   if (list_empty(list))
      printf(" (empty).\n");
   else {
      list_traverse(list, (char *) 0, print_element,
		    (LIST_FRNT | LIST_FORW | LIST_SAVE));
      printf(".\n");
   }
}
