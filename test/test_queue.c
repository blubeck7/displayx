#include <stdio.h>
#include "../inc/queue.h"


int main(int argc, char *argv[])
{
	Queue queue;
	int i;
	
	init_queue(&queue);
	printf("The queue size is %d\n", queue_size(&queue));
	i = dequeue(&queue, NULL);
	printf("Return value is %d\n", i);

	enqueue(&queue, 77);
	enqueue(&queue, 44);
	enqueue(&queue, 55);
	enqueue(&queue, 102);
	enqueue(&queue, -1);
	printf("The queue size is %d\n", queue_size(&queue));
	dequeue(&queue, &i);
	printf("Front of the queue is %d\n", i);
	printf("The queue size is %d\n", queue_size(&queue));
	dequeue(&queue, &i);
	printf("Front of the queue is %d\n", i);
	printf("The queue size is %d\n", queue_size(&queue));
	peak(&queue, &i, 0);
	printf("Front of the queue is %d\n", i);
	printf("The queue size is %d\n", queue_size(&queue));
	dequeue(&queue, &i);
	dequeue(&queue, &i);
	dequeue(&queue, &i);
	printf("The queue size is %d\n", queue_size(&queue));

	for (i = 0; i < MAX_QUEUE_SIZE; i++)
		enqueue(&queue, i);
	printf("The queue size is %d\n", queue_size(&queue));
	i = enqueue(&queue, 1024);
	printf("Return value is %d\n", i);
	printf("The queue size is %d\n", queue_size(&queue));

	i = lock_queue(&queue);
	printf("Return value is %d\n", i);
	i = unlock_queue(&queue);
	printf("Return value is %d\n", i);


	return 0;
}

