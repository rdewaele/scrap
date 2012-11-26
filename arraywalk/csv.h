#pragma once

#include <stdint.h>
#include <stdio.h>

FILE * CSV_OpenLog(const char * filename);
void CSV_LogTimings(FILE * log, size_t sz, uint_least64_t nsec, uint_least64_t stddev);
