#include "csv.h"

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>

void CSV_LogTimings(FILE * log, struct walkArray * wa, nsec_t nsec, nsec_t stddev) {
	fprintf(log,
			"%zu,%"PRIuLEAST64",%"PRIuLEAST64"\n",
			wa->size, nsec, stddev);
}
