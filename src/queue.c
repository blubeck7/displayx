#include <stdio.h>
#include <stdlib.h>
#include "../inc/queue.h"


static void copy_to_node(Item item, Node *pn);
static void copy_to_item(Node *pn, Item *pi);

int init_queue(Queue *pq);
int is_queue_full(const Queue *pq);
int is_queue_empty(const Queue *pq);
int count_queue_items(const Queue *pq);
int enqueue(Item item, Queue *pq);
int dequeue(Item *pitem, Queue *pq);
int empty_queue(Queue *pq);
