#pragma once

#include "arraywalk.h"

#include <stdint.h>
#include <stdio.h>

typedef long long nsec_t;
#define PRINSEC "llu"
void CSV_LogTimings(FILE * log, struct walkArray * wa, nsec_t nsec, nsec_t stddev);
