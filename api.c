/* api function definitions
 * vim: set noet tw=80 ts=4: */
#define _POSIX_C_SOURCE 200809L

#include <stdio.h>

#define HOST
#include "api.h"
#undef HOST

int
testfunc(void)
{
	printf("this is a function from the host program\n");

	return 0;
}
