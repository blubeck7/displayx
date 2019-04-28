#include "../src/display.c"


int main(void)
{
	init_scr(250, 250, 1);
	printf("Logical height %d\n", scr.height);
	printf("Logical width %d\n", scr.width);
	printf("cur buf %d\n", scr.cur_buf);
	printf("row scalar %d\n", scr.row_sc);
	printf("col scalar %d\n", scr.col_sc);
	printf("pixels per row %d\n", scr.ppr);
	printf("pixels per col %d\n", scr.ppc);

	return 0;
}
