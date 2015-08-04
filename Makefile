CC = gcc
CFLAGS = -c -Wall -g -DDEBUG
LDFLAGS = -lSDL2
SOURCES = 6502.c apple_II.c applescr.c diskII.c language.c main.c
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
