#include "util.h"
#include "arraywalk.h"
#include "csv.h"
#include "options.h"

#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <inttypes.h>

// TODO: autodection
#define CLOCK_FREQ 2.2 

// program entry point
int main(int argc, char * argv[]) {
	struct options options = options_parse(argc, argv);
	FILE *csvlog = NULL;

	// seed random at program startup
	srand(time(NULL));

	// user wants to log to file? (csv format)
	switch (argc) {
		case 0:
		case 1:
			break;
		case 2:
			csvlog = CSV_OpenLog(argv[1]);
			break;
		default:
			fprintf(stderr, "Wrong number of arguments! Run %s either with no arguments, or with the filename to save a CSV log to as single argument.\n", *argv);
			exit(EXIT_FAILURE);
	}

	// benchmarking constants (TODO: extract to arguments of this program)
	// repeat each test instance
	const size_t repetitions = 50;
	// number of array accesses to be performed
	const size_t aaccesses = 3 * 1024 * 1024;
	// linear increment step in elements
	const size_t array_incstep = 64;
	// amount of increments to test
	size_t array_increments = 8 * 1024; // 8MiB when incstep is 1KiB

	// bookkeeping variables
	uint_least64_t timings[repetitions];
	size_t repetitions_ctr;
	struct timespec elapsed;
	uint_least64_t totalnsec;
	uint_least64_t new_avg = 0;
	uint_least64_t old_avg = 0;
	double stddev;
	size_t array_len = 0;
	struct walkArray * array;

	// print timer resolution
	clock_getres(CLOCK_MONOTONIC, &elapsed);
	printf("Timer resolution: %ld seconds, %lu nanoseconds\n", elapsed.tv_sec, elapsed.tv_nsec);

	// test for increasing cache sizes
	while((array_len += array_incstep), array_increments--) {
		// array creation (timed)
		totalnsec = 0;
		elapsed = makeRandomWalkArray(array_len, &array);
		totalnsec += 1000 * 1000 * 1000 * elapsed.tv_sec + elapsed.tv_nsec;
		if (array->size < 1024)
			printf("%.6lu B", array->size);
		else
			printf("%.6lu KiB", array->size / 1024);
		printf(" (= %lu elements) randomized in %"PRIuLEAST64" usec | %lu reads:\n",
				array->len,
				totalnsec / 1000,
				aaccesses);

		// test case warmup (helps reducing variance)
		(void) walkArray(array, aaccesses);
		// test each case 'repetions' times (timed)
		repetitions_ctr = repetitions;
		while (repetitions_ctr--) {
			elapsed = walkArray(array, aaccesses);
			timings[repetitions_ctr] = 1000 * 1000 * 1000 * elapsed.tv_sec + elapsed.tv_nsec;
		}

		// average
		totalnsec = 0;
		repetitions_ctr = repetitions;
		while (repetitions_ctr--)
			totalnsec += timings[repetitions_ctr];
		// XXX whole division should be OK: timings are in the millions of nsec
		new_avg = totalnsec / repetitions;

		// standard deviation
		totalnsec = 0;
		repetitions_ctr = repetitions;
		while (repetitions_ctr--) {
			uint_least64_t current = timings[repetitions_ctr];
			current = current > new_avg ? current - new_avg : new_avg - current;
			totalnsec += current * current;
		}
		stddev = sqrt(totalnsec / repetitions);
	
		// report results
		if (csvlog)
			CSV_LogTimings(csvlog, array, new_avg, lround(stddev));

		printf(">>>\t%"PRIuLEAST64" usec"
				" | delta %+2.2lf%% (%"PRIuLEAST64" -> %"PRIuLEAST64")"
				" | stddev %ld usec (%2.2lf%%)\n\n",
				new_avg / 1000,
				100 * ((double)new_avg - old_avg) / old_avg, old_avg, new_avg,
				lround(stddev / 1000), 100 * stddev / new_avg
				);

		// prepare for next test instance
		old_avg = new_avg;
		freeWalkArray(array);
	}

	return 0;
}
