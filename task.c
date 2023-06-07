#include "task.h"
/*-------------------------------------Global Variables------------------------------------------------*/
/* Task pool */
static struct Task *taskPool = NULL;
/* Pointer to head of task queue (NULL if empty) */
static struct Task *taskQueue[5] = {NULL, NULL, NULL, NULL, NULL};
/* Pointer to currently running task (NULL if none) */
static struct Task *currentTask = NULL;
/* Pointer to currently when a task finished (NULL if none) */
static struct Task *nextTask = NULL;
/* dummy pool */
static struct Task dummy;
/* Timeslice */
static int timeSlice; 
static int timeCount;
int preempt; /* non static, will be modified by semaphore archive. Enable or disable context switch using when semaphores work(preempt = 0) */
static int portMaxDelay; /* max delay permited to semaphores */
static int taskCount = 0;
static int layersPriority = 1; /* number of layers of priority run*/
static int currentLayer = 0; /* actual layers of priority run*/
/*-------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------Task-------------------------------------------------------*/
/**
 * Creates a new task by allocating a new stack, clear it, save SP and
 * add it to the queue of tasks
 * @param  runner Pointer to function where the task resides
 * @return        0 on success; -1 on error
 */
int createTask(Task *newTask, void (*runner)(void *runnerArg), void *arg, int sizeStack, int priority) { /* LUAN FERREIRA DOS REIS */
    int i;
    char *stack;

    /* Ensure the size of stack between allowed*/
    if(sizeStack > TASKMAXSTACKSIZE)
      sizeStack = TASKMAXSTACKSIZE;
    if(sizeStack < TASKMINSTACKSIZE)
      sizeStack = TASKMINSTACKSIZE;

    /* Not allowed to create while running */
    if (currentTask) return -1;

    /* Get a new task from the task pool */
    taskPool = (Task*)realloc(taskPool, (taskCount + 1) * sizeof(Task));
    newTask = &taskPool[taskCount];
    
    /* save the task struct data*/
    newTask->taskId = taskCount++;
    newTask->code = runner;
    newTask->state = READY;
    
    /* Achieve a reduction of about 40% to 7% dynamic allocation memory*/
    /* Get memory for stack from the stack pool with dynamic allocation */ 
    stack = (char*)malloc(sizeStack * sizeof(char));

    /* Prepare the stack */
    stack = stack + sizeStack - 1;
    *(stack--) = 0x00;              /* Safety distance */
    
    /* save pointer to call function task code to stack */ /* LUAN FERREIRA DOS REIS */
    *(stack--) = lower8((void *)makeCallfunc);
    *(stack--) = upper8((void *)makeCallfunc); 

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
    newTask->arg = arg; /* LUAN FERREIRA DOS REIS */
    newTask->stack = stack; /* LUAN FERREIRA DOS REIS */
    
    /* stack is now the stack pointer. Add the task to the queue */
    newTask->spLow = lower8(stack);
    newTask->spHigh = upper8(stack);

    /* ensure validate number of layer of priority [priority0, priority1, ..., priorityX]*/
    if(priority >= layersPriority){ layersPriority = priority + 1;}
    /* add the task to queueTask[layer]*/
    enqueue(newTask, priority);
    
    return 0;
}
/**
 * Insert *task into back of queue by modifying pointers of the head and tail
 * of the queue, and the pointers of *task itself.
 * @param head   Head of the queue
 * @param task The task to insert into the queue
 */
void enqueue(struct Task *task, int layer) {
    if (taskQueue[layer] == NULL) { /* If empty, just set task to be head */
        taskQueue[layer] = task;
        task->next = task;
        task->prev = task;
    } else { /* not empty => add to the back of the queue */
        struct Task *head = taskQueue[layer];
        struct Task *tail = head->prev;
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
struct Task *dequeue(struct Task *task, int layer) {
    /* If 1 element in queue => now empty */
    if (taskQueue[layer]->prev == taskQueue[layer]->next)
        taskQueue[layer] = NULL;
    else if (task == taskQueue[layer]) /* If task is head of queue */
        taskQueue[layer] = task->next; /* => task_queue points to new head */
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
void idleTask(void *args) {
  int i = 0;
  while(1){
      i++; 
  }
}
/**
 * perform call to task code
 * @param  void 
 * @return void
 */
void makeCallfunc(void){
    /*  call task code */
    currentTask->code(currentTask->arg);
    /*  if task code return need to remove task from queue */
    DISABLE_INTERRUPTS();
    /* updates the task state (realy not necessary)*/
    currentTask->state = FINISHED;
    /* remove task from queue, context switch and enable interrupts */  
    DEQUEUE();
    switchTask();
    ENABLE_INTERRUPTS();
}
/*-------------------------------------------------------------------------------------------------------*/
/*------------------------------------------Kernel functions----------------------------------------*/
/**
 * Initializes the kernel by setting up the interrupt routine to fire each 1 ms
 * Timer interrupt regards:
 *     http://popdevelop.com/2010/04/mastering-timer-interrupts-on-the-arduino/
 * @param  ts Desired size of a timeslice
 * @return    -1 if error during initialization (does not return otherwise!)
 */
int startSystem(int ts, int maxDelay) {
    /* Create 2 idle task that always run in eos*/
    createTask(&dummy, idleTask, NULL, TASKMINSTACKSIZE, layersPriority-1);
    
    if (taskQueue[currentLayer] == NULL)
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
    
    /* Set the size of a timeslice  and port_max_delay from parameter */
    timeSlice = ts;
    if(maxDelay > PORTMAXDELAY || maxDelay < 0){ maxDelay = PORTMAXDELAY;}
    portMaxDelay = maxDelay; 
    preempt = 1; /* enable preemption preempt */ 

    /* Used by eos_switch_task for the first task switch */
    currentTask = &dummy;
    currentLayer = 0; ;
    DISABLE_INTERRUPTS();
    switchTask();
    ENABLE_INTERRUPTS();
    
    while (1); /* Will never reach this while loop */
    return 0;  /* And definitely not this return statement */
}
/*------------------------------------------------------------------------------------------------------*/
/*----------------------------------- ISR AND CONTEXT SWITCH -------------------------------------------*/
/**
 * Interrupt service routine
 * ISR run with 1 kHz
 */
ISR(TIMERx_OVF_vect, ISR_NAKED) {
    /* Save registers on stack */
    PUSHREGISTERS();
    /* Reset timer */
    TCNTx = TIMERPRESETx;
    /* If the timeslice has expired, we perform a context switch or reach the maximum delay */
    if (timeCount >= timeSlice && preempt || timeCount >= portMaxDelay) {
        timeCount = 0;
        preempt = 1;
        CONTEXT_SWITCH();
    } else
        timeCount++;
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
void __attribute__ ((naked, noinline)) switchTask(void) {
    /* Save registers on stack */
    PUSHREGISTERS();
    CONTEXT_SWITCH();
    /* Restore registers from stack and return */
    POPREGISTERS();
    RET();
}
