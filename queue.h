#ifndef _QUEUE_H_
#define _QUEUE_H_

#include "task.h"

#ifdef __cplusplus
extern "C" {
#endif

/* queue to share data */ 
typedef struct Queue{
    int sizeQueue;       /* size of queue */
    int sizeElements;    /* size of elements to work */
    int *data;          /* array of element int */
    float *value;          /* array of element float*/
    char *mensg;          /* array of element char*/
}Queue;

/*-------------------------------------------Queue-------------------------------------------------*/
/* create a queue to share date */
struct Queue createQueue(int sizeQueue, int sizeElements);
/* loss the last element of the queue data[sizeQueue-1] and write on the first data[0]. Obs: begin of queue [last element of array] or the first in*/
void queueWrite(Queue *queue, void *value);
/* Add elements to the begin of the queue and increase the queue*/
void queueSend(Queue *queue, void *value);
/* FIFO first in first out return the last element to in */
void *queueRead(Queue *queue);
int queueReadInt(Queue *queue);
float queueReadFloat(Queue *queue);
char queueReadChar(Queue *queue);
/* receive a element of queue and remove the element */
void *queueReceive(Queue *queue);
int queueReceiveInt(Queue *queue);
float queueReceiveFloat(Queue *queue);
char queueReceiveChar(Queue *queue);
/*--------------------------------------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* _QUEUE_H_ */
