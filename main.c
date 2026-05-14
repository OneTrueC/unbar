/* C quickstart main file */
#define _POSIX_C_SOURCE 200809L

#include <stdio.h>

#include <dlfcn.h>

#define HOST
#include "api.h"
#undef HOST

typedef int (*plugin_main)(void);

int dlink(void* lib);

int
main(void)
{
	char* err;
	void* plugin;
	plugin_main pl_main;

	plugin = dlopen("./plugin/test.so", RTLD_NOW | RTLD_LOCAL);
	err = dlerror();
	if (err != NULL) {
		printf("%s\n", err);
		return 3;
	}
	if (!plugin)
		return 1;
	*(void**)(&pl_main) = dlsym(plugin, "plugin_main");
	if (!pl_main)
		return 2;

	if (dlink(plugin) == -1) {
		return -1; //@TODO: error about already linked function
	}

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
			return -1;                                                         \
		*sym = &name;                                                          \
	}                                                                          \
} while (0)

#include "api.h"

#undef LINK

	return 0;
}
