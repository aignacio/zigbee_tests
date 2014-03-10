#include "u_sensor.h"

unsigned int
  new_cap    = 0, 
  old_cap    = 0,
  cap_diff   = 0,
  count      = 0,
  centimeter = 0,
  store_cap[NUM_CAP];

bool cap_st = 0;
int written = 0;


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




void start_usens(void)
{
  P2DIR |= VCC_ULTRA;
  P2OUT &= ~VCC_ULTRA;
  P4DIR |= TRIGGER;
  P4OUT &= ~TRIGGER;
  P4DIR &= ~ECHO;
  P4SEL |= ECHO;
}
void trigger(void)
{
  volatile int i;
  P4OUT |= TRIGGER;
  for(i = 0; i < 50; i++);
  P4OUT &= ~TRIGGER;
  for(i = 0; i < 50; i++);
  count++;
  __delay_cycles(1);
}

int getEcho(void)
{
  int i, echoValue;
  bool fsm_control = 1;
  bool flag_start = 1;
 
  fsm_usens = idle;
  
  __istate_t s = __get_interrupt_state();
  
  while(fsm_control == 1)
  {
    switch(fsm_usens)
    {
      case idle: 
                  if(flag_start)
                  {
                    for(i = 0; i < NUM_CAP; i++)
                      store_cap[i] = 0;
                    fsm_usens = start;
                    written = 0;
                    flag_start = 0;
                    count = 0;
                    echoValue = 0;
                  }
                  else
                    fsm_usens = capturing;
                  break;
      case start: 
                    __disable_interrupt();
                    TBCCTL0 = CCIE + CM_3 + SCS + CCIS_1 + CAP + ID_1;                           
                    TBCTL = TBSSEL_2 + MC_2;
                    __bis_SR_register(GIE);
                    flag_start = 0;
                    P2OUT |= VCC_ULTRA;
                    __delay_cycles(100000);
                    fsm_usens = capturing;
                    break;
      case capturing:
                  cap_st = 1;
                  //TBCCR0 = 0;
                  //new_cap = 0;
                  //old_cap = 0;
                  //cap_diff = 0;
                  trigger();
                  fsm_usens = end_cap;
                  break;
      case end_cap:
                  if(written > 0)
                  {
                    cap_st = 0;
                    written = 0;
                    if(count == NUM_CAP)
                    {
                      count = 0;
                      fsm_control = 0;
                      for(i = 0; i < NUM_CAP; i++)
                        echoValue = echoValue + store_cap[count];
                      P4OUT &= ~TRIGGER;
                      P2OUT &= ~VCC_ULTRA;
                      __delay_cycles(100);
                      __disable_interrupt();
                      __set_interrupt_state(s);
                    }  
                    else
                    {
                      __delay_cycles(100);
                      fsm_usens = capturing;
                    }
                  }
                  break;
      default: 
                  break;
    }
    
  }
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
        if(cap_diff > 21000 & !(P4IN & ECHO))
        {
          TBCCR0 = 0;
          store_cap[count-1] = 21000;
          written++;
        }
        else if(!(P4IN & ECHO))
        {
            TBCCR0 = 0;
            store_cap[count-1] = cap_diff;
            written++;
        }
    }
}





