#include "queue.h"

/* Queue pool */
static struct eos_queue *queue_pool = NULL;
static int queue_count = 0;

/*-------------------------------------------Queue-------------------------------------------------*/
/*
 * Create a queue to share date
 * @param  handler to semaphore, size queue
 * @return        a queue
 */
struct eos_queue eos_create_queue(eos_queue *q, int size_queue, int size_elements){
  /* Get a new queue from the queue pool */
  queue_pool = (eos_queue*)realloc(queue_pool,(queue_count + 1) * sizeof(eos_queue));
  q = &queue_pool[queue_count];
  /* initialize values */
  q->size_queue = size_queue;
  q->size_elements = size_elements;
  /* alloc memory space and set values 0*/
  (q->data) = (int*)calloc(size_queue, sizeof(int)); 
  (q->value) = (float*)calloc(size_queue, sizeof(float));
  (q->mensg) = (char*)calloc(size_queue, sizeof(char));   
  queue_count++;
  return *q;
}

/*
 * Write in the begin of the queue
 * @param  queue, pointer to the value
 * Obs: loss the last element of the queue data[sizeQueue-1] and write on the first data[0].
 *      begin of queue [last element of array] or the first in
 * @return        void*/
void eos_queue_write(eos_queue *q, void *value){
  DISABLE_INTERRUPTS();
  for(int i = q->size_queue - 1; i > 0; i--){
    q->data[i] = q->data[i-1];
    q->value[i] = q->value[i-1];
    q->mensg[i] = q->mensg[i-1];
  }
  q->data[0] = *(int*)value; 
  q->value[0] = *(float*)value;
  q->mensg[0] = *(char *)value;
  ENABLE_INTERRUPTS();
}

/*
 * Write in the send elements to the queue, Add elements to the begin of the queue and increase the queue
 * @param  queue, pointer to the value
 * @return        void
 */
void eos_queue_send(eos_queue *q, void *value){
  DISABLE_INTERRUPTS();
  q->data = (int*)realloc(q->data, ((q->size_queue) + 1)*sizeof(int));
  q->value = (float*)realloc(q->value, ((q->size_queue) + 1)*sizeof(float));
  q->mensg = (char*)realloc(q->mensg, ((q->size_queue) + 1)*sizeof(char));
  q->size_queue++;
  eos_queue_write(q, value);  
  ENABLE_INTERRUPTS();
}
/*
 * Read elements from queue, but to retire
 * FIFO first in first out return the last element
 * @param  queue, pointer to the value
 * @return        the respective value
 */
void *eos_queue_read(eos_queue *q){
  DISABLE_INTERRUPTS();
  int bufferi;
  float bufferf;
  char bufferc;
  void *bufferptr;
  if(q->size_elements == 2){
    bufferi = eos_queue_read_int(q);
    bufferptr = &bufferi;
    ENABLE_INTERRUPTS();
    return bufferptr;
  }
  else if(q->size_elements == 4){
    bufferf = eos_queue_read_float(q);
    bufferptr = &bufferf;
    ENABLE_INTERRUPTS();
    return bufferptr;
  }
  else {  
    bufferc = eos_queue_read_char(q);
    bufferptr = &bufferc;
    ENABLE_INTERRUPTS();
    return bufferptr;
  } 
}
/* auxilar function to read */
int eos_queue_read_int(eos_queue *q){
  return q->data[q->size_queue-1]; 
}
float eos_queue_read_float(eos_queue *q){
  return q->value[q->size_queue-1]; 
}
char eos_queue_read_char(eos_queue *q){
  return q->mensg[q->size_queue-1]; 
}

/*
 * Receive a element of queue and remove the element
 * FIFO first in first out return the last element
 * @param  queue, pointer to the value
 * @return        the respective value
 */
void *eos_queue_receive(eos_queue *q){
  DISABLE_INTERRUPTS();
  int bufferi;
  float bufferf;
  char bufferc;
  void *bufferptr;
  if(q->size_elements == 2){
    bufferi = eos_queue_receive_int(q);
    bufferptr = &bufferi;
    ENABLE_INTERRUPTS();
    return bufferptr;
  }
  else if(q->size_elements == 4){
    bufferf = eos_queue_receive_float(q);
    bufferptr = &bufferf;
    ENABLE_INTERRUPTS();
    return bufferptr;
  }
  else {  
    bufferc = eos_queue_receive_char(q);
    bufferptr = &bufferc;
    ENABLE_INTERRUPTS();
    return bufferptr;
  } 
}
/* auxiliar function to receive */
int eos_queue_receive_int(eos_queue *q){
  //DISABLE_INTERRUPTS();
  /* save the first out*/
  int buffer_temp = eos_queue_read_int(q);
  /* free memory */
  int last = (q->size_queue)-1;
    q->data = (int*)realloc(q->data, ((q->size_queue) - 1)*sizeof(int));
  q->size_queue--;
  //ENABLE_INTERRUPTS();
  return (buffer_temp); 
}
float eos_queue_receive_float(eos_queue *q){
  //DISABLE_INTERRUPTS();
  /* save the first out*/
  float buffer_temp = eos_queue_read_float(q);
  /* free memory */
  int last = (q->size_queue)-1;
    q->value = (float*)realloc(q->value, ((q->size_queue) - 1)*sizeof(float));
  q->size_queue--;
  //ENABLE_INTERRUPTS();
  return (buffer_temp);  
}
char eos_queue_receive_char(eos_queue *q){
  //DISABLE_INTERRUPTS();
  /* save the first out*/
  char buffer_temp = eos_queue_read_char(q);
  /* free memory */
  char last = (q->size_queue)-1;
    q->mensg = (char*)realloc(q->mensg, ((q->size_queue) - 1)*sizeof(char));
  q->size_queue--;
  //ENABLE_INTERRUPTS();
  return (buffer_temp); 
}
/*-------------------------------------------------------------------------------------------------*/
