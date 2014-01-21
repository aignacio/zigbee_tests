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
#define AP_GROUP 'A'
#define EXTENED_API
#define TIMEOUT_CHECK_PEERS 3
#define TIMEOUT_JOIN 2

//Prototypes
__interrupt void Timer_A (void);
static uint8_t sCB(linkID_t); 
void   init_t(void);
void   process_message(uint8_t i,uint8_t msg[MAX_APP_PAYLOAD]);
void   con_t(void);

//Globals
static linkID_t sLID[50] = {0};
static uint8_t  sNumCurrentPeers = 0;
static uint8_t	AP_send,AP_send_inf='0';
static uint8_t	AP_receive,AP_receive_inf='0';	
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

fsm_states	fsm_ap=con_1;
 char table[3];
volatile peerFrame_t	peer_list[NUM_CONNECTIONS];

// Work Loop semaphores 
static volatile uint8_t sPeerFrameSem = 0;
static volatile uint8_t sJoinSem = 0;
static volatile uint8_t sTable = 0;
static volatile uint8_t sCheck = 0;
static volatile uint8_t sTimeout = 0;
static volatile uint8_t sSendRequest = 0;

//Globals to serial
const char initial_message[] = {"\n\r-- Network Manager -- \n\r"};
const char initial_ED[] = {"\n\r Enviando broadcast \n\r"};


//Main
void main (void)
{
	init_t();
	//con_t();
	
	TXString((char *)initial_ED, sizeof initial_ED);
	
	/* main work loop */
	while (1)
	{
		table[0]='A';
		table[1]='B';
		table[2]='C';
		SMPL_Send(SMPL_LINKID_USER_UUD,table,sizeof table);
		table[0]='B';
		table[1]='A';
		table[2]='C';
		SMPL_Send(SMPL_LINKID_USER_UUD,table,sizeof table);
		table[0]='C';
		table[1]='B';
		table[2]='A';
		SMPL_Send(SMPL_LINKID_USER_UUD,table,sizeof table);
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
	
	TXString(output_serial,sizeof output_serial);					
}

/* Runs in ISR context. Reading the frame should be done in the
   application thread not in the ISR thread. */
static uint8_t sCB(linkID_t lid)
{
  if (lid)
  {
	if(lid==sLID[AP_receive])
		sSendRequest++;
	else if(lid!=SMPL_LINKID_USER_UUD)
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
	TXString((char *)AP_GROUP,1);
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

void con_t()
{
	/*
		Frame boradcast by the NM-Network Manager
		20 Frames - Each frame has the information of connection of each AP(Access Point)
		and the AP needs to know your frame and how he must connect on your FSM. While the
		
		Default frame inf.:
		buffer[0]-	Letter of the AP
		buffer[1]-	Letter of the send AP
		buffer[2]-  Letter of the receive AP
	*/
	
	uint8_t buffer[MAX_APP_PAYLOAD],len;
	
	//Connecting to manager
	while(fsm_ap!=ready)
		switch(fsm_ap)
		{
			case con_1:
				TXString("\n\rAguardando o Broadcast do NM",30);
				while(buffer[0]!=AP_GROUP)
					while(SMPL_SUCCESS!=SMPL_Receive(SMPL_LINKID_USER_UUD,buffer,&len))	
						TXString(".",1);
				AP_send_inf = buffer[1];
				AP_receive_inf = buffer[2];
				TXString("Recebido!",9);
				fsm_ap = con_2;
			break;
			case con_2:
				TXString("\n\rAguardando conexao com os APs",31);
				bool flag_out=0,flag_AP1=0,flag_AP2=0;
				uint8_t timeout_r=0;
				while(!flag_out)
				{
					if(sJoinSem)
					{
						timeout_r = 0;
						while (timeout_r < 2)
						{
							if (SMPL_SUCCESS == SMPL_LinkListen(&sLID[sNumCurrentPeers]))
							{
								sNumCurrentPeers++;
								break;
							}
							else
							{	
								TXString(".",1);
								timeout_r++;
							}
							/* Implement fail-to-link policy here. otherwise, listen again. */
						}  
						BSP_ENTER_CRITICAL_SECTION(intState);
						sJoinSem--;
						BSP_EXIT_CRITICAL_SECTION(intState);	
					}
					if(sPeerFrameSem)
					{
						uint8_t i;
			
						BSP_TOGGLE_LED1();
						/* process all frames waiting */
						for (i=0; i<sNumCurrentPeers; ++i)
						{
							if (SMPL_SUCCESS == SMPL_Receive(sLID[i], buffer, &len))
							{
								if(buffer[0]==AP_send_inf)
								{
									flag_AP1 = 1;
									AP_send = i;
								}
								else 
									if(buffer[0]==AP_receive_inf)	
									{
										flag_AP2 = 1;
										AP_receive = i;
									}
									else	SMPL_Unlink(sLID[i]);
								BSP_ENTER_CRITICAL_SECTION(intState);
								sPeerFrameSem--;
								BSP_EXIT_CRITICAL_SECTION(intState);
							}
						}		
					}
					if(sTable==3)
					{
						uint8_t d;
						for(d=0; d<sNumCurrentPeers; ++d)	SMPL_Send(sLID[d],(char *)AP_GROUP,1);
						sTable=0;
					}
					if(flag_AP1&&flag_AP2)	flag_out = 1;
				}
				TXString("conectado!",10);
				fsm_ap = con_3;
			break;
			case con_3:
				fsm_ap = ready;
			break;
			default:
				fsm_ap = con_1;
			break;
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
