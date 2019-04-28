#ifndef DISPLAY_H
#define DISPLAY_H


//typedef struct screen Screen;
/* Screen represents the pixel screen. There is a single, global instance of
 * the pixel screen. However, it has two buffers. One of the buffers is the
 * current buffer and the other buffer is the next buffer. The current buffer
 * is always the buffer that is displayed.
 */


int init_scr(char title[], int height, int width, int pix_size);
/* Initializes the screen.
 *
 * title - the string to display across the top of the screen, which is limited
 *  	to 32 characters
 * height - the height of the screen in pixels
 * width - the width of the screen in pixels
 * pix_size - the size of a pixel
 *
 * This function opens a new terminal window and changes the settings of the
 * terminal window to the given arguments. It then initializes its internal
 * state.
 *
 * Returns 0 if successful; -1 if an error occurred.
 */


int restore_scr(void);
/* Restores the screen.
 *
 * This function restores the screen to its settings before the program ran.
 *
 * Returns 0 if successful; -1 if an error occurred.
 */


//typedef struct window_t Window;
/* WINDOW is the primary type. It represents a two-dimensional array of pixels
 * that are simulated by using the character-based capabilities of the
 * terminal.
 */


//int init_window(void);
/* Initializes the window.
 *
 * This function intializes the window. It saves the state of the current
 * window so that it can be restored when the program exits and sets the state
 * of the new window.
 *
 * Returns -1 on error; 0 otherwise.
 */


//int move(Window *win, int row, int col);
/* Moves the cursor.
 *
 * win - pointer to a window.
 * row - row to move the cursor to.
 * col - column to move the cursir to.
 *
 * Returns ERROR if win is NULL or if (row, col) are outside the window's
 * dimensions.
 */
#endif /* DISPLAY_H */
