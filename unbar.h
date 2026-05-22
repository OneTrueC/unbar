/* unbar centric suchlikes */
#define _POSIX_C_SOURCE 200809L

#ifndef _UNBAR_H
#define _UNBAR_H

#include "ws-types.h"

//@TODO: uthash these 2 structs to go into a hash table

typedef struct {
	//@TODO
	void (*t)(void);
} Widget;

typedef struct {
	//@TODO
	void (*t)(void);
} Source;

typedef struct {
	Widget widget;
	Source source;
	DrawCtx* draw;
} Block;

#endif /* _UNBAR_H */
