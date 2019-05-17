/* queue.h - interface for a queue that stores integers. */
#ifndef QUEUE_H
#define QUEUE_H

#include <pthread.h>

#define MAX_QUEUE_SIZE 1024

typedef struct queue {
	int size;
	int front;
	int back;
	int items[MAX_QUEUE_SIZE];
	pthread_mutex_t lock;
} Queue;

int init_queue(Queue *queue);
int enqueue(Queue *queue, int item);
int dequeue(Queue *queue, int *item);
int queue_size(Queue *queue);
int peak(Queue *queue, int *item, int pos);
int is_queue_full(Queue *queue);
int is_queue_empty(Queue *queue);
int lock_queue(Queue *queue);
int unlock_queue(Queue *queue);

#endif //QUEUE_H
