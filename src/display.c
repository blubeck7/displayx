/* TODO: add exit handler using atexit function
 */
#define _POSIX_C_SOURCE 200809L

#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <termios.h>
#include <unistd.h>

#include "../inc/display.h"
#include "../inc/event.h"
#include "../inc/term.h"
#include "../inc/window.h"


#define MAX_BUF 500000
#define MAX_COL 256
#define MAX_IN 256 // max num of chars that can be stored in the input buffer
#define MIN_HEIGHT 10
#define MIN_WIDTH 10
#define DEFAULT_FONT "Courier New PMST"
#define DEFAULT_FONT_SIZE 1 
#define MAX_TTYNAME 32

/* ANSI Escape sequence */
#define ENTER_ALT_SCREEN "\x1b[?1049h" 
#define EXIT_ALT_SCREEN "\x1b[?10491" 
#define CLEAR_SCREEN "\x1b[2J" 

/* Basic colors */
#define BLACK 0
#define WHITE 15

#ifdef __APPLE__

#define OSA "osascript -e \"tell app \\\"Terminal\\\" "
#define NEW_WIN "open -a Terminal.app res/screen.terminal"
#define CNT_WIN OSA"to count windows\""
#define EXIT_WIN OSA"to close window id %d\""

#define GET_WIN_TTY OSA"to get tty of window %d\""
#define GET_WIN_ID OSA"to get id of window %d\""
#define GET_FONT_NAME OSA"to get font name of window id %d\""
#define GET_FONT_SIZE OSA"to get font size of window id %d\""
#define GET_WIN_SIZE OSA"to get size of window id %d\"" 
#define GET_WIN_BNDS OSA"to get bounds of window id %d\"" 
#define GET_WIN_ROWS OSA"to get number of rows of window id %d\"" 
#define GET_WIN_COLS OSA"to get number of columns of window id %d\"" 
#define GET_TERM_SIZE

#define SET_FONT_NAME OSA"to set font name of window id %d to \\\"%s\\\"\""
#define SET_FONT_SIZE OSA"to set font size of window id %d to %d\""
#define SET_WIN_SIZE OSA"to get size of window id to {%d,%d}\"" 
#define SET_WIN_BNDS OSA"to get bounds of window id to {%d,%d,%d,%d}\"" 
#define SET_WIN_ROWS OSA"to set number of rows of window id %d to %d\"" 
#define SET_WIN_COLS OSA"to set number of columns of window id %d to %d\"" 
#define SET_WIN_TITLE OSA"to set custom title of window id %d to \\\"%s\\\"\""

#endif

/*

#define UP(N) printf("\x1b[%dA", N) 
#define DOWN(N) printf("\x1b[%dB", N)
#define RIGHT(N) printf("\x1b[%dC", N) 
#define LEFT(N) printf("\x1b[%dD", N) 

#ifdef __APPLE__

*/
/*
#define SET_SCR_SIZE \
	"osascript -e \"tell application \\\"Terminal\\\" "\
	"to set bounds of window 1 to {of desktop\""
*/
//#endif


/* Font Name, Font Size, Rows, Columns, Row Pixels, Column Pixels
 * Courier New PMST, 12, 30, 86, 420, 602
 * Courier New PMST, 11
 * Courier New PMST, 10
 * Courier New PMST, 9, 24, 80, 264, 400
 * Courier New PMST, 8, 24, 80, 240, 400
 * Courier New PMST, 7
 * Courier New PMST, 6
 * Courier New PMST, 5
 * Courier New PMST, 4
 * Courier New PMST, 3, 24, 80, 96, 160
 * Courier New PMST, 2, 24, 80, 72, 80
 * Courier New PMST, 1, 24, 80, 48, 80
 */

struct pix {
	int size;
	int ppr; // physical pixels per row
	int ppc; // physical pixels per column
	int row_sc; // row scalar so that logical pixels are squares
	int col_sc; // column scalar so that logical pixels are squares
};

static struct pix pix_tbl[] = {
	{1, 2, 1, 1, 2},
	{2, 3, 1, 1, 3},
	{3, 4, 2, 1, 2},
	{8, 10, 5, 1, 2},
	{9, 11, 5, 5, 11},
	{12, 14, 7, 1, 2}
};
static int n_pix = sizeof(pix_tbl) / sizeof(struct pix);


typedef struct scr {
	char title[MAX_TITLE + 1]; // string to display above the window
	int pix_size; // the size of the font
	int height; // number of rows
	int width; // number of columns
	int cur_buf; // the buffer to be displayed
	int buf[2][MAX_BUF]; // 256 color array
	int row_sc; // scalar to multiply the height by to get the column
	int col_sc; // scalar to multiply the width by to get the row
	int ppr; // the number of physical pixels per row 
	int ppc; // the number of physical pixels per column
	int win_id; // the unique id of the window from the OS
	char ttyname[MAX_TTYNAME + 1]; // the name of the terminal
	int fdr; // the read file descriptor for the terminal
	int fdw; // the write file descriptor for the terminal
	int cur_x; // the x coordinate of the screen pointer
	int cur_y; // the y coordinate of the screen pointer
	pthread_t read_th; // thread that continually reads from the terminal
	int in_buf[MAX_IN]; // buffer for input
	int in_pos; // position 
	struct termios term; // terminal properties
	struct termios pterm; // previous terminal properties
	int ttystate; // raw mode or normal mode
} Scr;
Scr scr;


static int check_args(char title[], int height, int width, int pix_size);
static int init_props(char title[], int height, int width, int pix_size);
static int init_term(void);
static int get_ttyname(void);
static int get_win_id(void);
static int set_win_props(void);
static int tty_raw(int fd);
static int tty_reset(int fd);
static int do_event_loop(void);
static void *read_loop(void *args);
static void print_in_buf(void);


int init_scr(char title[], int height, int width, int pix_size)
{
	if (!check_args(title, height, width, pix_size))
		return -1;
	
	if (init_props(title, height, width, pix_size) == -1)
		return -1;
	
	if (init_term() == -1)
		return -1;

	if (tty_raw(scr.fdr) == -1)
		return -1;
	
	if (do_event_loop() == -1)
		return -1;

	return 0;
}


int close_scr(void)
{
	int status;
	char cmd[100];

	pthread_cancel(scr.read_th);
	pthread_join(scr.read_th, NULL);
	
	tty_reset(scr.fdr);
	print_in_buf();
	sleep(3);

	sprintf(cmd, EXIT_WIN, scr.win_id);
	status = system(cmd);
	if (!WIFEXITED(status))
		return -1;
	
	return 0;
}


int switch_buf(void)
{
	scr.cur_buf = (scr.cur_buf + 1) % 2;

	return scr.cur_buf;
}


int color(int y, int x, int col)
{
	if (y > scr.height || y < 0)
		return -1;

	if (x > scr.width || x < 0)
		return -1;

	if (col > MAX_COL || col < 0)
		return -1;

	scr.buf[scr.cur_buf][y * scr.width + x] = col;

	return 0;
}
	

int display(void)
{
	int i, j, n, col;
	char cmd[20];

	for (i = 0; i < scr.height; i++)
		for (j = 0; j < scr.width; j++) {
			/* move the cursor. top left corner is 1,1 */
			n = sprintf(cmd, "\x1b[%d;%dH", i + 1, j + 1);
			write(scr.fdw, cmd, n);

			/* color the pixel */
			col = scr.buf[scr.cur_buf][i * scr.width + j];
			n = sprintf(cmd, "\x1b[48;5;%dm \x1b[0m", col);
			write(scr.fdw, cmd, n);
		}

	// delete this later
	// cursor color
	n = sprintf(cmd, "\x1b]12;red\x1b\\");
			//"\x1b]12;blue\x1b\x5C");
	write(scr.fdw, cmd, n);
		
	return 0;
}


static int check_args(char title[], int height, int width, int pix_size)
{
	int i, flag = 0;

	if (strnlen(title, MAX_TITLE + 1) > MAX_TITLE)
		return 0;

	if (height > MAX_HEIGHT || height < MIN_HEIGHT)
		return 0;

	if (width > MAX_WIDTH || width < MIN_WIDTH)
		return 0;

	for (i = 0; i < n_pix; i++)
		if (pix_size == pix_tbl[i].size) {
			flag = 1;
			break;
		}
	if (flag == 0)
		return 0;

	return 1;
}


static int init_props(char title[], int height, int width, int pix_size)
{
	int i, j;

	strcpy(scr.title, title); 
	scr.pix_size = pix_size;
	scr.height = height;
	scr.width = width;
	scr.cur_buf = 0;
	for (i = 0; i < height; i++)
		for (j = 0; j < width; j++)
			scr.buf[0][i * width + j] = scr.buf[1][i * width + j] = WHITE;
	for (i = 0; i < n_pix; i++) {
		if (pix_tbl[i].size == pix_size) {
			scr.row_sc = pix_tbl[i].row_sc; 
			scr.col_sc = pix_tbl[i].col_sc; 
			scr.ppr = pix_tbl[i].ppr; 
			scr.ppc = pix_tbl[i].ppc; 
		}
	}
	for (i = 0; i < MAX_IN; i++)
	   scr.in_buf[i] = -1;
	scr.win_id = -1;
	scr.ttyname[0] = '\0';
	scr.fdr = STDIN_FILENO;
	scr.fdw = STDOUT_FILENO;;
	scr.cur_x = scr.cur_y = 1;
	scr.in_pos = 0;
	scr.ttystate = RESET;

	return 0;
}


static int init_term(void)
{
	/* Get the terminal's name and the id of the terminal's window. Then set
	 * the title, height, width and pixel size.
	 */

	if (get_ttyname() == -1)
		return -1;

	if (get_win_id() == -1)
		return -1;

	if (set_win_props() == -1)
		return -1;

	return 0;
}


static int get_ttyname(void)
{
	char *name;

	if ((name = ttyname(scr.fdr)) == NULL)
		return -1;
	strncpy(scr.ttyname, name, MAX_TTYNAME + 1);

	return 0;
}


static int get_win_id(void)
{
	/* Loop through the windows and get the id of the window whose tty is the
	 * one for the screen.
	 */

	FILE *out;
	char buf[32], cmd[100], ttyname[MAX_TTYNAME + 1];
	int n_win, i;

	if ((out = popen(CNT_WIN, "r")) == NULL)
		return -1;
	if (fgets(buf, 32, out) == NULL)
		return -1;
	if (pclose(out) == -1)
		return -1;
	n_win = atoi(buf);

	for (i = 0; i < n_win; i++) {
		sprintf(cmd, GET_WIN_TTY, i);
		if ((out = popen(cmd, "r")) == NULL)
			return -1;
		if (fgets(ttyname, 32, out) == NULL)
			return -1;
		if (pclose(out) == -1)
			return -1;
		/* fgets includes a newline character at the end */
		ttyname[strnlen(ttyname, MAX_TTYNAME + 1) - 1] = '\0';

		if (strncmp(scr.ttyname, ttyname, MAX_TTYNAME + 1) == 0) {
			// Found the window
			sprintf(cmd, GET_WIN_ID, i);
			if ((out = popen(cmd, "r")) == NULL)
				return -1;
			if (fgets(buf, 32, out) == NULL)
				return -1;
			if (pclose(out) == -1)
				return -1;
			scr.win_id = atoi(buf);
			break;
		}
	}

	return 0;
}


/*
static int conn_win(void)
{
	if ((scr.fdr = open(scr.tty, O_RDONLY)) == -1)
		return -1;
	
	if ((scr.fdw = open(scr.tty, O_WRONLY)) == -1)
		return -1;

	if (set_win_props() == -1)
		return -1;

	if (tty_raw(scr.fdr) == -1)
		return -1;
	
//	if (tty_raw(scr.fdw) == -1)
//		return -1;
	
	if (write(scr.fdw, ENTER_ALT_SCREEN, strlen(ENTER_ALT_SCREEN)) == -1)
		return -1;

	return 0;
}
*/

static int tty_raw(int fd)
{
	int err;

	if (scr.ttystate != RESET) {
		errno = EINVAL;
		return -1;
	}
	
	tcflush(fd, TCIOFLUSH);

	if (tcgetattr(fd, &scr.term) < 0)
		return -1;
	scr.pterm = scr.term;

	/* Echo off, canonical mode off, extended input processing off, signal
	 * chars off;
	 */
	scr.term.c_lflag &= ~(ECHO|ICANON|IEXTEN|ISIG);

	/* No SIGINT on BREAK, turn off CR to NL, input parity check off, don't
	 * strip the 8th bit of the input, output flow control off.
	 */
	scr.term.c_iflag &= ~(BRKINT|ICRNL|INPCK|ISTRIP|IXON);

	/* Clear size bits, parity checking off */
	scr.term.c_cflag &= ~(CSIZE|PARENB);

	/*set 8 bits/char */
	scr.term.c_cflag |= CS8;

	/* output processing is off */
	scr.term.c_oflag &= ~OPOST;

	/* 1 byte at a time, no timer */
	scr.term.c_cc[VMIN] = 1;
	scr.term.c_cc[VTIME] = 0;
	if (tcsetattr(fd, TCSANOW, &scr.term) == -1)
		return -1;

	if (tcgetattr(fd, &scr.term) < 0) {
		err = errno;
		tcsetattr(fd, TCSANOW, &scr.pterm);
		errno = err;
		return -1;
	}
	if ((scr.term.c_lflag & (ECHO|ICANON|IEXTEN|ISIG)) ||
		(scr.term.c_iflag & (BRKINT|ICRNL|INPCK|ISTRIP|IXON)) ||
		(scr.term.c_cflag & (CSIZE|PARENB|CS8)) != CS8 ||
		(scr.term.c_oflag & OPOST) || scr.term.c_cc[VMIN] != 1 ||
		scr.term.c_cc[VTIME] != 0) {
		// only some changes were made
		tcsetattr(fd, TCSANOW, &scr.pterm);
		errno = EINVAL;
		return -1;
	}
	
	scr.ttystate = RAW;
	
	return 0;
}

int tty_reset(int fd)
{
	if (scr.ttystate == RESET)
		return 0;
	if (tcsetattr(fd, TCSANOW, &scr.pterm) < 0)
		return -1;
	scr.ttystate = RESET;

	return 0;
}


static int set_win_props(void)
{
	int status;
	char cmd[100];

	/* pixel size */
	sprintf(cmd, SET_FONT_SIZE, scr.win_id, scr.pix_size);
	status = system(cmd);
	if (!WIFEXITED(status))
		return -1;

	/* height */
	sprintf(cmd, SET_WIN_ROWS, scr.win_id, scr.height);
	status = system(cmd);
	if (!WIFEXITED(status))
		return -1;

	/* width */
	sprintf(cmd, SET_WIN_COLS, scr.win_id, scr.width);
	status = system(cmd);
	if (!WIFEXITED(status))
		return -1;

	/* title */
	sprintf(cmd, SET_WIN_TITLE, scr.win_id, scr.title);
	status = system(cmd);
	if (!WIFEXITED(status))
		return -1;

	return 0;
}


static int do_event_loop(void)
{
	/* Continually reads from the terminal and generates events */
	if (pthread_create(&scr.read_th, NULL, read_loop, NULL) != 0)
		return -1;

	return 0;
}


static void *read_loop(void *args)
{
	char ch;

	while (1) {
		read(scr.fdr, &ch, 1);
	   	scr.in_buf[scr.in_pos] = ch;
		scr.in_pos = (scr.in_pos + 1) % MAX_IN;
	}

	return NULL;
}


static void print_in_buf(void)
{
	int i;

	for (i = 0; i < 12; i++)
		printf("%#x", scr.in_buf[i]);
	printf("\n");
}

/*
static int print_win_size(void)
{
	struct winsize size; ws_row, ws_col, ws_ypixel, ws_xpixel

	ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);
	printf("Rows %d\n", size.ws_row);
	printf("Cols %d\n", size.ws_col);
	printf("Height %d\n", size.ws_ypixel);
	printf("Width %d\n", size.ws_xpixel);

	return 0;
}
*/

/*
static int save_pwin(void)
{
	FILE *fp;
	char buf[32];
	*/
	//struct winsize size; /* ws_row, ws_col, ws_ypixel, ws_xpixel */

	/*
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);
	pwin.rows = size.ws_row;
	pwin.cols = size.ws_col;
	pwin.height = size.ws_ypixel;
	pwin.width = size.ws_xpixel;
	*/

	/* font name */
	//fp = popen(GET_FONT_NAME, "r");
	//fgets(pwin.font_name, 32, fp);
	//pwin.font_name[strlen(pwin.font_name) - 1] = '\0';
	//pclose(fp);

	/* font size */
	//fp = popen(GET_FONT_SIZE, "r");
	//fgets(buf, 32, fp);
	//pwin.font_size = atoi(buf);
	//pclose(fp);

	//return 0;
//}



//static int set_scr(int height, int width, int res)
//{
	//int i, j;

	//scr.height = height;
	//scr.width = width;
	//scr.cur_buf = 0;
	//for (i = 0; i < height; i++)
		//for (j = 0; j < width; j++)
			//scr.buf[0][i][j] = scr.buf[1][i][j] = 0;
	//for (i = 0; i < n_fonts; i++) {
		//if (font_tbl[i].size == res) {
			//scr.row_sc = font_tbl[i].row_sc; 
			//scr.col_sc = font_tbl[i].col_sc; 
			//scr.ppr = font_tbl[i].ppr; 
			//scr.ppc = font_tbl[i].ppc; 
		//}
	//}

	//return 0;
//}


	// struct scr_t {
//	int height; /* logical height of the screen */
//	int width; /* logical width of the screen */
//	int cur_buf;
//	int buf[2][SCREEN_MAX_HEIGHT][SCREEN_MAX_WIDTH]; /* rgb array */
//	int height_sc; /* scalar to multiply the height by to get the column */
//	int width_sc; /* scalar to multiply the width by to get the row */
//	int row_res; /* the number of pixels per row */
//	int col_res; /* the number of pixels per column */
//int init_window(void)
//{
	//int err = 0;
	//FILE *out;
	//char buf[32];
//
	///* window dimensions. */
	//if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &size) < 0) {
		//err = -1;
		//s_win.rows = -1;
		//s_win.cols = -1;
		//s_win.height = -1;
		//s_win.width = -1;
		//s_win.x = -1;
		//s_win.y = -1;
	//}
	//else {
		//s_win.rows = size.ws_row;
		//s_win.cols = size.ws_col;
		//s_win.height = size.ws_ypixel;
		//s_win.width = size.ws_xpixel;
		//s_win.x = size.ws_ypixel / size.row;
		//s_win.y = size.ws_xpixel / size.row;
//
	///* font name */
	//if ((out = popen(GET_FONT_NAME, "r")) == NULL)
		//err = -1;
	//else 
		//if (fgets(pwin.font_name, 32, out) == NULL)
			//err = -1;
	//if (pclose(out) == -1)
		//err = -1;
	//
	///* font size */
	//if ((out = popen(GET_FONT_SIZE, "r")) == NULL)
		//err = -1;
	//else 
		//if (fgets(buf, 32, out) != NULL)
			//pwin.font_size = atoi(buf);
		//else
			//err = -1;
	//if (pclose(out) == -1)
		//err = -1;
//
	//// printf("\x1b[8;30;70;t");
	////printf("\x1b]2;My-Dumb-Window\a");
	////system("date");
	////printf("\x1b[21;t hi\n");
	//// if ((f = 
	//// fopen("./sd.txt", "w")) == NULL) {
		//// printf("Error opening file.\n");
		//// exit(-1);
	//// }
//
	//printf("%d rows, %d cols\n", pwin.size.ws_row, pwin.size.ws_col);
	//printf("%d row pixels, %d col pixels\n", pwin.size.ws_ypixel, pwin.size.ws_xpixel);
	//printf("%s", pwin.font_name);
	//printf("%d\n", pwin.font_size);
//
	//struct timespec t;
	//long one, two;
	//clock_gettime(CLOCK_THREAD_CPUTIME_ID, &t);
	//one = t.tv_nsec;
	//printf("%ld\n", one);
	//clock_gettime(CLOCK_THREAD_CPUTIME_ID, &t);
	//two = t.tv_nsec;
	//printf("%ld\n", two - one);
//
//
	//return err;
//}

// Window *new_window(int nrows, int ncols, int row, int col)
// {
	// return NULL;
// }

	/* Need to retrieve the id and terminal name of the new window in order to
	 * read and write to it via stdin and stdout. First, we get the ids and
	 * teminal names for all open windows. Then we create the new window and
	 * see what the new id and new terminal name is.
	FILE *out;
	char buf[32], cmd[100], tty[10][32];
	int n_win, win_id[10], i, j, n, id, flag, status;
	if (flag) {
		ttyname[strnlen(ttyname, MAX_TTYNAME) - 1] = '\0';
		strcpy(scr.tty, ttyname);
		scr.win_id = id;
		scr.cur_x = scr.cur_y = 0;
	}


	Get the existing terminal windows id and name
	if ((out = popen(CNT_WIN, "r")) == NULL)
		return -1;
	if (fgets(buf, 32, out) == NULL)
		return -1;
	if (pclose(out) == -1)
		return -1;
	n_win = atoi(buf);

	for (i = 0; i < n_win; i++) {
		sprintf(cmd, GET_WIN_TTY, i);
		if ((out = popen(cmd, "r")) == NULL)
			return -1;
		if (fgets(tty[i], 32, out) == NULL)
			return -1;
		if (pclose(out) == -1)
			return -1;

		sprintf(cmd, GET_WIN_ID, i);
		if ((out = popen(cmd, "r")) == NULL)
			return -1;
		if (fgets(buf, 32, out) == NULL)
			return -1;
		if (pclose(out) == -1)
			return -1;
		win_id[i] = atoi(buf);
	}

	Create the new terminal window/
	status = system(NEW_WIN);
	if (!WIFEXITED(status))
		return -1;

	Get the terminal and id of the new window
	if ((out = popen(CNT_WIN, "r")) == NULL)
		return -1;
	if (fgets(buf, 32, out) == NULL)
		return -1;
	if (pclose(out) == -1)
		return -1;
	n = atoi(buf);

	for (i = 0; i < n; i++) {
		flag = 1;

		sprintf(cmd, GET_WIN_TTY, i);
		if ((out = popen(cmd, "r")) == NULL)
			return -1;
		if (fgets(ttyname, 32, out) == NULL)
			return -1;
		if (pclose(out) == -1)
			return -1;

		sprintf(cmd, GET_WIN_ID, i);
		if ((out = popen(cmd, "r")) == NULL)
			return -1;
		if (fgets(buf, 32, out) == NULL)
			return -1;
		if (pclose(out) == -1)
			return -1;
		id = atoi(buf);

		for (j = 0; j < n_win; j++) {
			if (strcmp(ttyname, tty[j]) == 0) {
				flag = 0;
				break;
			}
		}

		if (flag) {
			ttyname[strnlen(ttyname, MAX_TTYNAME) - 1] = '\0';
			strcpy(scr.tty, ttyname);
			scr.win_id = id;
			scr.cur_x = scr.cur_y = 0;
		}

	}

	return 0;
}
*/
