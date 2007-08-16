# This is a temporary makefile. It should be replaced when configure is set up.
CC = gcc
CFLAGS = -g -Wall
prefix = .
library = liblhpc-aff.a

sources=$(treap.sources) \
        $(stable.sources)

treap.sources = treap-extend.c	\
                treap-fini.c \
	        treap-init.c \
                treap-lookup.c \
                treap-print.c

treap.headers = treap-i.h \
                treap.h

stable.sources = stable-fini.c \
                 stable-index.c \
                 stable-print.c \
                 stable-foreach.c \
                 stable-init.c \
                 stable-size.c \
                 stable-fsize.c \
                 stable-insert.c \
                 stable-iblock.c \
                 stable-lookup.c

stable.headers = stable-i.h \
                 stable.h

.PHONY: all clean realclean install

all: $(library)

clean:
	$(RM) $(sources:%.c=%.o)

realclean: clean
	$(RM) $(library)

install:
	echo "Install is not implemented yet"
	exit 1

$(library): $(sources:%.c=%.o)
	ar cr $(library) $(sources:%.c=%.o)
	ranlib $(library)

$(treap.sources:%.c=%.o): %.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(treap.sources:%.c=%.o): $(treap.headers)

$(stable.sources:%.c=%.o): %.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(stable.sources:%.c=%.o): $(stable.headers) treap.h

