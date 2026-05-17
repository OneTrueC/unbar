/* function declarations for window system functions */
#define _POSIX_C_SOURCE 200809L

struct windowCtx;
typedef struct windowCtx windowCtx;

/* return windowCtx type pointer (defined per ws) */
windowCtx* initWS(void);
