#ifndef _TIMERS_H_
#define _TIMERS_H_

#ifdef __cplusplus
extern "C" {
#endif

/*to choice a timer*/
#ifndef EOSTMR
#define EOSTMR 3
#endif

/*use of TIMER 0 is avoid because of delay, millis, etc*/
#if (EOSTMR == 0)
  #define TIMERx_OVF_vect TIMER0_OVF_vect 
  #define TIMSKx TIMSK0
  #define NORMALMODExA (~((1<<WGM01) | (1<<WGM00)))
  #define NORMALMODExB (~(1<<WGM02))
  #define TCCRxA TCCR0A
  #define TCCRxB TCCR0B
  #define TCNTx TCNT0
  #define TOIEx TOIE0
  #define OCIExA OCIE0A
  #define TIMERPRESETx 131
  #define SETBITS ((1<<CS02)  | (1<<CS00))
  #define CLEARBITS ((1<<CS02)  | (1<<CS00)) 

#elif (EOSTMR == 1)
  #define TIMERx_OVF_vect TIMER1_OVF_vect 
  #define TIMSKx TIMSK1
  #define NORMALMODExA (~((1<<WGM11) | (1<<WGM10)))
  #define NORMALMODExB (~(1<<WGM12))
  #define TCCRxA TCCR1A
  #define TCCRxB TCCR1B
  #define TCNTx TCNT1
  #define TOIEx TOIE1
  #define OCIExA OCIE1A
  #define TIMERPRESETx 65520
  #define SETBITS ((1<<CS12)  | (1<<CS10))
  #define CLEARBITS ((1<<CS12)  | (1<<CS10)) 

#elif (EOSTMR == 2)
  #define TIMERx_OVF_vect TIMER2_OVF_vect 
  #define TIMSKx TIMSK2
  #define NORMALMODExA (~((1<<WGM21) | (1<<WGM20)))
  #define NORMALMODExB (~(1<<WGM22))
  #define TCCRxA TCCR2A
  #define TCCRxB TCCR2B
  #define TCNTx TCNT2
  #define TOIEx TOIE2
  #define OCIExA OCIE2A
  #define TIMERPRESETx 131
  #define SETBITS ((1<<CS22)  | (1<<CS20))
  #define CLEARBITS ((1<<CS22)  | (1<<CS20)) 

#elif (EOSTMR == 3)
  #define TIMERx_OVF_vect TIMER3_OVF_vect 
  #define TIMSKx TIMSK3
  #define NORMALMODExA (~((1<<WGM31) | (1<<WGM30)))
  #define NORMALMODExB (~(1<<WGM32))
  #define TCCRxA TCCR3A
  #define TCCRxB TCCR3B
  #define TCNTx TCNT3
  #define TOIEx TOIE3
  #define OCIExA OCIE3A
  #define TIMERPRESETx 65520
  #define SETBITS ((1<<CS32)  | (1<<CS30))
  #define CLEARBITS ((1<<CS32)  | (1<<CS30)) 

#elif (EOSTMR == 4)
  #define TIMERx_OVF_vect TIMER4_OVF_vect 
  #define TIMSKx TIMSK4
  #define NORMALMODExA (~((1<<WGM41) | (1<<WGM40)))
  #define NORMALMODExB (~(1<<WGM42))
  #define TCCRxA TCCR4A
  #define TCCRxB TCCR4B
  #define TCNTx TCNT4
  #define TOIEx TOIE4
  #define OCIExA OCIE4A
  #define TIMERPRESETx 65520
  #define SETBITS ((1<<CS42)  | (1<<CS40))
  #define CLEARBITS ((1<<CS42)  | (1<<CS40)) 

#elif (EOSTMR == 5)
  #define TIMERx_OVF_vect TIMER5_OVF_vect 
  #define TIMSKx TIMSK5
  #define NORMALMODExA (~((1<<WGM51) | (1<<WGM50)))
  #define NORMALMODExB (~(1<<WGM52))
  #define TCCRxA TCCR5A
  #define TCCRxB TCCR5B
  #define TCNTx TCNT5
  #define TOIEx TOIE5
  #define OCIExA OCIE5A
  #define TIMERPRESETx 65520
  #define SETBITS ((1<<CS52)  | (1<<CS50))
  #define CLEARBITS ((1<<CS52)  | (1<<CS50)) 

#endif

#ifdef __cplusplus
}
#endif

#endif /* _TIMERS_H_ */
