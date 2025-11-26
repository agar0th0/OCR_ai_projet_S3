#include <stdlib.h>
#include "queue.h"

void queue_int(Queue *q) {
	q->front = q->back = NULL;
	q->size = 0;
}

void queue_push(Queue *q, int value) {
	QueueNode *node = malloc(sizeof(QueueNode));
	node->value = value;
	node->next = NULL;
	
	if (q->back)
		q->back->next = node;
	else
		q->front = node;

	q->back = node;
	q->size++;
}

int queue_pop(Queue *p) {
	if (!q->front) return -1;

	QueueNode *node = q->front;
	int value = node->value;

	q->front = node->next;
	if (!q->front)
		q->back = NULL;

	free(node);
	q->size--;
	return value;
}

int queue_empty(Queue *q) {
	return q->size == 0;
}

void queue_free(Queue *q) {
	while(!queue_empty(q))
		queue_pop(q);
}
