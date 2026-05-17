/* general utility functions */
#define _POSIX_C_SOURCE 200809L

#ifndef _UTIL_H
#define _UTIL_H

enum SIDE {
	TOP,
	BOTTOM,
	LEFT,
	RIGHT
};

void die(int err, const char* msg, ...);

#endif /* _UTIL_H */
