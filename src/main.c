#include <stdio.h>
#include "../inc/display.h"


int main(void)
{
	int i;

	i = init_scr("Chess", 20, 40, 12);
	printf("Return code is %d\n", i);
	//print_win_size();

	return 0;
}
