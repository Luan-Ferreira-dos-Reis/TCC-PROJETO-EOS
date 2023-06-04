/*
    EMBEDDED OPERATING SYSTEM - EOS
    
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
