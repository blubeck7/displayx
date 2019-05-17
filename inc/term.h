#ifndef TERM_H
#define TERM_H

/* Terminal Modes */
#define RESET_MODE 0
#define RAWB_MODE 1 //Blocking read
#define RAWN_MODE 2 //Nonblocking read

/* ANSI Escape sequence */
#define ENTER_ALT_SCREEN "\x1b[?1049h" 
#define EXIT_ALT_SCREEN "\x1b[?10491" 
#define CLEAR_SCREEN "\x1b[2J" 

int tty_rawn(int fd);
int tty_rawb(int fd);
int tty_reset(int fd);

#endif //TERM_H
