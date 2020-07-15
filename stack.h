/* stack.h -- present stack abstraction with list primitives.
 *
 * Last edited: Tue Jul 28 15:33:54 1992 by bcs (Bradley C. Spatz) on wasp
 *
 * Copyright (C) 1992, Bradley C. Spatz, bcs@ufl.edu
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <list.h>

/* Present the stack datatypes in terms of list types (our point exactly). */
#define STACK LIST
#define STACK_ELEMENT LIST_ELEMENT

/* Now map the stack functions onto the list primitives.   The current
 * element pointer will always point to front of the list, which is
 * where we push and pop.  With this model, we can map into the list
 * primitives directly.
 */
#define stack_init()                     list_init()
#define stack_push(stack, data, bytes)   list_insert_before(stack, data, bytes)
#define stack_pop(stack)                 list_remove_front(stack)
#define stack_top(stack)                 list_front(stack)
#define stack_size(stack)                list_size(stack)
#define stack_empty(stack)               list_empty(stack)
#define stack_free(stack, dealloc)       list_free(stack, dealloc)

/* Define the deallocation constants. */
#define STACK_DEALLOC   LIST_DEALLOC
#define STACK_NODEALLOC LIST_NODEALLOC
