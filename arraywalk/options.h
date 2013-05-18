#pragma once

#include <stdbool.h>
#include <stdio.h>

struct options {
	// amount of array accesses to perform (improves bench accuracy)
	const size_t aaccesses;
	// upper size limit for the benchmark's array
	const size_t end;
	// file to write Comma Separated Values to
	FILE * csvlog;
	// number of processes to run in parallel
	const int processes;
	// repeats for each test case (increases test significance)
	const size_t repetitions;
	// the test array grows linearly by this amount (determines granularity)
	const size_t step;
	// silent mode
	const bool Silent;
};

struct options options_parse(int argc, char * argv[]);
