/*-------------------------------------------------------- TESTE 1 --------------------------------------------------------------*/
#include "eos.h"

/*inicialização de tarefas, filas e semáforos*/
Task i, i2, i3, i4, i5, i6;
Semaphore serialSemaphore;
Queue valueint;

/* tarefas de impressão na serial */
void imprimir(void *p);
void imprimir2(void *p);
void imprimir3(void *p);
void imprimir4(void *p);
void imprimir5(void *p);

/*setup*/
void setup() {
    /* 
     *  Parâmetros passados as tarefas.
     *  Obs: precisam ser passados por referencia (&arg1, &arg2, &arg3...)
     */
    int num = 63, num2 = 32;
    char letra = 'a', letra2 = 'b';
    float G = 6.67384, C = 2.99792458;
    Serial.begin(9600);

    /* Criando semáforo createSemaphore()*/ 
      serialSemaphore = createSemaphore(); 
    /* Criando fila eos_create_queue(numElementos >= 1, sizeof(type))*/ 
      valueint = createQueue(4, sizeof(int));
    /* Criando tarefas createTask(handler ,void codigo,void *argumento(endereço), int sizeOfStack(128-1024)bytes, int priority) */ 
    Serial.println("Creating task....................................................................");delay(1000);
    createTask(&i, imprimir, &num, 256, 0);
    createTask(&i2, imprimir2, &letra, 256, 1);
    createTask(&i3, imprimir3, &G, 256, 1);
    createTask(&i4, imprimir4, NULL, 256, 0);
    createTask(&i5, imprimir5, NULL, 256, 0);
    createTask(&i6, imprimir6, NULL, 256, 1);
    /* as tarefas: imprimir, imprimir4 e imprimir 5 irão executar primeiro (prioridade 0)*/
    /* a tarefa: imprimir5 vai utilizar taskToWait() parando de executar até a tarefa: imprimir 2*/
    /* despertá-la utilizando taskToReady()*/
    /* as tarefas imprimir e imprimir 4 trocam dados através da fila(Queue): valueint *
    /* a tarefa: imprimir2 faz uso de um semáforo para imprimir algumas frases */
    
    /* inicia o sistema eos_start(time slice, portMaxDelay)  max_delay(max 25000ms)*/
    startSystem(5, 10000);    
}
 
void loop() {/* Nada é executado aqui */}

/*Tarefas de impressão*/
/*  
 * Tarefas para impressão na serial que trocam valores entre si (através de filas)
 * recebem parâmetros como endereço e tratam de acordo com seu próprio código
 * fazem uso de semáforos  
 * todas as tarefas realizam alguns delays que somados são iguais a 1 segundo p/ cada tarefa
 * A tarefa 5 usa a função taskToWait(), parando de executar como prioridade e sendo despertada depois
 */ 
void imprimir(void *p){
    int n = *(int*)p; /* recebe um endereço como parâmetro e transforma em inteiro */
    /* valor n será enviado pela queue e incrementado, recebido pela tarefa: imprimir4*/
    for(int i = 0; i < 10; i++){
    Serial.print("tarefa1 inicio "); 
    Serial.println(i+1);
    queueWrite(&valueint,&n); /* escreve o valor recebido na fila: valueint */
    Serial.print("valor enviado: "); 
    Serial.println(n); n++;   
    delay(300);
    Serial.println("tarefa1 meio");
    delay(300);
    Serial.println("tarefa1 fim"); 
    delay(400);
    }
  }

void imprimir2(void *p){
  char l = *(char*)p; /* recebe um endereço como parâmetro e transforma em char */
  int max2 = 5; /* quantidade de execuções da tarefa */       
  for(int i = 0; i < max2; i++){
    Serial.print("tarefa2 inicio "); Serial.println(i+1); /* essa tarefa tem executa uma quantidade de vezes */
    delay(200);
    Serial.print("tarefa2 meio: parametro recebido: "); Serial.println(l);
    delay(700);
    if(semaphoreTake(&serialSemaphore)){ /* pega um semáforo e escreve na serial */
        Serial.println("não, não desista dos seus sonhos até porque você já ouviu "); delay(400);
        Serial.println("falar em cabo de vassoura, supunhamos que você vai Ter a vida que"); delay(400);
        Serial.println("você é hoje, Daqui uns anos, Num sei, Vai que  Nasça uma planta Pra sobreviver"); delay(400);
        Serial.println("E q a.. Pode ser que mude A alternativa Do.. Dom pedro segundo e o padre quando"); delay(400); 
        Serial.println("for rezar a missa Vai querer uma Latinha de cerveja E aí? interprete..."); delay(400);
        semaphoreGive(&serialSemaphore);
     }
     Serial.println("tarefa2 fim"); 
     delay(100);
    }
    taskToReady(); /* desperta tarefa imprimir5 */
  }

 void imprimir3(void *p){
  int max3 = 10;
  float r = *(float*)p;  /* recebe um endereço como parâmetro e transforma em char */
    for(int i = 0; i < max3; i++){
    Serial.print("tarefa3 inicio "); 
    Serial.println(i+1);
    Serial.print("recebi o parâmetro: "); 
    Serial.println(r);
    delay(200);
    Serial.println("tarefa3 meio");
    delay(400);
    Serial.println("tarefa3 fim"); 
    delay(200);
    }
  }

 void imprimir4(void *p){
  int received;
  int max4 = 10; /* quantidade de execuções da tarefa */
  for(int i = 0; i < max4; i++){
    Serial.print("tarefa4 inicio "); 
    Serial.println(i+1);
    received = *(int*)queueRead(&valueint); /* ler valores de uma pilha */
    Serial.print("valor recebido da queue: ");
    Serial.println(received); /* escreve valor lido */
    delay(100);
    Serial.println("tarefa4 meio");
    delay(300);
    Serial.println("tarefa4 fim"); 
    delay(600);
    }
  }

 void imprimir5(void *p){
  float r = *(float*)p;  /* recebe um endereço como parâmetro e transforma em char */
    /* executa primeira vez e vai esperar ser despertada */
    int i = 0;
    Serial.println("tarefa5 inicio "); 
    Serial.println(i+1); 
    Serial.print("recebi o valor: "); 
    Serial.println(r);
    delay(250);
    Serial.println("tarefa5 meio");
    delay(500);
    Serial.println("tarefa5 fim"); 
    delay(250);
    taskToWait();   /* task5 permite que outra tarefas executem*/
    /* não executa esse trecho até ser despertada */
    for(int i = 1; i < 10; i++){
    Serial.print("tarefa5 inicio "); 
    Serial.println(i); 
    Serial.print("recebi o valor: "); 
    Serial.println(r);
    delay(250);
    Serial.println("tarefa5 meio");
    delay(500);
    Serial.println("tarefa5 fim"); 
    delay(250);  
    }
  }

void imprimir6(void *p){
  /* a tarefa 6 executa indefinadamente */
  int i = 0;
  while(1){
    Serial.print("tarefa6 inicio "); 
    Serial.println(i+1); i++;
    delay(250);
    Serial.println("tarefa6 meio");
    delay(500);
    Serial.println("tarefa6 fim"); 
    delay(250);  
  }
}

/*-----------------------------------------------------------------------------------------------------------------------------------*/
/*-------------------------------------------------------- TESTE 2 --------------------------------------------------------------*/
/*
 *  COMENTAR o código acima^   e DESCOMENTAR o código abaixo para observar
 */
//#include "eos.h"
//
//#include <math.h>
//
//#define AOUT       7
//#define BOUT       4
//#define LED_BLINK  13
//#define LED_BLINK2  9
//
///*inicialização de tarefas, filas e semáforos*/
//Task p, p2, st, ct;
//
///* tarefas de atividade em leds para verificar troca de contexto */
//void piscar(void *p);
//void sinTask(void *p);
//void cosTask(void *p);
//
//
///*setup*/
//void setup() {
//    /* 
//     *  Parâmetros passados as tarefas.
//     *  Obs: precisam ser passados por referencia (&arg1, &arg2, &arg3...)
//     */
//    int led1 = LED_BLINK;
//    int led2 = LED_BLINK2;
//    Serial.begin(9600);
//    
//    Serial.println("Creating task....................................................................");delay(1000);
//    createTask(&p, piscar, &led1, 256, 0);
//    createTask(&p2, piscar, &led2, 256, 0); /* reutilização da função piscar com parâmetro diferente */
//    createTask(&st, sinTask, NULL, 256, 0);
//    createTask(&ct, cosTask, NULL, 256, 0);
//  
//    /* inicia o sistema eos_start(time slice, portMaxDelay)  max_delay(max 25000ms)*/
//    startSystem(5, 10000);    
//}
// 
//void loop() {/* Nada é executado aqui */}
//
///*Tarefas de manipulação de LEDs*/
///* piscar */
//void piscar(void *p){
//  /*define o led para piscar*/
//  int led = *(int*)p;
//  while(1){
//    Serial.print("LED: ");Serial.println(led); 
//    digitalWrite(led, HIGH);    
//    delay(500); 
//    digitalWrite(led, LOW);
//    delay(500);
//  }
//}
//
///* sineTask */
//void sinTask(void *p) {    
//    unsigned int stime=0, etime=0;
//    pinMode(AOUT, OUTPUT);
//    unsigned int outpv = 0;
//    unsigned int period = 0;
//    while(1) {
//        Serial.println("SineTask");
//        stime = millis();
//        for(period = 0; period < 16; ++period){
//            etime = millis();
//            outpv = (unsigned int)((sin(2*PI*2*(etime - stime)*0.001)+1)*127.5);  /*Senoide = seno(2*PI*Freq*t)*/
//            analogWrite(AOUT, outpv);
//            delay(33);
//        }
//    }
//}
//
///* cosTask */
//void cosTask(void *p) {    
//    unsigned int stime=0, etime=0;
//    pinMode(BOUT, OUTPUT);
//    unsigned int outpv = 0;
//    unsigned int period = 0;
//    while(1) {
//        Serial.println("CosineTask");
//        stime = millis();
//        for(period = 0; period < 16; ++period){
//            etime = millis();
//            outpv = (unsigned int)((cos(2*PI*2*(etime - stime)*0.001)+1)*127.5);  /*cossenoide = cosseno(2*PI*Freq*t)*/
//            analogWrite(BOUT, outpv);
//            delay(33);
//        }
//    }
//}

/*----------------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------------- TESTE 3 ------------------------------------------------------------*/
/*
 *  COMENTAR o código acima^   e DESCOMENTAR o código abaixo para observar
 */

//#include "eos.h"
///*inicialização de tarefas, filas e semáforos*/
//Task t1, t2, t3, t4;
//Semaphore serialSemaphore;
//Queue measurefloat;
//Queue datachar;
//
//int socializing = 0; 
//char palavra[20] = "senha #5798";
//
///* push button, leds, filas e semáforos */
//void task1(void *p);
//void task2(void *p);
//void task3(void *p);
//void task4(void *p);
///*setup*/
//void setup() {
//    /* 
//     *  Parâmetros passados as tarefas.
//     *  Obs: precisam ser passados por referencia (&arg1, &arg2, &arg3...)
//     */
//    Serial.begin(9600);
//       
//    /* Criando fila createQueue(num_elementos, sizeof(type))*/ 
//     datachar = createQueue(5, sizeof(char));
//     measurefloat = createQueue(5, sizeof(float));
//    
//    /* Criando tarefas createTask(handler ,void codigo,void *argumento(endereço), int sizeOfStack(128-1024)bytes, int priority) valores testados por enquanto)*/    
//    Serial.println("Creating task....................................................................");delay(1000);
//    createTask(&t1, task1, NULL, 128, 0);
//    createTask(&t2, task2, NULL, 128, 0);
//    createTask(&t3, task3, NULL, 128, 0);
//    createTask(&t4, task4, NULL, 128, 0);
//  
//    /* inicia o sistema com valor de time slice e tempo máximo de ocupaçao de semáforos eos_start(time_slice, max_delay)*/
//    startSystem(5, 5000);    
//}
//
//void loop() {/* Nada é executado aqui */}
//
///**
// * TASKS
// */
//
///* TASK 1 ESCREVE UMA FRASE QUE É RECEBIDA POR TASK 2 UTILIZANDO UMA ESTRUTURA DE FILAS
// * TASK 4 ENVIA DADOS PSEUDO ALEÁTORIOS QUE PODEM SER RECEBIDOS POR TASK 3
// * OBS: O TAMANHO DA PILHA(STACK) DE TASK 3 E TASK 4 IMPORTAM POIS DEPENDENDO DA VELOCIDADE(DELAY ENTRE ATIVIDADES) 
// * QUE CRIAM DADOS A PILHA PODE CRESCER ALÉM DO ESPAÇO RESERVADO PARA CADA TASK
// */ 
// 
// /* task 1 */
//void task1(void *p) {
//    int tamFrase = 10;
//    char frase[tamFrase] = {'b', 'e', 'm', ' ', 'v', 'i', 'n', 'd', 'o', '!'};
//    int letra = 0;
//    Serial.println("Task 1 was started");
//    while (1) {
//        /* envia os valores para fila */
//        queueWrite(&datachar,&frase[letra]);
//        letra++;
//        if(letra >= tamFrase) letra = 0;
//        delay(500);
//        Serial.print("1 WRITE:              ");
//        Serial.println(frase[letra]);
//    }
//}
///* task 2 */
//void task2(void *p) {
//    char l;
//    Serial.println("Task 2 was started");
//    while (1) {
//        /* recebe os valores da fila e imprime */
//        l = *(char*)queueRead(&datachar);
//        delay(500);
//        Serial.print("2 READ:                         ");
//        Serial.println(l);
//    }
//}
///* task 3 */
//void task3(void *p) {
//    float receive = 0;
//    Serial.println("Task 3 was started");
//    while (1) {
//        receive = *(float*)queueReceive(&measurefloat);
//        /* recebe os valores da fila e imprime */
//        Serial.print("3: RECEIVE: ");Serial.println(receive);
//        delay(500);
//    }
//}
///* task 4 */
//void task4(void *p) {
//    float measure;
//    Serial.println("Task 4 was started");
//    while (1) {
//        /* envia os valores para fila */
//        measure = millis();
//        queueSend(&measurefloat, &measure);
//        /* cria um novo valor */
//        Serial.print("4: SEND: "); Serial.println(measure);
//        delay(500);    
//    }
//}
