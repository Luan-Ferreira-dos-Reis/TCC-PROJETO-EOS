# TCC-PROJETO-EOS
disciplina de projeto de conclusão de curso - IFPB - Campus Campina Grande - Eng. de Computação

EMBEDDED OPERATING SYSTEM - EOS

É um sistema operacional para plataforma Arduino
Entre as características do projeto estão:
  * Escalonamento preemptivo em round roubin (com opção de escolha do timeslice)
  * Implementação de aspectos básicos de filas e semáforos
  * Flexibilidade de utilização de todos os timers (0, 1, 2, 3, 4, 5) do arduino
  * Flexibilidade no tamanho da pilha necessária para execução dos processos
  * Reutilização de códigos de tarefas com argumentos diferents (void function(void *arg))



Este projeto está sendo construído para disciplina de conclusão de curso.
A partir de uma modificação razoável do seguinte código: 
  https://github.com/SneManden/arduous/tree/master
 
 Além do entendimento das seguintes referênicas:
 Biblitoca de troca de contexto em avr: https://github.com/arbv/avr-context 
 Exemplo de kernel:  https://github.com/jdn-aau/krnl
 
 Leituras: https://embarcados.com.br/meu-kernel-minha-vida-round-robin/ , 
          
          https://embarcados.com.br/rtos-para-iniciantes-com-arduino-e-freertos/#Criacao-de-filas-queues, 
           
           https://www.freertos.org/implementation/a00015.html, 
          
          https://jensd.dk/edu/doc/arduino/krnl/MultitaskingonanAVR.pdf
           
           
 Autor do projeto: Luan Ferreira dos Reis 
 
 
 
 
 
 
 

