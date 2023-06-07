/*-------------------------------------------------------- TESTE 1 --------------------------------------------------------------*/
#include "eos.h"

#include <math.h>

#define AOUT       7
#define LED_BLINK  13
#define LED_BLINK2  9
#define LED_BLINK3  7

/*inicialização de tarefas, filas e semáforos*/
Task p, p2, p3,st, i, i2, i3, i4, i5;
Semaphore serialSemaphore;
Queue valueint;

/* tarefas de atividade em leds para verificar troca de contexto */
void piscar(void *p);
void SineTask(void *p);
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
    int led1 = LED_BLINK;
    int led2 = LED_BLINK2;
    int led3 = LED_BLINK3;
    Serial.begin(9600);

    /* Criando semáforo createSemaphore()*/ 
      serialSemaphore = createSemaphore(); 
    /* Criando fila eos_create_queue(numElementos >= 1, sizeof(type))*/ 
      valueint = createQueue(4, sizeof(int));
    /* Criando tarefas createTask(handler ,void codigo,void *argumento(endereço), int sizeOfStack(128-1024)bytes, int priority) valores testados por enquanto)*/ 
    Serial.println("Creating task....................................................................");delay(1000);
    createTask(&p, piscar, &led1, 256, 1);
    createTask(&p2, piscar, &led2, 256, 1);
    createTask(&st, SineTask, NULL, 256, 1);
    createTask(&i, imprimir, &num, 256, 0);
    createTask(&i2, imprimir2, &letra, 256, 0);
    createTask(&i3, imprimir3, &G, 256, 0);
    createTask(&i4, imprimir4, NULL, 256, 0);
    //createTask(&i5, imprimir5, NULL, 128, 0);
    
    /* inicia o sistema eos_start(time slice, portMaxDelay)  max_delay(max 25000ms)*/
    startSystem(5, 10000);    
}
 
void loop() {/* Nada é executado aqui */}

/*Tarefas de manipulação de LEDs*/
/* piscar */
void piscar(void *p){
  /*define o led para piscar*/
  int led = *(int*)p;
  while(1){
    Serial.print("LED: ");Serial.println(led);
    // Ativamos o pino 13 (colocando 5v nele)  
    digitalWrite(led, HIGH);    
    // Aguardamos 1 segundo
    delay(500); 
    // Desligamos o pino 13
    digitalWrite(led, LOW);
    // Aguardamos mais um segundo
    delay(500);
  }
}

/* sineTask */
void SineTask(void *p) {    
    unsigned int stime=0, etime=0;
    pinMode(AOUT, OUTPUT);
    unsigned int outpv = 0;
    unsigned int period = 0;
    while(1) {
        Serial.println("SineTask");
        stime = millis();
        for(period = 0; period < 16; ++period){
            etime = millis();
            outpv = (unsigned int)((sin(2*PI*2*(etime - stime)*0.001)+1)*127.5);  /*Senoide = seno(2*PI*Freq*t)*/
            analogWrite(AOUT, outpv);
            delay(33);
        }
    }
}

/*Tarefas de impressão*/
/*  
 * Tarefas para impressão na serial que trocam valores entre si (através de filas)
 * recebem parâmetros como endereço e tratam de acordo com seu próprio código
 * fazem uso de semáforos  
 * todas as tarefas realizam alguns delays que somados são iguais a 1 segundo p/ cada tarefa
 */ 
void imprimir(void *p){
  int n = *(int*)p; /* recebe um endereço como parâmetro e transforma em inteiro */
  int exe = 1; /* quantidade de execuções da tarefa */
  //while(1){
    for(int i = 0; i < 10; i++){
    Serial.print("tarefa1 inicio "); 
    Serial.println(exe);
    exe++;
    queueWrite(&valueint,&n); /* escreve o valor recebido em uma fila */
    Serial.print("valor enviado: "); 
    Serial.println(n);
    n++;  /* altera o valor */   
    delay(300);
    Serial.println("tarefa1 meio");
    delay(300);
    Serial.println("tarefa1 fim"); 
    delay(400);
    }
  }

void imprimir2(void *p){
  char l = *(char*)p; /* recebe um endereço como parâmetro e transforma em char */
  int _max = 5; /* quantidade de execuções da tarefa */       
  for(int i = 0; i < _max; i++){
    Serial.print("tarefa2 inicio "); Serial.println(i+1); /* essa tarefa tem executa uma quantidade de vezes */
    delay(200);
    Serial.print("tarefa2 meio: recebi o char: "); Serial.println(l);
    delay(700);
    if(semaphoreTake(&serialSemaphore)){ /* pega um semáforo e escreve na serial */
        Serial.println("não, não desista dos seus sonhos até porque você já ouviu "); delay(200);
        Serial.println("falar em cabo de vassoura, supunhetamos que você vai Ter a vida que"); delay(200);
        Serial.println("você é hoje, Daqui uns anos, Num sei, Vai que  Nasça uma planta Pra sobreviver"); delay(200);
        Serial.println("E q a.. Pode ser que mude A alternativa Do.. Dom pedro segundo e o padre quando"); delay(200); 
        Serial.println("for rezar a missa Vai querer uma Latinha de cerveja E aí? interprete..."); delay(200);
        semaphoreGive(&serialSemaphore);
     }
     Serial.println("tarefa2 fim"); 
     delay(100);
    }
  }

 void imprimir3(void *p){
  float r = *(float*)p;  /* recebe um endereço como parâmetro e transforma em char */
  int exe = 1; /* quantidade de execuções da tarefa */
  //while(1){ /* quantidade de execuções da tarefa */
    for(int i = 0; i < 10; i++){
    Serial.print("tarefa3 inicio "); 
    Serial.println(exe);
    exe++;
    Serial.print("recebi o valor: "); 
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
  int maximum = 10; /* quantidade de execuções da tarefa */
  for(int i = 0; i < maximum; i++){
    Serial.print("tarefa4 inicio "); 
    Serial.println(i+1);
    received = *(int*)queueRead(&valueint); /* ler valores de uma pilha */
    Serial.print("valor recebido: ");
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
  //while(1){ /* quantidade de execuções da tarefa */
    for(int i = 0; i < 10; i++){
    Serial.println("tarefa5 inicio "); 
    Serial.print("recebi o valor: "); 
    Serial.println(r);
    delay(250);
    Serial.println("tarefa5 meio");
    delay(500);
    Serial.println("tarefa5 fim"); 
    delay(250);
    }
  }

/*------------------------------------------------------------- TESTE 2 ------------------------------------------------------------*/
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
//    /* Criando semáforo createSemaphore()*/ 
//    serialSemaphore = createSemaphore();
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
//    float cte = 161803.3987; /* proporção áurea*/
//    Serial.println("Task 4 was started");
//    while (1) {
//        /* envia os valores para fila */
//        measure = millis(); //random(10)/cte;
//        queueSend(&measurefloat, &measure);
//        /* cria um novo valor */
//        Serial.print("4: SEND: "); Serial.println(measure);
//        delay(500);    
//    }
//}