#pragma once

#include <inttypes.h>
#include <stdint.h>
#include <stdlib.h>

#ifndef WALKING_MIN_WIDTH
// default to a minimum width of 32 bits;
// assuming CHAR_BIT = 8, 16 bits would yield a maximal array size of
// 2 * 2^16 bytes = 128KiB, which is smaller than most caches today
// (32 bits should be plenty for quite a while: 4 * 2^32 bytes = 16GiB)
#define WALKING_MIN_WIDTH 32
#endif

// Configure the type of the elements of walkarrays.
#if WALKING_MIN_WIDTH == 8
typedef uint_fast8_t walking_t;
#define PRIWALKING PRIuFAST8
#define WALKING_MAX UINT_FAST8_MAX
#elif WALKING_MIN_WIDTH == 16
typedef uint_fast16_t walking_t;
#define PRIWALKING PRIuFAST16
#define WALKING_MAX UINT_FAST16_MAX
#elif WALKING_MIN_WIDTH == 32
typedef uint_fast32_t walking_t;
#define PRIWALKING PRIuFAST32
#define WALKING_MAX UINT_FAST32_MAX
#elif WALKING_MIN_WIDTH == 64
typedef uint_fast64_t walking_t;
#define PRIWALKING PRIuFAST64
#define WALKING_MAX UINT_FAST64_MAX
#else // no valid min width
#error "no valid min width for walking array type"
#endif

#ifndef NDEBUG
#if WALKING_MAX == UINT8_MAX
#pragma message "sizeof walking_t == 1"
#elif WALKING_MAX == UINT16_MAX
#pragma message "sizeof walking_t == 2"
#elif WALKING_MAX == UINT32_MAX
#pragma message "sizeof walking_t == 4"
#elif WALKING_MAX == UINT64_MAX
#pragma message "sizeof walking_t == 8"
#endif
#endif

// arithmetic operations promote their arguments to int; casting makes our
// intention explicit, when walking_t has a smaller type than int
#define WALKING_T_CAST(EXP) (walking_t)(EXP)

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
