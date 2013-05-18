#include "util.h"
#include "arraywalk.h"
#include "csv.h"
#include "options.h"

#include <errno.h>
#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <inttypes.h>

// TODO: autodection
#define CLOCK_FREQ 2.2 

// user reporting
void verbose(struct options * options, const char *format, ...) {
	va_list args;
	va_start(args, format);

	if(!options->Silent)
		vprintf(format, args);

	va_end(args);
}

// program entry point
int main(int argc, char * argv[]) {
	struct options options = options_parse(argc, argv);

	// seed random at program startup
	srand(time(NULL));

	// bookkeeping variables
	uint_least64_t timings[options.repetitions];
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
	verbose(&options, "Timer resolution: %ld seconds, %lu nanoseconds\n",
			elapsed.tv_sec, elapsed.tv_nsec);

	// test for increasing cache sizes
	while((array_len += options.step) <= options.end) {
		// array creation (timed)
		totalnsec = 0;
		elapsed = makeRandomWalkArray(array_len, &array);
		totalnsec += 1000 * 1000 * 1000 * elapsed.tv_sec + elapsed.tv_nsec;
		if (array->size < 1024)
			verbose(&options, "%.6lu B", array->size);
		else
			verbose(&options, "%.6lu KiB", array->size / 1024);

		verbose(&options, " (= %lu elements) randomized in %"PRIuLEAST64" usec | %lu reads:\n",
				array->len,
				totalnsec / 1000,
				options.aaccesses);

		// test case warmup (helps reducing variance)
		(void) walkArray(array, options.aaccesses);
		// test each case 'repetions' times (timed)
		repetitions_ctr = options.repetitions;
		while (repetitions_ctr--) {
			elapsed = walkArray(array, options.aaccesses);
			timings[repetitions_ctr] = 1000 * 1000 * 1000 * elapsed.tv_sec + elapsed.tv_nsec;
		}

		// average
		totalnsec = 0;
		repetitions_ctr = options.repetitions;
		while (repetitions_ctr--)
			totalnsec += timings[repetitions_ctr];
		// XXX whole division should be OK: timings are in the millions of nsec
		new_avg = totalnsec / options.repetitions;

		// standard deviation
		totalnsec = 0;
		repetitions_ctr = options.repetitions;
		while (repetitions_ctr--) {
			uint_least64_t current = timings[repetitions_ctr];
			current = current > new_avg ? current - new_avg : new_avg - current;
			totalnsec += current * current;
		}
		stddev = sqrt(totalnsec / options.repetitions);

		// report results
		if (options.csvlog)
			CSV_LogTimings(options.csvlog, array, new_avg, lround(stddev));

		verbose(&options, ">>>\t%"PRIuLEAST64" usec"
				" | delta %+2.2lf%% (%"PRIuLEAST64" -> %"PRIuLEAST64")"
				" | stddev %ld usec (%2.2lf%%)\n\n",
				new_avg / 1000,
				100 * ((double)new_avg - old_avg) / old_avg, old_avg, new_avg,
				lround(stddev / 1000), 100 * stddev / new_avg
				);

		// inform user in time about every iteration
		fflush(stdout);

		// prepare for next test instance
		old_avg = new_avg;
		freeWalkArray(array);
	}

	return 0;
}
