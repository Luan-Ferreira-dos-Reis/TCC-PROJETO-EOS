#ifndef _QUEUE_H_
#define _QUEUE_H_



#ifdef __cplusplus
extern "C" {
#endif

/* queue to share data */ 
typedef struct eos_queue{
    int size_queue;       /* size of queue */
    int *data;          /* array of element int */
    float *value;          /* array of element float*/
    char *mensg;          /* array of element char*/
}eos_queue;

#ifdef __cplusplus
}
#endif

#endif /* _QUEUE_H_ */
