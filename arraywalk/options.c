#include "options.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// ? h - explicit help request (prevents invalid option warning)
// (a)accesses
// (e)nd
// (l)og
// (p)rocesses
// (r)epetitions
// (s)tep
//
// (S)ilent - flag to suppress stdout output
static const char * OPTSTR = "h?a:e:l:p:r:s:S";

static const size_t AACCESSES = 4 * 1024 * 1024;
static const size_t END = 8 * 1024 * 1024;
static FILE * CSVLOG = NULL;
static const int PROCESSES = 1;
static const size_t REPETITIONS = 50;
static const size_t STEP = 4 * 1024;
static const bool SILENT = false;

static void options_help(const char * name) {
	fprintf(stderr, "Usage: %s [-a num] [-l file] [-r num] [-s num]\n", name);

	fprintf(stderr, "-a\tarray accesses to perform (default: %zd)\n", AACCESSES);
	fprintf(stderr, "-e\ttest array maximum size (default: %zd)\n", END);
	fprintf(stderr, "-l\tlog file to write CSV data to (default: <none>)\n");
	fprintf(stderr, "-p\tnumber of simultaneous tests to start (default: %u)\n", PROCESSES);
	fprintf(stderr, "-r\tnumber of times to repeat a single test instance (default: %zd)\n", REPETITIONS);
	fprintf(stderr, "-s\tincrease test array linearly by this amount (default: %zd)\n", STEP);
	fprintf(stderr, "-S\tenable silent mode: no output to stdout (default: %s)\n",
			SILENT ? "on" : "off");
}

// TODO error handling
struct options options_parse(int argc, char * argv[]) {
	int opt;

	size_t aaccesses = AACCESSES;
	size_t end = END;
	FILE * csvlog = CSVLOG;
	int processes = PROCESSES;
	size_t repetitions = REPETITIONS;
	size_t step = STEP;
	bool Silent = SILENT;

	while (-1 != (opt = getopt(argc, argv, OPTSTR))) {
		switch (opt) {
			case 'a':
				aaccesses = atoll(optarg);
				break;
			case 'e':
				end = atoll(optarg);
				break;
			case 'l':
				csvlog = fopen(optarg, "w");
				break;
			case 'p':
				processes = atoi(optarg);
				break;
			case 'r':
				repetitions = atoll(optarg);
				break;
			case 's':
				step = atoll(optarg);
				break;
			case 'S':
				Silent = true;
				break;
			default: /* '?' */
				options_help(*argv);
				exit(EXIT_FAILURE);
		}
	}

	return (struct options) {
		aaccesses,
		end,
		csvlog,
		processes,
		repetitions,
		step,
		Silent
	};
}
