/* hash.c -- routines to implement a hash table based list(3)-based 
 * hashed cache package.
 *
 * Last edited: Time-stamp: <2023-01-30 17:44:41 pak>
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
 */

#include <list.h>
#include <pthread.h>

/* Make HASH opaque */
typedef void HASH;

/*
 * Cache Entry Descriptor
 * This structure is a small header placed on key value buffers
 */
typedef struct ced {
	uint64_t	ced_key;
	uint32_t	ced_refcnt;
	void		*ced_buf;
	uint64_t	ced_buflen;
} ced_t;

typedef struct hash_stats {
	uint64_t	checks;		/* Hash checks called */
	uint64_t	insertions;	/* cachelines inserted */
	uint64_t	hits;		/* Successful hash_checks */
	uint64_t	misses;		/* Unsuccessful hash_checks */
	uint64_t	evictions;	/* Evictions */
} hash_stats_t;

/* Provide some useful prototypes. */

HASH *hash_create(uint32_t max_items);
void  hash_destroy(HASH *hash);
void *hash_put(HASH *hash,
	       uint64_t key, void *data, uint32_t bytes, void **evicted);
void *hash_get(HASH *hash, uint64_t key);

/* Some diagnostics */
hash_stats_t *hash_get_stats(HASH *hash);
void hash_print(HASH *hash);
void hash_check(HASH *hash);

