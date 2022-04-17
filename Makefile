# utils - miscellaneous utilities
# Copyright (C) 2022 ArcNyxx
# see LICENCE file for licensing information

.POSIX:

include config.mk

SRC = fl.c rev.c
OBJ = fl.o rev.o
OUT = fl   rev
MAN = fl.1 rev.1

all: fl rev

options:
	@echo utils build options:
	@echo "COMPILER = $(CC)"
	@echo "CFLAGS   = $(CFLAGS)"
	@echo "LDFLAGS  = $(LDFLAGS)"

$(OBJ): config.mk

.c.o:
	$(CC) -c $(CFLAGS) $<

fl: fl.o
	$(CC) fl.o -o $@ $(LDFLAGS)

rev: rev.o
	$(CC) rev.o -o $@ $(LDFLAGS)

clean:
	rm -f $(OUT) $(OBJ) utils-*.tar.gz

dist: clean
	mkdir -p utils-$(VERSION)
	cp -R LICENCE README Makefile config.mk $(SRC) $(MAN) utils-$(VERSION)
	tar -cf utils-$(VERSION).tar utils-$(VERSION)
	gzip utils-$(VERSION).tar
	rm -rf utils-$(VERSION)

install:
	mkdir -p $(DESTDIR)$(PREFIX)/bin $(DESTDIR)$(MANPREFIX)/man1
	for UTIL in $(OUT); do \
		if [ -e "$$UTIL" ]; then \
			cp -f $$UTIL $(DESTDIR)$(PREFIX)/bin; \
			chmod 755 $(DESTDIR)$(PREFIX)/bin/$$UTIL; \
		fi; \
	done
	for PAGE in $(MAN); do \
		if [ -e "$${PAGE%%.*}" ]; then \
			cat $$PAGE | sed 's/VERSION/$(VERSION)/g' > \
				$(DESTDIR)$(MANPREFIX)/man1/$$PAGE; \
			chmod 644 $(DESTDIR)$(MANPREFIX)/man1/$$PAGE; \
		fi; \
	done

uninstall:
	for UTIL in $(OUT); do \
		rm -f $(DESTDIR)$(PREFIX)/bin/$$UTIL; \
	done
	for PAGE in $(MAN); do \
		rm -f $(DESTDIR)$(MANPREFIX)/man1/$$PAGE; \
	done

.PHONY: all options clean dist install uninstall
