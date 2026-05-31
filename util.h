/* general utility functions
 * vim: set noet tw=80 ts=4: */
#define _POSIX_C_SOURCE 200809L

#ifndef _UTIL_H
#define _UTIL_H

#define DOOM die(4, "Out of Memory")

#define M_PI 3.14159265358979323846

enum SIDE {
	TOP,
	BOTTOM,
	LEFT,
	RIGHT
};

void die(int err, const char* msg, ...) __attribute__((noreturn));

#endif /* _UTIL_H */
