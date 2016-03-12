NAME=		pokoy
CC=		gcc
CFLAGS=		-O2 -Wpedantic -Wall -Werror -Wextra -Iinclude/

HEADERS=	include/pokoy.h include/date.h

SOURCES=	src/http.c src/date.c

OBJECTS=	http.o date.o

all: $(SOURCES) $(HEADERS)
	$(CC) $(CFLAGS) -c $(SOURCES)

clean:
	rm -f $(OBJECTS)
