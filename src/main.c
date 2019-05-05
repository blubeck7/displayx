#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "../inc/display.h"


int main(void)
{
	int i, buf;

	if (init_scr("Chess", 20, 40, 12) == -1)
		return -1;

	display();
	color(10, 10, 67);
	color(10, 11, 67);
	display();
	sleep(10);
	close_scr();

	return 0;
}
