CC = gcc -std=c99
CFLAGS = -Wall -g

.PHONY: all clean realclean lib

program = lhpc-aff
headers = util.h \
          procs.def

sources = lhpc-aff.c \
          check.c \
          ls.c \
	  import.c \
	  cat.c \
	  insert.c \
	  extract.c \
	  copy.c \
	  chdir_keypath.c \
	  copy_nodes.c \
	  common.c

all: $(program)

clean:
	$(RM) $(sources:%.c=%.o)

realclean: clean
	$(RM) $(program)

lib:
	make -C ../lib

$(program): $(sources:%.c=%.o)
	$(CC) $(CFLAGS) -static -I../lib -o $@ $^ -L../lib -llhpc-aff -lm

$(sources:%.c=%.o): %.o: %.c
	$(CC) $(CFLAGS) -I../lib -c -o $@ $<

$(sources:%.c=%.o): $(headers)
