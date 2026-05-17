/* function declarations for window system functions */
#define _POSIX_C_SOURCE 200809L

struct WindowCtx;
typedef struct WindowCtx WindowCtx;

/* return WindowCtx type pointer (defined per ws) */
WindowCtx* initWS(void);
/* destroy/free/etc. all context */
void cleanWS(WindowCtx* ctx);
