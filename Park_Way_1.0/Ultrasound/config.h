#include <msp430f2274.h>
#include <stdbool.h>
#include <string.h>
/*
#include "DEFINES/TI_CC_CC1100-CC2500.h"
#include "DEFINES/TI_CC_msp430.h"
#include "../SPI/TI_CC_spi.h"
#include "../RF/CC1100-CC2500.h"

*/
#define AP            1               // 1 - AP(serial), 0 - ED(no serial)
#define FIX_ADD       1

/******************************ENDEREÇAMENTO************************************
===============================================================================*/

#define ADD_AP        0x00            // AP sempre 0x00 para cada nó
#define ADD_ED        0x02            // ED linear {0x01...0xff}

/*==============================================================================
*******************************************************************************/

/*******************************CONSTANTES**************************************
===============================================================================*/

#define PACKET_LEN    0x0B            // Tamanho do buffer+1 
#define CLK           1               // 1, 8, 12, 16 MHz
#define Ttotal        500             // Ttotal*10ms com CCR0 = 5000 @ 1MHz/2
#define Tidle         50              // 5% do Ttotal - polling idle
#define Trx           450             // 95% do Ttotal - polling rx
#define Tsensor       500             // Tsensor*10ms com CCR0 = 5000 @ 1MHz/2
#define Tbat          3000      
#define OFFSET        0x04            // OFFSET para buildPacket
#define NUM_SENS      5               // Número de sensores na ntw

/*==============================================================================
*******************************************************************************/

#define MSG                      "\n\r\t\t\t\t-- MVP1 -- "
#define VER                      "0.0" 

/***********************************HW******************************************
===============================================================================*/
// MSP430F2274-EZ430 IO
/*********************************PORT1****************************************/

#define LED_VM        BIT0
#define LED_VD        BIT1
#define SW            BIT2
#define LEDS          (LED_VM+LED_VD)

//Nao podem ser utilizados
//#define P1          GND
//#define P2          VCC_EXT
/*********************************PORT2****************************************/
#define P3            BIT0
#define P4            BIT1
#define P5            BIT2
#define P6            BIT3
#define P7            BIT4
#define VBAT          P7
#define USENS         (P3+P4+P5+P6)
/*********************************PORT3****************************************/
#define TX            BIT4
#define RX            BIT5
/*********************************PORT4****************************************/
#define P8            BIT3
#define P9            BIT4        
#define P10           BIT5
#define P11           BIT6
#define GET_ADD       P8+P9+P10+P11

//Nao podem ser utilizados
//#define P12         GND
//#define P13         GDO0
//#define P14         GDO2
//#define P15         UCB0SOMI
//#define P16         UCD0CLK
//#define P17         UCB0STE
//#define P18         UCB0SMI0
/*==============================================================================
************************************HW*****************************************/


#if AP == 1
#pragma message("AP DEVICE")
#define MY_ADD  ADD_AP
#else 
#pragma message("ED DEVICE")
#define MY_ADD  ADD_ED
#endif

#if FIX_ADD == 1
#pragma message("Endereco fixo ativo!")
#else
#pragma message("Endereco fixo desativado!")
#endif







