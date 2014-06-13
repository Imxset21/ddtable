CC := gcc
CFLAGS ?= -std=c99 -O0 -Wall -Wextra -Wpedantic -g3
LDFLAGS ?= 
DEFINES ?= 
INCLUDES ?= -I.

# Ctags flags
CTAGS_FLAGS := -e -f TAGS --verbose -R --exclude=doc --langmap=c:.c.h --fields="+afikKlmnsSzt"

SOURCES := ddtable.c spooky-c.c ddtable_test.c
OBJECTS = $(SOURCES:%.c=%.o)
BINARY := ddtable_test.bin

.PHONY: all build rebuild clean run ctags

all: build

build: $(BINARY)

clean:
	-$(RM) $(OBJECTS)

rebuild: clean $(BINARY)

$(OBJECTS): %.o : %.c
	$(CC) $(CFLAGS) $(INCLUDES) $(DEFINES) -o $@ -c $<

$(BINARY): $(OBJECTS)
	$(CC) $^ -o $@ $(LDFLAGS)

run: $(BINARY)
	./$(BINARY)

ctags:
	ctags $(CTAGS_FLAGS)

