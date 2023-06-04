#ifndef _QUEUE_H_
#define _QUEUE_H_

#include "task.h"

#ifdef __cplusplus
extern "C" {
#endif

/* queue to share data */ 
typedef struct eos_queue{
    int size_queue;       /* size of queue */
    int size_elements;    /* size of elements to work */
    int *data;          /* array of element int */
    float *value;          /* array of element float*/
    char *mensg;          /* array of element char*/
}eos_queue;

/*-------------------------------------------Queue-------------------------------------------------*/
/* create a queue to share date */
struct eos_queue eos_create_queue(eos_queue *q, int size_queue, int size_elements);
/* loss the last element of the queue data[sizeQueue-1] and write on the first data[0]. Obs: begin of queue [last element of array] or the first in*/
void eos_queue_write(eos_queue *q, void *value);
/* Add elements to the begin of the queue and increase the queue*/
void eos_queue_send(eos_queue *q, void *value);
/* FIFO first in first out return the last element to in */
void *eos_queue_read(eos_queue *q);
int eos_queue_read_int(eos_queue *q);
float eos_queue_read_float(eos_queue *q);
char eos_queue_read_char(eos_queue *q);
/* receive a element of queue and remove the element */
void *eos_queue_receive(eos_queue *q);
int eos_queue_receive_int(eos_queue *q);
float eos_queue_receive_float(eos_queue *q);
char eos_queue_receive_char(eos_queue *q);
/*--------------------------------------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* _QUEUE_H_ */
