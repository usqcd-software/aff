# This is a temporary makefile. It should be replaced when configure is set up.
CC = gcc
CFLAGS = -g -Wall
prefix = .
library = liblhpc-aff.a

sources=$(treap.sources) \
        $(stable.sources) \
        $(tree.sources) \
        $(node.sources) \
        $(md5.sources) \
        $(coding.sources) \
        $(aff.sources)

treap.sources = treap-insert.c	\
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
                 stable-lookup.c \
                 stable-name.c \
                 stable-id.c \

stable.headers = stable-i.h \
                 stable.h

tree.sources = tree-fini.c \
               tree-foreach.c \
               tree-fsize.c \
               tree-iblock.c \
               tree-index.c \
               tree-init.c \
               tree-insert.c \
               tree-lookup.c \
               tree-print.c \
               tree-root.c \
               tree-size.c

tree.headers = tree-i.h \
               tree.h

node.sources = node-foreach.c \
               node-id.c \
               node-name.c \
               node-parent.c \
               node-type.c \
               node-size.c \
               node-offset.c \
               node-assign.c \
               node-chdir.c \
               node-cda.c \
               node-cdv.c \
               node-cd.c

node.headers = node-i.h \
               node.h

md5.sources = md5.c

md5.headers = md5.h

aff.sources = waff-init.c \
              waff-fini.c \
              waff-errstr.c \
              waff-root.c \
              waff-mkdir.c \
              waff-putchar.c

aff.headers = aff-i.h \
              aff.h

coding.sources = encode-type.c \
                 encode-u32.c \
                 encode-u64.c

coding.headers = coding.h

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

$(sources:%.c=%.o): %.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(node.sources:%.c=%.o): $(node.headers)
$(treap.sources:%.c=%.o): $(treap.headers)
$(stable.sources:%.c=%.o): $(stable.headers) treap.h
$(tree.sources:%.c=%.o): $(tree.headers) treap.h stable.h node.h node-i.h
$(md5.sources:%.c=%.o): $(md5.headers)
$(aff.sources:%.c=%.o): $(aff.headers) md5.h tree.h stable.h coding.h
$(coding.sources:%.c=%.o): $(coding.headers) node.h