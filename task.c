#include "task.h"
/*-------------------------------------Global Variables----------------------------------------------*/
/* Task pool */
static struct eos_task *task_pool = NULL;
/* Pointer to head of task queue (NULL if empty) */
static struct eos_task *task_queue = NULL;
/* Pointer to currently running task (NULL if none) */
static struct eos_task *current_task = NULL;
/* dummy pool */
static struct eos_task dummy, dummy2;
/* Timeslice */
static int time_slice; 
static int time_count;
int preempt; /* non static, will be modified by semaphore archive. Enable or disable context switch using when semaphores work(preempt = 0) */
static int port_max_delay; /* max delay permited to semaphores */
static int task_count = 0;
/*-------------------------------------------------------------------------------------------------------*/
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
void idle_task(void *args) {
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
int eos_create_task(eos_task *new_task, void (*runner)(void *runner_arg), void *arg, int size_stack) { /* LUAN FERREIRA DOS REIS */
    int i;
    char *stack;
    //struct eos_task *new_task;

    /* Ensure the size of stack between allowed*/
    if(size_stack > TASKMAXSTACKSIZE)
      size_stack = TASKMAXSTACKSIZE;
    if(size_stack < TASKMINSTACKSIZE)
      size_stack = TASKMINSTACKSIZE;

    /* Not allowed to create while running */
    if (current_task) return -1;

    /* Get a new task from the task pool */
    task_pool = (eos_task*)realloc(task_pool, (task_count + 1) * sizeof(eos_task));
    new_task = &task_pool[task_count];
    
    /* save the task struct data*/
    new_task->task_id = task_count++;
    new_task->code = runner;
    new_task->state = READY;
    
    /* Achieve a reduction of about 40% to 7% dynamic allocation memory*/
    /* Get memory for stack from the stack pool with dynamic allocation */ 
    stack = (char*)malloc(size_stack * sizeof(char));

    /* Prepare the stack */
    stack = stack + size_stack - 1;
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
    DEQUEUE();
    eos_switch_task();
    ENABLE_INTERRUPTS();
}
/*-------------------------------------------------------------------------------------------------------*/
/*------------------------------------------Kernel functions----------------------------------------*/
/**
 * perform initial values to kernel
 * create idle task, create task pool
 * @param  int number of tasks
 * @return void
 */
void eos_initial(){  
}
/**
 * Initializes the kernel by setting up the interrupt routine to fire each 1 ms
 * Timer interrupt regards:
 *     http://popdevelop.com/2010/04/mastering-timer-interrupts-on-the-arduino/
 * @param  ts Desired size of a timeslice
 * @return    -1 if error during initialization (does not return otherwise!)
 */
int eos_start(int ts, int max_delay) {
    /* Create 2 idle task that always run in eos*/
    eos_create_task(&dummy, idle_task, NULL, TASKMINSTACKSIZE);
    
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
    
    /* Set the size of a timeslice  and port_max_delay from parameter */
    time_slice = ts;
    if(max_delay > PORTMAXDELAY){ max_delay = PORTMAXDELAY;}
    port_max_delay = max_delay; 
    preempt = 1; /* enable preemption preempt */ 

    /* Used by eos_switch_task for the first task switch */
    current_task = &dummy;
    DISABLE_INTERRUPTS();
    eos_switch_task();
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
    if (time_count >= time_slice && preempt || time_count >= port_max_delay) {
        time_count = 0;
        preempt = 1;
        eos_context_switch();
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
    eos_context_switch();
    /* Restore registers from stack and return */
    POPREGISTERS();
    RET();
}
