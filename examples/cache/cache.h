/* cache.h -- declarations and such for a generic, list(3)-based cache package.
 *
 * Last edited: Tue Jul 28 15:42:15 1992 by bcs (Bradley C. Spatz) on wasp
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

typedef struct cache_t {
   int max_elements;
   LIST *list;
} CACHE;

/* Provide some useful prototypes. */
CACHE *cache_init();
char *cache_enter();
char *cache_check();
void cache_free();

/* Define the deallocation constants. */
#define CACHE_DEALLOC   LIST_DEALLOC
#define CACHE_NODEALLOC LIST_NODEALLOC
