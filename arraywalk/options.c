#include "options.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>

// ? h - explicit help request (prevents invalid option warning)
// (a)accesses
// (c)reate method
// (e)nd
// (l)og
// (p)rocesses
// (r)epetitions
// (s)tep
//
// (S)ilent - flag to suppress stdout output
//
// (I)nformation about run configuration
// -> this is a meta option, and not present in the option structure
static const char * OPTSTR = "h?a:c:e:f:Il:p:r:s:S";

// 'end' default must (only) be lowered when it doesn't fit in walking_t
#if WALKING_MAX >= 1 << 23
#define END_INIT 1 << 23
#elif WALKING_MAX >= ((1 << 16) - 1)
#define END_INIT ((1 << 16) - 1)
#else // < 2^16
#define END_INIT ((1 << 8) - 1)
#endif

// 'step' default must (only) be lowered when it doesn't fit in walking_t
#if WALKING_MAX >= 1 << 12
#define STEP_INIT 1 << 12
#else // < 2^12
#define STEP_INIT 1 << 4
#endif

static const unsigned AACCESSES = 4 * 1024 * 1024;
static const enum spawn_type CREATE = TREE;
static const walking_t END = END_INIT;
static const float FREQUENCY = 1;
static char * CSVLOGNAME = NULL;
static const unsigned PROCESSES = 1;
static const unsigned REPETITIONS = 50;
static const walking_t STEP = STEP_INIT;
static const bool SILENT = false;

static void options_help(const char * name) {
	fprintf(stderr, "Usage: %s [-a num] [-c {tree,linear}] [-e num] [-l file]"
			" [-p num] [-r num] [-s num] [-S] [-I]\n", name);

	fprintf(stderr, "-a\tarray accesses to perform (default: %u)\n", AACCESSES);
	fprintf(stderr, "-c\tthread spawn method (default: %s)\n", spawn_typeToString(CREATE));
	fprintf(stderr, "-e\tmaximum array size (default: %"PRIWALKING")\n", END);
	fprintf(stderr, "-f\tcpu clock frequency to base cycle calculations on (default: %.3f GHz)\n", FREQUENCY);
	fprintf(stderr, "-I\tprint run configuration to stdout at program startup\n");
	fprintf(stderr, "-l\tCSV log basic filename (process id's will be appended) (default: %s)\n",
			CSVLOGNAME ? CSVLOGNAME : "<none>");
	fprintf(stderr, "-p\tnumber of simultaneous tests to start (default: %u)\n", PROCESSES);
	fprintf(stderr, "-r\tnumber of times to repeat a single test instance (default: %u)\n", REPETITIONS);
	fprintf(stderr, "-s\tincrease test array linearly by this amount (default: %"PRIWALKING")\n", STEP);
	fprintf(stderr, "-S\tenable silent mode: no output to stdout (default: %s)\n",
			SILENT ? "on" : "off");
}

static void options_print(const struct options * options) {
	fprintf(stderr,
			"array accesses to perform: %u\n"
			"thread spawn method: %s\n"
			"maximum array size: %"PRIWALKING"\n"
			"cpu clock frequency: %.3f\n"
			"CSV log base name: %s\n"
			"operating threads: %u\n"
			"single test configuration repeat: %u\n"
			"array increment: %"PRIWALKING"\n"
			"silent mode: %s\n"
			"\n",
			options->aaccesses,
			spawn_typeToString(options->create),
			options->end,
			options->frequency,
			options->csvlogname ? options->csvlogname : "<no csv logging>",
			options->processes,
			options->repetitions,
			options->step,
			options->Silent ? "on" : "off"
			);
}

void options_free(struct options * options) {
	if (options->csvlogname && options->csvlogname != CSVLOGNAME) {
		free(options->csvlogname);
		options->csvlogname = NULL;
	}

	if (options->csvlog) {
		fclose(options->csvlog);
		options->csvlog = NULL;
	}
}

// TODO error handling
struct options options_parse(int argc, char * argv[]) {
	int opt;

	unsigned aaccesses = AACCESSES;
	enum spawn_type create = CREATE;
	walking_t end = END;
	float frequency = FREQUENCY;
	char * csvlogname = CSVLOGNAME;
	unsigned processes = PROCESSES;
	unsigned repetitions = REPETITIONS;
	walking_t step = STEP;
	bool Silent = SILENT;

	bool print_configuration = false;

	while (-1 != (opt = getopt(argc, argv, OPTSTR))) {
		switch (opt) {
			case 'a':
				aaccesses = (unsigned)atol(optarg);
				break;
			case 'c':
				create = spawn_typeFromString(optarg);
				break;
			case 'e':
				end = (walking_t)atoll(optarg);
				break;
			case 'f':
				frequency = (float)atof(optarg);
				break;
			case 'I':
				print_configuration = true;
				break;
			case 'l':
				csvlogname = strdup(optarg);
				break;
			case 'p':
				processes = (unsigned)atol(optarg);
				break;
			case 'r':
				repetitions = (unsigned)atol(optarg);
				break;
			case 's':
				step = (walking_t)atoll(optarg);
				break;
			case 'S':
				Silent = true;
				break;
			default: /* '?' */
				options_help(*argv);
				exit(EXIT_FAILURE);
		}
	}

	struct options options = {
		aaccesses,
		create,
		end,
		frequency,
		csvlogname,
		NULL,
		processes,
		repetitions,
		step,
		Silent
	};

	if (print_configuration)
		options_print(&options);

	return options;
}

#define CASE_ENUM2STRING(ENUM) case ENUM: return #ENUM
const char * spawn_typeToString(enum spawn_type st) {
	switch (st) {
		CASE_ENUM2STRING(TREE);
		CASE_ENUM2STRING(LINEAR);
		default:
		return NULL;
	}
}

#define STRING2ENUM(SRC,ENUM) if (0 == strcasecmp(#ENUM, SRC)) return ENUM
enum spawn_type spawn_typeFromString(const char * st) {
	STRING2ENUM(st, TREE);
	STRING2ENUM(st, LINEAR);
	return CREATE;
}
