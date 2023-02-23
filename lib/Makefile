CC := gcc
CFLAGS := -Wall -g

BIN := libtlpi.a

all: $(BIN) install clean

install:
	cp $(BIN) ../

clean:
	$(RM) $(BIN) *.o

$(BIN): *.c
	$(CC) $(CFLAGS) -c *.c
	ar rs $(BIN) *.o

.PHONY: all clean install
