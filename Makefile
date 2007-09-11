# This is a main Makefile for the AFF library and utilities.
#
CONFIG=config.none
subdirs=lib utils

prefix=/usr/local
prefix_lib=$(prefix)/lib
prefix_include=$(prefix)/include
prefix_bin=$(prefix)/bin
prefix_doc=$(prefix)/doc
.PHONY: all clean realclean install do.install

ifeq "$(wildcard $(CONFIG))" ""
all install:
	@echo "configuration $(CONFIG) not found"
	@exit 1
else
include $(CONFIG)
all:
	for d in $(subdirs); do \
	  make CC='$(CC)' CFLAGS='$(CFLAGS)' -C $$d $@; \
	done
install: all do.install
endif

clean:
	for d in $(subdirs); do \
	  make -C $$d $@; \
	done

realclean:
	for d in $(subdirs); do \
	  make -C $$d $@; \
	done
	$(RM) lhpc-aff-config

do.install: lhpc-aff-config docs/aff_spec.pdf
	mkdir -p $(prefix_bin)
	$(RM) $(prefix_bin)/lhpc-aff-config
	cp lhpc-aff-config $(prefix_bin)/lhpc-aff-config
	chmod 555 $(prefix_bin)/lhpc-aff-config
	$(RM) $(prefix_bin)/lhpc-aff
	cp utils/lhpc-aff $(prefix_bin)/lhpc-aff
	chmod 555 $(prefix_bin)/lhpc-aff
	mkdir -p $(prefix_lib)
	$(RM) $(prefix_lib)/liblhpc-aff.a
	cp lib/liblhpc-aff.a $(prefix_lib)/liblhpc-aff.a
	chmod 444 $(prefix_lib)/liblhpc-aff.a
	mkdir -p $(prefix_include)
	$(RM) $(prefix_include)/lhpc-aff.h
	cp lib/lhpc-aff.h $(prefix_include)/lhpc-aff.h
	chmod 444 $(prefix_include)/lhpc-aff.h
	mkdir -p $(prefix_doc)
	$(RM) $(prefix_doc)/aff_spec.pdf
	cp docs/aff_spec.pdf $(prefix_doc)/aff_spec.pdf
	chmod 444 $(prefix_doc)/aff_spec.pdf

lhpc-aff-config: Makefile $(CONFIG) aff-config.in
	sed -e 's?@CC@?$(install.CC)?' \
            -e 's?@CFLAGS@?$(install.CFLAGS)?' \
            -e 's?@LIBS@?$(install.LIBS)?' \
            -e 's?@LDFLAGS@?$(install.LDFLAGS)?' \
            -e 's?@prefix_include@?$(prefix_include)?' \
            -e 's?@prefix_lib@?$(prefix_lib)?' \
        < aff-config.in > lhpc-aff-config
