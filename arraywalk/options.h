#pragma once

#include "arraywalk.h"

#include <stdbool.h>
#include <stdio.h>

enum spawn_type {LINEAR, TREE};

const char * spawn_typeToString(enum spawn_type st);
enum spawn_type spawn_typeFromString(const char * st);

struct options {
	// amount of array accesses to perform (improves bench accuracy)
	const unsigned aaccesses;
	// thread Creation
	const enum spawn_type create;
	// upper size limit for the benchmark's array
	const walking_t end;
	// file to write Comma Separated Values to
	FILE * csvlog;
	// number of processes to run in parallel
	const unsigned processes;
	// repeats for each test case (increases test significance)
	const unsigned repetitions;
	// the test array grows linearly by this amount (determines granularity)
	const walking_t step;
	// silent mode
	const bool Silent;
};

struct options options_parse(int argc, char * argv[]);
