NAME=		pokoy
CC=		gcc
CFLAGS=		-O2 -Wpedantic -Wall -Werror -Wextra -Iinclude/

HEADERS=	include/pokoy.h

SOURCES=	src/http.c

OBJECTS=	http.o

all: $(SOURCES) $(HEADERS)
	$(CC) $(CFLAGS) -c $(SOURCES)

clean:
	rm -f $(OBJECTS)
