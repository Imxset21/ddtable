CC := gcc
CFLAGS ?= -std=c99 -O0 -Wall -Wextra -Wpedantic -g3 -fPIC
CPPFLAGS ?= 
LDFLAGS ?= 
DEFINES ?= 
INCLUDES ?= -I.

# Ctags flags
CTAGS_FLAGS := -e -f TAGS --verbose -R --exclude=doc --langmap=c:.c.h --fields="+afikKlmnsSzt"

SOURCES := ddtable.c spooky-c.c ddtable_test.c
OBJECTS = $(SOURCES:%.c=%.o)
BINARY := ddtable_test.bin
LIB := libddtable.so

.PHONY: all build rebuild clean run ctags check install uninstall

all: build

build: $(BINARY) $(LIB)

clean:
	$(RM) $(OBJECTS) $(LIB) $(BINARY)

rebuild: clean build

$(OBJECTS): %.o : %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $(INCLUDES) $(DEFINES) -o $@ -c $<

$(BINARY): $(OBJECTS)
	$(CC) $^ -o $@ $(LDFLAGS)

$(LIB): $(OBJECTS)
	$(CC) -shared $^ -o $@

run: $(BINARY)
	./$(BINARY)

check: all
	./$(BINARY) == 0
	@echo "*** ALL TESTS PASSED ***"

install: $(LIB)
	install -d $(DESTDIR)$(libdir)
	install -m 0755 $(DESTDIR)$(LIB) $(libdir)

uninstall:
	-rm $(DESTDIR)$(libdir)/$(LIB)

ctags:
	ctags $(CTAGS_FLAGS)


