/* bar without the status */
#define _POSIX_C_SOURCE 200809L

#include <dlfcn.h>

#include "util.h"
#define HOST
#include "api.h"
#undef HOST

typedef int (*plugin_main)(void);

int dlink(void* lib);

int
main(void)
{
	void* plugin;
	plugin_main pl_main;

	plugin = dlopen("./plugin/test.so", RTLD_NOW | RTLD_LOCAL);
	if (!plugin) die(3, "couldn't open plugin: %s", dlerror());

	*(void**)(&pl_main) = dlsym(plugin, "plugin_main");
	if (!pl_main) die(2, "no entrypoint for plugin");

	dlink(plugin);

	(*pl_main)();

	return 0;
}

int
dlink(void* plugin)
{
#define LINK(ret, name, ...) do {                                              \
	ret (**sym)(__VA_ARGS__);                                                  \
	*(void**)(&sym) = dlsym(plugin, #name);                                    \
	if (sym) {                                                                 \
		if (*sym && *sym != &name)                                             \
			die(1, "attempting to link already linked funcion");               \
		*sym = &name;                                                          \
	}                                                                          \
} while (0)

#include "api.h"

#undef LINK

	return 0;
}
