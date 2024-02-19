.POSIX:

include config.mk

SRC = src/main.c src/calculator.c src/error.c
OBJ = $(SRC:.c=.o)

calculator: $(OBJ)
	$(CC) -o $@ $(OBJ) $(CFLAGS)

src/%.o: src/%.c
	$(CC) -c -o $@ $(CFLAGS) $<

src/main.o: src/calculator.h src/error.h
src/calculator.o: src/error.h

clean:
	rm -f calculator $(OBJ)

check: calculator
	-./test

install: calculator
	mkdir -p $(PREFIX)/bin
	cp -f $< $(PREFIX)/bin/$<
	chmod 755 $(PREFIX)/bin/$<

uninstall:
	rm -f $(PREFIX)/bin/calculator

.PHONY: calculator clean check install uninstall
