#include "util.h"

#include <stdlib.h>

// return a random number ranging from min to max inclusive
inline unsigned randMinMax(unsigned min, unsigned max) {
	unsigned ret = min + abs(rand()) % ((max + 1) - min);
#ifdef DEBUG
	printf("DEBUG: randMinMax(%u, %u) -> %u\n", min, max, ret);
#endif // #ifdef DEBUG
	return ret;
}
