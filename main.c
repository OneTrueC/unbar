/* C quickstart main file */
#define _POSIX_C_SOURCE 200809L

#include <stdio.h>

#include <dlfcn.h>

#include "plugin/api.h"

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
	int (**sym)(void);
	*(void**)(&sym) = dlsym(plugin, "testfunc");

	if (sym) {
		if (*sym && *sym != &testfunc)
			return -1;
		*sym = &testfunc;
	}

	return 0;
}

int
testfunc(void)
{
	printf("this is a function from the host program\n");

	return 0;
}
