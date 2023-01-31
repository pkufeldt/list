#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <time.h>
	
#include "hash.h"

#define ITERATIONS 100000000
#define BLENGTH    1048576
#define KEYRANGE   65536
#define MAXITEMS   32

int
hash_test(HASH *h, uint32_t iter)
{
	uint64_t	key, oldkey;
	ced_t		*ced, *eced;
	uint32_t 	r, i, hit;
	hash_stats_t 	*hs;

	for(hit=0,i=0;i<iter; i++) {
		r = rand();	// Get a pseudo-random int between 0-RAND_MAX.
		key = r % KEYRANGE;

		ced = hash_get(h, key);
		if (!ced) {
		        ced = malloc(sizeof(ced_t));
			if (!ced) {
				printf("Memory failure (ced)\n");
				return(-1);
			}
			
			ced->ced_key = key;
			ced->ced_refcnt = 0;
			ced->ced_buflen = BLENGTH;
			ced->ced_buf = malloc(BLENGTH);
			if (!ced->ced_buf) {
				printf("Memory failure (buf)\n");
				return(-1);
			}
			*(int *)ced->ced_buf = i; /* tag the buffer */

			if (hash_put(h, ced, &eced) < 0) {
				printf("Hash enter failure\n");
				return(-1);
			}

			if (eced) {
				free(eced->ced_buf);
				free(eced);
			}
		} else {
			/* Cache Hit */
		}
	}

	hs = hash_get_stats(h);
	if (hs) {
		printf("Checks    : %10lu\n", hs->checks);
		printf("Insertions: %10lu (%f)\n",
		       hs->insertions, (float)hs->insertions/ hs->checks);
		printf("Evictions : %10lu (%f)\n", hs->evictions,
		       (float)hs->evictions/hs->insertions);
		printf("Misses    : %10lu (%f)\n", hs->misses,
		       (float) hs->misses/hs->checks);
		printf("Hits      : %10lu (%f)\n", hs->hits,
		       (float) hs->hits/ hs->checks);
	}
	hash_print(h);
	hash_check(h);
}

int
main(int argc, char *argv[])
{
	uint32_t	max = MAXITEMS;
	HASH 		*h;
	
	h = hash_create(max);
	
	srand(time(NULL));   // Initialization, should only be called once.

	hash_test(h, ITERATIONS);
	hash_destroy(h);
}

