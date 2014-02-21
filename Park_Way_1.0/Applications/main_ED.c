/*****************************************************************************
	Project: Park Way
	
	Ver.: 1.0
	Date: 10/01/14
	Developer:	Ânderson Ignácio da Silva
	Obs.: Observar ao criar um projeto do zero do simpliciti para 
	configurar o .dat no options quando selecionado o dispositivo,
	ao selecionar o ap o .dat adicionado muda e ao selecionar o ED
	também.	

*****************************************************************************/

//Headers and libraries
#include "bsp.h"
#include "mrfi.h"
#include <string.h>
#include "nwk_types.h"
#include "nwk_api.h"
#include "bsp_leds.h"
#include "bsp_buttons.h"
#include "vlo_rand.h"
#include "u_sensor.h"
#include "nwk.h"
#include "virtual_com_cmds.h"

//Defines
#define MISSES  5
#define EXTENED_API
#define LED     BIT5
#define SETPOINT_RESET 15
//Peer TAG
#define PEER_1	'E'
#define PEER_2	'D'
#define PEER_3	'2'
//Must be in centimeters
#define SETPOINT_ULTRA  150 

//Prototypes
static void linkTo(void);
static uint8_t sCB(linkID_t);
uint8_t read_bat(void);
void 	init_t(void);
uint8_t read_ultra(void);

__interrupt void		ADC10_ISR(void);
__interrupt void		Timer_A (void);

//Globals
static linkID_t sLinkID1 = 0;
char * Flash_Addr = (char *)0x10F0;
volatile int * tempOffset = (int *)0x10F4;
static volatile uint8_t sSelfMeasureSem = 0;

static volatile uint8_t        sAPstatus=0;

//Main
void main (void)
{
	init_t();
	int misses;
	/* Keep trying to join (a side effect of successful initialization) until
	* successful. Toggle LEDS to indicate that joining has not occurred.
	*/
	//Configure the RX for max power
	SMPL_Ioctl(IOCTL_OBJ_RADIO, IOCTL_ACT_RADIO_SETPWR,(void *) IOCTL_LEVEL_2);
	while (SMPL_SUCCESS != SMPL_Init(sCB))
	{
		BSP_TOGGLE_LED1();
		BSP_TOGGLE_LED2();
		if(misses>MISSES)
		{
			WDTCTL &= ~WDTHOLD;  
			WDTCTL &= ~WDTNMI; 
			WDTCTL &= ~WDTTMSEL;
			IE1 |= WDTIE;     
		}
		misses++;
		__bis_SR_register(LPM3_bits+GIE);  // LPM3 with interrupts enabled
	}

	linkTo();
	while(1);
}

static void linkTo()
{
	uint8_t msg[5];
	int timeout=0;
	smplStatus_t test;

	msg[0]= PEER_1;
	msg[1]= PEER_2;
	msg[2]= PEER_3;
	
	/* Keep trying to link... */
	while (SMPL_SUCCESS != SMPL_Link(&sLinkID1))
	{
		BSP_TOGGLE_LED1();
		BSP_TOGGLE_LED2();
		/* Go to sleep (LPM3 with interrupts enabled)
		 * Timer A0 interrupt will wake CPU up every second to retry linking
		 */
		__bis_SR_register(LPM3_bits+GIE);
	}

	/* Turn off LEDs. */
	BSP_TURN_OFF_LED1();
	BSP_TURN_OFF_LED2();

	/* Put the radio to sleep */
	SMPL_Ioctl(IOCTL_OBJ_RADIO, IOCTL_ACT_RADIO_SLEEP, 0);

	while (1)
	{
		/* Go to sleep, waiting for interrupt every second to acquire data */
		__bis_SR_register(LPM3_bits);																																																																	
		if (sSelfMeasureSem) 
		{
			msg[3]=read_ultra();
			msg[4]=read_bat();
			
			SMPL_Ioctl( IOCTL_OBJ_RADIO, IOCTL_ACT_RADIO_AWAKE, 0);
			SMPL_Send(sLinkID1,  msg, sizeof msg);
			test=SMPL_Ping(sLinkID1);
			
			if(sAPstatus==SETPOINT_RESET)
			{
				WDTCTL &= ~WDTHOLD;  
				WDTCTL &= ~WDTNMI; 
				WDTCTL &= ~WDTTMSEL;
				IE1 |= WDTIE;  
			}
			else
			{
				sAPstatus++;
			}
			// switch(test)
			// {
				// case SMPL_SUCCESS:
					// TXString("\n\rSMPL_SUCCESS",14);
				// break;
				// case SMPL_BAD_PARAM:
					// TXString("\n\rSMPL_BAD_PARAM",16);
				// break;
				// case SMPL_NOMEM:
					// TXString("\n\rSMPL_NOMEM",12);
				// break;
				// case SMPL_TX_CCA_FAIL:
					// TXString("\n\rSMPL_TX_CCA_FAIL",18);
				// break;
				// default:
					// TXString("\n\rSMPL_DEFAULT",14);
				// break;
			// }
			
			// if(test!=SMPL_SUCCESS)
			// {
				// TXString("\n\rSMPL_DEFAULT",14);
				// // test=0;
				// // if(timeout>MISSES)
				// // {
					// WDTCTL &= ~WDTHOLD;  
					// WDTCTL &= ~WDTNMI; 
					// WDTCTL &= ~WDTTMSEL;
					// IE1 |= WDTIE;   
				// // }
				// // else
				// // {
                    // // BSP_TOGGLE_LED2();
					// // timeout++;
				// // }
			// }
			// // else
			// // {
				// // test=0;
				// // timeout = 0;
			// // }
			// test=0;
		}
		SMPL_Ioctl( IOCTL_OBJ_RADIO, IOCTL_ACT_RADIO_SLEEP, 0);
		sSelfMeasureSem = 0;
    }
}

static uint8_t sCB(linkID_t lid)
{
	if(lid==sLinkID1)
	{
		sAPstatus = 0;
	}
	/* leave frame to be read by application. */
	return 0;
}

uint8_t read_ultra()
{
	
	char aux[5];
	int cent;
	
	cent = getEcho();
	aux[0] = (cent/10000) + 0x30;
	aux[1] = (cent%10000)/1000 + 0x30;
	aux[2] = (((cent%10000)%1000)/100) + 0x30;
	aux[3] = ((((cent%10000)%1000)%100)/10) +0x30; 
	aux[4] = ((((cent%10000)%1000)%100)%10) + 0x30;
	
	TXString("\n\rUltrasom:",11);
	TXString((char *)aux,sizeof aux);

	if(cent <= SETPOINT_ULTRA )
	{
		P4OUT |= LED;
		return '1'; 
	}  
	else
	{
		P4OUT &= ~LED;
		return '0'; 
	}
	return 0;
}

uint8_t read_bat()
{
	volatile long temp;
	int degC, volt;
	int results[2];
	char msg[3];

	/* Get temperature */
	ADC10CTL1 = INCH_10 + ADC10DIV_4;       // Temp Sensor ADC10CLK/5
	ADC10CTL0 = SREF_1 + ADC10SHT_3 + REFON + ADC10ON + ADC10IE + ADC10SR;
	/* Allow ref voltage to settle for at least 30us (30us * 8MHz = 240 cycles)
	* See SLAS504D for settling time spec
	*/
	__delay_cycles(240);
	ADC10CTL0 |= ENC + ADC10SC;             // Sampling and conversion start
	__bis_SR_register(CPUOFF + GIE);        // LPM0 with interrupts enabled
	results[0] = ADC10MEM;                  // Retrieve result
	ADC10CTL0 &= ~ENC;

	/* Get voltage */
	ADC10CTL1 = INCH_11;                     // AVcc/2
	ADC10CTL0 = SREF_1 + ADC10SHT_2 + REFON + ADC10ON + ADC10IE + REF2_5V;
	__delay_cycles(240);
	ADC10CTL0 |= ENC + ADC10SC;             // Sampling and conversion start
	__bis_SR_register(CPUOFF + GIE);        // LPM0 with interrupts enabled
	results[1] = ADC10MEM;                  // Retrieve result

	/* Stop and turn off ADC */
	ADC10CTL0 &= ~ENC;
	ADC10CTL0 &= ~(REFON + ADC10ON);

	/* oC = ((A10/1024)*1500mV)-986mV)*1/3.55mV = A10*423/1024 - 278
	* the temperature is transmitted as an integer where 32.1 = 321
	* hence 4230 instead of 423
	*/
	temp = results[0];
	degC = ((temp - 673) * 4230) / 1024;
	if( (*tempOffset) != 0xFFFF )
	{
		degC += (*tempOffset);
	}

	temp = results[1];
	volt = (temp*25)/512;
	msg[0] = degC&0xFF;
	msg[1] = (degC>>8)&0xFF;
	msg[2] = volt;

	return msg[2];
}

void init_t() 
{
	addr_t lAddr;

	/* Initialize board-specific hardware */
	BSP_Init();

	// /* Check flash for previously stored address */
	// if(Flash_Addr[0] == 0xFF && Flash_Addr[1] == 0xFF &&
	 // Flash_Addr[2] == 0xFF && Flash_Addr[3] == 0xFF )
	// {
	// createRandomAddress(); // Create and store a new random address
	// }

	/* Read out address from flash */
	lAddr.addr[0] = 0x7A;
	lAddr.addr[1] = 0x56+(int)PEER_3;
	lAddr.addr[2] = 0x34+(int)PEER_3;
	lAddr.addr[3] = 0x12;

	/* Tell network stack the device address */
	SMPL_Ioctl(IOCTL_OBJ_ADDR, IOCTL_ACT_SET, &lAddr);
  
	//Sets the led to output
	P4DIR |= LED;
	P4OUT |= LED;

	/* Initialize TimerA and oscillator */
	BCSCTL3 |= LFXT1S_2;                      // LFXT1 = VLO
	TACCTL0 = CCIE;                           // TACCR0 interrupt enabled
	TACCR0 = 12000;                           // ~ 1 sec
	TACTL = TASSEL_1 + MC_1;                  // ACLK, upmode

	//Starts the ultrasound sensor
	start_usens();
	
	//Initialize the COM-Serial
	COM_Init();

	TXString("\n\r--------Peer Device:--------",30);
	char testss[]="\n\rInicializando...";
	TXString((char *)testss,sizeof testss);
	//while(!BSP_BUTTON1());

	BSP_TURN_OFF_LED1();
	BSP_TURN_OFF_LED2();

}

/*------------------------------------------------------------------------------
 * ADC10 interrupt service routine
 *----------------------------------------------------------------------------*/
#pragma vector=ADC10_VECTOR
__interrupt void ADC10_ISR(void)
{
  __bic_SR_register_on_exit(CPUOFF);        // Clear CPUOFF bit from 0(SR)
}

/*------------------------------------------------------------------------------
 * Timer A0 interrupt service routine
 *----------------------------------------------------------------------------*/
#pragma vector=TIMERA0_VECTOR
__interrupt void Timer_A (void)
{
  sSelfMeasureSem = 1;
  __bic_SR_register_on_exit(LPM3_bits);        // Clear LPM3 bit from 0(SR)
}
