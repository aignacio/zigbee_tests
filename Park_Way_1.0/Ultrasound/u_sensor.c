#include "u_sensor.h"

unsigned int
  new_cap    = 0, 
  old_cap    = 0,
  cap_diff   = 0,
  count      = 0,
  centimeter = 0,
  store_cap[NUM_CAP];

typedef enum 
{
  idle,
  start,
  capturing, 
  w8_cap,
  end_cap, 
  error
}FSM;
FSM fsm_usens;


bool cap_st = 0;
int written = 0;

void start_usens(void)
{
  P2DIR |= VCC_ULTRA;
  P2OUT &= ~VCC_ULTRA;
  P4DIR |= TRIGGER;
  P4OUT &= ~TRIGGER;
  P4DIR &= ~ECHO;
  P4SEL |= ECHO;
  TBCCTL0 = CCIE + CM_3 + SCS + CCIS_1 + CAP + ID_3;                           
  TBCTL = TBSSEL_2 + MC_2;

}
void trigger(void)
{
  volatile int i,j;
  P4OUT |= TRIGGER;
  for(i = 0; i < 50; i++);
  P4OUT &= ~TRIGGER;
  for(i = 0; i < 50; i++);
  count++;
}

int getEcho(void)
{
  int i, echoValue = 0;
  bool fsm_control = 1;
  bool flag_start = 1;
 
  //__disable_interrupt();
  //TBCCTL0 = CCIE;
  
  count = 0;
  fsm_usens = idle;
  while(fsm_control == 1)
  {
    switch(fsm_usens)
    {
      case idle: 
                  if(flag_start)
                  {
                    //__disable_interrupt();
                    //TBCCTL0 = CCIE + CM_3 + SCS + CCIS_1 + CAP;
                    for(i = 0; i < 1; i++)
                      store_cap[i] = 0;
                    fsm_usens = start;
                    cap_st = 1;
                    written = 0;
                  }
                  else
                    fsm_usens = capturing;
                  break;
      case start: 
                  if(flag_start)
                  {
                    flag_start = 0;
                    P2OUT |= VCC_ULTRA;
                    __delay_cycles(100000);
                  }
                  fsm_usens = capturing;
                  break;
      case capturing:
                  trigger();
                  __delay_cycles(100000);
                    fsm_usens = end_cap;
                  break;
      case end_cap:
                  for(i = 0; i < NUM_CAP; i++)
                    echoValue = echoValue + store_cap[i];
                    fsm_control = 0;
                    cap_st = 0;
                    count = 0;
                    P4OUT &= ~TRIGGER;
                    P2OUT &= ~VCC_ULTRA;
                  break;
      default: 
                  break;
    }
    
  }
  //__enable_interrupt();
  return (echoValue/(60*NUM_CAP));
}
#pragma vector=TIMERB0_VECTOR
__interrupt void Timer_B(void)
{
  if(cap_st)
  {
    new_cap = TBCCR0;
    cap_diff = new_cap - old_cap;
    old_cap = new_cap; 
    if(cap_diff > 21000)
    {
      store_cap[count-1] = 21000;
      written++;
    }
    else if(!(P4IN & ECHO))
    {
        store_cap[count-1] = cap_diff;
        written++;
    }
  }
}





