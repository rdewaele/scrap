#include "csv.h"

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>

void CSV_LogTimings(FILE * log, pid_t pid, struct walkArray * wa, nsec_t nsec, nsec_t stddev) {
	fprintf(log,
			"%lld,%zu,%llu,%llu\n",
			(long long)pid, wa->size, nsec, stddev);
}
