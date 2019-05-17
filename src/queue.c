#include <pthread.h>
#include "../inc/queue.h"


int init_queue(Queue *queue)
{
	queue->size = queue->front = queue->back = 0;
	if (pthread_mutex_init(&queue->lock, NULL) != 0)
		return -1;

	return 0;
}

int enqueue(Queue *queue, int item)
{
	if (is_queue_full(queue))
		return -1;
	
	queue->items[queue->back] = item;
	queue->back = (queue->back + 1) % MAX_QUEUE_SIZE;
	queue->size++;

	return 0;
}

int dequeue(Queue *queue, int *item)
{
	if (is_queue_empty(queue))
		return -1;

	*item = queue->items[queue->front];
	queue->front = (queue->front + 1) % MAX_QUEUE_SIZE;
	queue->size--;

	return 0;
}

int queue_size(Queue *queue)
{
	return queue->size;
}

int peak(Queue *queue, int *item, int pos)
{
	if (pos < 0 || pos >= queue->size)
		return -1;

	*item = queue->items[queue->front + pos];

	return 0;
}

int is_queue_full(Queue *queue)
{
	if (queue->size == MAX_QUEUE_SIZE)
		return 1;

	return 0;
}

int is_queue_empty(Queue *queue)
{
	if (queue->size == 0)
		return 1;

	return 0;
}
	
int lock_queue(Queue *queue)
{
	if (pthread_mutex_lock(&queue->lock) != 0)
		return -1;

	return 0;
}

int unlock_queue(Queue *queue)
{
	if (pthread_mutex_unlock(&queue->lock) != 0)
		return -1;

	return 0;
}
