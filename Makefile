NAME=		pokoy
CC=		gcc
CFLAGS=		-O2 -Wpedantic -Wall -Werror -Wextra -Iinclude/

HEADERS=	include/pokoy.h include/date.h include/http.h include/router.h

SOURCES=	src/http.c src/date.c src/server.c src/router.c

OBJECTS=	http.o date.o server.o router.o

all: $(SOURCES) $(HEADERS)
	$(CC) $(CFLAGS) -c $(SOURCES)

clean:
	rm -f $(OBJECTS)
