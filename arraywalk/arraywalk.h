#pragma once

#include <stdint.h>
#include <stdlib.h>

// The type of the elements of walkarrays.
typedef uint32_t walking_t;

// Walkarrays know their length.
struct walkArray {
	size_t len;
	walking_t * array;
};

// Embed a random generated, non-cyclic path in an array of given length.
struct timespec makeRandomWalkArray(size_t len, struct walkArray ** result);

// Free the walkArray structure.
void freeWalkArray(struct walkArray * array);

// Walk the array as encoded by makeRandomWalkArray(size_t len).
struct timespec walkArray(struct walkArray * array, size_t steps);
