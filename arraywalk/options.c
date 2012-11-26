#include "options.h"

#include <stdio.h>
#include <unistd.h>

// (a)accesses
// (l)og
// (r)epetitions
// (s)tep
#define OPTSTR "a:l:r:s:"

struct options options_parse(int argc, char * argv[]) {
	int opt;
	
	size_t repetitions = 50;
	size_t aaccesses = 4 * 1024 * 1024;
	size_t step = 4 * 1024;
	FILE * csvlog = NULL;

	while (-1 != (opt = getopt(argc, argv, OPTSTR))) {
		printf("option: %c\n", opt);
	}

	return (struct options){repetitions, aaccesses, step, csvlog};
}
