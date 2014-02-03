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
#include "stdbool.h"

//Defines
#define AP_GROUP		   'A'
#define AP_GROUP_1		   "A"
#define EXTENED_API
#define TIMEOUT_CHECK_PEERS 5
#define TIMEOUT_JOIN 		2

//Prototypes
__interrupt void Timer_A (void);
static uint8_t sCB(linkID_t); 
void   init_t(void);
void   process_message(uint8_t i,uint8_t msg[MAX_APP_PAYLOAD]);
void   con_t(void);

//Globals
static linkID_t sLID[NUM_CONNECTIONS] = {0};
static linkID_t LD_AP[2]={0};
static uint8_t  sNumCurrentPeers = 0;
static uint8_t	AP_send_inf,AP_receive_inf,AP=0,vector_send,vector_receive;	
bspIState_t intState;				

typedef struct peerFrame
{
	char frame[6];
	bool active; 
}peerFrame_t;

typedef enum 
{
	con_1,
	con_2,
	con_3,
	con_4,
	ready,
	error
}fsm_states;

typedef enum 
{
	link_1,
	frame_1,
	link_2,
	frame_2,
	ready_con,
	error_con
}con_states;

con_states	fsm_con=link_1;
fsm_states	fsm_ap=con_1;		//Just for debug initial

volatile peerFrame_t	peer_list[NUM_CONNECTIONS];
   

// Work Loop semaphores 
static volatile uint8_t sPeerFrameSem = 0;
static volatile uint8_t sJoinSem = 0;
static volatile uint8_t sTable = 0;
static volatile uint8_t sCheck = 0;
static volatile uint8_t sTimeout = 0;
static volatile uint8_t sApRequest = 0;

//Globals to serial
const char initial_message[] = {"\n\r-- PARK WAY - ACCESS POINT = "};
const char initial_ED[] = {"\n\r Entrando no modo de conexao com as Vagas \n\r"};

//Main
void main (void)
{
	init_t();
	
	
	TXString((char *)initial_ED, sizeof initial_ED);
	
	/* main work loop */
	while (1)
	{
		//Wait for a join connection
		if (sJoinSem && (sNumCurrentPeers < NUM_CONNECTIONS))
		{
			sTimeout = 0;
			TXString("\n\rSolicitacao de conexao   ",27);
			/* listen for a new connection */
			while (sTimeout < TIMEOUT_JOIN)
			{
				if (SMPL_SUCCESS == SMPL_LinkListen(&sLID[sNumCurrentPeers]))
				{
					TXString("conectado com sucesso!",22);
					sNumCurrentPeers++;
					break;
				}
				else
				{	
					TXString(".",1);
					sTimeout++;
				}
				/* Implement fail-to-link policy here. otherwise, listen again. */
			}  
			if(sTimeout>TIMEOUT_JOIN)	TXString("Timeout!",8);
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
					SMPL_Send(sLID[i],".",1);
					BSP_ENTER_CRITICAL_SECTION(intState);
					sPeerFrameSem--;
					BSP_EXIT_CRITICAL_SECTION(intState);
				}
			}
		}
		
		//Check if the peer is online or not
		if(sCheck>=TIMEOUT_CHECK_PEERS)
		{
			sCheck = 0;
			
			TXString("\n\r",2);
			TXString("*AP1",3);
			TXString("\n\r",2);
			int i;
			for(i=0;i<sNumCurrentPeers;i++)
			{
				if(peer_list[i].active)
				{
					TXString((char *)peer_list[i].frame,sizeof peer_list[i].frame);
					TXString("\n\r",2);
				}
			}
			TXString("*AP1",3);
			
			int j,k;
			for(j=0;j<sNumCurrentPeers;j++)
			{
				if(!peer_list[j].active)
				{
					for(k=0;k<6;k++)	peer_list[j].frame[k]='0';
				}
				peer_list[j].active=0;
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
	
	int j;
	for(j=0;j<4;j++)	peer_list[i].frame[j]=msg[j];
	peer_list[i].active=1;
	
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
	
	peer_list[i].frame[4]=output_serial[44];
	peer_list[i].frame[5]=output_serial[46];
	
	//TXString(output_serial,sizeof output_serial);					
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
	lAddr.addr[1] = 0x56+(int)AP_GROUP;
	lAddr.addr[2] = 0x34+(int)AP_GROUP;
	lAddr.addr[3] = 0x12;

	/* Tell network stack the device address */
	SMPL_Ioctl(IOCTL_OBJ_ADDR, IOCTL_ACT_SET, &lAddr);	
	

	BSP_TURN_OFF_LED1();
	BSP_TURN_OFF_LED2();

	/* Initialize TimerA and oscillator */
	BCSCTL3 |= LFXT1S_2;                      // LFXT1 = VLO
	TACCTL0 = CCIE;                           // TACCR0 interrupt enabled
	TACCR0 = 1200;                           // ~1 second in 8MHz
	TACTL = TASSEL_1 + MC_1;                  // ACLK, upmode
  
  
	/* Initialize serial port */
	COM_Init();

	//Transmit initial_message screen and network init notification
	TXString( (char*)initial_message, sizeof initial_message);
	TXString(AP_GROUP_1,1);
	TXString(" -- \n\r",6);
	
	char ini[]={"Inicizalizando o SIMPLICITI..."};
	TXString((char *)ini,sizeof ini);

	// Initialize the SimpliciTI protocol stack  - sCB is the callback handler
	SMPL_Init(sCB);

	//Configure the RX for max power
	SMPL_Ioctl(IOCTL_OBJ_RADIO, IOCTL_ACT_RADIO_SETPWR,(void *) IOCTL_LEVEL_2);
  
	// Network initialized
	TXString( "Pronto!\r\n",9);

	BSP_TURN_OFF_LED1();
	BSP_TURN_OFF_LED2();
	
	//Initializes the struct of peers
	int i,j;
	for(i=0;i<NUM_CONNECTIONS;i++)
	{
		for(j=0;j<5;j++)	peer_list[i].frame[j]='0';
		peer_list[i].active=0;
	}	
}

/*------------------------------------------------------------------------------
* Timer A0 interrupt service routine
------------------------------------------------------------------------------*/
#pragma vector=TIMERA0_VECTOR
__interrupt void Timer_A (void)
{
    sCheck++;
	sTable++;
}
