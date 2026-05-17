/* function declarations for Xorg functions */
#define _POSIX_C_SOURCE 200809L

#include <stdlib.h>

#include "util.h"
#include "ws.h"

struct windowCtx {
	//@TODO
};

windowCtx*
initWS(void)
{
	windowCtx* ret = malloc(sizeof(windowCtx));

	if (!ret) die(4, "Out of Memory");

	return ret;
}
