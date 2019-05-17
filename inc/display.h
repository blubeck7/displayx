/* display.h
 *
 * This is the header file for a library that emulates a pixel screen using
 * xterm. There is a single, global instance of the screen. It has two buffers.
 * One of the buffers is the current buffer and the other buffer is the next
 * buffer. The current buffer is always the buffer that is displayed.
 */

#ifndef DISPLAY_H
#define DISPLAY_H

#define MAX_TITLE 32
#define MAX_HEIGHT 500
#define MAX_WIDTH 1000

int init_scr(char title[], int height, int width, int pix_size);
/* Initializes the screen.
 *
 * This function initializes a new terminal window to serve as the screen.
 *
 * title - the string to display across the top of the screen. The length of
 *  	the string, not including the null byte, must be less than or equal to
 *  	MAX_TITLE.
 * height - the height of the screen in pixels.
 * width - the width of the screen in pixels.
 * pix_size - the size of a pixel.
 *
 * Returns 0 if successful; -1 if an error occurred.
 */


int close_scr(void);
/* Closes the screen and exits the program.
 *
 * This function closes the screen and exits the program. It is the last thing
 * that should be called from main.
 *
 * Returns 0 if successful; -1 if an error occurred.
 */


//int switch_buf(void);
/* Switches the current buffer
 *
 * Returns the id of the new buffer; -1 if an error occurred.
 */


//int color(int y, int x, int col);
/* Colors the pixel located at y, x
 *
 * This function sets the color of the pixel located at coordinates y, x in the
 * current buffer.
 *
 * y - the y coordinate of the pixel
 * x - the x coordinate of the pixel
 * col - the color to set the pixel to
 *
 * Returns 0 if successful; -1 if an error occurred.
 */


//int draw(int *img, int buf);
/* Draws the image to the buffer. 
 *
 * img - a pointer to the image to draw.
 * buf - the buffer to draw to
 *
 * Returns 0 if successful; -1 if an error occurred.
 */


//int display(void);
/* Displays the current buffer.
 *
 * Returns 0 if successful; -1 if an error occurred.
 */


#endif /* DISPLAY_H */
