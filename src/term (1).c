#define _POSIX_C_SOURCE 200809L

#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

static struct termios save_termios;
static int ttysavefd = -1;
static enum {RESET, RAW, CBREAK} ttystate = RESET;

int tty_raw(int fd);
int tty_reset(int fd);

int tty_raw(int fd) //put terminal into raw mode
{
	int err;
	struct termios buf;

	if (ttystate != RESET) {
		errno = EINVAL;
		return -1;
	}
	
	tcflush(fd, TCIOFLUSH);

	if (tcgetattr(fd, &buf) < 0)
		return -1;
	save_termios = buf;

	/* Echo off, canonical mode off, extended input processing off, signal
	 * chars off;
	 */
	buf.c_lflag &= ~(ECHO|ICANON|IEXTEN|ISIG);

	/* No SIGINT on BREAK, turn off CR to NL, input parity check off, don't
	 * strip the 8th bit of the input, output flow control off.
	 */
	buf.c_iflag &= ~(BRKINT|ICRNL|INPCK|ISTRIP|IXON);

	/* Clear size bits, parity checking off */
	buf.c_cflag &= ~(CSIZE|PARENB);

	/*set 8 bits/char */
	buf.c_cflag |= CS8;

	/* output processing is off */
	buf.c_oflag &= ~OPOST;

	/* 1 byte at a time, no timer */
	buf.c_cc[VMIN] = 1;
	buf.c_cc[VTIME] = 0;
	if (tcsetattr(fd, TCSANOW, &buf) == -1)
		return -1;

	if (tcgetattr(fd, &buf) < 0) {
		err = errno;
		tcsetattr(fd, TCSANOW, &save_termios);
		errno = err;
		return -1;
	}
	if ((buf.c_lflag & (ECHO|ICANON|IEXTEN|ISIG)) ||
		(buf.c_iflag & (BRKINT|ICRNL|INPCK|ISTRIP|IXON)) ||
		(buf.c_cflag & (CSIZE|PARENB|CS8)) != CS8 ||
		(buf.c_oflag & OPOST) || buf.c_cc[VMIN] != 1 ||
		buf.c_cc[VTIME] != 0) {
		// only some changes were made
		tcsetattr(fd, TCSANOW, &save_termios);
		errno = EINVAL;
		return -1;
	}
	
	ttystate = RAW;
	ttysavefd = fd;
	
	return 0;
}

int tty_reset(int fd)
{
	if (ttystate == RESET)
		return 0;
	if (tcsetattr(fd, TCSANOW, &save_termios) < 0)
		return -1;
	ttystate = RESET;

	return 0;
}
