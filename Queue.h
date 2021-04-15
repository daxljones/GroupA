#ifndef QUEUE_H
#define QUEUE_H

#define QUEUE_SIZE 50

struct queue{
    int *queueList[QUEUE_SIZE];
    int tail;
    int head;
};

struct queue * createQueue();
int enqueue(struct queue *, int *);
int * dequeue(struct queue *);

#endif