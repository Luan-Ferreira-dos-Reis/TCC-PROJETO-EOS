/*
    EMBEDDED OPERATING SYSTEM - EOS

    It is an operating system for the Arduino platform. Among the project's features are:
    
    Preemptive scheduling in round roubin (with option to choose timeslice)
    Implementation of basic aspects of queues and semaphores
    Flexibility to use all timers (0, 1, 2, 3, 4, 5) of the arduino
    Flexibility in the size of the battery needed to run the processes
    Reusing task codes with different arguments (void function(void *arg))
    This project is being built for a course conclusion discipline.
    
    From a reasonable modified of the following code:
    https://github.com/SneManden/arduous/tree/master

    Author: Luan Ferreira dos Reis - github: https://github.com/Luan-Ferreira-dos-Reis/TCC-PROJETO-EOS

    [PORTUGUESE]
    É um sistema operacional para plataforma Arduino Entre as características do projeto estão:
    
    Escalonamento preemptivo em round roubin (com opção de escolha do timeslice)
    Implementação de aspectos básicos de filas e semáforos
    Flexibilidade de utilização de todos os timers (0, 1, 2, 3, 4, 5) do arduino
    Flexibilidade no tamanho da pilha necessária para execução dos processos
    Reutilização de códigos de tarefas com argumentos diferents (void function(void *arg))
    Este projeto está sendo construído para disciplina de conclusão de curso.
    
    A partir de uma modificação razoável do seguinte código:
    https://github.com/SneManden/arduous/tree/master 

    Autor: Luan Ferreira dos Reis - github: https://github.com/Luan-Ferreira-dos-Reis/TCC-PROJETO-EOS
 */
 /* BASICS DOCUMENTATION----------------------------------------------------------------------------------------------------------
   START THE SYSTEM      -> eos_start(int timeSlice, int maxPortDelay);
   CREATE A TASK         -> eos_create_task(eos_task *task, void function (), void *arg, int size_Of_stack)
   CREATE A SEMAPHORE    -> eos_create_semaphore(eos_semaphore *semaphore)
      TAKE A SEMAPHORE   -> eos_semaphore_take(eos_semaphore *semaphore)
      GIVE A SEMAPHORE   -> eos_semaphore_give(eos_semaphore *semaphore)
   CREATE A QUEUE        -> eos_create_queue(eos_queue *semaphore, int sizeOfQueue, int sizeOfType[obs: sizeof(int) or sizeof(float) or sizeof(char)])
      WRITE IN QUEUE     -> eos_queue_write(eos_queue *queue, *value) obs: only overwrite
      READ FROM QUEUE    -> eos_queue_read(eos_queue *queue)          obs: only read the a value and return: (void*) need to cast
      SEND TO QUEUE      -> eos_queue_send(eos_queue *queue, *value)  obs: push a element and increase the queue
      RECEIVE FROM QUEUE -> eos_queue_receive(eos_queue *queue)       obs: pop a element and reduce the queue and return: (void*) need to cast
 -----------------------------------------------------------------------------------------------------------------------------------*/

#ifndef _EOS_H_
#define _EOS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "task.h"

#ifdef __cplusplus
}
#endif

#endif /* _EOS_H_ */
