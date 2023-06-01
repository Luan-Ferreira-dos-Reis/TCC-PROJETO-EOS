#ifndef _SEMAPHORE_H_
#define _SEMAPHORE_H_



#ifdef __cplusplus
extern "C" {
#endif

/*A bynary semaphore*/
typedef struct eos_semaphore{
  int unlock; /* can assume busy => unlock == 0 and free => unlock = 1*/ 
  int time_free; /* time that semaphore give to task*/
}eos_semaphore;

#ifdef __cplusplus
}
#endif

#endif /* _SEMAPHORE_H_ */
