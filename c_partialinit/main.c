// Illustrates partial initialization of an array.

/* Output on my machine:
 * -----
 * ./main 
 * 1 2 0 0 0 
 * -----
 */

#include <stdio.h>

int main() {
	int foo[5] = {1,2};
	int i = 0;

	for (; i < sizeof(foo) / sizeof(int); ++i)
		printf("%d ", foo[i]);
	printf("\n");
};
