/*-------------------------------------------------------- TESTE 1 --------------------------------------------------------------*/
//#include "eos.h"
//
//#include <math.h>
//
//#define AOUT       7
//#define LED_BLINK  13
//#define LED_BLINK2  9
//
///*inicialização de filas e semáforos*/
//eos_semaphore serialSemaphore;
//eos_queue valueint;
//
///* tarefas de atividade em leds para verificar troca de contexto */
//void piscar(void *p);
//void SineTask(void *p);
///* tarefas de impressão na serial */
//void imprimir(void *p);
//void imprimir2(void *p);
//void imprimir3(void *p);
//void imprimir4(void *p);
//
///*setup*/
//void setup() {
//    /* 
//     *  Parâmetros passados as tarefas.
//     *  Obs: precisam ser passados por referencia (&arg1, &arg2, &arg3...)
//     */
//    int num = 63;;
//    char letra = 'a';
//    float G = 6.67384;
//    int led1 = LED_BLINK;
//    int led2 = LED_BLINK2;
//    Serial.begin(9600);
//
//    /*  Inicializa as tarefas, semáforos e filas: eos_initial(num_tarefas, num_semáforos, num_filas) */
//      eos_initial(7,1,1);  /*observar piscar, sineTask e imprimir1,2,3,4*/
//    /* Criando semáforo eos_create_semaphore(&nome_semaforo, tempo)*/ 
//      serialSemaphore = eos_create_semaphore(&serialSemaphore, 2000); 
//    /* Criando fila eos_create_queue(&nome_fila, num_elementos, tamanho dos elementos)*/ 
//      valueint = eos_create_queue(&valueint, 4, 8);
//
//    /* Criando tarefas eos_create_task(codigo, argumento(endereço), tam_pilha(64-256)bytes))*/ 
//    Serial.println("Creating task...");
//    eos_create_task(piscar, &led1, 256);
//    eos_create_task(piscar, &led2, 256);
//    eos_create_task(SineTask, NULL, 256);
//    eos_create_task(imprimir, &num, 256);
//    eos_create_task(imprimir2, &letra, 256);
//    eos_create_task(imprimir3, &G, 256);
//    eos_create_task(imprimir4, NULL, 256);
//    
//    /* inicia o sistema com valor de time slice eos_start(time_slice)*/
//    eos_start(5);    
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
//    // Ativamos o pino 13 (colocando 5v nele)  
//    digitalWrite(led, HIGH);    
//    // Aguardamos 1 segundo
//    delay(500); 
//    // Desligamos o pino 13
//    digitalWrite(led, LOW);
//    // Aguardamos mais um segundo
//    delay(500);
//  }
//}
//
///* sineTask */
//void SineTask(void *p) {    
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
///*Tarefas de impressão*/
///*  
// * Tarefas para impressão na serial que trocam valores entre si (através de filas)
// * recebem parâmetros como endereço e tratam de acordo com seu próprio código
// * fazem uso de semáforos  
// * todas as tarefas realizam alguns delays que somados são iguais a 1 segundo p/ cada tarefa
// */ 
//void imprimir(void *p){
//  int n = *(int*)p; /* recebe um endereço como parâmetro e transforma em inteiro */
//  int exe = 1; /* quantidade de execuções da tarefa */
//  while(1){
//    Serial.print("tarefa1 inicio "); Serial.println(exe);
//    exe++;
//    eos_queue_write(&valueint,&n); /* escreve o valor recebido em uma fila */
//    Serial.print("valor enviado: "); Serial.println(n);
//    n++;  /* altera o valor */   
//    delay(300);
//    Serial.println("tarefa1 meio");
//    delay(300);
//    Serial.println("tarefa1 fim"); 
//    delay(400);
//    }
//  }
//
//void imprimir2(void *p){
//  char l = *(char*)p; /* recebe um endereço como parâmetro e transforma em char */
//  int _max = 20; /* quantidade de execuções da tarefa */       
//  for(int i = 0; i < _max; i++){
//    Serial.print("tarefa2 inicio "); Serial.println(i+1); /* essa tarefa tem executa uma quantidade de vezes */
//    delay(200);
//    Serial.print("tarefa2 meio: recebi o char: "); Serial.println(l);
//    eos_semaphore_give(&serialSemaphore);   
//    delay(700);
//    Serial.println("tarefa2 f"); 
//    delay(100);
//    if(eos_semaphore_take(&serialSemaphore)){ /* pega um semáforo e escreve na serial */
//        Serial.println("nao desista dos seus sonhos até porque você já ouviu ");delay(1000);
//        Serial.println("falar em cabo de vassoura, supunhetamos que você vai Ter a vida que");delay(1000);
//        Serial.println("você é hoje, Daqui uns anos, Num sei, Vai que  Nasça uma planta Pra sobreviver"); delay(1000);
//        Serial.println("E q a.. Pode ser que mude A alternativa Do.. Dom pedro segundo e o padre quando");delay(1000); 
//        Serial.println("for rezar a missa Vai querer uma Latinha de cerveja E aí? interprete...");delay(1000);
//       eos_semaphore_give(&serialSemaphore);
//      }
//    }
//  }
//
// void imprimir3(void *p){
//  float r = *(float*)p;  /* recebe um endereço como parâmetro e transforma em char */
//  int exe = 1; /* quantidade de execuções da tarefa */
//  while(1){ /* quantidade de execuções da tarefa */
//    Serial.print("tarefa3 inicio "); Serial.println(exe);
//    exe++;
//    Serial.print("recebi o valor: "); Serial.println(r);
//    delay(200);
//    Serial.println("tarefa3 meio");
//    delay(400);
//    Serial.println("tarefa3 fim"); 
//    delay(200);
//    }
//  }
//
// void imprimir4(void *p){
//  int received;
//  int _max = 50; /* quantidade de execuções da tarefa */
//  for(int i = 0; i < _max; i++){
//    Serial.print("tarefa4 inicio "); Serial.println(i+1);
//    received = eos_queue_read(&valueint); /* ler valores de uma pilha */
//    Serial.print("valor recebido: ");Serial.println(received); /* escreve valor lido */
//    delay(100);
//    Serial.println("tarefa4 meio");
//    delay(300);
//    Serial.println("tarefa4 fim"); 
//    delay(600);
//    }
//  }

/*------------------------------------------------------------- TESTE 2 ------------------------------------------------------------*/
/*
 *  COMENTAR o código acima^   e DESCOMENTAR o código abaixo para observar
 */

#include "eos.h"
/*inicialização de filas e semáforos*/
eos_semaphore serialSemaphore;
eos_queue measurefloat;
eos_queue datachar;

int socializing = 0; 
char palavra[20] = "senha #5798";

/* push button, leds, filas e semáforos */
void task1(void *p);
void task2(void *p);
void task3(void *p);
void task4(void *p);
/*setup*/
void setup() {
    /* 
     *  Parâmetros passados as tarefas.
     *  Obs: precisam ser passados por referencia (&arg1, &arg2, &arg3...)
     */
    Serial.begin(9600);

    /* Inicializa as tarefas, semáforos e filas: eos_initial(num_tarefas, num_semáforos, num_filas) */
      
    eos_initial(2,0,1);/*observar troca de mensagem entre task1 e task2*/

    /* Criando semáforo eos_create_semaphore(&nome_semaforo, tempo)*/ 
    // serialSemaphore = eos_create_semaphore(&serialSemaphore, 2000);
       
    /* Criando fila eos_create_queue(&nome_fila, num_elementos, tamanho dos elementos)*/ 
     datachar = eos_create_queue(&datachar, 10, 8);
     //measurefloat = eos_create_queue(&measurefloat, 5, 32);
    
    /*Criando tarefas eos_create_task(codigo, argumento(endereço), tam_pilha(64-256)bytes))*/   
    Serial.println("Creating task...");
    eos_create_task(task1, NULL, 150);
    eos_create_task(task2, NULL, 150);
    //eos_create_task(task3, NULL, 150);
    //eos_create_task(task4, NULL, 150);
  
    /* inicia o sistema com valor de time slice eos_start(time_slice)*/
    eos_start(5);    
}

void loop() {/* Nada é executado aqui */}

/**
 * TASKS
 */
/* task 1 */
void task1(void *p) {
    int tamFrase = 10;
    char frase[tamFrase] = {'b', 'e', 'm', ' ', 'v', 'i', 'n', 'd', 'o', '!'};
    char caractere = 'h';
    int letra = 0;
    Serial.println("Task 1 was started");
    while (1) {
        /* envia os valores para fila */
        eos_queue_write(&datachar,&frase[letra]);
        letra++;
        if(letra >= tamFrase) letra = 0;
        delay(300);
        Serial.print("1 SEND:              ");
        Serial.println(frase[letra]);
    }
}
/* task 2 */
void task2(void *p) {
    char l;
    Serial.println("Task 2 was started");
    while (1) {
        /* recebe os valores da fila e imprime */
        l = eos_queue_read_char(&datachar);
        delay(250);
        Serial.print("2 RECEIVE: ");
        Serial.println(l);
    }
}
/* task 3 */
void task3(void *p) {
    float receive = 0;
    Serial.println("Task 3 was started");
    while (1) {
        receive = eos_queue_read_float(&measurefloat);
        /* recebe os valores da fila e imprime */
        Serial.print("3: receive");Serial.println(receive);
        delay(450);
    }
}
/* task 4 */
void task4(void *p) {
    float measure = 2.345;
    Serial.println("Task 4 was started");
    while (1) {
        /* envia os valores para fila */
        eos_queue_write(&measurefloat, &measure);
        /* cria um novo valor */
        Serial.print("4: send"); Serial.println(measure);
        delay(550);    
    }
}
