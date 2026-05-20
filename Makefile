ISXORG = ${shell ps -e | grep Xorg | sed -e 's/.*Xorg.*/YES/'}

INC = -I. -I/usr/include
LIB =
DEF = -DPRGNAME=\"unbar\"

CFLAGS := -Wall -Wextra -std=c99 -pedantic $(INC) $(DEF) $(CFLAGS)
LDFLAGS := $(LIB) $(LDFLAGS)

PROFFLAGS = -Os
BUILDFLAGS = $(PROFFLAGS) -s
DEBUGFLAGS = -g3 -Og -fanalyzer

SRC = $(filter-out wlr-layer-shell-unstable-v1-client-protocol.c xdg-shell-protocol.c, $(wildcard *.c))
PLGIN = plugin/test.so

RUNOPTS =

CCOMP = $(CC) $(CFLAGS) $(LDFLAGS) $(SRC) -o

.PHONY: clean profclean build debug gdb strace memcheck perfprof memprof
.POSIX:

ifeq (${ISXORG},YES)
    X11INC := /usr/X11R6/include
    X11LIB := /usr/X11R6/lib
    SRC := ${SRC} ws/x.c
    CFLAGS := ${CFLAGS} -I${X11INC}
    LDFLAGS := ${LDFLAGS} -L${X11LIB} -lX11 -lXrandr
else
    WLPROTDIR := $(shell pkg-config --variable=pkgdatadir wayland-protocols)
    WLROOTSPROT := /usr/share/wlroots/protocol
    SRC := ${SRC} ws/y.c ws/wlr-layer-shell-unstable-v1-client-protocol.c ws/xdg-shell-protocol.c
    CFLAGS := ${CFLAGS} $(shell pkg-config --cflags wayland-client)
    LDFLAGS := ${LDFLAGS} $(shell pkg-config --libs wayland-client)
    WLHEADER := ws/wlr-layer-shell-unstable-v1-client-protocol.h ws/xdg-shell-protocol.h
endif

all: debug

build: $(WLHEADER) $(SRC) $(PLGIN)
	$(CCOMP) $@ $(BUILDFLAGS)
	chmod +x $@

plugin/%.so: plugin/%.c
	$(MAKE) -C plugin

include ws/Makefile.wayland

clean: wlclean
	rm -f debug
	rm -f gdb
	rm -f memcheck
	rm -f build
	rm -f strace
	rm -f perfprof
	rm -f memprof

profclean:
	rm -f callgrind.out*
	rm -f massif.out*

debug: $(WLHEADER) $(SRC) $(PLGIN)
	@$(CCOMP) $@ $(DEBUGFLAGS)
	@chmod +x $@
	@./$@ $(RUNOPTS)
	@rm -f $@

gdb: $(WLHEADER) $(SRC) $(PLGIN)
	$(CCOMP) $@ $(DEBUGFLAGS)
	chmod +x $@
	gdb ./$@
	rm -f ./$@

strace: $(WLHEADER) $(SRC) $(PLGIN)
	$(CCOMP) $@ $(DEBUGFLAGS)
	chmod +x $@
	strace -- ./$@ $(RUNOPTS)
	rm -f ./$@

memcheck: $(WLHEADER) $(SRC) $(PLGIN)
	$(CCOMP) $@ $(DEBUGFLAGS)
	chmod +x $@
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes -s -- \
	         ./$@ $(RUNOPTS)
	rm -f $@

threadcheck: $(WLHEADER) $(SRC) $(PLGIN)
	$(CCOMP) $@ $(DEBUGFLAGS)
	chmod +x $@
	valgrind --tool=drd --check-stack-var=yes --free-is-write=yes -- ./$@ \
	         $(RUNOPTS)
	rm -f $@

perfprof: $(WLHEADER) $(SRC) $(PLGIN)
	$(CCOMP) $@ $(DEBUGFLAGS) $(PROFFLAGS)
	chmod +x $@
	valgrind --tool=callgrind --cache-sim=yes --enable-debuginfod=yes  \
	         --trace-children=yes --dump-instr=yes --collect-jumps=yes \
	         --branch-sim=yes -- ./$@ $(RUNOPTS)
	callgrind_annotate --auto=yes callgrind.out*
	rm -f $@

memprof: $(WLHEADER) $(SRC) $(PLGIN)
	$(CCOMP) $@ $(DEBUGFLAGS) $(PROFFLAGS)
	chmod +x $@
	valgrind --tool=massif --heap=yes --stacks=yes --threshold=0.0 -- ./$@ \
	         $(RUNOPTS)
	rm -f $@
