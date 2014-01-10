/***********************************GEC*****************************************
================================================================================
Author: Guilherme Cavalheiro.
Title: MVP1
Date: 
Rev.: 0.
BSP ultrassom
================================================================================
************************************GEC****************************************/

#include "config.h"
#include "u_sensor.h"

int count1 = 0;
int main(void)
{  
  WDTCTL = WDTPW + WDTHOLD;
  DCOCTL = 0;                               
  BCSCTL1 = CALBC1_1MHZ;                    
  DCOCTL = CALDCO_1MHZ;                     
  BCSCTL3 |= LFXT1S_2;    
  
  
  P1DIR |= LEDS;
  P1OUT |= LEDS;
  P1IES = SW;               
  P1IFG &= ~SW;          
  P1IE = SW;                
  P1OUT |=  SW;
  P1REN |=  SW;
 
  
  __delay_cycles(10000);
  P1OUT &= ~LEDS;
  
  serialInit();
  start_usens();
  CCTL0 = CCIE;                             
  TACTL = TASSEL_2 + MC_1 + ID_3;           
  CCR0 =  50000;
  
  
  txSerial("Funcionando\r\n",13);
  __enable_interrupt();
  
  int cent,i;
  char test[5];
  
  while(1)
  {
   
   if(count1%1000)
   {
     count1 = 0;
        
        for(i = 0; i < 100; i++)
          __delay_cycles(1000);
        cent = getEcho();
        test[0] = (cent/10000) + 0x30;
        test[1] = (cent%10000)/1000 + 0x30;
        test[2] = (((cent%10000)%1000)/100) + 0x30;
        test[3] = ((((cent%10000)%1000)%100)/10) +0x30; 
        test[4] = ((((cent%10000)%1000)%100)%10) + 0x30;
        txSerial("\n\r",3);    
        txSerial("ECHO = ",7);
        txSerial(test,5);
        txSerial(" cm",3); 
   }
  }
}

#pragma vector=PORT1_VECTOR
__interrupt void port1_ISR (void)
{    
  if(P1IFG & SW)                       
  {
    count1++;
  }
  P1IFG &= ~(SW);
}

#pragma vector=TIMERA0_VECTOR
__interrupt void Timer_A(void)
{
   count1 ++;
}
