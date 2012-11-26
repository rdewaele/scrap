#include "csv.h"

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// open a file for appending 
// TODO: complete this feature
FILE * CSV_OpenLog(const char * filename) {
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

void CSV_LogTimings(FILE * log, size_t sz, uint_least64_t nsec, uint_least64_t stddev) {
	fprintf(log, "%zu,%"PRIuLEAST64",%"PRIuLEAST64"\n", sz, nsec, stddev);
}