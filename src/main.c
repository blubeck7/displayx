#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "../inc/display.h"


int main(void)
{
	//set_event_handler(event_handler);	
	if (init_scr("Chess", 20, 40, 12) == -1)
		return -1;
	//while (run)
		//;
	sleep(10);
	close_scr();
	return 0;
}
