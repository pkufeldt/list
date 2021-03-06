/* stacktest.c -- test program for generic stack package
 *
 * Last edited: Tue Jul 28 15:38:19 1992 by bcs (Bradley C. Spatz) on wasp
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

static char brag[] = "$$Version: stacktest-2.1 Copyright (C) 1992 Bradley C. Spatz";

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include "../stack.h"

list_boolean_t print_element(void *input, void *curr);
int print_stack(STACK *stack);

int
main(int argc, char *argv[])
{
   STACK *stack;
   char cmd;
   int val, *pval;

   /* Print some instructions. */
   printf("This program demonstrates the various stack manipulation\n");
   printf("routines supplied by this package.  Although this program\n");
   printf("manipulates a stack of integers, data of any type and size\n");
   printf("(and not just integers) are supported by the routines.  See\n");
   printf("the man page for more information.\n\n");

   /* Allocate a new stack. */
   stack = stack_init();
   print_stack(stack);

   /* Get some commands. */
   printf("(p)ush, p(o)p, (t)op, (s)ize, (c)heck, (q)uit: ");
   while (scanf("%1s", &cmd) != EOF) {
      switch (cmd) {
	 case 'p':
	    printf("Value (int) to push: ");
	    if (scanf("%d", &val))
	       /* We ignore the return code here, but in practice, we may
		* fail (with a return code of NULL).
                */
	       stack_push(stack, &val, sizeof(val));
	    break;
	 case 'o':
	    pval = (int *) stack_pop(stack);
	    if (pval != NULL)
	       printf("%d popped.\n", *pval);
	    else
	       printf("Stack is empty!\n");
	    break;
	 case 't':
	    pval = (int *) stack_top(stack);
	    if (pval == NULL) 
	       printf("Stack is empty!\n");
	    else
	       printf("%d on top.\n", *pval);
	    break;
	 case 's':
	    val = stack_size(stack);
	    printf("Stack has %d element%s.\n", val, ((val == 1) ? "":"s"));
	    break;
	 case 'c':
	    printf("Stack is%s empty.\n", (stack_empty(stack) ? "" : " not"));
	    break;
	 case 'q':
	    stack_free(stack, (void *)STACK_DEALLOC);
	    exit(0);
	    break;
         default:
	    printf("'%c' not a recognized command!\n", cmd);
	    break;
      }
      print_stack(stack);
      printf("\n(p)ush, p(o)p, (t)op, (s)ize, (c)heck, (q)uit: ");
   }

   exit(0);
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
   return(STACK_TRUE);
}


/* Routine to print a stack of integers. */
int
print_stack(STACK *stack)
{
   printf("Stack: ");
   if (stack_empty(stack))
      printf("(empty)\n");
   else {
      list_traverse(stack, (char *) 0, print_element,
		    (LIST_FRNT | LIST_FORW | LIST_SAVE));
      printf("\n");
   }
}
