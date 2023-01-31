#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <time.h>
	
#include "hash.h"

#define ITERATIONS 10000000
#define BLENGTH    1048576
#define KEYRANGE   65536
#define MAXITEMS   32

int
hash_test(HASH *h, uint32_t iter)
{
	uint64_t	key, oldkey;
	void 		*buf, *ebuf;
	int 		buflen=BLENGTH;
	uint32_t 	r, i, hit;
	hash_stats_t 	*hs;

	for(hit=0,i=0;i<iter; i++) {
		r = rand();	// Get a pseudo-random int between 0-RAND_MAX.
		key = r % KEYRANGE;

		buf = hash_get(h, key);
		if (!buf) {
			buf = malloc(buflen);
			if (!buf) {
				printf("Memory failure\n");
				return(-1);
			}

			*(int *)buf = i;
			
			if (!hash_put(h, key, buf, buflen, &ebuf)) {
				printf("Hash enter failure\n");
				return(-1);
			}

			if (ebuf) {
				free(ebuf);
			}
		} else {
//			printf("Cache HIT[%d]: (%ld, %p) = %d\n", i, key, buf, *(int *)buf);
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

