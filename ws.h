/* function declarations for window system functions
 * vim: set noet tw=80 ts=4: */
#define _POSIX_C_SOURCE 200809L

#ifndef _WS_H
#define _WS_H

#include "util.h"
#include "unbar.h"
#include "ws-types.h"

/* connect or equivalent to the window system */
WindowCtx* initWS(void);
/* destroy/free/etc. all context */
void cleanWS(WindowCtx* c);

/* create bar windows on every monitor */
void createBar(WindowCtx* c, unsigned int barWidth, enum SIDE side);
/* destroy all bar windows */
void destroyBar(WindowCtx* c);

/* draw a line */
void drawLine(DrawCtx* c, Point a, Point b, Color color);
/* draw a point */
void drawPoint(DrawCtx* c, int x, int y, Color color);
/* draw an arc */
void drawArc(DrawCtx* c, int x, int y, unsigned int width, unsigned int height,
             int angle1, int angle2, Color color);
/* draw rectangle */
void drawRectangle(DrawCtx* c, int x, int y, unsigned int width,
                   unsigned int height, Color color);
/* draw a polygon */
void drawPolygon(DrawCtx* c, Point points[], int npoints, Color color);

/* draw a filled arc */
void fillArc(DrawCtx* c, int x, int y, unsigned int width, unsigned int height,
             int angle1, int angle2, Color color);
/* draw a filled rectangle */
void fillRectangle(DrawCtx* c, int x, int y, unsigned int width,
                   unsigned int height, Color color);
/* draw a filled polygon */
void fillPolygon(DrawCtx* c, Point points[], int npoints, Color color);

/* load a font */
FontObj* loadFont(DrawCtx* c, char* name);
/* unload a font */
void unloadFont(DrawCtx* c, FontObj* font);
/* draw text with a font */
void drawText(DrawCtx* c, int x, int y, char* string, FontObj* font,
              Color color);

/* copy content from widget buffer to bar */
void drawBlock(WindowCtx* c, Block block, int bar, int x, int y);

#endif /* _WS_H */
