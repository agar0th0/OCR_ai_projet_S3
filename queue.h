#ifndef QUEUE_H
#define QUEUE_H

typedef struct QueueNode {
	int value;
	struct QueueNode *next;
} QueueNode;

typedef struct { 
	QueueNode *front;
	QueueNode *back;
	int size;
} Queue;

void queue_init(Queue *q);
void queue_push(Queue *q, int value);
int queue_pop(Queue *p);
int queue_empty(Queue *q);
void queue_free(Queue *p);

#endif
