#ifndef _SEMAPHORE_H_
#define _SEMAPHORE_H_

#include "task.h"

#ifdef __cplusplus
extern "C" {
#endif

extern int preempt;

/*A bynary semaphore*/
typedef struct eos_semaphore{
  int unlock; /* can assume busy => unlock == 0 and free => unlock = 1*/ 
}eos_semaphore;

/*-------------------------------------------------------------------------------------------------------*/
/*----------------------------------------Semaphores-----------------------------------------------*/
/* Creates a new semaphore */
struct eos_semaphore eos_create_semaphore();
/* Take a created semaphore*/
int eos_semaphore_take(eos_semaphore* semaphore);
/* free a lock semaphore*/
void eos_semaphore_give(eos_semaphore* semaphore);
/*-------------------------------------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* _SEMAPHORE_H_ */
