/* C quickstart main file */
#define _POSIX_C_SOURCE 200809L

#include <stdio.h>

#include "api.h"

void myfunc(void);

int
plugin_main(void)
{
	myfunc();
	testfunc();

	return 0;
}

void
myfunc(void)
{
	printf("this is a function from the plugin\n");
}
