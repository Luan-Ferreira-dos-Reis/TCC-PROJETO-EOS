#include <stdlib.h>
 
#include "eos.h"

/*-------------------------------------Global Variables----------------------------------------------*/
/* Task pool */
static struct eos_task *task_pool = NULL;
/* Pointer to head of task queue (NULL if empty) */
static struct eos_task *task_queue = NULL;
/* Pointer to currently running task (NULL if none) */
static struct eos_task *current_task = NULL;
/* dummy pool */
static struct eos_task dummy;
/* Semaphore pool */
static struct eos_semaphore *semaphore_pool = NULL;
/* Queue pool */
static struct eos_queue *queue_pool = NULL;
/* Timeslice */
static int time_slice; /* Number of miliseconds given to each task */
static int time_count;
static int task_count = 0;
static int semaphore_count = 0;
static int queue_count = 0;
/*-------------------------------------Global Variables--------------------------------------------------*/

/*--------------------------------------------Task-------------------------------------------------------*/

/**
 * Insert *task into back of queue by modifying pointers of the head and tail
 * of the queue, and the pointers of *task itself.
 * @param head   Head of the queue
 * @param task The task to insert into the queue
 */
void eos_enqueue(struct eos_task *task) {
    if (task_queue == NULL) { /* If empty, just set task to be head */
        task_queue = task;
        task->next = task;
        task->prev = task;
    } else { /* not empty => add to the back of the queue */
        struct eos_task *head = task_queue;
        struct eos_task *tail = head->prev;
        head->prev = task;
        tail->next = task;
        task->prev = tail;
        task->next = head;
    }
}

/**
 * Removes an element from the queue by updating pointers. Returns the element
 * @param  task The task to remove from the queue
 * @return        Pointer to the element
 */
struct eos_task *eos_dequeue(struct eos_task *task) {
    /* If 1 element in queue => now empty */
    if (task_queue->prev == task_queue->next)
        task_queue = NULL;
    else if (task == task_queue) /* If task is head of queue */
        task_queue = task->next; /* => task_queue points to new head */
    task->prev->next = task->next;
    task->next->prev = task->prev;
    return task;
}

/**
 * Nothing, absolutely nothing
 * add it to the queue of tasks
 * @param  void pointer only to beform other task code
 * @return        nothing
 */
void idle_task(void *p) {
  int i = 0;
  while(1){
        i++;
  }
}

/**
 * Creates a new task by allocating a new stack, clear it, save SP and
 * add it to the queue of tasks
 * @param  runner Pointer to function where the task resides
 * @return        0 on success; -1 on error
 */
int eos_create_task(void (*runner)(void *runner_arg), void *arg, int size_stack) { /* LUAN FERREIRA DOS REIS */
    int i;
    char *stack;
    struct eos_task *new_task;

    /* Ensure the size of stack between allowed*/
    if(size_stack > TASKMAXSTACKSIZE)
      size_stack = TASKMAXSTACKSIZE;
    if(size_stack < TASKMINSTACKSIZE)
      size_stack = TASKMINSTACKSIZE;

    /* Not allowed to create while running */
    if (current_task) return -1;

    /* Get a new task from the task pool */
    new_task = &task_pool[task_count];
    new_task->task_id = task_count++;
    new_task->code = runner;
    new_task->state = READY;
    
    /* Get memory for stack from the stack pool */
    /*stack = (char*) &stack_pool[new_task->task_id];*/
    
    /* Achieve a reduction of about 40% to 7% dynamic allocation memory*/
    /* Get memory for stack from the stack pool with dynamic allocation */ 
    stack = (char*)malloc(size_stack * sizeof(char));

    /* Prepare the stack */
    stack = stack + TASKMAXSTACKSIZE - 1;
    *(stack--) = 0x00;              /* Safety distance */
    
    /* save pointer to call function task code to stack */ /* LUAN FERREIRA DOS REIS */
    *(stack--) = lower8((void *)make_callfunc);
    *(stack--) = upper8((void *)make_callfunc); 

    /* Mega 2560 use 3 byte for call/ret addresses the rest only 2 */
    #if defined (__AVR_ATmega2560__)
        *(stack--) = EIND;
    #endif
    *(stack--) = 0x00; /* R1 */
    *(stack--) = 0x00; /* R0 */
    *(stack--) = 0x00; /* SREG */
    /* Mega 1280 and 2560 need to save rampz reg just in case */
    #if defined (__AVR_ATmega2560__) || defined (__AVR_ATmega1280__)
        *(stack--) = RAMPZ;
        *(stack--) = EIND;
    #endif
    
    /* R2-R23 = 30 registers */ /* LUAN FERREIRA DOS REIS */
    for (i=2; i<=23; i++)    
        *(stack--) = 0x00;
    /* pointer to task argument */ /* LUAN FERREIRA DOS REIS */
    *(stack--) = lower8(arg); 
    *(stack--) = upper8(arg);
    /* R26-R31 = 30 registers */ /* LUAN FERREIRA DOS REIS */
   for (i=26; i<=31; i++)    
        *(stack--) = 0x00;
        
    /* pass argument and stack to task struct pointer */ /* LUAN FERREIRA DOS REIS */
    new_task->arg = arg; /* LUAN FERREIRA DOS REIS */
    new_task->stack = stack; /* LUAN FERREIRA DOS REIS */
    
    /* *stack is now the stack pointer. Add the task to the queue */
    new_task->sp_low = lower8(stack);
    new_task->sp_high = upper8(stack);
    eos_enqueue(new_task);

    /* compile for debug option */
    #ifdef DEBUG
      Serial.print(" => task created with id ");
      Serial.println(new_task->task_id);
      eos_print_stack(stack, size_stack);
    #endif
    
    return 0;
}

/**
 * perform call to task code
 * @param  void 
 * @return void
 */
void make_callfunc(void){
    /*  call task code */
    current_task->code(current_task->arg);
    /*  if task code return need to remove task from queue */
    DISABLE_INTERRUPTS();
    /* updates the task state (realy not necessary)*/
    current_task->state = FINISHED;
    /* remove task from queue, context switch and enable interrupts */  
    eos_dequeue(current_task);
    eos_switch_task();
    ENABLE_INTERRUPTS();
}
/*--------------------------------------------Task-------------------------------------------------------*/

/*----------------------------------------semaphores-----------------------------------------------*/
/*
 * Creates a new semaphore 
 * @param  handler to semaphore
 * @return        a semaphore
 */
struct eos_semaphore eos_create_semaphore(eos_semaphore *new_semaphore, int semaphore_time){
  /* Get a new semaphore from the semaphore pool */
    new_semaphore = &semaphore_pool[semaphore_count];
  /*initialize semaphore free*/
    new_semaphore->unlock = 1;
    new_semaphore->time_free = semaphore_time;
    semaphore_count++;

    return *new_semaphore; 
}

/**
 * A task can take a semaphore to apropriate hardware recurse
 * @param  semaphore
 * @return        0 if semaphore is avaible; 0 if not
 */
int eos_semaphore_take(eos_semaphore* semaphore){ 
  /* test if semaphore is unlock*/
  if(semaphore->unlock == 1){
    /* semaphore lock  and task work alone*/
    DISABLE_INTERRUPTS();
    semaphore->unlock = 0;   
    return 1;
  }/* to ocupy semaphore */ 
  else{
    return 0;
  }     
}

/**
 * A task can give up a semaphore
 * @param  semaphore
 * @return        void
 */
void eos_semaphore_give(eos_semaphore* semaphore){
  /* test if semaphore is lock*/
  if(semaphore->unlock == 0){
   /* free semaphore */
    semaphore->unlock = 1;
    ENABLE_INTERRUPTS();
  }/* free semaphore */; 
}
/*----------------------------------------semaphores-----------------------------------------------*/

/*-------------------------------------------queue-------------------------------------------------*/
/*
 * Create a queue to share date
 * @param  handler to semaphore, size queue, size of elements
 * @return        a queue
 */
struct eos_queue eos_create_queue(eos_queue *q, int size_queue, int size_elements){
  /* Get a new queue from the queue pool */
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
int eos_queue_read(eos_queue *q){
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
int eos_queue_receive(eos_queue *q){
  DISABLE_INTERRUPTS();
  /* save the first out*/
  int buffer_temp = eos_queue_read(q);
  /* free memory */
  int last = (q->size_queue)-1;
    q->data = (int*)realloc(q->data, ((q->size_queue) - 1)*sizeof(int));
  q->size_queue--;
  ENABLE_INTERRUPTS();
  return (buffer_temp); 
}
float eos_queue_receive_float(eos_queue *q){
  DISABLE_INTERRUPTS();
  /* save the first out*/
  float buffer_temp = eos_queue_read_float(q);
  /* free memory */
  int last = (q->size_queue)-1;
    q->value = (float*)realloc(q->value, ((q->size_queue) - 1)*sizeof(float));
  q->size_queue--;
  ENABLE_INTERRUPTS();
  return (buffer_temp);  
}
char eos_queue_receive_char(eos_queue *q){
  DISABLE_INTERRUPTS();
  /* save the first out*/
  char buffer_temp = eos_queue_read_char(q);
  /* free memory */
  char last = (q->size_queue)-1;
    q->mensg = (char*)realloc(q->mensg, ((q->size_queue) - 1)*sizeof(char));
  q->size_queue--;
  ENABLE_INTERRUPTS();
  return (buffer_temp); 
}
/*-------------------------------------------Queue-------------------------------------------------*/

/*---------------------------------------------Kernel functions-------------------------------------*/
/**
 * perform initial values to kernel
 * create idle task, create task pool
 * @param  int number of tasks
 * @return void
 */
void eos_initial(int num_task, int num_semaphore, int num_queue){
  /* for a while to allocate 2 idle task(idle_task and scheduler_main) */
  num_task = num_task + 2;
  /*ensure that the number of task be permited*/
  if(num_task > MAXTASKS)
    num_task = MAXTASKS;
  /* creats the task pool array */
  task_pool = (eos_task*)malloc(num_task * sizeof(eos_task));

  /* First create idle task 2 times to ensure always have a task in kernel*/
    eos_create_task(idle_task, NULL, TASKMINSTACKSIZE);
    eos_create_task(idle_task, NULL, TASKMINSTACKSIZE);
  
   /* creats the semaphore pool array */
  if(num_semaphore > 0){
    semaphore_pool = (eos_semaphore*)malloc(num_semaphore * sizeof(eos_semaphore));
  }
  /* creats the queue pool array */
  if(num_queue > 0){
    queue_pool = (eos_queue*)malloc(num_queue * sizeof(eos_queue));
  }
  
}

/**
 * Initializes the kernel by setting up the interrupt routine to fire each 1 ms
 * Timer interrupt regards:
 *     http://popdevelop.com/2010/04/mastering-timer-interrupts-on-the-arduino/
 * @param  ts Desired size of a timeslice
 * @return    -1 if error during initialization (does not return otherwise!)
 */
int eos_start(int ts) {
    if (task_queue == NULL)
        return -1;

    DISABLE_INTERRUPTS();
    if(EOSTMR == 2){
      /* First disable the timer overflow interrupt while we're configuring */
      TIMSKx &= ~(1<<TOIEx);
      /* Configure timer2 in normal mode (pure counting, no PWM etc.) */
      TCCRxA &= NORMALMODExA;
      TCCRxB &= NORMALMODExB;
      /* Select clock source: internal I/O clock */
      ASSR &= ~(1<<AS2);
      /* Disable Compare Match A interrupt enable (only want overflow) */
      TIMSKx &= ~(1<<OCIExA);
      /* Now configure the prescaler to CPU clock divided by 128 */
      TCCRxB |= SETBITS; // Set bits
      TCCRxB &= CLEARBITS; // Clear bit
      /* Finally load end enable the timer */
      TCNTx = TIMERPRESETx;
      TIMSKx |= (1<<TOIEx);
    }else{
      /* First disable the timer overflow interrupt while we're configuring */
      TIMSKx &= ~(1<<TOIEx);
      /* set timer to normal operation OC1A e OC1B  pin disconected */
      TCCRxA = 0;  
      /* clean register */                      
      TCCRxB = 0;    
      /* set prescaler to 1024: CS12 = 1 and CS10 = 1 */                    
      TCCRxB |= (1<<CS10)|(1 << CS12);   
      /* load register to 1 miliseconds overflow*/
      TCNTx = TIMERPRESETx;                                                       
      /* enable the TIMER1*/
      TIMSKx |= (1 << TOIEx);           
    }
    
    /* Set the size of a timeslice from parameter */
    time_slice = ts;

    /* Used by eos_switch_task for the first task switch */
    current_task = &dummy;
    DISABLE_INTERRUPTS();
    eos_switch_task();
    ENABLE_INTERRUPTS();
    
    while (1); /* Will never reach this while loop */
    return 0;  /* And definitely not this return statement */
}

/**
 * Interrupt service routine
 * ISR run with 1 kHz
 */
ISR(TIMERx_OVF_vect, ISR_NAKED) {
    /* Save registers on stack */
    PUSHREGISTERS();
    /* Reset timer */
    TCNTx = TIMERPRESETx;

    /* If the timeslice has expired, we perform a context switch */
    if (time_count == time_slice) {
        time_count = 0;
        eos_context_switch2();
    } else
        time_count++;
    /* Restore registers from stack and return */
    POPREGISTERS();
    RET();
}

/**
 * Performs a context switch and invokes a new task to execute
 * See
 *     "Multitasking on an AVR" by Richard Barry, March 2004 (avrfreaks.net)
 * for further info regarding the "naked" and "__attribute__" keywords
 */
void __attribute__ ((naked, noinline)) eos_switch_task(void) {
    /* Save registers on stack */
    PUSHREGISTERS();
    eos_context_switch2();
    /* Restore registers from stack and return */
    POPREGISTERS();
    RET();
}
/*---------------------------------------------Kernel functions-------------------------------------*/

/*--------------------------------------------debug functions----------------------------------------*/
/**
 * Prints the task queue
 */
void eos_print_queue(void) {
    struct eos_task *iter = task_queue;
    Serial.println("Queue:");
    do {
        Serial.print("  task ");
        Serial.println(iter->task_id);
        iter = iter->next;
    } while (iter != task_queue);
    Serial.println("----------");
}

/**
 * Prints the stack provided for a task in HEX
 * @param stack Pointer to memory area of stack
 * @param bytes Number of bytes of stack to print
 */
  
void eos_print_stack(char *stack, int bytes) {
    int i;
    Serial.print("Stack: (starting at ");
    Serial.print( upper8(stack) , HEX);
    Serial.print( lower8(stack) , HEX);
    Serial.println(")");
    for (i=0; i<bytes; i++) {
        Serial.print("  ");
        Serial.print(upper8(stack+i), HEX);
        Serial.print(lower8(stack+i), HEX);
        Serial.print(": ");
        Serial.println( *(stack+i), HEX );
    }
}
/*--------------------------------------------debug functions----------------------------------------*/
