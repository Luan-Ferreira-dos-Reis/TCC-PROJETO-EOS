#include "semaphore.h"

/* Semaphore pool */
static struct eos_semaphore *semaphore_pool = NULL;
static int semaphore_count = 0;
/*----------------------------------------Semaphores-----------------------------------------------*/
/*
 * Creates a new semaphore 
 * @param  handler to semaphore
 * @return        a semaphore
 */
struct eos_semaphore eos_create_semaphore(eos_semaphore *new_semaphore){
  /* Get a new semaphore from the semaphore pool */
    semaphore_pool = (eos_semaphore*)realloc(semaphore_pool, (semaphore_count + 1) * sizeof(eos_semaphore));
    new_semaphore = &semaphore_pool[semaphore_count];
  /*initialize semaphore free*/
    new_semaphore->unlock = 1;
    semaphore_count++;
    return *new_semaphore; 
}

/**
 * A task can take a semaphore to apropriate hardware recurse
 * @param  semaphore
 * @return        0 if semaphore is avaible; 0 if not
 */
int eos_semaphore_take(eos_semaphore* semaphore){ 
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
void eos_semaphore_give(eos_semaphore* semaphore){
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
