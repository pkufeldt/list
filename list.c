/* list.c -- a generic list package
 * 
 * Last edited: Tue Jul 28 15:37:24 1992 by bcs (Bradley C. Spatz) on wasp
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
 *
 * We define the following routines here:
 *
 * LIST *list_create()
 * void  list_destroy(LIST *list, list_deallocator_t func)
 *
 * Simple list size utility routines
 * int   list_empty(LIST *list)
 * int   list_size(LIST *list)
 *
 * Basic list data access routines
 * void *list_front(LIST *list)
 * void *list_curr(LIST *list)
 * void *list_rear(LIST *list)
 *
 * List element access
 * LIST_ELEMENT *list_element_front(LIST *list)
 * LIST_ELEMENT *list_element_curr(LIST *list)
 * LIST_ELEMENT *list_element_rear(LIST *list)
 *
 * Basic list movement routines
 * LIST *list_mvprev(LIST *list)
 * LIST *list_mvnext(LIST *list)
 * LIST *list_mvfront(LIST *list)
 * LIST *list_mvrear(LIST *list)
 * LIST *list_mvcurr(LIST *list, LIST_ELEMENT *element)
 *
 * List manipulation routines
 * void *list_insert_before(LIST *list, void *data, size_t bytes)
 * void *list_insert_after(LIST *list, void *data, size_t bytes)
 * void *list_remove_front(LIST *list)
 * void *list_remove_rear(LIST *list)
 * void *list_remove_curr(LIST *list)
 *
 * int   list_traverse(LIST *list, void *data, list_comparator_t func, int opts)
 */

static char brag[] = "$$Version: list-2.1 Copyright (C) 1992 Bradley C. Spatz\n\
$$Version: list-2.2 Copyright (C) 2021 Philip Kufeldt";

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include "list.h"

static uint32_t list_id=1000; /* list ids are allocated from here */

/* Define an internal structure to describe each element in the list. */
typedef struct list_element {
	uint32_t id; 
	struct list_element *prev;
	struct list_element *next;
	void *data;
} list_element_t;

/* Define an internal structure to describe the list */
typedef struct list {
	uint32_t id;			/* List ID, matches with Element IDs */
	uint32_t size;			/* Number of elements in the list */
	list_element_t *front;	/* Front element of the list */
	list_element_t *rear;	/* Tail element of the list */
	list_element_t *curr;	/* Current element */
} list_t;

/* List creation and destruction routines */
#define SBCAS __sync_bool_compare_and_swap

LIST *
list_create()
{
	list_t *list;

	/* Allocate, initialize, and return a new list. */
	list = (LIST *) malloc(sizeof(list_t));
	list->size = 0;
	list->front = NULL;
	list->rear = NULL;
	list->curr = NULL;

	/* Grab the next id, use compare and swap since list_id is a global */
	list->id = list_id;
	while (!SBCAS(&list_id, list->id, list->id + 1)) {
		list->id = list_id;
	}
	
	return((LIST *)list);
}


void
list_destroy(LIST *ulist, list_deallocator_t func)
{
	void *data;
	list_t *list = (list_t *)ulist;
	
	/* 
	 * Move to the front of the list.  Start removing elements from the
	 * front.  Free up the data element by either applying the user-supplied
	 * deallocation routine or free(3).  When we've gone through all the
	 * elements, free the list descriptor.
	 */
	list_mvfront(list);
	while (! list_empty(list)) {
		data = list_remove_front(list);
		/* Apply either no deallocation function to each node, 
		 * our own, or a user-supplied version.
		 */
		if ((enum list_constants)func != LIST_NODEALLOC) {
			if ((enum list_constants)func == LIST_DEALLOC) {
				free(data);
			} else {
				(*func)(data);
			}
		}
	}

	free(list);
}


/* Simple list size utility routines */
list_boolean_t
list_empty(LIST *ulist)
{
	list_t *list = (list_t *)ulist;

	/* Return 1 if the list is empty.  0 otherwise. */
	return((list->front == NULL) ? LIST_TRUE : LIST_FALSE);
}


uint32_t
list_size(LIST *ulist)
{
	list_t *list = (list_t *)ulist;
	return(list->size);
}


/* Basic list data access routines */
void *
list_front(LIST *ulist)
{
	list_t *list = (list_t *)ulist;
	return((list->front == NULL) ? NULL : (list->front->data));
}


void *
list_curr(LIST *ulist)
{
	list_t *list = (list_t *)ulist;
	return((list->curr == NULL) ? NULL : (list->curr->data));
}


void *
list_rear(LIST *ulist)
{
	list_t *list = (list_t *)ulist;
	return((list->rear == NULL) ? NULL : (list->rear->data));
}


/* List element access */
LIST_ELEMENT *
list_element_front(LIST *ulist)
{
	list_t *list = (list_t *)ulist;
	return((list->front == NULL) ? NULL : ((LIST_ELEMENT *)list->front));
}

 
LIST_ELEMENT *
list_element_curr(LIST *ulist)
{
	list_t *list = (list_t *)ulist;
	return((list->front == NULL) ? NULL : ((LIST_ELEMENT *)list->curr));
}

 
LIST_ELEMENT *
list_element_rear(LIST *ulist)
{
	list_t *list = (list_t *)ulist;
	return((list->front == NULL) ? NULL : ((LIST_ELEMENT *)list->rear));
}


/* Basic list movement routines */
LIST *
list_mvprev(LIST *ulist)
{
	list_t *list = (list_t *)ulist;
	
	/* Move to the previous link, if possible.  Note that the following
	 * compound conditional expression *requires* a short-circuit evaluation.
	 */
	
	if ((list->curr != NULL) && (list->curr->prev != NULL)) {
		list->curr = list->curr->prev;
		return((LIST *)list);
	} else {
		return(NULL);
	}
}


LIST *
list_mvnext(LIST *ulist)
{
	list_t *list = (list_t *)ulist;

	/* Move to the next link, if possible.  Note that the following
	 * compound conditional expression *requires* a short-circuit evaluation.
	 */
	if ((list->curr != NULL) && (list->curr->next != NULL)) {
		list->curr = list->curr->next;
		return((LIST *)list);
	} else {
		return(NULL);
	}
}


LIST *
list_mvfront(LIST *ulist)
{
	list_t *list = (list_t *)ulist;

	/* Move to the front of the list.*/
	list->curr = list->front;
	return((LIST *)list);
}


LIST *
list_mvrear(LIST *ulist)
{
	list_t *list = (list_t *)ulist;

	/* Move to the front of the list.*/
	list->curr = list->rear;
	return((LIST *)list);
}


LIST *
list_setcurr(LIST *ulist, LIST_ELEMENT *uelement)
{
	list_t *list = (list_t *)ulist;
	list_element_t *element = (list_element_t *)uelement;

	/* Sanity check */ 
	if (list->id != element->id)
		return(NULL);
	
	/* Move to the front of the list.*/
	list->curr = element;
	return((LIST *)list);

}


/* Internal List element creation and single element remove routines */
static list_element_t *
list_create_element(void *data, size_t bytes)
{
   list_element_t *new;

   /* Allocate storage for the new node and its data.  Return NULL if
    * unable to allocate.
    */
   new = (list_element_t *) malloc(sizeof(list_element_t));
   if (new == NULL) {
	   return(NULL);
   }

   /* Allocate storage for the data only if requested; i.e. if bytes > 0.
    * Then either copy the data or just the reference into the node.
    */
   if (bytes > 0) {
	   new->data = (char *) malloc(bytes);
	   if (new->data == NULL) {
		   return(NULL);
	   }
	   (void) memcpy(new->data, data, bytes);
   } else {
	   new->data = (char *) data;
   }

   return(new);
}


static void *
list_remove_single(list_t *list)
{
	char *data;

	/* The list has one element.  Easy. */
	data = list->curr->data;
	free(list->curr);
	list->front = list->rear = list->curr = NULL;
	list->size--;
	return (data);
}


/* List manipulation routines */
void *
list_insert_before(LIST *ulist, void *data, size_t bytes)
{
	list_element_t *new;
	list_t *list = (list_t *)ulist;

	/* Allocate storage for the new element and its data.*/
	new = list_create_element(data, bytes);
	if (new == NULL)
		return(NULL);
	new->id = list->id;

	/* Now insert the element before the current, considering the cases:
	 *    1) List is empty
	 *    2) Inserting at front
	 *    3) Otherwise
	 * We handle them directly, in order.
	 */
	if (list->front == NULL) {
		/* The list is empty.  Easy. */
		new->prev = new->next = NULL;
		list->front = list->rear = list->curr = new;
	} else if (list->curr->prev == NULL) {
		/* Inserting at the front. */
		new->prev = NULL;
		new->next = list->curr;
		list->curr->prev = new;
		list->front = new;
	} else {
		/* Otherwise. */
		new->prev = list->curr->prev;
		list->curr->prev->next = new;
		new->next = list->curr;
		list->curr->prev = new;
	}

	list->curr = new;
	list->size++;
	return(new->data);
}


void *
list_insert_after(LIST *ulist, void *data, size_t bytes)
{
	list_element_t *new;
	list_t *list = (list_t *)ulist;

	/* Allocate storage for the new element and its data.*/
	new = list_create_element(data, bytes);
	if (new == NULL)
		return(NULL);
	new->id = list->id;

	/* Now insert the element after the current, considering the cases:
	 *    1) List is empty
	 *    2) Inserting at rear
	 *    3) Otherwise
	 * We handle them directly, in order.
	 */
	if (list->front == NULL) {
		/* The list is empty.  Easy. */
		new->prev = new->next = NULL;
		list->front = list->rear = list->curr = new;
	} else if (list->curr->next == NULL) {
		/* Inserting at the rear. */
		new->next = NULL;
		new->prev = list->curr;
		list->curr->next = new;
		list->rear = new;
	} else {
		/* Otherwise. */
		new->next = list->curr->next;
		new->next->prev = new;
		new->prev = list->curr;
		list->curr->next = new;
	}

	list->curr = new;
	list->size++;
	return(new->data);
}


void *
list_remove_front(LIST *ulist)
{
	list_element_t *temp;
	list_t *list = (list_t *)ulist;
	void *data;

	/* Removing and return front element, or NULL if empty.  If curr
	 * is the front, then curr becomes the next element.
	 */
	if (list->front == NULL) {
		/* List is empty.  Easy. */
		return(NULL);
	} else if (list->front == list->rear) {
		/* List has only one element.  Easy. */
		data = list_remove_single(list);
	} else {
		/* List has more than one element.  Make sure to check if curr
		 * points to the front.
		 */
		data = list->front->data;
		list->front->next->prev = NULL;
		temp = list->front;
		list->front = temp->next;
		if (list->curr == temp)
			list->curr = temp->next;
		free(temp);
		list->size--;
	}

	return(data);
}


void *
list_remove_rear(LIST *ulist)
{
	list_element_t *temp;
	list_t *list = (list_t *)ulist;
	void *data;

	/* Removing and return rear element, or NULL if empty.  If curr
	 * is the rear, then curr becomes the previous element.
	 */
	if (list->front == NULL) {
		/* List is empty.  Easy. */
		return(NULL);
	} else if (list->front == list->rear) {
		/* List has only one element.  Easy. */
		data = list_remove_single(list);
	} else {
		/* List has more than one element.  Make sure to check if curr
		 * points to the rear.
		 */
		data = list->rear->data;
		list->rear->prev->next = NULL;
		temp = list->rear;
		list->rear = temp->prev;
		if (list->curr == temp)
			list->curr = temp->prev;
		free(temp);
		list->size--;
	}

	return(data);
}


void *
list_remove_curr(LIST *ulist)
{
	list_element_t *temp;
	list_t *list = (list_t *)ulist;
	void *data;

	/* Remove the current element, returning a pointer to the data, or
	 * NULL if the list is empty.  Set curr to the next element unless
	 * curr is at the rear, in which case curr becomes the previous
	 * element.
	 */
	if (list->front == NULL) {
		/* The list is empty.  Easy. */
		return(NULL);
	} else if (list->front == list->rear) {
		/* The list has one element.  Easy. */
		data = list_remove_single(list);
	} else if (list->curr == list->front) {
		/* Removing front element.  Easy. */
		data = list_remove_front(list);
	} else if (list->curr == list->rear) {
		/* Removing from the rear.  Easy.*/
		data = list_remove_rear(list);
	} else {
		/* Otherwise.  Must be inside a 3-element or larger list. */
		data = list->curr->data;
		temp = list->curr;
		temp->next->prev = temp->prev;
		temp->prev->next = temp->next;
		list->curr = temp->next;
		free(temp);
		list->size--;
	}

	return(data);
}


int
list_traverse(LIST *ulist, void *data, list_comparator_t func, int opts)
{
	list_element_t *lp;
	list_t *list = (list_t *)ulist;
	int status, rc;

	/* Traverse the list according to opts, calling func at each element,
	 * until func returns 0 or the extent of the list is reached.  We return
	 * 0 if the list is empty, 2 if we tried to go beyond the extent of the
	 * list, and 1 otherwise.  We may or may not affect the current element
	 * pointer.
	 */
	if (list->front == NULL)
		return(LIST_EMPTY);
   
	/* Decide where to start. */
	if ((opts & LIST_CURR) == LIST_CURR) {
		lp = list->curr;
	} else if ((opts & LIST_REAR) == LIST_REAR) {
		lp = list->rear;
	} else {
		lp = list->front;
	}
   
	/* Now decide if to update the current element pointer. */
	if ((opts & LIST_ALTR) == LIST_ALTR)
		list->curr = lp;

	/* Now go until 0 is returned or we hit the extent of the list. */
	rc = LIST_OK;
	status = LIST_TRUE;
	while(status) {
		status = (*func)(data, lp->data);
		if (status) {
			if ((((opts & LIST_BACK) == LIST_BACK) ?
			     (lp->prev) : (lp->next)) == NULL) {
				/* Tried to go beyond extent of list. */
				status = LIST_FALSE;
				rc = LIST_EXTENT;
			} else {
				/* Decide where to go next. */
				lp = (((opts & LIST_BACK) == LIST_BACK) ?
				      (lp->prev) : (lp->next));

				/* Now decide if to update the current 
				 * element pointer. */
				if ((opts & LIST_ALTR) == LIST_ALTR)
					list->curr = lp;
			}
		}
	}

	return(rc);
}


