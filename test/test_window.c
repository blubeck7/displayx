#include <stdio.h>
#include <unistd.h>
#include "../inc/window.h"

int main(void)
{
	Window *window;

	window = open_window();
	//printf("Window id: %d\n", window_get_id(window));
	sleep(2);
	close_window(window);

	return 0;
}

