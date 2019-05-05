/* queue.h - interface for a generic queue */

#ifndef QUEUE_H
#define QUEUE_H


#define MAXQUEUE 1000

typedef void *Item;

typedef struct node {
	Item item;
	struct node *next;
} Node;

typedef struct queue {
	Node *front;
	Node *rear;
	int items;
} Queue;


int init_queue(Queue *pq);
int is_queue_full(const Queue *pq);
int is_queue_empty(const Queue *pq);
int count_queue_items(const Queue *pq);
int enqueue(Item item, Queue *pq);
int dequeue(Item *pitem, Queue *pq);
int empty_queue(Queue *pq);


#endif
