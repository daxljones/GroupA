#include "Queue.h"
#include "Definitions.h"

struct queue * createQueue()
{
    struct queue *q = malloc(sizeof(struct queue));

    q->head = 0;
    q->tail = 0;

    return q;
}

int enqueue(struct queue *q, int *clientConnection)
{
    if((q->tail + 1) % QUEUE_SIZE == q->head)
    {
        printf("--QUEUE IS FULL!--\n");
        return -1;
    }

    q->queueList[q->tail] = clientConnection;
    q->tail = (q->tail + 1) % QUEUE_SIZE;

    return 1;
}

int * dequeue(struct queue *q)
{
    int *tbr;

    if(q->head == q->tail)
    {
        //printf("--Nothing in queue!--\n");
        *tbr = -1;
        return tbr;
    }

    tbr = q->queueList[q->head];

    q->head = (q->head + 1) % QUEUE_SIZE;

    return tbr;

}