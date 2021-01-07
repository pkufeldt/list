/* list.h -- data structures and such for generic list package
 * 
 * Last edited: Tue Jul 28 15:29:56 1992 by bcs (Bradley C. Spatz) on wasp
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

#ifndef _LIST_H
#define _LIST_H

/* Make LISTs opaque */
typedef void LIST;
typedef void LIST_ELEMENT;

/* Define some constants for controlling list traversals.  We
 * bit-code the attributes so they can be OR'd together.
 */
typedef enum list_bits {
	LIST_FORW	= 0x00,
	LIST_BACK	= 0x02, 
       	LIST_FRNT	= 0x04 | LIST_FORW, /* def, FRNT implies FORWwards. */
	LIST_CURR	= 0x08,
	LIST_REAR	= 0x10 | LIST_BACK, /* REAR implies BACKwards. */
	LIST_SAVE	= 0x20,		    /* def */
	LIST_ALTR	= 0x40,
} list_bits_t;

/* Define some constants for return codes and such. */
typedef enum list_boolean {
	LIST_TRUE  = 1,
	LIST_FALSE = 0,
} list_boolean_t;

typedef enum list_constants {
	LIST_DEALLOC   = -1,
	LIST_NODEALLOC = -2,
	LIST_EMPTY     = 0,
	LIST_OK        = 1,
	LIST_EXTENT    = 2,
} list_constants_t;

/* 
 * List comparator function type, used in list traversal for 
 * caller supplied custom comparators. Takes two void * data pointers, 
 * does the custom compare and returns list_boolean, LIST_TRUE or LIST_FALSE,
 * as the result.
 */
typedef list_boolean_t (*list_comparator_t)(void *userdata, void *listdata); 

/* 
 * List deallocator function type, used in list destroy for 
 * caller supplied custom data deallocation. Takes a void * data pointer, 
 * does the custom deallocation and returns nothing as the result.
 */
typedef void (*list_deallocator_t)(void *data);

/* Prototype ahoy! */

/* List creation and destruction routines */
LIST *list_create();
void  list_destroy(LIST *list, list_deallocator_t func);

/* Simple list size utility routines */
list_boolean_t	list_empty(LIST *list);
uint32_t   	list_size(LIST *list);

/* Basic list data access routines */
void *list_front(LIST *list);
void *list_curr(LIST *list);
void *list_rear(LIST *list);

/* List element access */
LIST_ELEMENT *list_element_front(LIST *list);
LIST_ELEMENT *list_element_curr(LIST *list);
LIST_ELEMENT *list_element_rear(LIST *list);

 /* Basic list movement routines */
LIST *list_mvprev(LIST *list);
LIST *list_mvnext(LIST *list);
LIST *list_mvfront(LIST *list);
LIST *list_mvrear(LIST *list);
LIST *list_setcurr(LIST *list, LIST_ELEMENT *element);

/* List manipulation routines */
void *list_insert_before(LIST *list, void *data, size_t bytes);
void *list_insert_after(LIST *list, void *data, size_t bytes);
void *list_remove_front(LIST *list);
void *list_remove_rear(LIST *list);
void *list_remove_curr(LIST *list);

int   list_traverse(LIST *list, void *userdata,
		    list_comparator_t func, int opts);

#endif /* _LIST_H */
