#ifndef WINDOW_H
#define WINDOW_H

#define WIN_MAX_TITLE 32 
#define WIN_MAX_HEIGHT 500
#define WIN_MAX_WIDTH 1000

#ifdef __APPLE__
#define OSA "osascript -e \"tell app \\\"Terminal\\\" "
#define NEW_WINA "open -a Terminal.app res/screen.terminal"
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


typedef struct window Window;
//int destroy_window(void);

Window *open_window(void);
int close_window(Window *window);
int window_get_id(Window *window);
//int init_win(char title[], int height, int width, int pix_size);
//int win_get_ttyname(void);
//int win_set_props(void);

#endif //WINDOW_H
