/* function declarations for Xorg functions */
#define _POSIX_C_SOURCE 200809L

#include <stdlib.h>

#include <assert.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include "util.h"
#include "ws.h"

struct WindowCtx {
	Display *dpy;
	int scr;
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
	assert(ret);

	ret->dpy = XOpenDisplay(NULL);
	if (!ret->dpy) die(5, "failed to connect to X display");
	assert(ret->dpy);

	ret->scr = DefaultScreen(ret->dpy);

	ret->nscreens = ScreenCount(ret->dpy);

	ret->bars = calloc(ret->nscreens, sizeof(Window));
	if (!ret->bars) die(4, "Out of Memory");
	assert(ret->bars);

	return ret;
}

void
createBar(WindowCtx* c, unsigned int barWidth, enum SIDE side)
{
	int i;
	unsigned int width, height, x, y;
	XSetWindowAttributes wa;

	wa.background_pixel = 0xFFFFFFFF;
	wa.override_redirect = 1;

	for (i = 0; i < c->nscreens; i++) {
		switch (side) {
		case TOP:
			width = DisplayWidth(c->dpy, i);
			height = barWidth;
			x = y = 0;
			break;
		case BOTTOM:
			width = DisplayWidth(c->dpy, i);
			height = barWidth;
			x = 0;
			y = DisplayHeight(c->dpy, i) - height;
			break;
		case LEFT:
			width = barWidth;
			height = DisplayHeight(c->dpy, i);
			x = y = 0;
			break;
		default:
		case RIGHT:
			width = barWidth;
			height = DisplayHeight(c->dpy, i);
			x = DisplayWidth(c->dpy, i) - width;
			y = 0;
			break;
		}

		c->bars[i] = XCreateWindow(c->dpy, RootWindow(c->dpy, i), x, y, width,
		                           height, 0, DefaultDepth(c->dpy, i),
		                           CopyFromParent, DefaultVisual(c->dpy, i),
		                           CWOverrideRedirect | CWBackPixel, &wa);

		XMapWindow(c->dpy, c->bars[i]);
	}

	XSync(c->dpy, c->bars[i]);
}

void
destroyBar(WindowCtx* c)
{
	int i;

	for (i = 0; i < c->nscreens; i++) {
		XUnmapWindow(c->dpy, c->bars[i]);
		XDestroyWindow(c->dpy, c->bars[i]);
	}

	XSync(c->dpy, 0);
}

void
cleanWS(WindowCtx* c)
{
	free(c->bars);

	XCloseDisplay(c->dpy);
	free(c);
}
