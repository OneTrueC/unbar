/* general utility functions
 * vim: set noet tw=80 ts=4: */
#define _POSIX_C_SOURCE 200809L

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

void
die(int err, const char* msg, ...)
{
	va_list ap;

	fprintf(stderr, PRGNAME": ");
	va_start(ap, msg);
	vfprintf(stderr, msg, ap);
	va_end(ap);
	fputs("\n", stderr);

	exit(err);
}
