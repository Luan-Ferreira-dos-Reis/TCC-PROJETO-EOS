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
#define CONTEXT_SWITCH() do { \
init: if (currentTask != NULL && taskQueue[currentLayer] != NULL && \
          currentTask != taskQueue[currentLayer]) { \
          currentTask->spLow = SPL; \
          currentTask->spHigh = SPH; \
          currentTask = taskQueue[currentLayer]; \
          taskQueue[currentLayer] = taskQueue[currentLayer]->next; \
          SPL = currentTask->spLow; \
          SPH = currentTask->spHigh;} \
      else if(currentTask == NULL && taskQueue[currentLayer] != NULL){ \
          currentTask = nextTask; \
          goto init; \
        } \
      else if(taskQueue[currentLayer] == NULL && currentLayer < layersPriority){ \
        currentLayer++; \
        goto init; \
     } } while(0)

/* Removes  e the current task from task queue for in case the current task is complete */
#define DEQUEUE() do { \
  if (taskQueue[currentLayer]->prev == taskQueue[currentLayer]->next) \
        taskQueue[currentLayer] = NULL; \
    else if (currentTask == taskQueue[currentLayer]) \
        taskQueue[currentLayer] = currentTask->next; \
    currentTask->prev->next = currentTask->next; \
    currentTask->next->prev = currentTask->prev; \
    nextTask = currentTask; \
    currentTask = NULL; \
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

enum state{READY = 0, RUNNING, WAITING ,FINISHED};   /* task state (ready = 0, running = 1, finished = 2*/
 
typedef struct Task{                /* A kernel task structure */
    int taskId;                        /* Unique identifier */
    ptrFunc code;                       /* pointer to task code */
    void *arg;                          /* argument */    /* LUAN FERREIRA DOS REIS */
    char *stack;                        /* pointer to task stack */
    int state;                          /* task state (ready = 0, running = 1, waiting = 2, finished = 3*/
    struct Task *next;              /* Pointer to next task in queue */
    struct Task *prev;              /* Pointer to previous task */
    volatile char spLow, spHigh;      /* Lower/higher 8 bit of StackPointer */
    volatile char pcLow, pcHigh;      /* Lower/higher 8 bit of CounterProgramPointer */
}Task;

/*--------------------------------------------Task-------------------------------------------------------*/
/* Creates a new thread */
int createTask(Task *newTask, void (*runner)(void *runnerArg), void *arg, int sizeStack, int priority); /* LUAN FERREIRA DOS REIS */
/* Add an element in the back of the queue */
void enqueue(struct Task *task, int layer);
/* Return an element from the front of the queue */
struct Task *dequeue(struct Task *task, int layer);
/* idle_task*/
void idleTask(void *args);
/* run task properly */
void makeCallfunc(void);
/*--------------------------------------------- Kernel ----------------------------------------------*/
/* Starts the Arduous kernel */
int startSystem(int ts, int maxDelay);
/* Switches between tasks */
void switchTask(void) __attribute__ ((naked));
/*---------------------------------------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* _TASK_H_ */
