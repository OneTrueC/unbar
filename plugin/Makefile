INC = -I. -I/usr/include
LIB =

CFLAGS = -Wall -Wextra -std=c99 -pedantic $(INC)
LDFLAGS = $(LIB) -fPIC -shared

PROFFLAGS = -Os
BUILDFLAGS = $(PROFFLAGS)
DEBUGFLAGS = -g3 -Og -fanalyzer

SRC = $(wildcard *.c)

RUNOPTS =

CCOMP = $(CC) $(CFLAGS) $(LDFLAGS) $(SRC) -o

.PHONY: clean

all: test.so

test.so: $(SRC)
	$(CCOMP) $@ $(BUILDFLAGS)

clean:
	rm -f test.so
