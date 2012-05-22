#include <stdio.h>
#include <stdlib.h>

int main(int, char**);
int getnumber(char*);
unsigned gcd(unsigned, unsigned);

int main(int argc, char * argv[]) {
	unsigned a = 0;
	unsigned b = 0;
	switch (argc) {
		case 1:
			a = getnumber("Enter first number: ");
			b = getnumber("Enter second number: ");
			break;
		case 3:
			a = atoi(argv[1]);
			b = atoi(argv[2]);
			break;
		default:
			fprintf(stderr, "Either run %s without arguments, or with two numbers as arguments.\n", argv[0]);
			return -1;
	}
	printf("The GCD of %u and %u is %u\n.", a, b, gcd(a, b));
	return 0;
}

int getnumber(char * prompt) {
	char * buffer = NULL;
	size_t length = 0;
	printf("%s", prompt);
	getline(&buffer, &length /* ignored */, stdin);
	return atoi(buffer);
}

unsigned gcd(unsigned a, unsigned b) {
	unsigned temp;
	// make sure a > b
	if (a < b)
		temp = a, a = b, b = temp;
	// when b is zero, a will be the gcd
	while (b > 0) {
		printf("GCD(%u,%u)\n", a, b);
		temp = b;
		// subtract until the rest is found
		while (a >= b)
			a -= b;
		// continue calculation
		b = a;
		a = temp;
	}
	printf("GCD(%u,%u)\n", a, b);
	return a;
}
