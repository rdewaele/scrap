#pragma once

#include <stdio.h>

struct options {
	// repeats for each test case (lowers stddev)
	const size_t repetitions;
	// amount of array accesses to perform (improves bench accuracy)
	const size_t aaccesses;
	// the test array grows linearly by this amount (determines granularity)
	const size_t step;
	// file to write Comma Separated Values to
	const FILE * csvlog;
};

struct options options_parse(int argc, char * argv[]);
