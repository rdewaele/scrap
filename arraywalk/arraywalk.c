#include "arraywalk.h"
#include "util.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TIGHTLY_TIMED(bench_code, time_var) \
	struct timespec start, stop; \
	/* start the clock*/ \
	if (clock_gettime(CLOCK_MONOTONIC, &start)) \
		perror("clock_gettime"); \
	/* run the benchmark */ \
	bench_code \
	/* stop the clock */ \
	if (clock_gettime(CLOCK_MONOTONIC, &stop)) \
		perror("clock_settime"); \
	/* check result sanity */ \
	if (stop.tv_sec < start.tv_sec) \
		fprintf(stderr, "TIMING ERROR: This experiment started SECONDS in the future ...\n"); \
	else if (stop.tv_sec == start.tv_sec && stop.tv_nsec < start.tv_nsec) \
		fprintf(stderr, "TIMING ERROR: This experiment started NANOSECONDS in the future ...\n"); \
	/* report timing */ \
	time_var = (struct timespec){ stop.tv_sec - start.tv_sec, stop.tv_nsec - start.tv_nsec }

// Create a cycle using Sattolo's algorithm. This is the way the array will be
// traversed in the benchmarks.
// TODO: check return values (malloc, ..)
struct timespec makeRandomWalkArray(size_t len, struct walkArray ** result) {
	struct timespec elapsed;
	walking_t * array = malloc(sizeof(walking_t) * len);
	size_t i;

	// initialization step encodes index as values
	for (i = 0; i < len; ++i)
		array[i] = i;

	// shuffle the array
	// *boundaries voodoo:
	//  - for(): i goes up to the penultimate element instead of the last element
	//    because element i is swapped with an element > i
	//  - randMinMax(): the element to be swapped is
	//    one of the elements following element i
	size_t rnd;
	size_t swap;
	TIGHTLY_TIMED(
	for (i = 0; i < len - 1; ++i) {
		rnd = randMinMax(i + 1, len - 1);
		swap = array[i];
		array[i] = array[rnd];
		array[rnd] = swap;
	},
	elapsed);

	*result = malloc(sizeof(struct walkArray));
	(*result)->array = array;
	(*result)->len = len;

	return elapsed;
}

// Free the walkArray structure.
void freeWalkArray(struct walkArray * array) {
	free(array->array);
	free(array);
}

// walk the array as encoded by makeRandomWalkArray(size_t size)
struct timespec walkArray(struct walkArray * array, size_t steps) {
	struct timespec elapsed;
	walking_t * a = array->array;
	// make idx volatile to deny code emission by compiler optimizations
	volatile size_t idx = randMinMax(0, array->len - 1);

	TIGHTLY_TIMED(while(steps--) { idx = a[idx]; }, elapsed);
	/* On my system, gcc -03 generates the following code for the hot loop:
	 *
	 * 400fd0:       48 8b 54 24 38          mov    0x38(%rsp),%rdx
	 * 400fd5:       48 83 e8 01             sub    $0x1,%rax
	 * 400fd9:       48 83 f8 ff             cmp    $0xffffffffffffffff,%rax
	 * 400fdd:       8b 14 93                mov    (%rbx,%rdx,4),%edx
	 * 400fe0:       48 89 54 24 38          mov    %rdx,0x38(%rsp)
	 * 400fe5:       75 e9                   jne    400fd0 <walkArray+0x40>
	 *
	 * We can see that half of this code is loop administration!
	 *
	 * Again on my system, gcc -03 -funroll-loops generates the following code
	 * for the hot loop:
	 *
	 * 401582:       48 8b 44 24 38          mov    0x38(%rsp),%rax
	 * 401587:       49 83 e9 08             sub    $0x8,%r9
	 * 40158b:       49 83 f9 ff             cmp    $0xffffffffffffffff,%r9
	 * 40158f:       8b 14 83                mov    (%rbx,%rax,4),%edx
	 * 401592:       48 89 54 24 38          mov    %rdx,0x38(%rsp)
	 * 401597:       48 8b 4c 24 38          mov    0x38(%rsp),%rcx
	 * 40159c:       8b 34 8b                mov    (%rbx,%rcx,4),%esi
	 * 40159f:       48 89 74 24 38          mov    %rsi,0x38(%rsp)
	 * 4015a4:       48 8b 7c 24 38          mov    0x38(%rsp),%rdi
	 * 4015a9:       44 8b 04 bb             mov    (%rbx,%rdi,4),%r8d
	 * 4015ad:       4c 89 44 24 38          mov    %r8,0x38(%rsp)
	 * 4015b2:       4c 8b 54 24 38          mov    0x38(%rsp),%r10
	 * 4015b7:       46 8b 1c 93             mov    (%rbx,%r10,4),%r11d
	 * 4015bb:       4c 89 5c 24 38          mov    %r11,0x38(%rsp)
	 * 4015c0:       48 8b 6c 24 38          mov    0x38(%rsp),%rbp
	 * 4015c5:       8b 04 ab                mov    (%rbx,%rbp,4),%eax
	 * 4015c8:       48 89 44 24 38          mov    %rax,0x38(%rsp)
	 * 4015cd:       48 8b 54 24 38          mov    0x38(%rsp),%rdx
	 * 4015d2:       8b 0c 93                mov    (%rbx,%rdx,4),%ecx
	 * 4015d5:       48 89 4c 24 38          mov    %rcx,0x38(%rsp)
	 * 4015da:       48 8b 74 24 38          mov    0x38(%rsp),%rsi
	 * 4015df:       8b 3c b3                mov    (%rbx,%rsi,4),%edi
	 * 4015e2:       48 89 7c 24 38          mov    %rdi,0x38(%rsp)
	 * 4015e7:       4c 8b 44 24 38          mov    0x38(%rsp),%r8
	 * 4015ec:       46 8b 14 83             mov    (%rbx,%r8,4),%r10d
	 * 4015f0:       4c 89 54 24 38          mov    %r10,0x38(%rsp)
	 * 4015f5:       75 8b                   jne    401582 <walkArray+0x112>
	 *
	 * As we can see, the loop administration overhead is now a lot smaller.
	 * This did not result in a measurable improvement however.
	 */
	return elapsed;
}
