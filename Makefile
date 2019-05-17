#main: main.o display.o
	#cc -Wall -std=c11 -lpthread -o bin/main temp/main.o temp/display.o

#main.o: src/main.c inc/display.h
	#cc -Wall -std=c11 -c -o temp/main.o src/main.c

#display.o: src/display.c inc/display.h
	#cc -Wall -std=c11 -c -o temp/display.o src/display.c

#window
test_window: test_window.o window.o
	cc -Wall -std=c11 -o bin/test_window temp/test_window.o temp/window.o

test_window.o: test/test_window.c inc/window.h
	cc -Wall -std=c11 -c -o temp/test_window.o test/test_window.c

window.o: src/window.c inc/window.h
	cc -Wall -std=c11 -c -o temp/window.o src/window.c

#queue
test_queue: test_queue.o queue.o
	cc -Wall -std=c11 -lpthread -o bin/test_queue temp/test_queue.o \
	temp/queue.o

test_queue.o: test/test_queue.c inc/queue.h
	cc -Wall -std=c11 -c -o temp/test_queue.o test/test_queue.c

queue.o: src/queue.c inc/queue.h
	cc -Wall -std=c11 -c -o temp/queue.o src/queue.c

clean:
	rm temp/*.o
