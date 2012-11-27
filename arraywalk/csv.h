#pragma once

#include "arraywalk.h"

#include <stdint.h>
#include <stdio.h>

typedef uint_least64_t nsec_t;
void CSV_LogTimings(FILE * log, struct walkArray * wa, nsec_t nsec, nsec_t stddev);
