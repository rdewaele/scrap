// Illustrates that the nul byte is chopped off a string literal in some cases.
// see http://c-faq.com/ansi/nonstrings.html

/* Output on my machine:
 * -----
 * B
 * L
 * A
 *
 * BLA_
 * -----
 * (followed by some non-printable characters)
 */

#include <stdio.h>

int main() {
	char foo[3] = "BLA";
	char bar;

	printf("%c\n%c\n%c\n%c\n%s\n", foo[0], foo[1], foo[2], bar, foo);
};
