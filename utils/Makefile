CC = gcc -std=c99
CFLAGS = -Wall -g

.PHONY: all clean realclean lib

program = lhpc-aff
headers = util.h \
          procs.def
sources = lhpc-aff.c \
          check.c

all: $(program)

clean:
	$(RM) $(sources:%.c=%.o)

realclean: clean
	$(RM) $(program)

lib:
	make -C ../lib

$(program): $(sources:%.c=%.o)
	$(CC) $(CFLAGS) -I../lib -o $@ $^ -L../lib -llhpc-aff

$(sources:%.c=%.o): %.o: %.c
	$(CC) $(CFLAGS) -I../lib -c -o $@ $<

$(sources:%.c=%.o): $(headers)
