#ifndef _TASK_H_
#define _TASK_H_

#include <stdlib.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include "Arduino.h"
#include "assembly.h"
#include "timers.h"
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
#define TASKMINSTACKSIZE 128  
#endif

/* Min stacksize for a task */
#ifndef PORTMAXDELAY
#define PORTMAXDELAY 25000  
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
init: if (current_task != NULL && task_queue[current_layer] != NULL && \
          current_task != task_queue[current_layer]) { \
          current_task->sp_low = SPL; \
          current_task->sp_high = SPH; \
          current_task = task_queue[current_layer]; \
          task_queue[current_layer] = task_queue[current_layer]->next; \
          SPL = current_task->sp_low; \
          SPH = current_task->sp_high;} \
      else if(current_task == NULL && task_queue[current_layer] != NULL){ \
          current_task = next_task; \
          goto init; \
        } \
      else if(task_queue[current_layer] == NULL && current_layer < layers_priority){ \
        current_layer++; \
        goto init; \
     } } while(0)

/* Removes  e the current task from task queue for in case the current task is complete */
#define DEQUEUE() do { \
  if (task_queue[current_layer]->prev == task_queue[current_layer]->next) \
        task_queue[current_layer] = NULL; \
    else if (current_task == task_queue[current_layer]) \
        task_queue[current_layer] = current_task->next; \
    current_task->prev->next = current_task->next; \
    current_task->next->prev = current_task->prev; \
    next_task = current_task; \
    current_task = NULL; \
  } while(0)
 
/* Enable and disable preemption */   /* LUAN FERREIRA DOS REIS */
#define ENABLE_PREEMPT() do { \
  preempt = 1;\
  } while(0)

#define DISABLE_PREEMPT() do { \
  preempt = 0; \
  }  while(0)

#ifdef __cplusplus
extern "C" {
#endif
/**
 * Data structures (eos_task == [eos-K]uous [k]ernel task)
 */
/* function pointer */
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

/*--------------------------------------------Task-------------------------------------------------------*/
/* Creates a new thread */
int eos_create_task(eos_task *new_task, void (*runner)(void *runner_arg), void *arg, int size_stack, int priority); /* LUAN FERREIRA DOS REIS */
/* Switches between tasks */
void eos_switch_task(void) __attribute__ ((naked));
/* run task properly */
void make_callfunc(void);
/* Add an element in the back of the queue */
void eos_enqueue(struct eos_task *task, int layer);
/* Return an element from the front of the queue */
struct eos_task *eos_dequeue(struct eos_task *task, int layer);
/* idle_task*/
void idle_task(void *args);
/*--------------------------------------------- Kernel ----------------------------------------------*/
void eos_initial(int layers);
/* Starts the Arduous kernel */
int eos_start(int ts, int max_delay) ;
/*---------------------------------------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* _TASK_H_ */
