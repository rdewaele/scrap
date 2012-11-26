#include "util.h"
#include "arraywalk.h"

#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <inttypes.h>

// TODO: autodection
#define CLOCK_FREQ 2.2 

// forward declarations
int main();
void reportTimings(size_t cache_sz, struct timespec * start, struct timespec * stop);
FILE * openCSVlog(const char * filename);


// program entry point
int main(int argc, char * argv[]) {
	FILE *csvlog = NULL;

	// seed random at program startup
	srand(time(NULL));

	// user wants to log to file? (csv format)
	switch (argc) {
		case 0:
		case 1:
			break;
		case 2:
			csvlog = openCSVlog(argv[argc]);
			break;
		default:
			fprintf(stderr, "Wrong number of arguments! Run %s either with no arguments, or with the filename to save a CSV log to as single argument.\n", *argv);
			exit(EXIT_FAILURE);
	}

	// benchmarking constants (TODO: extract to arguments of this program)
	// repeat each test instance
	const size_t repetitions = 20;
	// number of array accesses to be performed
	const size_t aaccesses = 1000 * 1000;
	// linear increment step
	const size_t array_incstep = 512;
	// amount of increments to test
	size_t array_increments = 8 * 1024; // 8MiB when incstep is 1KiB

	// bookkeeping variables
	uint_least64_t timings[repetitions];
	size_t repetitions_ctr;
	struct timespec elapsed;
	uint_least64_t totalnsec;
	uint_least64_t new_avg = 0;
	uint_least64_t old_avg = 0;
	double delta;
	double stddev;
	size_t array_sz = 0;
	struct walkArray * array;

	// print timer resolution
	clock_getres(CLOCK_MONOTONIC, &elapsed);
	printf("Timer resolution: %ld seconds, %lu nanoseconds\n", elapsed.tv_sec, elapsed.tv_nsec);

	// test for increasing cache sizes
	while((array_sz += array_incstep), array_increments--) {
		// array creation (timed)
		totalnsec = 0;
		elapsed = makeRandomWalkArray(array_sz, &array);
		totalnsec += 1000 * 1000 * 1000 * elapsed.tv_sec + elapsed.tv_nsec;
		printf("%.6lu KiB (= %lu elements) randomized in %"PRIuLEAST64" usec | %lu reads:\n",
				array_sz / 1024, array_sz / sizeof(walking_t), totalnsec / 1000, aaccesses);

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
	
		// new average versus old average delta
		delta = 100.0 * ((double)new_avg - (double)old_avg) / (double)old_avg;

		// report results
		printf(">>>\t%"PRIuLEAST64" usec | delta %+2.2lf%% (%"PRIuLEAST64" -> %"PRIuLEAST64") | stddev %ld usec (%2.2lf%%)\n\n",
				new_avg / 1000, delta, old_avg, new_avg, lround(stddev / 1000), 100 * stddev / new_avg);

		// prepare for next test instance
		old_avg = new_avg;
		freeWalkArray(array);
	}

	return 0;
}

// open a file for appending 
// TODO: complete this feature
FILE * openCSVlog(const char * filename) {
	// error handling
	const char * errprefix = "Error opening CSV log";
	// +2 for a space character and the null terminator
	char * errstring = malloc(strlen(errprefix) + strlen(filename) + 2);
	sprintf(errstring, "%s %s", errprefix, filename);

	// open the file
	FILE * log = fopen(filename, "a+b");
	if (!log)
		perror(errstring);
	else
		printf("Writing CSV formatted data to %s\n", filename);

	free(errstring);
	return log;
}
