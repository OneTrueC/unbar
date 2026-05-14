INC = -I. -I/usr/include
LIB =
DEF = -DPRGNAME=\"unbar\"

CFLAGS = -Wall -Wextra -std=c99 -pedantic $(INC) $(DEF)
LDFLAGS = $(LIB)

PROFFLAGS = -Os
BUILDFLAGS = $(PROFFLAGS) -s
DEBUGFLAGS = -g3 -Og -fanalyzer

SRC = $(wildcard *.c)
PLGIN = plugin/test.so

RUNOPTS =

CCOMP = $(CC) $(CFLAGS) $(LDFLAGS) $(SRC) -o

.PHONY: clean profclean build debug gdb strace memcheck perfprof memprof

all: debug

build: $(SRC) $(PLGIN)
	$(CCOMP) $@ $(BUILDFLAGS)
	chmod +x $@

plugin/%.so: plugin/%.c
	$(MAKE) -C plugin

clean:
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

debug: $(SRC) $(PLGIN)
	@$(CCOMP) $@ $(DEBUGFLAGS)
	@chmod +x $@
	@./$@ $(RUNOPTS)
	@rm -f $@

gdb: $(SRC) $(PLGIN)
	$(CCOMP) $@ $(DEBUGFLAGS)
	chmod +x $@
	gdb ./$@
	rm -f ./$@

strace: $(SRC) $(PLGIN)
	$(CCOMP) $@ $(DEBUGFLAGS)
	chmod +x $@
	strace -- ./$@ $(RUNOPTS)
	rm -f ./$@

memcheck: $(SRC) $(PLGIN)
	$(CCOMP) $@ $(DEBUGFLAGS)
	chmod +x $@
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes -s -- \
	         ./$@ $(RUNOPTS)
	rm -f $@

threadcheck: $(SRC) $(PLGIN)
	$(CCOMP) $@ $(DEBUGFLAGS)
	chmod +x $@
	valgrind --tool=drd --check-stack-var=yes --free-is-write=yes -- ./$@ \
	         $(RUNOPTS)
	rm -f $@

perfprof: $(SRC) $(PLGIN)
	$(CCOMP) $@ $(DEBUGFLAGS) $(PROFFLAGS)
	chmod +x $@
	valgrind --tool=callgrind --cache-sim=yes --enable-debuginfod=yes  \
	         --trace-children=yes --dump-instr=yes --collect-jumps=yes \
	         --branch-sim=yes -- ./$@ $(RUNOPTS)
	callgrind_annotate --auto=yes callgrind.out*
	rm -f $@

memprof: $(SRC) $(PLGIN)
	$(CCOMP) $@ $(DEBUGFLAGS) $(PROFFLAGS)
	chmod +x $@
	valgrind --tool=massif --heap=yes --stacks=yes --threshold=0.0 -- ./$@ \
	         $(RUNOPTS)
	rm -f $@
