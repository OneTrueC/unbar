/* function declarations for window system functions */
#define _POSIX_C_SOURCE 200809L

#include "util.h"

/* the WindowCtx interface and all relevant functions are defined per window
 * system, (i.e. x.c and y.c define all the same functions with identical
 * interfaces but the context is entirely different. */
struct WindowCtx;
typedef struct WindowCtx WindowCtx;

/* struct that will abstract the relevant things for drawing, one per block on
 * the statusbar */
struct DrawCtx;
typedef struct DrawCtx DrawCtx;

typedef unsigned long Color;

typedef struct {
	int x;
	int y;
} Point;

/* this is probably the best way to deal with fonts */
struct FontObj;
typedef struct FontObj FontObj;

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
