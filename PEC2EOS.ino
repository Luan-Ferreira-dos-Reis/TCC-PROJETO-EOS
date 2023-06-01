#include "eos.h"
#include <math.h>

#define AOUT       7
#define LED_BLINK  13

eos_semaphore serialSemaphore;
eos_queue valueint;

int socializing = 0; 
char palavra[20] = "senha #5798";

/* push button, leds, filas e semáforos */
void task1(void *p);
void task2(void *p);
void task3(void *p);
void task4(void *p);
/* tarefas de atividade em leds para verificar troca de contexto */
void piscar(void *p);
void SineTask(void *p);
/* tarefas de impressão na serial */
void imprimir(void *p);
void imprimir2(void *p);
void imprimir3(void *p);
void imprimir4(void *p);

/*setup*/
void setup() {
    /* 
     *  Parâmetros passados as tarefas.
     *  Obs: precisam ser passados por referencia (&arg1, &arg2, &arg3...)
     */
    int num = 63;;
    char letra = 'a';
    float G = 6.67384;
    Serial.begin(9600);

    /*  Inicializa as tarefas, semáforos e filas: eos_initial(num_tarefas, num_semáforos, num_filas) */
      eos_initial(6,1,1);
    /* Criando semáforo eos_create_semaphore(&nome_semaforo, tempo)*/ 
      serialSemaphore = eos_create_semaphore(&serialSemaphore, 2000); 
    /* Criando fila eos_create_queue(&nome_fila, num_elementos, tamanho dos elementos)*/ 
      valueint = eos_create_queue(&valueint, 4, 8);
      
    /*Serial.println("Creating task...");
    eos_create_task(task1, NULL, 150);
    eos_create_task(task2, NULL, 150);
    eos_create_task(task3, NULL, 150);
    eos_create_task(task4, NULL, 150);*/

    /* Criando tarefas eos_create_task(codigo, argumento(endereço), tam_pilha(64-256)bytes))*/ 
    Serial.println("Creating task...");
    eos_create_task(piscar, NULL, 256);
    eos_create_task(SineTask, NULL, 256);
    eos_create_task(imprimir, &num, 256);
    eos_create_task(imprimir2, &letra, 256);
    eos_create_task(imprimir3, &G, 256);
    eos_create_task(imprimir4, NULL, 256);
    /* inicia o sistema com valor de time slice eos_start(time_slice)*/
    eos_start(5);    
}

void loop() {/* Nada é executado aqui */}

/**
 * TASKS
 */
/* task 1 */
void task1(void *p) {
    Serial.println("Task 1 was started");
    while (1) {
        socializing++;
        delay(300);
        Serial.print("1: ");
        Serial.println(socializing);
    }
}
/* task 2 */
void task2(void *p) {
    Serial.println("Task 2 was started");
    while (1) {
        socializing--;
        delay(250);
        Serial.print("2: ");
        Serial.println(socializing);
    }
}
/* task 3 */
void task3(void *p) {
    Serial.println("Task 3 was started");
    while (1) {
        socializing++;
        delay(450);
        Serial.print("3: ");
        Serial.println(socializing);
    }
}
/* task 4 */
void task4(void *p) {
    Serial.println("Task 4 was started");
    while (1) {
        socializing--;
        delay(550);
        Serial.print("4: ");
        Serial.println(socializing);
    }
}

/*Tarefas de manipulação de LEDs*/
/* piscar */
void piscar(void *p){ 
  while(1){
    // Ativamos o pino 13 (colocando 5v nele)  
    digitalWrite(LED_BLINK, HIGH);    
    // Aguardamos 1 segundo
    delay(500); 
    // Desligamos o pino 13
    digitalWrite(LED_BLINK, LOW);
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
  while(1){
    Serial.print("tarefa1 inicio "); Serial.println(exe);
    exe++;
    eos_queue_write(&valueint,&n); /* escreve o valor recebido em uma fila */
    Serial.print("valor enviado: "); Serial.println(n);
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
  int _max = 20; /* quantidade de execuções da tarefa */       
  for(int i = 0; i < _max; i++){
    Serial.print("tarefa2 inicio "); Serial.println(i+1); /* essa tarefa tem executa uma quantidade de vezes */
    delay(200);
    Serial.print("tarefa2 meio: recebi o char"); Serial.print(l); 
    delay(700);
    Serial.println("tarefa2 f"); 
    delay(100);
    if(eos_semaphore_take(&serialSemaphore)){ /* pega um semáforo e escreve na serial */
        Serial.println("nao desista dos seus sonhos até porque você já ouviu ");delay(1000);
        Serial.println("falar em cabo de vassoura, supunhetamos que você vai Ter a vida que");delay(1000);
        Serial.println("você é hoje, Daqui uns anos, Num sei, Vai que  Nasça uma planta Pra sobreviver"); delay(1000);
        Serial.println("E q a.. Pode ser que mude A alternativa Do.. Dom pedro segundo e o padre quando");delay(1000); 
        Serial.println("for rezar a missa Vai querer uma Latinha de cerveja E aí? interprete...");delay(1000);
       eos_semaphore_give(&serialSemaphore);
      }
    }
  }

 void imprimir3(void *p){
  float r = *(float*)p;  /* recebe um endereço como parâmetro e transforma em char */
  int exe = 1; /* quantidade de execuções da tarefa */
  while(1){ /* quantidade de execuções da tarefa */
    Serial.print("tarefa3 inicio "); Serial.println(exe);
    exe++;
    Serial.print("recebi o valor: "); Serial.println(r);
    delay(200);
    Serial.println("tarefa3 meio");
    delay(400);
    Serial.println("tarefa3 fim"); 
    delay(200);
    }
  }

 void imprimir4(void *p){
  int received;
  int _max = 50; /* quantidade de execuções da tarefa */
  for(int i = 0; i < _max; i++){
    Serial.print("tarefa4 inicio "); Serial.println(i+1);
    received = eos_queue_read(&valueint); /* ler valores de uma pilha */
    Serial.print("valor recebido: ");Serial.println(received); /* escreve valor lido */
    delay(100);
    Serial.println("tarefa4 meio");
    delay(300);
    Serial.println("tarefa4 fim"); 
    delay(600);
    }
  }

///* Variáveis de bush button */
//const int buttonPin = 2;     // the number of the pushbutton pin
//const int ledPin =  13;      // the number of the LED pin
//int buttonState = 0;         // variable for reading the pushbutton status
//void pushButtonTask(void *p);
//void pushButtonTask(void *p){ 
//    /* initialize the LED pin as an output:*/
//    pinMode(ledPin, OUTPUT);
//    /*  initialize the pushbutton pin as an input:*/
//    pinMode(buttonPin, INPUT);
//    while(true) {
//       /*  read the state of the pushbutton value:*/
//        buttonState = digitalRead(buttonPin);   
//        /*  check if the pushbutton is pressed. If it is, the buttonState is HIGH*/:
//        if(buttonState == HIGH){
//          /*  turn LED on:*/
//          digitalWrite(ledPin, HIGH);
//        }else{
//          /*  turn LED off:*/
//          digitalWrite(ledPin, LOW);
//        }
//    }
//}
