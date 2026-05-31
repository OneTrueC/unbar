/* ws types that can be used across different h files
 * vim: set noet tw=80 ts=4: */
#define _POSIX_C_SOURCE 200809L

#ifndef _WS_TYPES_H
#define _WS_TYPES_H

/* the WindowCtx interface and all relevant functions are defined per window
 * system, (i.e. x.c and y.c define all the same functions with identical
 * interfaces but the context is entirely different. */
struct WindowCtx;
typedef struct WindowCtx WindowCtx;

/* struct that will abstract the relevant things for drawing, one per block on
 * the statusbar */
struct DrawCtx;
typedef struct DrawCtx DrawCtx;

typedef unsigned long Color;

typedef struct {
	int x;
	int y;
} Point;

/* this is probably the best way to deal with fonts */
struct FontObj;
typedef struct FontObj FontObj;

#endif /* _WS_TYPES_H */
