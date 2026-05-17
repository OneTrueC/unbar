/* function declarations for Xorg functions */
#define _POSIX_C_SOURCE 200809L

#include <stdlib.h>

#include <X11/Xlib.h>

#include "util.h"
#include "ws.h"

struct WindowCtx {
	Display *dpy;
	int scr;
};

WindowCtx*
initWS(void)
{
	WindowCtx* ret;

	ret = malloc(sizeof(WindowCtx));
	if (!ret) die(4, "Out of Memory");

	ret->dpy = XOpenDisplay(NULL);
	if (!ret->dpy) die(5, "failed to connect to X display");

	ret->scr = DefaultScreen(ret->dpy);

	return ret;
}
