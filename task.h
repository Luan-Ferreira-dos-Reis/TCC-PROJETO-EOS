#ifndef _TASK_H_
#define _TASK_H_

#include "eos.h"


#ifdef __cplusplus
extern "C" {
#endif

/**
 * Data structures (eos_task == [eos-K]uous [k]ernel task)
 */
// function pointer
typedef void(*ptrFunc)(void*); 

enum state{READY = 0, RUNNING, FINISHED};   /* task state (ready = 0, running = 1, finished = 2*/
 
typedef struct eos_task{                /* A kernel task structure */
    int task_id;                        /* Unique identifier */
    ptrFunc code;                       /* pointer to task code */
    void *arg;                          /* argument */    /* LUAN FERREIRA DOS REIS */
    char *stack;                        /* pointer to task stack */
    int state;                          /* task state (ready = 0, running = 1, finished = 2*/
    struct eos_task *next;              /* Pointer to next task in queue */
    struct eos_task *prev;              /* Pointer to previous task */
    volatile char sp_low, sp_high;      /* Lower/higher 8 bit of StackPointer */
    volatile char pc_low, pc_high;      /* Lower/higher 8 bit of CounterProgramPointer */
}eos_task;

#ifdef __cplusplus
}
#endif

#endif /* _TASK_H_ */
