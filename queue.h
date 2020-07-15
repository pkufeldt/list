/* queue.h -- present queue abstraction with list primitives.
 *
 * Last edited: Tue Jul 28 15:34:15 1992 by bcs (Bradley C. Spatz) on wasp
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

/* Present the queue datatypes in terms of list types (our point exactly). */
#define QUEUE LIST
#define QUEUE_ELEMENT LIST_ELEMENT

/* Now map the queue functions onto the list primitives.  The current
 * element pointer will always point to the end of the list, which is
 * where we add new elements.  We remove elements from the front.
 * With this model, we map onto the list primitives directly.
 */
#define q_init()                        list_init()
#define q_enqueue(queue, data, bytes)   list_insert_after(queue, data, bytes)
#define q_dequeue(queue)                list_remove_front(queue)    
#define q_front(queue)                  list_front(queue)
#define q_size(queue)                   list_size(queue)
#define q_empty(queue)                  list_empty(queue)
#define q_free(queue, dealloc)          list_free(queue, dealloc)

/* Define the deallocation constants. */
#define QUEUE_DEALLOC   LIST_DEALLOC
#define QUEUE_NODEALLOC LIST_NODEALLOC
