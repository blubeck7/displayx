#include <stdio.h>
#include <unistd.h>
#include "../inc/display.h"


int main(void)
{
	int i, buf;

	i = open_scr("Chess", 20, 40, 12);
	display();
	color(10, 10, 67);
	color(10, 11, 67);
	display();
	sleep(60);
	close_scr();

	return 0;
}
