#include <stdio.h>
#include <unistd.h>
#include "../inc/display.h"


int main(void)
{
	int i, buf;

	i = open_scr("Chess", 20, 40, 12);
	display();
	//close_scr();

	return 0;
}
