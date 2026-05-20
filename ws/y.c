/* function declarations for wayland functions */
#define _GNU_SOURCE
#define _POSIX_C_SOURCE 200809L

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#include <wayland-client.h>
#include <wayland-client-core.h>
#include <wayland-client-protocol.h>
#include "ws/wlr-layer-shell-unstable-v1-client-protocol.h"

#include "util.h"
#include "ws.h"

#define BAR_COLOR 0xFFFFFFFF;

struct Bar {
	struct wl_output* output;
	struct wl_surface* surface;
	struct zwlr_layer_surface_v1* layer;
	struct wl_buffer* buffer;
	uint32_t* pixels;
	int height;
	int width;
	int configured;
};

struct WindowCtx {
	struct wl_display* dpy;
	struct wl_registry* registry;

	struct wl_compositor* compositor;
	struct wl_shm* shm;
	struct zwlr_layer_shell_v1* layer_shell;

	int nbars;
	struct Bar* bars;
};

static void output_geometry(void* data, struct wl_output* output, int x, int y,
                            int pw, int ph, int subpixel, const char* make,
                            const char* model, int transform);
static void output_mode(void* data, struct wl_output* output, uint32_t flags,
                        int w, int h, int refresh);
static void output_done(void* data, struct wl_output* output);
static void output_scale(void* data, struct wl_output* output, int32_t factor);
static void layer_surface_configure(void* data, struct zwlr_layer_surface_v1*
                                    layer_surface, uint32_t serial, uint32_t w,
                                    uint32_t h);
static void layer_surface_closed(void* data, struct zwlr_layer_surface_v1*
                                 layer_surface);
static void registry_global(void* data, struct wl_registry* registry,
                            uint32_t name, const char* interface,
                            uint32_t version);
static void registry_global_remove(void* data, struct wl_registry* registry,
                                   uint32_t name);

static int create_shm_file(size_t size);

/* listening stuff */

/* output geoetry listener */
static void
output_geometry(void* data, struct wl_output* output,
                int x, int y, int pw, int ph,
                int subpixel, const char* make,
                const char* model, int transform)
{
	(void)data; (void)output; (void)x; (void)y;
	(void)pw; (void)ph; (void)subpixel;
	(void)make; (void)model; (void)transform;
}

static void
output_mode(void* data, struct wl_output* output,
            uint32_t flags, int w, int h, int refresh)
{
	struct Bar* bar = data;

	if (flags & WL_OUTPUT_MODE_CURRENT) {
		bar->width = w;
		bar->height = h;
	}
	(void)output; (void)refresh;
}

static void
output_done(void* data, struct wl_output* output)
{
	(void)data; (void)output;
}

static void
output_scale(void* data, struct wl_output* output, int32_t factor)
{
	(void)data; (void)output; (void)factor;
}

static const struct wl_output_listener output_listener = {
	.geometry = output_geometry,
	.mode = output_mode,
	.done = output_done,
	.scale = output_scale
};

/* surface layer listener */
static void
layer_surface_configure(void* data,
                        struct zwlr_layer_surface_v1* layer_surface,
                        uint32_t serial, uint32_t w, uint32_t h)
{
	struct Bar* bar = data;

	bar->width = w;
	bar->height = h;
	bar->configured = 1;

	zwlr_layer_surface_v1_ack_configure(layer_surface, serial);
}

static void
layer_surface_closed(void* data,
                     struct zwlr_layer_surface_v1* layer_surface)
{
	struct Bar* bar = data;
	zwlr_layer_surface_v1_destroy(layer_surface);
	wl_surface_destroy(bar->surface);
	bar->layer = NULL;
	bar->surface = NULL;
	(void)data;
}

static const struct zwlr_layer_surface_v1_listener layer_surface_listener = {
	.configure = layer_surface_configure,
	.closed = layer_surface_closed
};

/* registry listener */
static void
registry_global(void* data, struct wl_registry* registry,
                uint32_t name, const char* interface, uint32_t version)
{
	WindowCtx* ctx = data;
	(void)version;
	struct Bar* newbars;
	struct wl_output* output;

	if (strcmp(interface, wl_compositor_interface.name) == 0) {
		ctx->compositor = wl_registry_bind(registry, name,
		                                  &wl_compositor_interface, 1);
	} else if (strcmp(interface, wl_shm_interface.name) == 0) {
		ctx->shm = wl_registry_bind(registry, name,
		                            &wl_shm_interface, 1);
	} else if (strcmp(interface, zwlr_layer_shell_v1_interface.name) == 0) {
		ctx->layer_shell = wl_registry_bind(registry, name,
		                                    &zwlr_layer_shell_v1_interface, 1);
	} else if (strcmp(interface, wl_output_interface.name) == 0) {


		output = wl_registry_bind(registry, name, &wl_output_interface, 1);

		newbars = realloc(ctx->bars, (ctx->nbars + 1) * sizeof(struct Bar));
		if (!newbars) DOOM;
		ctx->bars = newbars;

		memset(&ctx->bars[ctx->nbars], 0, sizeof(struct Bar));
		ctx->bars[ctx->nbars].output = output;

		wl_output_add_listener(output, &output_listener,
		                       &ctx->bars[ctx->nbars]);
		ctx->nbars++;
	}
}

static void
registry_global_remove(void* data, struct wl_registry* registry,
                       uint32_t name)
{
	(void)data; (void)registry; (void)name;
}

static const struct wl_registry_listener registry_listener = {
	.global = registry_global,
	.global_remove = registry_global_remove,
};

static int
create_shm_file(size_t size)
{
    int fd = memfd_create("unbar-shm", MFD_CLOEXEC);
    if (fd < 0)
        die(5, "memfd_create failed");
    if (ftruncate(fd, size)) {
        close(fd);
        die(5, "ftruncate failed");
    }
    return fd;
}

/* high level functions */

WindowCtx*
initWS(void)
{
	WindowCtx* ctx;

	ctx = calloc(1, sizeof(WindowCtx));
	if (!ctx) DOOM;

	ctx->dpy = wl_display_connect(NULL);
	if (!ctx->dpy)
		die(5, "couldn't connect to wayland display");

	ctx->registry = wl_display_get_registry(ctx->dpy);
	if (!ctx->registry)
		die(5, "couldn't get wayland registry");

	wl_registry_add_listener(ctx->registry, &registry_listener, ctx);
	wl_display_roundtrip(ctx->dpy);

	if (!ctx->compositor)
		die(5, "compositor doesn't support wl_compositor");
	if (!ctx->shm)
		die(5, "compositor doesn't support wl_shm");
	if (!ctx->layer_shell)
		die(5, "compositor doesn't support zwlr_layer_shell");
	if (ctx->nbars == 0)
		die(5, "no monitors found");

	wl_display_roundtrip(ctx->dpy);

	return ctx;
}

void
cleanWS(WindowCtx* ctx)
{
	for (int i = 0; i < ctx->nbars; i++)
		if (ctx->bars[i].output)
			wl_output_release(ctx->bars[i].output);

	free(ctx->bars);

	if (ctx->layer_shell)  zwlr_layer_shell_v1_destroy(ctx->layer_shell);
	if (ctx->shm)          wl_shm_destroy(ctx->shm);
	if (ctx->compositor)   wl_compositor_destroy(ctx->compositor);

	wl_registry_destroy(ctx->registry);
	wl_display_disconnect(ctx->dpy);
	free(ctx);
}

void
createBar(WindowCtx* ctx, unsigned barWidth, enum SIDE side)
{
	struct Bar* bar;

	for (int i = 0; i < ctx->nbars; i++) {
		bar = &ctx->bars[i];

		bar->surface = wl_compositor_create_surface(ctx->compositor);
		if (!bar->surface)
			die(5, "couldn't create surface");

		bar->layer = zwlr_layer_shell_v1_get_layer_surface(
		    ctx->layer_shell,
		    bar->surface,
		    bar->output,
		    ZWLR_LAYER_SHELL_V1_LAYER_TOP,
		    "bar");

		if (!bar->layer)
			die(5, "couldn't create layer surface");

		switch (side) {
		case TOP:
			zwlr_layer_surface_v1_set_anchor(bar->layer,
				ZWLR_LAYER_SURFACE_V1_ANCHOR_TOP   |
				ZWLR_LAYER_SURFACE_V1_ANCHOR_LEFT  |
				ZWLR_LAYER_SURFACE_V1_ANCHOR_RIGHT);
			break;
		case BOTTOM:
			zwlr_layer_surface_v1_set_anchor(bar->layer,
				ZWLR_LAYER_SURFACE_V1_ANCHOR_BOTTOM |
				ZWLR_LAYER_SURFACE_V1_ANCHOR_LEFT   |
				ZWLR_LAYER_SURFACE_V1_ANCHOR_RIGHT);
			break;
		case LEFT:
			zwlr_layer_surface_v1_set_anchor(bar->layer,
				ZWLR_LAYER_SURFACE_V1_ANCHOR_LEFT  |
				ZWLR_LAYER_SURFACE_V1_ANCHOR_TOP   |
				ZWLR_LAYER_SURFACE_V1_ANCHOR_BOTTOM);
			break;
		case RIGHT:
			zwlr_layer_surface_v1_set_anchor(bar->layer,
				ZWLR_LAYER_SURFACE_V1_ANCHOR_RIGHT |
				ZWLR_LAYER_SURFACE_V1_ANCHOR_TOP   |
				ZWLR_LAYER_SURFACE_V1_ANCHOR_BOTTOM);
			break;
		}

		zwlr_layer_surface_v1_set_size(bar->layer,
		    (side == TOP || side == BOTTOM) ? 0 : barWidth,
		    (side == TOP || side == BOTTOM) ? barWidth : 0);

		zwlr_layer_surface_v1_set_exclusive_zone(bar->layer, barWidth);

		zwlr_layer_surface_v1_add_listener(bar->layer,
		                                   &layer_surface_listener, bar);

		wl_surface_commit(bar->surface);
	}

		wl_display_roundtrip(ctx->dpy);

		for (int i = 0; i < ctx->nbars; i++) {
			if (!ctx->bars[i].configured)
				die(5, "bar %d was not configured by compositor", i);
		}

	/*drawing*/
	for (int i = 0; i < ctx->nbars; i++) {
	    struct Bar* bar = &ctx->bars[i];
		size_t size = bar->width * bar->height * 4;
	    struct wl_shm_pool* pool;
		int fd;

		fd = create_shm_file(size);
		bar->pixels = mmap(NULL, size, PROT_READ | PROT_WRITE,
		                   MAP_SHARED, fd, 0);
		if (bar->pixels == MAP_FAILED)
			die(5, "mmap failed");

		for (int j = 0; j < bar->width * bar->height; j++) {
			bar->pixels[j] = BAR_COLOR;
		}

		pool = wl_shm_create_pool(ctx->shm, fd, size);
		bar->buffer = wl_shm_pool_create_buffer(pool, 0,
		                                        bar->width, bar->height,
                                                bar->width * 4,
                                                WL_SHM_FORMAT_ARGB8888);
		wl_shm_pool_destroy(pool);
		close(fd);

		wl_surface_attach(bar->surface, bar->buffer, 0, 0);
		wl_surface_damage(bar->surface, 0, 0, bar->width, bar->height);
		wl_surface_commit(bar->surface);
	}
	wl_display_roundtrip(ctx->dpy);
}

void
destroyBar(WindowCtx* ctx)
{
	for (int i = 0; i < ctx->nbars; i++) {
		if (ctx->bars[i].layer)
			zwlr_layer_surface_v1_destroy(ctx->bars[i].layer);
		if (ctx->bars[i].surface)
			wl_surface_destroy(ctx->bars[i].surface);
	}

	wl_display_roundtrip(ctx->dpy);
}
