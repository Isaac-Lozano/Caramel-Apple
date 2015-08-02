CC = gcc
CFLAGS = -c -Wall -g -O3
LDFLAGS = -lSDL2
SOURCES = 6502.c appleII.c applescr.c diskII.c
OBJECTS = $(SOURCES:.c=.o)
EXECUTABLE = appleII

all: $(SOURCES) $(EXECUTABLE)


$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

.PHONY: clean

clean:
	rm $(EXECUTABLE) $(OBJECTS)