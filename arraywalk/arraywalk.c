#include "arraywalk.h"
#include "util.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TIGHTLY_TIMED(bench_code, time_var) \
	struct timespec start, stop; \
	/* start the clock*/ \
	if (clock_gettime(CLOCK_MONOTONIC, &start)) \
		perror("clock_gettime"); \
	/* run the benchmark */ \
	bench_code \
	/* stop the clock */ \
	if (clock_gettime(CLOCK_MONOTONIC, &stop)) \
		perror("clock_settime"); \
	/* check result sanity */ \
	if (stop.tv_sec < start.tv_sec) \
		fprintf(stderr, "TIMING ERROR: This experiment started SECONDS in the future ...\n"); \
	else if (stop.tv_sec == start.tv_sec && stop.tv_nsec < start.tv_nsec) \
		fprintf(stderr, "TIMING ERROR: This experiment started NANOSECONDS in the future ...\n"); \
	/* report timing */ \
	time_var = (struct timespec){ stop.tv_sec - start.tv_sec, stop.tv_nsec - start.tv_nsec }

// Create a cycle using Sattolo's algorithm. This is the way the array will be
// traversed in the benchmarks.
// TODO: check return values (malloc, ..)
struct timespec makeRandomWalkArray(size_t len, struct walkArray ** result) {
	struct timespec elapsed;
	walking_t * array = malloc(sizeof(walking_t) * len);
	size_t i;

	// initialization step encodes index as values
	for (i = 0; i < len; ++i)
		array[i] = i;

	// shuffle the array
	// *boundaries voodoo:
	//  - for(): i goes up to the penultimate element instead of the last element
	//    because element i is swapped with an element > i
	//  - randMinMax(): the element to be swapped is
	//    one of the elements following element i
	size_t rnd;
	size_t swap;
	TIGHTLY_TIMED(
	for (i = 0; i < len - 1; ++i) {
		rnd = randMinMax(i + 1, len - 1);
		swap = array[i];
		array[i] = array[rnd];
		array[rnd] = swap;
	},
	elapsed);

	*result = malloc(sizeof(struct walkArray));
	(*result)->array = array;
	(*result)->len = len;

	return elapsed;
}

// Free the walkArray structure.
void freeWalkArray(struct walkArray * array) {
	free(array->array);
	free(array);
}

// walk the array as encoded by makeRandomWalkArray(size_t size)
struct timespec walkArray(struct walkArray * array, size_t steps) {
	struct timespec elapsed;
	walking_t * a = array->array;
	size_t idx = randMinMax(0, array->len);

	TIGHTLY_TIMED(while(steps--) { idx = a[idx]; }, elapsed);
	return elapsed;
}
