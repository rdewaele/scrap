#pragma once

#include "arraywalk.h"

#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

typedef long long nsec_t;
#define PRINSEC "llu"
void CSV_LogTimings(FILE * log, pid_t pid, struct walkArray * wa, nsec_t nsec, nsec_t stddev);
