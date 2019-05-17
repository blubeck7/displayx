#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include "../inc/term.h"


#define KEY_EVENT 0

struct ascii_key {
	int code;
	char *name;
	char *alt_name;
};	

const struct ascii_key ascii[] = {
	{0, "NULL", "CTRL@"},
	{1, "SOH", "CTRLA"},
	{2, "STX", "CTRLB"},
	{3, "ETX", "CTRLC"},
	{4, "EOT", "CTRLD"},
	{5, "ENQ", "CTRLE"},
	{6, "ACK", "CTRLF"},
	{7, "BEL", "CTRLG"},
	{8, "BS", "CTRLH"},
	{9, "HT", "CTRLI"},
	{10, "LF", "CTRLJ"},
	{11, "VT", "CTRLK"},
	{12, "FF", "CTRLL"},
	{13, "CR", "CTRLM"},
	{14, "SO", "CTRLN"},
	{15, "SI", "CTRLO"},
	{16, "DLE", "CTRLP"},
	{17, "DC1", "CTRLQ"},
	{18, "DC2", "CTRLR"},
	{19, "DC3", "CTRLS"},
	{20, "DC4", "CTRLT"},
	{21, "NAK", "CTRLU"},
	{22, "SYN", "CTRLV"},
	{23, "ETB", "CTRLW"},
	{24, "CAN", "CTRLX"},
	{25, "EM", "CTRLY"},
	{26, "SUB", "CTRLZ"},
	{27, "ESC", "CTRL["},
	{28, "FS", "CTRL\\"},
	{29, "GS", "CTRL]"},
	{30, "RS", "CTRL^"},
	{31, "US", "CTRL_"},
	{32, "SPACE", " "},
	{33, "!", "EXCLAMATION MARK"},
	{34, "\"", "QUOTES"},
	{35, "#", "HASH"},
	{36, "$", "DOLLAR"},
	{37, "%", "PERCENT"},
	{38, "&", "AMPERSAND"},
	{39, "&", "APOSTROPHE"},
	{40, "(", "OPEN PARENTHESIS"},
	{41, ")", "CLOSE PARENTHESIS"},
	{42, "*", "ASTERISK"},
	{43, "+", "PLUS"},
	{44, ",", "COMMA"},
	{45, "-", "DASH"},
	{46, ".", "PERIOD"},
	{47, "/", "FORWARD SLASH"},
	{48, "0", "ZERO"},
	{49, "1", "ONE"},
	{50, "2", "TWO"},
	{51, "3", "THREE"},
	{52, "4", "FOUR"},
	{53, "5", "FIVE"},
	{54, "6", "SIX"},
	{55, "7", "SEVEN"},
	{56, "8", "EIGHT"},
	{57, "9", "NINE"},
	{58, ":", "COLON"},
	{59, ";", "SEMICOLON"},
	{60, "<", "LESS THAN"},
	{61, "=", "EQUAL"},
	{62, ">", "GREATER THAN"},
	{63, "?", "QUESTION MARK"},
	{64, "@", "AT"},
	{65, "A", "A"},
	{66, "B", "B"},
	{67, "C", "C"},
	{68, "D", "D"},
	{69, "E", "E"},
	{70, "F", "F"},
	{71, "G", "G"},
	{72, "H", "H"},
	{73, "I", "I"},
	{74, "J", "J"},
	{75, "K", "K"},
	{76, "L", "L"},
	{77, "M", "M"},
	{78, "N", "N"},
	{79, "O", "O"},
	{80, "P", "P"},
	{81, "Q", "Q"},
	{82, "R", "R"},
	{83, "S", "S"},
	{84, "T", "T"},
	{85, "U", "U"},
	{86, "V", "V"},
	{87, "W", "W"},
	{88, "X", "X"},
	{89, "Y", "Y"},
	{90, "Z", "Z"},
	{91, "[", "OPEN BRACKET"},
	{92, "\\", "BACKSLASH"},
	{93, "]", "CLOSE BRACKET"},
	{94, "^", "CARET"},
	{95, "_", "UNDERSCORE"},
	{96, "`", "ACCENT GRAVE"},
	{97, "a", "a"},
	{98, "b", "b"},
	{99, "c", "c"},
	{100, "d", "d"},
	{101, "e", "e"},
	{102, "f", "f"},
	{103, "g", "g"},
	{104, "h", "h"},
	{105, "i", "i"},
	{106, "j", "j"},
	{107, "k", "k"},
	{108, "l", "l"},
	{109, "m", "m"},
	{110, "n", "n"},
	{111, "o", "o"},
	{112, "p", "p"},
	{113, "q", "q"},
	{114, "r", "r"},
	{115, "s", "s"},
	{116, "t", "t"},
	{117, "u", "u"},
	{118, "v", "v"},
	{119, "w", "w"},
	{120, "x", "x"},
	{121, "y", "y"},
	{122, "z", "z"},
	{123, "{", "OPEN BRACE"},
	{124, "|", "PIPE"},
	{125, "}", "CLOSE BRACE"},
	{126, "~", "TILDE"},
	{127, "DEL", "DELETE"}};

typedef struct event {
	int type;
	int code;
} Event;

Event event;

pthread_mutex_t event_mutex = PTHREAD_MUTEX_INITIALIZER;

void *read_loop(void *args);

void sig_usr1(int signo)
{
	if (signo != SIGUSR1)
		return;
	printf("Thread %lu\n", pthread_self());
	pthread_mutex_lock(&event_mutex);
	printf("Received event %d, %s\n", event.code, ascii[event.code].name);
	pthread_mutex_unlock(&event_mutex);

	signal(SIGUSR1, sig_usr1);

	return;
}

static void sig_catch(int signo)
{
	printf("signal %d caught\n", signo);
	tty_reset(STDIN_FILENO);
	exit(0);
}

int main(void)
{
	pthread_t thread1;
	void *thread1_ret;
	int stop = 0;

	if (signal(SIGUSR1, sig_usr1) == SIG_ERR)
		return -1;

	if (signal(SIGINT, sig_catch) == SIG_ERR)
		return -1;
	if (signal(SIGQUIT, sig_catch) == SIG_ERR)
		return -1;
	if (signal(SIGTERM, sig_catch) == SIG_ERR)
		return -1;

	if (tty_raw(STDIN_FILENO) < 0) {
		printf("raw error\n");
		return -1;
	}

	if (pthread_create(&thread1, NULL, read_loop, NULL) != 0)
		return -1;

	printf("Main Thread %lu\x0a\x0d", pthread_self());
	while (!stop)
		;
	
	if (pthread_join(thread1, &thread1_ret) != 0)
		return -1;

	tty_reset(STDIN_FILENO);
	printf("Thread exit %p\n", thread1_ret);

	return 0;
}

void *read_loop(void *args)
{
	char c;

	printf("Read Thread %lu\x0a\x0d", pthread_self());
	while (1) {
		if (read(STDIN_FILENO, &c, 1) == -1) {
			printf("Read loop error. Exiting the thread.\n");
			return (void *)1;
		}
		if (c >= 0 && c < 128) {
			if (c == 0) {
				printf("Exiting read loop...\n");
				return NULL;
			}
			pthread_mutex_lock(&event_mutex);
			event.type = KEY_EVENT;
			event.code = c;
			pthread_mutex_unlock(&event_mutex);
		}
		raise(SIGUSR1);
	}

	return NULL;
}
