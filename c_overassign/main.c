// Illustrates that the nul byte is chopped off a string literal in some cases.

#include <stdio.h>

int main() {
	char foo[3] = "BLA";
	char bar;

	printf("%c\n%c\n%c\n%c\n%s\n", foo[0], foo[1], foo[2], bar, foo);
};
