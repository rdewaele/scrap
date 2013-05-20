#include "arraywalk.h"
#include "util.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#ifdef TIGHTER_TIMED // less error-checking
#define TIGHTLY_TIMED(bench_code, time_var) \
	struct timespec start, stop; \
	(void)clock_gettime(CLOCK_MONOTONIC, &start); \
	bench_code \
	(void)clock_gettime(CLOCK_MONOTONIC, &stop); \
	time_var = (struct timespec){ stop.tv_sec - start.tv_sec, stop.tv_nsec - start.tv_nsec }
#else
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
#endif

// Create a cycle using Sattolo's algorithm. This is the way the array will be
// traversed in the benchmarks.
// TODO: check return values (malloc, ..)
// TODO: size may overflow if len > (walking_t_max / sizeof(walking_t))
struct timespec makeRandomWalkArray(walking_t len, struct walkArray ** result) {
	struct timespec elapsed;
	walking_t size = (walking_t)sizeof(walking_t) * len;
	walking_t * array = malloc(size);
	walking_t i;

	// initialization step encodes index as values
	for (i = 0; i < len; ++i)
		array[i] = i;

	// shuffle the array
	// *boundaries voodoo:
	//  - for(): i goes up to the penultimate element instead of the last element
	//    because element i is swapped with an element > i
	//  - randMinMax(): the element to be swapped is
	//    one of the elements following element i
	walking_t rnd;
	walking_t swap;
	TIGHTLY_TIMED(
	for (i = 0; i < len - 1; ++i) {
		rnd = randMinMax(WALKING_T_CAST(i + 1), WALKING_T_CAST(len - 1));
		swap = array[i];
		array[i] = array[rnd];
		array[rnd] = swap;
	},
	elapsed);

	*result = malloc(sizeof(struct walkArray));
	(*result)->array = array;
	(*result)->size = size;
	(*result)->len = len;

	return elapsed;
}

// Free the walkArray structure.
void freeWalkArray(struct walkArray * array) {
	free(array->array);
	free(array);
}

// walk the array as encoded by makeRandomWalkArray(size_t size)
walking_t walkArray(struct walkArray * array, size_t steps, struct timespec * elapsed) {
	walking_t * a = array->array;
	// XXX do no just make idx volatile to prevent code omission by compiler
	// optimizations: having idx volatile means that idx must be written to,
	// which we don't care about; we only want <reads>
	// OTOH, not having to write may enable pipelined solutions that resilt in
	// optimistic results, which is also not okay
	// thus far, not enabling volatile and compiling with -O0 (gcc) results in
	// expected timings on my develpment machine, as they are documented by intel

	// return final idx to caller to prevent code omission by compiler
#if VOLATILE
	volatile walking_t idx;
#else
	register walking_t idx;
#endif
	idx = randMinMax(0, WALKING_T_CAST(array->len - 1));
	TIGHTLY_TIMED(while(steps--) { idx = a[idx]; }, (*elapsed));
	return idx;
}
