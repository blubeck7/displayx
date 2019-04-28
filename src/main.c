#include <stdio.h>
#include "../inc/display.h"


int main(void)
{
	int i;

	i = init_scr("Chess", 400, 600, 1);
	printf("Return code is %d\n", i);

	return 0;
}
