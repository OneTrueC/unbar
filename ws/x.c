/* function declarations for Xorg functions */
#define _POSIX_C_SOURCE 200809L

#include <stdlib.h>

#include <assert.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/Xrandr.h>

#include "util.h"
#include "ws.h"

struct WindowCtx {
	Display *dpy;
	/* one bar per monitor */
	int nscreens;
	XVisualInfo* vinfo;
	int* nmonitors;
	XRRMonitorInfo** minfos;
	int nbars;
	Window* bars;
};

struct DrawCtx {
	Pixmap pxm;
	WindowCtx* win;
	GC gc;
	unsigned int width;
	unsigned int height;
};

WindowCtx*
initWS(void)
{
	WindowCtx* ret;
	int i;

	ret = malloc(sizeof(WindowCtx));
	if (!ret) DOOM;
	assert(ret);

	ret->dpy = XOpenDisplay(NULL);
	if (!ret->dpy) die(5, "failed to connect to X display");
	assert(ret->dpy);

	ret->nscreens = ScreenCount(ret->dpy);

	ret->vinfo = calloc(ret->nscreens, sizeof(XVisualInfo));
	if (!ret->vinfo) DOOM;
	assert(ret->vinfo);

	ret->minfos = calloc(ret->nscreens, sizeof(XRRMonitorInfo*));
	if (!ret->minfos) DOOM;
	assert(ret->minfos);

	ret->nmonitors = calloc(ret->nscreens, sizeof(int*));
	if (!ret->nmonitors) DOOM;
	assert(ret->nmonitors);

	ret->nbars = 0;
	for (i = 0; i < ret->nscreens; i++) {
		ret->minfos[i] = XRRGetMonitors(ret->dpy, RootWindow(ret->dpy, i), 1,
		                                &ret->nmonitors[i]);
		ret->nbars += ret->nmonitors[i];

		XMatchVisualInfo(ret->dpy, i, 32, TrueColor, &ret->vinfo[i]);
	}

	ret->bars = calloc(ret->nbars, sizeof(Window));
	if (!ret->bars) DOOM;
	assert(ret->bars);

	return ret;
}

void
createBar(WindowCtx* c, unsigned int barWidth, enum SIDE side)
{
	int i, j, barItr;
	unsigned int width, height, x, y;
	XSetWindowAttributes wa;

	wa.background_pixel = 0xFFFFFFFF;
	wa.override_redirect = 1;
	wa.border_pixel = 0;

	barItr = 0;
	for (i = 0; i < c->nscreens; i++) {
		for (j = 0; j < c->nmonitors[i]; j++) {
			switch (side) {
			case TOP:
				width = c->minfos[i][j].width;
				height = barWidth;
				x = c->minfos[i][j].x;
				y = c->minfos[i][j].y;
				break;
			case BOTTOM:
				width = c->minfos[i][j].width;
				height = barWidth;
				x = c->minfos[i][j].x;
				y = c->minfos[i][j].height - height + c->minfos[i][j].y;
				break;
			case LEFT:
				width = barWidth;
				height = c->minfos[i][j].height;
				x = c->minfos[i][j].x;
				y = c->minfos[i][j].y;
				break;
			default:
			case RIGHT:
				width = barWidth;
				height = c->minfos[i][j].height;
				x = c->minfos[i][j].width - width + c->minfos[i][j].x;
				y = c->minfos[i][j].y;
				break;
			}

			wa.colormap = XCreateColormap(c->dpy, RootWindow(c->dpy, i),
			                              c->vinfo[i].visual, AllocNone);

			c->bars[barItr] = XCreateWindow(c->dpy, RootWindow(c->dpy, i), x, y,
			                                width, height, 0, c->vinfo[i].depth,
			                                CopyFromParent, c->vinfo[i].visual,
			                                CWOverrideRedirect | CWBackPixel |
			                                CWColormap | CWBorderPixel, &wa);

			XMapWindow(c->dpy, c->bars[barItr]);
			barItr++;
		}
	}

	XSync(c->dpy, 0);
}

void
destroyBar(WindowCtx* c)
{
	int i;

	for (i = 0; i < c->nbars; i++) {
		XUnmapWindow(c->dpy, c->bars[i]);
		XDestroyWindow(c->dpy, c->bars[i]);
	}

	XSync(c->dpy, 0);
}

void
cleanWS(WindowCtx* c)
{
	int i;

	for (i = 0; i < c->nscreens; i++)
		XRRFreeMonitors(c->minfos[i]);
	free(c->minfos);
	free(c->nmonitors);

	free(c->vinfo);

	free(c->bars);

	XCloseDisplay(c->dpy);
	free(c);
}

#define SETCOLOR XSetForeground(c->win->dpy, c->gc, color)

void
drawLine(DrawCtx *c, Point a, Point b, Color color)
{
	SETCOLOR;
	XDrawLine(c->win->dpy, c->pxm, c->gc, a.x, a.y, b.x, b.y);
}

void
drawPoint(DrawCtx *c, int x, int y, Color color)
{
	SETCOLOR;
	XDrawPoint(c->win->dpy, c->pxm, c->gc, x, y);
}

void
drawArc(DrawCtx *c, int x, int y, unsigned int width, unsigned int height,
        int angle1, int angle2, Color color)
{
	SETCOLOR;
	XDrawArc(c->win->dpy, c->pxm, c->gc, x, y, width, height, angle1, angle2);
}

void
drawRectangle(DrawCtx *c, int x, int y, unsigned int width, unsigned int height,
              Color color)
{
	SETCOLOR;
	XDrawRectangle(c->win->dpy, c->pxm, c->gc, x, y, width, height);
}

void
drawPolygon(DrawCtx *c, Point points[], int npoints, Color color)
{
	int i;
	XPoint xpoints[npoints];

	for (i = 0; i < npoints; i++) {
		xpoints[i].x = points[i].x;
		xpoints[i].y = points[i].y;
	}

	SETCOLOR;
	XDrawLines(c->win->dpy, c->pxm, c->gc, xpoints, npoints, CoordModeOrigin);
}

void
fillArc(DrawCtx *c, int x, int y, unsigned int width, unsigned int height,
        int angle1, int angle2, Color color)
{

	SETCOLOR;
	XFillArc(c->win->dpy, c->pxm, c->gc, x, y, width, height, angle1, angle2);
}

void
fillRectangle(DrawCtx *c, int x, int y, unsigned int width, unsigned int height,
              Color color)
{
	SETCOLOR;
	XFillRectangle(c->win->dpy, c->pxm, c->gc, x, y, width, height);
}

void
fillPolygon(DrawCtx *c, Point points[], int npoints, Color color)
{
	int i;
	XPoint xpoints[npoints];

	for (i = 0; i < npoints; i++) {
		xpoints[i].x = points[i].x;
		xpoints[i].y = points[i].y;
	}

	SETCOLOR;
	XFillPolygon(c->win->dpy, c->pxm, c->gc, xpoints, npoints, Complex,
	             CoordModeOrigin);
}
