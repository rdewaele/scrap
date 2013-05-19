#pragma once

#include <inttypes.h>
#include <stdint.h>
#include <stdlib.h>

// The type of the elements of walkarrays.
typedef uint32_t walking_t;
#define PRIWALKING PRIu32

// Walkarrays know their length. (And for convenience, their size in bytes.)
struct walkArray {
	walking_t len;
	size_t size;
	walking_t * array;
};

// Embed a random generated, non-cyclic path in an array of given length.
struct timespec makeRandomWalkArray(walking_t len, struct walkArray ** result);

// Free the walkArray structure.
void freeWalkArray(struct walkArray * array);

// Walk the array as encoded by makeRandomWalkArray(size_t len).
struct timespec walkArray(struct walkArray * array, size_t steps);
