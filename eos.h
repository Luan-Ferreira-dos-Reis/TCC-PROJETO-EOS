#ifndef _EOS_H_
#define _EOS_H_

#include <stdlib.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include "Arduino.h"
#include "assembly.h"
#include "timers.h"
#include "task.h"
#include "semaphore.h"
#include "queue.h"

/*-------------------------------------------System configuration-------------------------------------------------- */
/* Max number of tasks supported */
#ifndef MAXTASKS
#define MAXTASKS 32            
#endif

/* Max stacksize for a task */
#ifndef TASKMAXSTACKSIZE
#define TASKMAXSTACKSIZE 1024 
#endif

/* Min stacksize for a task */
#ifndef TASKMINSTACKSIZE
#define TASKMINSTACKSIZE 512  
#endif

/* debug option */
/*#ifndef DEBUG
#define DEBUG 0  
#endif*/

/* We need to calculate a proper value to load the timer counter.
 * The following loads the value 131 into the Timer 2 counter register
 * The math behind this is:
 * (CPU frequency = 16E6) / (prescaler value = 128) = 125000 Hz = 8 us.
 * (desired period = 1000 us) / 8 us = 125.
 * MAX(uint8) + 1 - 125 = 131;
 */

/**
 * MACROS
 */
#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))
/* Return lower/higher 8 bits of 16 bit value */
#define lower8(X)  ((unsigned char)((unsigned int)(X)))
#define upper8(X) ((unsigned char)((unsigned int)(X) >> 8))
/* Performs a context switch by setting stack pointer among tasks */
#define eos_context_switch() do { \
    if (current_task != NULL && task_queue != NULL && \
        current_task != task_queue) { \
        current_task->sp_low = SPL; \
        current_task->sp_high = SPH; \
        current_task = task_queue; \
        task_queue = task_queue->next; \
        SPL = current_task->sp_low; \
        SPH = current_task->sp_high; \
    } } while(0)

/* Removes  e the current task from task queue for in case the current task is complete */
#define DEQUEUE() do { \
  if (task_queue->prev == task_queue->next) \
        task_queue = NULL; \
    else if (current_task == task_queue) \
        task_queue = current_task->next; \
    current_task->state = FINISHED; \
    current_task->prev->next = current_task->next; \
    current_task->next->prev = current_task->prev; \
  } while(0)
 
/* Enable and disable preemption */   /* LUAN FERREIRA DOS REIS */
#define ENABLE_PREEMPT() do { \
  preempt = 1;\
  } while(0)

#define DISABLE_PREEMPT() do { \
  preempt = 0; \
  }  while(0)


/*------------------------------------------------------------------------------------------------------ */
/**
 * Method declarations
 */
/*--------------------------------------------Task-------------------------------------------------------*/
/* Creates a new thread */
int eos_create_task(eos_task *new_task, void (*runner)(void *runner_arg), void *arg, int size_stack); /* LUAN FERREIRA DOS REIS */
/* Switches between tasks */
void eos_switch_task(void) __attribute__ ((naked));
/* run task properly */
void make_callfunc(void);
/* Add an element in the back of the queue */
void eos_enqueue(struct eos_task *task);
/* Return an element from the front of the queue */
struct eos_task *eos_dequeue(struct eos_task *task);
/* idle_task*/
void idle_task(void *args);
/*-------------------------------------------------------------------------------------------------------*/
/*----------------------------------------Semaphores-----------------------------------------------*/
/* Creates a new semaphore */
struct eos_semaphore eos_create_semaphore(eos_semaphore *new_semaphore);
/* Take a created semaphore*/
int eos_semaphore_take(eos_semaphore* semaphore);
/* free a lock semaphore*/
void eos_semaphore_give(eos_semaphore* semaphore);
/*-------------------------------------------------------------------------------------------------*/
/*-------------------------------------------Queue-------------------------------------------------*/
/* create a queue to share date */
struct eos_queue eos_create_queue(eos_queue *q, int size_queue);
/* loss the last element of the queue data[sizeQueue-1] and write on the first data[0]. Obs: begin of queue [last element of array] or the first in*/
void eos_queue_write(eos_queue *q, void *value);
/* Add elements to the begin of the queue and increase the queue*/
void eos_queue_send(eos_queue *q, void *value);
/* FIFO first in first out return the last element to in */
int eos_queue_read(eos_queue *q);
float eos_queue_read_float(eos_queue *q);
char eos_queue_read_char(eos_queue *q);
/* receive a element of queue and remove the element */
int eos_queue_receive(eos_queue *q);
float eos_queue_receive_float(eos_queue *q);
char eos_queue_receive_char(eos_queue *q);
/*--------------------------------------------------------------------------------------------------*/
/*--------------------------------------------- Kernel ----------------------------------------------*/
void eos_initial();
/* Starts the Arduous kernel */
int eos_start(int ts, int max_delay) ;
/*---------------------------------------------------------------------------------------------------*/
///*--------------------------------------------debug functions----------------------------------------*/
///* Prints the task queue */
//void eos_print_queue(void);
///* Prints the task stack */
//void eos_print_stack(char *stack, int bytes);
/*---------------------------------------------------------------------------------------------------*/



#endif /* _EOS_H_ */
