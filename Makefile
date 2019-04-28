main: src/main.o src/display.o
	cc -Wall -std=c11 -o bin/main src/main.o src/display.o

src/main.o: src/main.c inc/display.h

src/display.o: src/display.c inc/display.h
	cc -Wall -std=c11 -c -o src/display.o src/display.c

clean:
	rm src/main.o
	rm src/display.o
