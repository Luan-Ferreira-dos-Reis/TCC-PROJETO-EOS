#ifndef _SEMAPHORE_H_
#define _SEMAPHORE_H_

#include "task.h"

#ifdef __cplusplus
extern "C" {
#endif

extern int preempt;

/*A bynary semaphore*/
typedef struct Semaphore{
  int unlock; /* can assume busy => unlock == 0 and free => unlock = 1*/ 
}Semaphore;

/*-------------------------------------------------------------------------------------------------------*/
/*----------------------------------------Semaphores-----------------------------------------------*/
/* Creates a new semaphore */
struct Semaphore createSemaphore(void);
/* Take a created semaphore*/
int semaphoreTake(Semaphore* semaphore);
/* free a lock semaphore*/
void semaphoreGive(Semaphore* semaphore);
/*-------------------------------------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* _SEMAPHORE_H_ */
