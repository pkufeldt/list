/* hash.c -- routines to implement a hash table based list(3)-based 
 * hashed cache package.
 *
 * Last edited: Time-stamp: <2023-01-31 18:54:43 pak>
 * 
 * Copyright (C) 2023, Philip Kufeldt, pak@integratus.com
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
 * We base this package on the list(3) package.
 *
 * This implements a hash based cache with an LRU eviction policy.
 * It does this by creating two data structures: an LRU list and 
 * a hash table that has a fixed number of entries (cachelines). Each
 * table entry is a list.  The premise is that a buffer address and a key
 * are provided by the caller, the key and buffer address are stored as 
 * an item in the front of the LRU list. Then the key is hashed to find the 
 * correct cacheline (table entry), the key and buffer are stored as another
 * item in that cacheline list. The LRU item and cacheline item cross
 * link to each other to permit quick list manipulations with out searching.
 * If there are hash collisions the cacheline list simply grows and list 
 * searching is required.  n the worst pathological cas ethis would be 
 * O(n), where n=max_items. With normal distributions should be O(logn).Once 
 * the max number of items are entered in the hash, future additions force
 * an eviction. Evictions simply remove the back item from LRU list and
 * delete its corresponding cacheline entry through its corss reference - no
 * searching the list is required.  Checking for the existance 
 * of an item, causes a reference and moves the item from its current 
 * location on the LRU list the the front. 
 *
 * Available APIS:
 * 	HASH *hash_init(int max_items);
 * 	void *hash_enter(HASH *hash, uint64_t key, void *data, int bytes, 
 * 			 void **evicted);
 * 	void *hash_check(HASH *hash, uint64_t key);
 * 	void  hash_free(HASH *hash);
 */

static char brag[] = "$$Version: hash-1.0 Copyright (C) 2023 Philip Kufeldt";

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include "hash.h"

typedef struct hash_item {
	LIST_ELEMENT	*element; 	/* Back pointer to element in list */
	LIST 		*list;		/* Back list */
	ced_t		*ced;		/* Caller supplied CED */
} hash_item_t;

typedef struct hash {
	pthread_mutex_t m;		/* Mutex to protect hash manipulations*/
	uint32_t	max_items;	/* Max number if items in the hash */
	hash_stats_t	stats;		/* Stats for this hash cache */
	LIST 		*lru_list;	/* Master entry list */ 
	LIST		*table[];	/* This is variable, so must be last */
} hash_t;

#define HASH_FN(_k, _i) ((_k) % (_i))


HASH *
hash_create(uint32_t max_items)
{
	hash_t	*new_hash;
	int 	i, table_len = sizeof(LIST *) * max_items;
	
	if (!max_items)
		return(NULL);
	
	/* Allocate, initialize, and return a new hash.   Return NULL if
	 * the malloc or list initialization fails.
	 */
	new_hash = (hash_t *)malloc(sizeof(hash_t) + table_len);
	if (new_hash == NULL) {
		return(NULL);
	}
	memset(new_hash->table, 0, (sizeof(LIST *) * max_items));

	new_hash->max_items		= max_items;
	new_hash->stats.checks		= 0;
	new_hash->stats.insertions	= 0;
	new_hash->stats.hits		= 0;
	new_hash->stats.misses		= 0;
	new_hash->stats.evictions	= 0;

	if ((new_hash->lru_list = list_create()) == NULL) {
		free(new_hash);
		return(NULL);
	}
	
	for (i=0; i<max_items; i++) {
		if ((new_hash->table[i] = (LIST *)list_create()) == NULL) {
			break;
		}
	}
	
	if (i == max_items) {
		/* Success */
		pthread_mutex_init(&new_hash->m, NULL);
		return(new_hash);
	}

	/* Failure */
	for (i=0; i<max_items; i++) {
		if (new_hash->table[i]) {
			list_destroy(new_hash->table[i],
				     (list_deallocator_t)LIST_DEALLOC);
		}
	}
	list_destroy(new_hash->lru_list, (list_deallocator_t)LIST_DEALLOC);
	free(new_hash);

	return(NULL);
}


void
hash_destroy(HASH *ohash)
{
	hash_t		*hash = (hash_t *)ohash;
	hash_item_t	*item;
	int		i;
	if (!hash) {
		return;
	}

	/* First free up the list, and then the cache descriptor. */
	pthread_mutex_lock(&hash->m);
	list_mvfront(hash->lru_list);
	while (list_curr(hash->lru_list) != NULL) {
		item = (hash_item_t *)list_remove_curr(hash->lru_list);
		free(item);
	}
	list_destroy(hash->lru_list, (list_deallocator_t)LIST_NODEALLOC);
	
	for (i=0; i<hash->max_items; i++) {
		while (list_curr(hash->table[i]) != NULL) {
			item = (hash_item_t *)list_remove_curr(hash->table[i]);
			free(item);
		}
		list_destroy(hash->table[i],
			     (list_deallocator_t)LIST_NODEALLOC);
	}
	pthread_mutex_unlock(&hash->m);
	free(hash);
	return;
}


int
hash_put(HASH *ohash, ced_t *ced, ced_t **evicted)
{
	uint32_t	 index;
	LIST		*l;
	hash_t		*hash = (hash_t *)ohash;
	hash_item_t 	*lru_item;
	hash_item_t 	*hash_item;
	hash_item_t 	*removed_item;
	hash_item_t 	*p;

	if (evicted)
		*evicted = NULL;
	
	if (!hash || !ced) {
		return(-1);
	}

	index = HASH_FN(ced->ced_key, hash->max_items);
	
	lru_item = malloc(sizeof(hash_item_t));
	hash_item = malloc(sizeof(hash_item_t));
	if (!lru_item || ! hash_item) {
		free(lru_item);
		free(hash_item);
		return(-1);
	}

	lru_item->ced   = ced;
	lru_item->list  = hash->table[index];

	hash_item->ced   = ced;
	hash_item->list  = hash->lru_list;

	pthread_mutex_lock(&hash->m);

	/* Place on the front of the LRU list, on the front its the MRU */
	list_mvfront(hash->lru_list);
        p = (hash_item_t *)list_insert_before(hash->lru_list, lru_item, 0);
	if (!p) {
		pthread_mutex_unlock(&hash->m);
		
		free(lru_item);
		free(hash_item);
		return(-1);
	}

	/* Now insert into the correct hash table list */
	list_mvfront(hash->table[index]);
        p = (hash_item_t *)list_insert_before(hash->table[index], hash_item, 0);
	if (!p) {
		list_remove_curr(hash->lru_list);
		pthread_mutex_unlock(&hash->m);

		free(lru_item);
		free(hash_item);
		return(-1);
	}

	/* Create the cross links */ 
	lru_item->element  = list_element_front(hash->table[index]);
	hash_item->element = list_element_front(hash->lru_list);

	/* Evictions if necessary */
	if (list_size(hash->lru_list) > hash->max_items) {
		/* here is the LRU removal */
		lru_item = (hash_item_t *) list_remove_rear(hash->lru_list);
		index = HASH_FN(lru_item->ced->ced_key, hash->max_items); 
	
		/* Use the removed lru_item to cleanup the hash table */
		l = list_setcurr(hash->table[index], lru_item->element);
		if (!l) {
			printf("list-set curr failed\n");
			exit(1);
		}
		hash_item = (hash_item_t *)list_remove_curr(hash->table[index]);

		/* set returned removed data */
		hash->stats.evictions++;
		*evicted = hash_item->ced;
		free(lru_item);
		free(hash_item);
	}

	hash->stats.insertions++;
	pthread_mutex_unlock(&hash->m);
	return(0);
}


list_boolean_t
hash_keymatch(void *data, void *hdata)
{
	list_boolean_t	match = LIST_TRUE;
	uint64_t	key   = *(uint64_t *)data;

	if (key == ((hash_item_t *)hdata)->ced->ced_key)
		match = LIST_FALSE;  /* False terminates the search, ie found */
	return (match);
}


list_boolean_t
hash_datamatch(void *data, void *hdata)
{
	list_boolean_t	match = LIST_TRUE;

	if (data == ((hash_item_t *)hdata)->ced->ced_buf)
		match = LIST_FALSE;  /* False terminates the search, ie found */
	return (match);
}


ced_t *
hash_get(HASH *ohash, uint64_t key)
{
	ced_t 		*ced = NULL;
	uint32_t	index;
	hash_t		*hash = (hash_t *)ohash;
	hash_item_t 	*lru_item;
	hash_item_t 	*hash_item;
	
	if (!hash) {
		return(NULL);
	}

	hash->stats.checks++;
	index = HASH_FN(key, hash->max_items); 
	
	/* Check for an empty cache. */
	if (list_size(hash->lru_list) == 0) {
		hash->stats.misses++;
		return(NULL);
	}

	if (list_size(hash->table[index]) == 0) {
		hash->stats.misses++;		
		return(NULL);
	}

	pthread_mutex_lock(&hash->m);
	if (list_traverse(hash->table[index], &key, hash_keymatch,
		     (LIST_FRNT | LIST_FORW | LIST_ALTR)) == LIST_OK) {

		hash_item = list_curr(hash->table[index]);

		/* Use the the located hash_item, to adjust lru_list */
		list_setcurr(hash->lru_list, hash_item->element);
		if (list_curr(hash->lru_list) != list_front(hash->lru_list)) {
			lru_item = (hash_item_t *)list_remove_curr(hash->lru_list);

			list_mvfront(hash->lru_list);
			list_insert_before(hash->lru_list, lru_item, 0);
		}

		ced = hash_item->ced;
		hash->stats.hits++;
	} else {
		hash->stats.misses++;
	}	

	pthread_mutex_unlock(&hash->m);
	return(ced);
}


/* Diagnostic routines */

hash_stats_t *
hash_get_stats(HASH *ohash)
{
	hash_t		*hash = (hash_t *)ohash;

	if (!hash) {
		return(NULL);
	}

	return(&hash->stats);
}


/* traverse help, print an item */
list_boolean_t
hash_keyprint(void *data, void *ohdata)
{
	ced_t *ced = ((hash_item_t *)ohdata)->ced;

	printf("[%lu, %p]->", ced->ced_key, ced->ced_buf);
	return (LIST_TRUE);
}


void
hash_print(HASH *ohash)
{
	hash_t	*hash = (hash_t *)ohash;
	int 	i;

	if (!hash) {
		printf("%s: Bad hash: %p\n", __func__, hash);
		return;
	}

	pthread_mutex_lock(&hash->m);
	printf("Hash:\n");
	printf("\tItems: %u of %u\n",
	       list_size(hash->lru_list), hash->max_items);
	printf("\tLRU List:");
	list_traverse(hash->lru_list, NULL, hash_keyprint,
		      (LIST_FRNT | LIST_FORW | LIST_ALTR));
	printf("(null)\n");

	for(i=0;i<hash->max_items;i++) {
		printf("\tTable[%d]:",i);
		list_traverse(hash->table[i], NULL, hash_keyprint,
			      (LIST_FRNT | LIST_FORW | LIST_ALTR));
		printf("(null)\n");
	}
	pthread_mutex_unlock(&hash->m);

}


/* traverse helper, Add up all the items */
list_boolean_t
hash_count(void *data, void *ohdata)
{
	uint32_t    *items = (uint32_t*)data;

	(*items)++;
	
	return (LIST_TRUE);
}


/* check the consistency of the hashed cache table */
void
hash_check(HASH *ohash)
{
	hash_t		*hash = (hash_t *)ohash;
	hash_item_t	*lru_item, *hash_item;
	LIST_ELEMENT 	*lru_element, *hash_element;
	uint32_t	items, iitems, titems;
	ced_t		*lced, *hced;
	int 		i, match, last=0;

	if (!hash) {
		printf("%s: Bad hash: %p\n", __func__, hash);
		return;
	}
	
	pthread_mutex_lock(&hash->m);

	/* Validate sizes first */
	items = list_size(hash->lru_list);
	if (items > hash->max_items) {
		printf("%s: Too many items in LRU list (%u > %u)",
		       __func__, items, hash->max_items);
	}

	/* Let's verify the list package's size function by iterating */
	iitems = 0;
	list_traverse(hash->lru_list, &iitems, hash_count,
		      (LIST_FRNT | LIST_FORW | LIST_ALTR));

	if (items != iitems) {
		printf("%s: LRU list inconsistent counts (%u != %u)\n",
		       __func__, items, iitems);
	}

	/* Now go check the item totals in the hash table */
	for(titems=0, i=0; i<hash->max_items; i++) {
		titems += list_size(hash->table[i]);

		iitems = 0;
		list_traverse(hash->table[i], &iitems, hash_count,
		      (LIST_FRNT | LIST_FORW | LIST_ALTR));

		if (list_size(hash->table[i]) != iitems) {
			printf("%s: table[%d] inconsistent counts (%u != %u)\n",
			       __func__, i, items, list_size(hash->table[i]));
		}
	}

	if (titems != items) {
		printf("%s: LRU list counts do match table counts (%u != %u)\n",
		       __func__, items, titems);
	}

	/* 
	 * Now validate content
	 *
	 * Make sure each item on LRU list is unique. To do this
	 * we have to hand craft a traversal.
	 * Next validate its corresponding table item is present and 
	 * that the items are consistent with each other
	 */
	list_mvfront(hash->lru_list);
	while (!last && ((lru_item = list_curr(hash->lru_list)) != NULL)) {
		/* 
		 * Check for both key dups and data dups.
		 * The list can be searched in two steps, everything up
		 * current lru_item then everything after. If the lru_list
		 * is correct the first traverse should find the 
		 * the same item and the second traverse should not find
		 * anything. For this to work the the list curr needs to
		 * be advanced. Need to make sure not to alter curr while 
		 * traversing.
		 * Unfortunately list curr can never be advanced beyond
		 * rear, ie null. So on last iteration the second traverse
		 * is unnecessary.
		 */
		lru_element = list_element_curr(hash->lru_list);
		if (!list_mvnext(hash->lru_list)) {
			last = 1;
		}

		lced = lru_item->ced;

		match=0;
		if (list_traverse(hash->lru_list, &lced->ced_key, hash_keymatch,
				  (LIST_FRNT | LIST_FORW)) == LIST_OK) {
			match++;
		}
		if (!last &&
		    (list_traverse(hash->lru_list,
				   &lced->ced_key, hash_keymatch,
				   (LIST_CURR | LIST_FORW)) == LIST_OK)) {
			match++;
		}

		if (match !=1 ) {
			printf("%s: lru key not unique (%lu)\n",
			       __func__, lced->ced_key);
		}

		match=0;
		if (list_traverse(hash->lru_list,
				  lced->ced_buf, hash_datamatch,
				  (LIST_FRNT | LIST_FORW)) == LIST_OK) {
			match++;
		}
		if (!last &&
		    (list_traverse(hash->lru_list,
				   lced->ced_buf, hash_datamatch,
				   (LIST_CURR | LIST_FORW)) == LIST_OK)) {
			match++;
		}

		if (match !=1 ) {
			printf("%s: lru data ptr not unique (%p)\n",
			       __func__, lced->ced_buf);
		}

		/* Now find the item in the hash list and validate it */
		i = HASH_FN(lced->ced_key, hash->max_items);
		if (list_traverse(hash->table[i], &lced->ced_key, hash_keymatch,
				  (LIST_FRNT | LIST_FORW | LIST_ALTR)) == LIST_OK) {

			hash_item = list_curr(hash->table[i]);
			hash_element = list_element_curr(hash->table[i]);
			hced = hash_item->ced;

			if (lru_item->element != hash_element) {
				printf("%s: lru element incorrect (%p != %p)\n",
				       __func__,
				       lru_item->element, hash_element);
			}

			if (lru_item->list != hash->table[i]) {
				printf("%s: lru list incorrect (%p != %p)\n",
				       __func__,
				       lru_item->list, hash->table[i]);
			}

			if (hash_item->element != lru_element) {
				printf("%s: hash element incorrect (%p != %p)\n",
				       __func__,
				       hash_item->element, lru_element);
			}

			if (hash_item->list != hash->lru_list) {
				printf("%s: hash list incorrect (%p != %p)\n",
				       __func__,
				       hash_item->list, hash->lru_list);
			}

			if (hced->ced_key != lced->ced_key) {
				printf("%s: key incorrect (%lu != %lu)\n",
				       __func__, hced->ced_key, lced->ced_key);
			}
			
			if (hced->ced_buf != lced->ced_buf) {
				printf("%s: data incorrect (%p != %p)\n",
				       __func__, hced->ced_buf, lced->ced_buf);
			}
			
			if (hced->ced_buflen != lced->ced_buflen) {
				printf("%s: bytes incorrect (%lu != %lu)\n",
				       __func__,
				       hced->ced_buflen, lced->ced_buflen);
			}

		} else {
			/* failed to find it */
			printf("%s: filed to find lru_item in table (%lu)\n",
			       __func__, lced->ced_key);
		}
	}

	pthread_mutex_unlock(&hash->m);
	

}
