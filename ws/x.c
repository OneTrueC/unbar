/* function declarations for Xorg functions */
#define _POSIX_C_SOURCE 200809L

#include <stdlib.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include "util.h"
#include "ws.h"

struct WindowCtx {
	Display *dpy;
	int scr;
	XVisualInfo vinfo;
	GC gc;
	/* one bar per screen */
	int nscreens;
	Window* bars;
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

	/* assuming 32bit color depth */
	XMatchVisualInfo(ret->dpy, ret->scr, 32, TrueColor, &ret->vinfo);

	return ret;
}

void
cleanWS(WindowCtx* ctx)
{
	XCloseDisplay(ctx->dpy);
	free(ctx);
}
