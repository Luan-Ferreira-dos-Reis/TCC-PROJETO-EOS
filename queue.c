#include "queue.h"

/* Queue pool */
static struct Queue *queuePool = NULL;
static int queueCount = 0;

/*-------------------------------------------Queue-------------------------------------------------*/
/*
 * Create a queue to share date
 * @param  size queue, size_elements
 * @return        a queue
 */
struct Queue createQueue(int sizeQueue, int sizeElements){
  Queue *queue;
  /* Get a new queue from the queue pool */
  queuePool = (Queue*)realloc(queuePool,(queueCount + 1) * sizeof(Queue));
  queue = &queuePool[queueCount];
  /* initialize values */
  queue->sizeQueue = sizeQueue;
  queue->sizeElements = sizeElements;
  /* alloc memory space and set values 0*/
  (queue->data) = (int*)calloc(sizeQueue, sizeof(int)); 
  (queue->value) = (float*)calloc(sizeQueue, sizeof(float));
  (queue->mensg) = (char*)calloc(sizeQueue, sizeof(char));   
  queueCount++;
  return *queue;
}

/*
 * Write in the begin of the queue
 * @param  queue, pointer to the value
 * Obs: loss the last element of the queue data[sizeQueue-1] and write on the first data[0].
 *      begin of queue [last element of array] or the first in
 * @return        void*/
void queueWrite(Queue *queue, void *value){
  DISABLE_INTERRUPTS();
  for(int i = queue->sizeQueue - 1; i > 0; i--){
    queue->data[i] = queue->data[i-1];
    queue->value[i] = queue->value[i-1];
    queue->mensg[i] = queue->mensg[i-1];
  }
  queue->data[0] = *(int*)value; 
  queue->value[0] = *(float*)value;
  queue->mensg[0] = *(char *)value;
  ENABLE_INTERRUPTS();
}

/*
 * Write in the send elements to the queue, Add elements to the begin of the queue and increase the queue
 * @param  queue, pointer to the value
 * @return        void
 */
void queueSend(Queue *queue, void *value){
  DISABLE_INTERRUPTS();
  queue->data = (int*)realloc(queue->data, ((queue->sizeQueue) + 1)*sizeof(int));
  queue->value = (float*)realloc(queue->value, ((queue->sizeQueue) + 1)*sizeof(float));
  queue->mensg = (char*)realloc(queue->mensg, ((queue->sizeQueue) + 1)*sizeof(char));
  queue->sizeQueue++;
  queueWrite(queue, value);  
  ENABLE_INTERRUPTS();
}
/*
 * Read elements from queue, but to retire
 * FIFO first in first out return the last element
 * @param  queue, pointer to the value
 * @return        the respective value
 */
void *queueRead(Queue *queue){
  DISABLE_INTERRUPTS();
  int bufferi;
  float bufferf;
  char bufferc;
  void *bufferptr;
  if(queue->sizeElements == 2){
    bufferi = queueReadInt(queue);
    bufferptr = &bufferi;
    ENABLE_INTERRUPTS();
    return bufferptr;
  }
  else if(queue->sizeElements == 4){
    bufferf = queueReadFloat(queue);
    bufferptr = &bufferf;
    ENABLE_INTERRUPTS();
    return bufferptr;
  }
  else {  
    bufferc = queueReadChar(queue);
    bufferptr = &bufferc;
    ENABLE_INTERRUPTS();
    return bufferptr;
  } 
}
/* auxilar function to read */
int queueReadInt(Queue *queue){
  return queue->data[queue->sizeQueue-1]; 
}
float queueReadFloat(Queue *queue){
  return queue->value[queue->sizeQueue-1]; 
}
char queueReadChar(Queue *queue){
  return queue->mensg[queue->sizeQueue-1]; 
}

/*
 * Receive a element of queue and remove the element
 * FIFO first in first out return the last element
 * @param  queue, pointer to the value
 * @return        the respective value
 */
void *queueReceive(Queue *queue){
  DISABLE_INTERRUPTS();
  int bufferi;
  float bufferf;
  char bufferc;
  void *bufferptr;
  if(queue->sizeElements == 2){
    bufferi = queueReceiveInt(queue);
    bufferptr = &bufferi;
    ENABLE_INTERRUPTS();
    return bufferptr;
  }
  else if(queue->sizeElements == 4){
    bufferf = queueReceiveFloat(queue);
    bufferptr = &bufferf;
    ENABLE_INTERRUPTS();
    return bufferptr;
  }
  else {  
    bufferc = queueReceiveChar(queue);
    bufferptr = &bufferc;
    ENABLE_INTERRUPTS();
    return bufferptr;
  } 
}
/* auxiliar function to receive */
int queueReceiveInt(Queue *queue){
  /* save the first out*/
  int bufferTemp = queueReadInt(queue);
  /* free memory */
  int last = (queue->sizeQueue)-1;
    queue->data = (int*)realloc(queue->data, ((queue->sizeQueue) - 1)*sizeof(int));
  queue->sizeQueue--;
  return (bufferTemp); 
}
float queueReceiveFloat(Queue *queue){
  /* save the first out*/
  float bufferTemp = queueReadFloat(queue);
  /* free memory */
  int last = (queue->sizeQueue)-1;
    queue->value = (float*)realloc(queue->value, ((queue->sizeQueue) - 1)*sizeof(float));
  queue->sizeQueue--;
  return (bufferTemp);  
}
char queueReceiveChar(Queue *queue){
  /* save the first out*/
  char bufferTemp = queueReadChar(queue);
  /* free memory */
  char last = (queue->sizeQueue)-1;
    queue->mensg = (char*)realloc(queue->mensg, ((queue->sizeQueue) - 1)*sizeof(char));
  queue->sizeQueue--;
  return (bufferTemp); 
}
/*-------------------------------------------------------------------------------------------------*/
