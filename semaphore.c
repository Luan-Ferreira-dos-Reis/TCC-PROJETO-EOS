#include "semaphore.h"

/* Semaphore pool */
static struct Semaphore *semaphorePool = NULL;
static int semaphoreCount = 0;
/*----------------------------------------Semaphores-----------------------------------------------*/
/*
 * Creates a new semaphore 
 * @param  void
 * @return        a semaphore
 */
struct Semaphore createSemaphore(void){
  Semaphore *newSemaphore;
  /* Get a new semaphore from the semaphore pool */
    semaphorePool = (Semaphore*)realloc(semaphorePool, (semaphoreCount + 1) * sizeof(Semaphore));
    newSemaphore = &semaphorePool[semaphoreCount];
  /*initialize semaphore free*/
    newSemaphore->unlock = 1;
    semaphoreCount++;
    return *newSemaphore; 
}
/**
 * A task can take a semaphore to apropriate hardware recurse
 * @param  semaphore
 * @return        0 if semaphore is avaible; 0 if not
 */
int semaphoreTake(Semaphore* semaphore){ 
  /* test if semaphore is unlock*/
  DISABLE_INTERRUPTS();
  if(semaphore->unlock == 1){
    /* semaphore lock  and task work alone*/   
    DISABLE_PREEMPT();
    semaphore->unlock = 0;
    ENABLE_INTERRUPTS();   
    return 1;
  }/* to ocupy semaphore */ 
  else{
    ENABLE_INTERRUPTS();
    return 0;
  }     
}
/**
 * A task can give up a semaphore
 * @param  semaphore
 * @return        void
 */
void semaphoreGive(Semaphore* semaphore){
  /* test if semaphore is lock*/
  DISABLE_INTERRUPTS();
  if(semaphore->unlock == 0){
   /* free semaphore */
    semaphore->unlock = 1;
      ENABLE_PREEMPT();    
  }/* free semaphore */; 
  ENABLE_INTERRUPTS(); 
}
/*-------------------------------------------------------------------------------------------------*/
