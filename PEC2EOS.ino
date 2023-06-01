#include "eos.h"
#include <math.h>

#define AOUT       7
#define LED_BLINK  13

eos_semaphore serialSemaphore;
eos_queue valueint;
int socializing = 0; 
char palavra[20] = "senha #5798";

void task1(void *p);
void task2(void *p);
void task3(void *p);
void task4(void *p);
void piscar(void *p);
void SineTask(void *p);
void imprimir(void *p);
void imprimir2(void *p);
void imprimir3(void *p);
void imprimir4(void *p);

/*setup*/
void setup() {
    int num = 63, num2 = 31;
    char letra = 'a';
    float G = 6.67384;
    Serial.begin(9600);

    eos_initial(3,1,1);
    /*não colocar nenhum código entre eos_initial e inicialização de variável*/
    serialSemaphore = eos_create_semaphore(&serialSemaphore, 2000); 
    valueint = eos_create_queue(&valueint, 4, 8);
    /*Serial.println("Creating task...");
    if(eos_create_task(task1, NULL, 150))
        Serial.println("task 1 returned with error");
    if (eos_create_task(task2, NULL, 150))
        Serial.println("task 2 returned with error");
    if (eos_create_task(task3, NULL, 150))
        Serial.println("task 3 returned with error");
    if (eos_create_task(task4, NULL, 150))
        Serial.println("task 4 returned with error");*/
        
    Serial.println("Creating task...");
    eos_create_task(piscar, NULL, 256);
    eos_create_task(SineTask, NULL, 256);
    eos_create_task(imprimir, &num, 256);
    eos_create_task(imprimir2, &letra, 256);
    eos_create_task(imprimir3, &G, 256);
    eos_create_task(imprimir4, &num2, 256);
    
    eos_start(5);    
}


void loop() {/* Move along */}

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

/* piscar */
void piscar(void *p){ 
  while(1){
    //Serial.println("piscar");
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
        //Serial.println("seno");
        stime = millis();
        for(period = 0; period < 16; ++period){
            etime = millis();
            outpv = (unsigned int)((sin(2*PI*2*(etime - stime)*0.001)+1)*127.5);  /*Senoide = seno(2*PI*Freq*t)*/
            analogWrite(AOUT, outpv);
            delay(33);
        }
    }
}

void imprimir(void *p){
  int n = *(int*)p;
  //while(1){
  for(int i = 0; i < 10; i++){ 
    eos_queue_write(&valueint,&n);
    n++;
    //Serial.print("tarefa1 i "); 
    Serial.print("tarefa1 i "); Serial.println(i);
    Serial.println(n);
    delay(300);
    Serial.println("tarefa1 m");
    delay(300);
    Serial.println("tarefa1 f"); 
    delay(400);
    }
  }

void imprimir2(void *p){
  char l = *(char*)p;
  //while(1){
  for(int i = 0; i < 5; i++){
    Serial.println(l);
    //;
    Serial.print("tarefa2 i "); Serial.println(i);
    delay(200);
    Serial.println("tarefa2 m");
    delay(700);
    Serial.println("tarefa2 f"); 
    delay(100);
    if(eos_semaphore_take(&serialSemaphore)){
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
  float r = *(float*)p;
  //while(1){
  for(int i = 0; i < 5; i++){
    Serial.print("tarefa3 i "); Serial.println(i);
    Serial.println(r);
    delay(200);
    Serial.println("tarefa3 m");
    delay(400);
    Serial.println("tarefa3 f"); 
    delay(200);
//    if(eos_semaphore_take(&serialSemaphore)){
//        Serial.println("quem não tiver as manha, ");delay(1000);
//        Serial.println("não entra não sem as instrução de ");delay(1000);
//        Serial.println("um profissional"); delay(1000);
//        Serial.println("eh eh eh "); delay(1000);
//       //eos_semaphore_give(&serialSemaphore);
//      }
    }
  }

 void imprimir4(void *p){
  int n = *(int*)p;
  int received;
  //Serial.println(n);
  //while(1){
  for(int i = 0; i < 10; i++){
    Serial.print("tarefa4 i "); Serial.println(i);
    received = eos_queue_read(&valueint);
    Serial.println(received);
    //Serial.println(n); n--;
    delay(100);
    Serial.println("tarefa4 m");
    delay(300);
    Serial.println("tarefa4 f"); 
    delay(600);
    }
  }
