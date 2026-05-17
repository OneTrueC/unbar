/* function declarations for window system functions */
#define _POSIX_C_SOURCE 200809L

#include "util.h"

/* the WindowCtx interface and all relevant functions are defined per window
 * system, (i.e. x.c and y.c define all the same functions with identical
 * interfaces but the context is entirely different. */
struct WindowCtx;
typedef struct WindowCtx WindowCtx;

/* connect or equivalent to the window system */
WindowCtx* initWS(void);
/* destroy/free/etc. all context */
void cleanWS(WindowCtx* ctx);

/* create bar windows on every monitor */
void createBar(WindowCtx* c, unsigned int barWidth, enum SIDE side);
/* destroy all bar windows */
void destroyBar(WindowCtx* c);
