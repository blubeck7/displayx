#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include "../inc/window.h"

#define OPEN_WIN "osascript "\
	"-e \"tell app \\\"Terminal\\\" to do script\" " \
	"-e \"tell app \\\"Terminal\\\" to set current settings of front window " \
		"to settings set \\\"screen\\\"\" " \
	"-e \"tell app \\\"Terminal\\\" to get id of front window\""
#define CLOSE_WIN "osascript -e \"tell app \\\"Terminal\\\" " \
	"to close window id %d\""

struct window {
	int id;
	char ttyname[32];
	char title[WIN_MAX_TITLE];
	int height;
	int width;
	int pix_size;
};

Window *open_window(void)
{
	Window *window;
	FILE *pipe;
	char buf[32];

	if ((window = (Window *) malloc(sizeof(Window))) == NULL)
		return NULL;

	if ((pipe = popen(OPEN_WIN, "r")) == NULL)
		return NULL;

	if (fgets(buf, 32, pipe) == NULL)
		return NULL;

	if (pclose(pipe) == -1)
		return NULL;

	window->id = atoi(buf);

	return window;
}

int close_window(Window *window)
{
	int status;
	char buf[100];

	sprintf(buf, CLOSE_WIN, window->id);
	status = system(buf);
	if (WIFEXITED(status) == 0)
		return -1;

	return 0;
}


int window_get_id(Window *window)
{
	if (window == NULL)
		return -1;

	return  window->id;
}
