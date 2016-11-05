#ifndef _QUEUE_ADT_H_
#define _QUEUE_ADT_H_

typedef void * qelem_t;

typedef struct queue * queueADT;

queueADT create_queue();

int is_empty(queueADT q);

void enqueue(queueADT q, qelem_t elem);

qelem_t dequeue(queueADT q);

#endif