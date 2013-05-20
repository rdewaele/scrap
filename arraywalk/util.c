#include "util.h"
#include "arraywalk.h"

#include <stdlib.h>

#ifndef NDEBUG
#include <stdio.h>
#endif

// return a random number ranging from min to max inclusive
inline walking_t randMinMax(walking_t min, walking_t max) {
	walking_t ret = WALKING_T_CAST(min + (walking_t)abs(rand()) % ((max + 1) - min));
#ifndef NDEBUG
	printf("DEBUG: randMinMax(%"PRIWALKING", %"PRIWALKING")"
			" -> %"PRIWALKING"\n",
			min, max, ret);
#endif // #ifndef NDEBUG
	return ret;
}
