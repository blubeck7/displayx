#ifndef EVENT_H
#define EVENT_H

/* Events */
#define KEY_EVENT 0
#define MOUSE_EVENT 1

typedef struct event {
	int event; /* KEY OR MOUSE */	
	int code; // extended ascii code for key events
	int y; // y coord for mouse event
	int x; // x coord for mouse event
} Event;

typedef void EventHandler(Event *event);

int set_event_handler(EventHandler *event_handler);

#endif
