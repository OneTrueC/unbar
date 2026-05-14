/* api function definitions */
#define _POSIX_C_SOURCE 200809L

#include <stdio.h>

int
testfunc(void)
{
	printf("this is a function from the host program\n");

	return 0;
}
