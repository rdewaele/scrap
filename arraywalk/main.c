#include <errno.h>
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <inttypes.h>

int main();
void reportTimings(size_t cache_sz, struct timespec * start, struct timespec * stop);
inline unsigned randMinMax(unsigned minimum, unsigned maximum);
uint32_t * makeRandomWalkArray(size_t size);
void walkArray(uint32_t * array, size_t size, size_t steps);

int main() {
	// seed random at program startup
	srand(time(NULL));

	const size_t repetitions = 50;
	const size_t aaccesses = 1000 * 1000;
	size_t repetitions_ctr;
	struct timespec start;
	struct timespec stop;
	uint_least64_t totalnsec;
	uint_least64_t old_avg = 0;
	uint_least64_t new_avg = 0;
	double delta;
	size_t array_sz = 0;
	size_t array_increments = 1024; // 1KiB steps
	uint32_t * array;

	// print timer resolution
	clock_getres(CLOCK_MONOTONIC, &start);
	printf("Timer resolution: %ld seconds, %lu nanoseconds\n", start.tv_sec, start.tv_nsec);

	// test for increasing cache sizes
	while((array_sz += array_increments) <= (6 * 1024 * 1024)) {
		totalnsec = 0;
		clock_gettime(CLOCK_MONOTONIC, & start);
		array = makeRandomWalkArray(array_sz);
		clock_gettime(CLOCK_MONOTONIC, & stop);
		totalnsec += (1000 * 1000 * 1000) * (stop.tv_sec - start.tv_sec);
		totalnsec += (stop.tv_nsec - start.tv_nsec);
		printf("%.6lu KiB randomized in %"PRIuLEAST64" usec (testing %lu reads)\n", array_sz / 1024, totalnsec / 1000, aaccesses);

		// test each case thouroughly and calculate an average and a standard deviation
		repetitions_ctr = repetitions;
		totalnsec = 0;
		while (repetitions_ctr--) {
			if (clock_gettime(CLOCK_MONOTONIC, &start))
				perror("clock_gettime");

			walkArray(array, array_sz / 4, aaccesses);

			if (clock_gettime(CLOCK_MONOTONIC, &stop))
				perror("clock_settime");

			if (stop.tv_sec < start.tv_sec)
				fprintf(stderr, "TIMING ERROR: This experiment started SECONDS in the future ...\n");
			else if (stop.tv_sec == start.tv_sec && stop.tv_nsec < start.tv_nsec)
				fprintf(stderr, "TIMING ERROR: This experiment started NANOSECONDS in the future ...\n");

			totalnsec += (1000 * 1000 * 1000) * (stop.tv_sec - start.tv_sec);
			totalnsec += (stop.tv_nsec - start.tv_nsec);
		}
		new_avg = totalnsec / repetitions;
		// XXX upon first iteration this will divide by zero - this is OK, but we
		// depend ond IEEE _float_ semantics, i.e. dividing by zero is either
		// +inf, -inf or NaN
		delta = 100 * ((double)new_avg - (double)old_avg) / (double)old_avg;
		printf(">>> %"PRIuLEAST64" usec | delta %+2.2lf\% (%"PRIuLEAST64" -> %"PRIuLEAST64")\n\n", new_avg / 1000, delta, old_avg, new_avg);
		old_avg = new_avg;

		//reportTimings(array_sz, &start, &stop);
		free(array);
	}

	return 0;
}

void reportTimings(size_t cache_sz, struct timespec * start, struct timespec * stop) {
	time_t seconds = stop->tv_sec - start->tv_sec;
	long nanoseconds = stop->tv_nsec - start->tv_nsec;
	printf("%lu KiB:\t%ld sec, %lu nsec\n", cache_sz / 1024, seconds, nanoseconds);
}

// return a random number ranging from min to max inclusive
inline unsigned randMinMax(unsigned min, unsigned max) {
	unsigned ret = min + abs(rand()) % ((max + 1) - min);
#ifdef DEBUG
	printf("DEBUG: randMinMax(%u, %u) -> %u\n", min, max, ret);
#endif // #ifdef DEBUG
	return ret;
}

// Create a cycle using Sattolo's algorithm. This is the way the array will be
// traversed in the benchmarks.
uint32_t * makeRandomWalkArray(size_t size) {
	uint32_t * array = malloc(sizeof(uint32_t) * size);
	size_t i;

	// initialization step encodes index as values
	for (i = 0; i < size; ++i)
		array[i] = i;

	// shuffle the array
	// *boundaries voodoo:
	//  - for(): i goes up to the penultimate element instead of the last element
	//    because element i is swapped with an element > i
	//  - randMinMax(): the element to be swapped is
	//    one of the elements following element i
	unsigned rnd;
	unsigned swap;
	for (i = 0; i < size - 1; ++i) {
		rnd = randMinMax(i + 1, size - 1);
		swap = array[i];
		array[i] = array[rnd];
		array[rnd] = swap;
	}

	return array;
}

// walk the array as encoded by makeRandomWalkArray(size_t size)
void walkArray(uint32_t * a, size_t sz, size_t steps) {
	size_t idx = randMinMax(0, sz);
	while(steps--)
		idx = a[idx];
}
