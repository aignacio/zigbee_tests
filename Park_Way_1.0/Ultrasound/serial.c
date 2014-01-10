/***********************************GEC*****************************************
================================================================================
Author: Guilherme Cavalheiro.
Title: MVP1
Date: 
Rev.: 0.
================================================================================
************************************GEC****************************************/

#include "config.h"

void serialInit(void)
{
  P3SEL |= RX+TX;                            // P3.4,5 = USCI_A0 TXD/RXD
  UCA0CTL1 = UCSSEL_2;                      // SMCLK
#if (CLK == 1)
  UCA0BR0 = 104;                            // 9600 from 1Mhz
  UCA0BR1 = 0;
  UCA0MCTL = UCBRS_1;
#elif (CLK == 8)
  UCA0BR0 = 0x41;                           // 9600 from 8Mhz
  UCA0BR1 = 0x3;
  UCA0MCTL = UCBRS_2;
#elif (CLK == 12)
  UCA0BR0 = 0xE2;                           // 9600 from 12Mhz
  UCA0BR1 = 0x4;
  UCA0MCTL = 0;
#elif (CLK == 16)
  UCA0BR0 = 0x82;                           // 9600 from 16Mhz
  UCA0BR1 = 0x6;
  UCA0MCTL = UCBRS_6;
#else
#error "ERROR: Unsupported clock speed.  Custom clock speeds are possible. See comments in code."
#endif
  UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
  IE2 |= UCA0RXIE;                          // Enable USCI_A0 RX interrupt
}

void txSerial(char string[], unsigned int len)
{
  volatile unsigned int i;
  for(i = 0; i < len; i++)
  {
    while (!(IFG2 & UCA0TXIFG));            // USART1 TX buffer ready?
    UCA0TXBUF  = string[i];                 // Output character
  }
}       
