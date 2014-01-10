/*******************************************************************
	Project: Park Way
	
	Ver.: 1.0
	Date: 10/01/14
	Developer:	Ânderson Ignácio da Silva
	Obs.: Observar ao criar um projeto do zero do simpliciti para 
	configurar o .dat no options quando selecionado o dispositivo,
	ao selecionar o ap o .dat adicionado muda e ao selecionar o ED
	também.	

*******************************************************************/

//Headers and libraries
#include <string.h>
#include "bsp.h"
#include "mrfi.h"
#include "bsp_leds.h"
#include "bsp_buttons.h"
#include "nwk_types.h"
#include "nwk_api.h"
#include "nwk_frame.h"
#include "nwk.h"
#include "virtual_com_cmds.h"

//Defines
#define EXTENED_API

//Prototypes
__interrupt void Timer_A (void);
static uint8_t sCB(linkID_t); 
void   init_t(void);
void   process_message(uint8_t i,uint8_t msg[MAX_APP_PAYLOAD]);

//Globals
static linkID_t sLID[NUM_CONNECTIONS] = {0};
static uint8_t  sNumCurrentPeers = 0;						

// Work Loop semaphores 
static volatile uint8_t sPeerFrameSem = 0;
static volatile uint8_t sJoinSem = 0;
static volatile uint8_t sTable = 0;
static volatile uint8_t sCheck = 0;

//Globals to serial
const char initial_message[] = {"\r\n-- MVP1 - ACCESS POINT --\r\n"};

/*------------------------------------------------------------------------------
 * Main
 *----------------------------------------------------------------------------*/
void main (void)
{
	bspIState_t intState;
	init_t();

	/* main work loop */
	while (1)
	{
		//Wait for a join connection
		if (sJoinSem && (sNumCurrentPeers < NUM_CONNECTIONS))
		{
			TXString("\n\rSolicitacao de conexao...",27);
			/* listen for a new connection */
			while (1)
			{
				if (SMPL_SUCCESS == SMPL_LinkListen(&sLID[sNumCurrentPeers]))
				{
					TXString("peer conectado com sucesso!",27);
					break;
				}
				/* Implement fail-to-link policy here. otherwise, listen again. */
			}  
			sNumCurrentPeers++;
			BSP_ENTER_CRITICAL_SECTION(intState);
			sJoinSem--;
			BSP_EXIT_CRITICAL_SECTION(intState);
		}

		//Read frames from buffer of received messages
		if (sPeerFrameSem)
		{
			uint8_t     msg[MAX_APP_PAYLOAD], len, i;
			
			BSP_TOGGLE_LED1();
			/* process all frames waiting */
			for (i=0; i<sNumCurrentPeers; ++i)
			{
				if (SMPL_SUCCESS == SMPL_Receive(sLID[i], msg, &len))
				{
					process_message(i,msg);
					BSP_ENTER_CRITICAL_SECTION(intState);
					sPeerFrameSem--;
					BSP_EXIT_CRITICAL_SECTION(intState);
				}
			}
		}
	}

}

void process_message(uint8_t i,uint8_t msg[MAX_APP_PAYLOAD])
{
	ioctlRadioSiginfo_t sigInfo;
	uint8_t rssiString[3];
	uint8_t output_serial[]="\n\rFrame:XXX-X RSSI:XXX LinkID:XXXXX Battery:X.XV";
	
	sigInfo.lid = sLID[i];	
	SMPL_Ioctl(IOCTL_OBJ_RADIO, IOCTL_ACT_RADIO_SIGINFO, (void *)&sigInfo);
	transmitData( i, sigInfo.sigInfo.rssi,0,&rssiString[0]);
	
	output_serial[8]=msg[0];
	output_serial[9]=msg[1];
	output_serial[10]=msg[2];
	output_serial[12]=msg[3];
	
	output_serial[19]='0';
	output_serial[20]=rssiString[0];
	output_serial[21]=rssiString[1];
	
	output_serial[30] = (i/10000) + 0x30;
	output_serial[31] = (i%10000)/1000 + 0x30;
	output_serial[32] = (((i%10000)%1000)/100) + 0x30;
	output_serial[33] = ((((i%10000)%1000)%100)/10) +0x30; 
	output_serial[34] = ((((i%10000)%1000)%100)%10) + 0x30;
	
	output_serial[44]='0'+(msg[4]/10)%10;
	output_serial[46]='0'+(msg[4]%10);
	
	TXString(output_serial,sizeof output_serial);					
}

/* Runs in ISR context. Reading the frame should be done in the
   application thread not in the ISR thread. */
static uint8_t sCB(linkID_t lid)
{
  if (lid)
  {
    sPeerFrameSem++;
  }
  else
  {
    sJoinSem++;
  }

  /* leave frame to be read by application. */
  return 0;
}

void init_t()
{
	/* Initialize board */
	BSP_Init();
	BSP_TURN_OFF_LED1();
	BSP_TURN_OFF_LED2();

	/* Initialize TimerA and oscillator */
	BCSCTL3 |= LFXT1S_2;                      // LFXT1 = VLO
	TACCTL0 = CCIE;                           // TACCR0 interrupt enabled
	TACCR0 = 12000;                           // ~1 second in 8MHz
	TACTL = TASSEL_1 + MC_1;                  // ACLK, upmode
  
  
	/* Initialize serial port */
	COM_Init();

	//Transmit initial_message screen and network init notification
	TXString( (char*)initial_message, sizeof initial_message);
	char ini[]={"Inicizalizando a rede..."};
	TXString((char *)ini,sizeof ini);

	// Initialize the SimpliciTI protocol stack  - sCB is the callback handler
	SMPL_Init(sCB);

	//Configure the RX for max power
	SMPL_Ioctl(IOCTL_OBJ_RADIO, IOCTL_ACT_RADIO_SETPWR,(void *) IOCTL_LEVEL_2);
  
	// Network initialized
	TXString( "Pronto!\r\n",9);

	BSP_TURN_OFF_LED1();
	BSP_TURN_OFF_LED2();
}

/*------------------------------------------------------------------------------
* Timer A0 interrupt service routine
------------------------------------------------------------------------------*/
#pragma vector=TIMERA0_VECTOR
__interrupt void Timer_A (void)
{
    //sCheck++;
	//sTable = 1;
}
