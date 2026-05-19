/* function declarations for wayland functions */
#define _POSIX_C_SOURCE 200809L

#include <wayland-client.h>
#include <wayland-client-core.h>
#include <wayland-client-protocol.h>

#include "util.h"
#include "ws.h"

struct Bar {
	struct wl_output* output;
	struct wl_surface* surface;
	struct zwlr_layer_surface_v1* layer;
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

WindowCtx*
initWS(void) {

}
