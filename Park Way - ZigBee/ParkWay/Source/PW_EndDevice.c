/*****************************************************************************
  Project: Park Way (ZigBee)
  End Device Firmware

  Ver.: 1.0
  Date: 27/06/14
  Developer:  Ânderson Ignácio da Silva
  Obs.:
    -LCD_SUPPORTED=DEBUG or LCD_SUPPORTED must be disable in the CC2530 China 
    modules to work
    
    -BEFORE PROGRAMMING is important to ERASE the FLASH memory with the FLASH
    programmer because the NV_RESTORE option is enabled  to restore the  End 
    Devices after the coordinator power down

  Pre-processor directives:
    NWK_AUTO_POLL
    ZTOOL_P1
    MT_TASK
    MT_SYS_FUNC
    MT_ZDO_FUNC
    xLCD_SUPPORTED=DEBUG
    POWER_SAVING

*****************************************************************************/

/*********************************************************************
 * INCLUDES
 */
#include "OSAL.h"
#include "AF.h"
#include "ZDApp.h"
#include "ZDObject.h"
#include "ZDProfile.h"
#include "ParkWay.h"
#include "DebugTrace.h"
#include "OnBoard.h"
/* HAL */
#include "hal_lcd.h"
#include "hal_led.h"
#include "hal_key.h"
#include "hal_uart.h"

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */
#define   DEVICE_PW_1                  '0'     //(first char)Tag of the device 
#define   DEVICE_PW_2                  '1'     //(second char)Tag of the device
#define   ParkWay_SEND_MSG_TIMEOUT    1500     //Time to send data to the coordinator(poll ultrasound)
#define   TIME_TO_CONNECT_COORD       1000     //Time to connect (auto bind) the device to the coord.
/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */
// This list should be filled with Application specific Cluster IDs.
const cId_t ParkWay_ClusterList[ParkWay_MAX_CLUSTERS] =
{
  ParkWay_CLUSTERID
};

const SimpleDescriptionFormat_t ParkWay_SimpleDesc =
{
  ParkWay_ENDPOINT,              //  int Endpoint;
  ParkWay_PROFID,                //  uint16 AppProfId[2];
  ParkWay_DEVICEID,              //  uint16 AppDeviceId[2];
  ParkWay_DEVICE_VERSION,        //  int   AppDevVer:4;
  ParkWay_FLAGS,                 //  int   AppFlags:4;
  ParkWay_MAX_CLUSTERS,          //  byte  AppNumInClusters;
  (cId_t *)ParkWay_ClusterList,  //  byte *pAppInClusterList;
  ParkWay_MAX_CLUSTERS,          //  byte  AppNumInClusters;
  (cId_t *)ParkWay_ClusterList   //  byte *pAppInClusterList;
};

// This is the Endpoint/Interface description.  It is defined here, but
// filled-in in ParkWay_Init().  Another way to go would be to fill
// in the structure here and make it a "const" (in code space).  The
// way it's defined in this sample app it is define in RAM.
endPointDesc_t ParkWay_epDesc;

/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */
byte ParkWay_TaskID;   // Task ID for internal task/event processing
                          // This variable will be received when
                          // ParkWay_Init() is called.
devStates_t ParkWay_NwkState;


byte ParkWay_TransID;  // This is the unique message ID (counter)

afAddrType_t ParkWay_DstAddr;

/*********************************************************************
 * LOCAL FUNCTIONS
 */
void SendUART(uint8 *data,uint8 len);
void uart_init(void);
static void ParkWay_ProcessZDOMsgs( zdoIncomingMsg_t *inMsg );
static void ParkWay_HandleKeys( byte shift, byte keys );
static void ParkWay_MessageMSGCB( afIncomingMSGPacket_t *pckt );
static void ParkWay_SendTheMessage( void );

/*********************************************************************
 * NETWORK LAYER CALLBACKS
 */

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

//Initialization function for the Park Way task. This is called during initialization and should contain any application specific initialization (ie. hardware initialization/setup, table initialization, power up notificaiton ... )
void ParkWay_Init( uint8 task_id )
{
  ParkWay_TaskID = task_id;
  ParkWay_NwkState = DEV_INIT;
  ParkWay_TransID = 0;

  // Device hardware initialization can be added here or in main() (Zmain.c).
  // If the hardware is application specific - add it here.
  // If the hardware is other parts of the device add it in main().

  ParkWay_DstAddr.addrMode = (afAddrMode_t)AddrNotPresent;
  ParkWay_DstAddr.endPoint = 0;
  ParkWay_DstAddr.addr.shortAddr = 0;

  // Fill out the endpoint description.
  ParkWay_epDesc.endPoint = ParkWay_ENDPOINT;
  ParkWay_epDesc.task_id = &ParkWay_TaskID;
  ParkWay_epDesc.simpleDesc
            = (SimpleDescriptionFormat_t *)&ParkWay_SimpleDesc;
  ParkWay_epDesc.latencyReq = noLatencyReqs;

  // Register the endpoint description with the AF
  afRegister( &ParkWay_epDesc );

  // Register for all key events - This app will handle all key events
  RegisterForKeys( ParkWay_TaskID );

  // Update the display
#if defined ( LCD_SUPPORTED )
  HalLcdWriteString( "ParkWay-Peer", HAL_LCD_LINE_1 );
#endif

  ZDO_RegisterForZDOMsg( ParkWay_TaskID, End_Device_Bind_rsp );
  ZDO_RegisterForZDOMsg( ParkWay_TaskID, Match_Desc_rsp );

  osal_start_timerEx( ParkWay_TaskID,
                      POLLING_OF_DEV_EVT,
                      TIME_TO_CONNECT_COORD);
}

//Main function to proccess the timers and events 
uint16 ParkWay_ProcessEvent( uint8 task_id, uint16 events )
{
  afIncomingMSGPacket_t *MSGpkt;
  afDataConfirm_t *afDataConfirm;
  zAddrType_t dstAddr;

  // Data Confirmation message fields
  byte sentEP;
  ZStatus_t sentStatus;
  byte sentTransID;       // This should match the value sent
  (void)task_id;  // Intentionally unreferenced parameter

  if ( events & SYS_EVENT_MSG )
  {
    MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive( ParkWay_TaskID );
    while ( MSGpkt )
    {
      switch ( MSGpkt->hdr.event )
      {
        case ZDO_CB_MSG: //Event trigger by ZigBee Device Object like connect, bind...etc
          ParkWay_ProcessZDOMsgs( (zdoIncomingMsg_t *)MSGpkt );
          break;

        case AF_DATA_CONFIRM_CMD: //Event trigger by a confirmation of sent message
          // This message is received as a confirmation of a data packet sent.
          // The status is of ZStatus_t type [defined in ZComDef.h]
          // The message fields are defined in AF.h
          // afDataConfirm = (afDataConfirm_t *)MSGpkt;
          // sentEP = afDataConfirm->endpoint;
          // sentStatus = afDataConfirm->hdr.status;
          // sentTransID = afDataConfirm->transID;
          // (void)sentEP;
          // (void)sentTransID;

          // // Action taken when confirmation is received.
          // if ( sentStatus != ZSuccess )
          // {
          //   // The data wasn't delivered -- Do something
          // }
          break;

        case AF_INCOMING_MSG_CMD: //Event trigger by a receive message
          ParkWay_MessageMSGCB( MSGpkt );
          break;

        case ZDO_STATE_CHANGE: //Event trigger by a connection to the coordinator or Router
          ParkWay_NwkState = (devStates_t)(MSGpkt->hdr.status);
          if ( (ParkWay_NwkState == DEV_ZB_COORD)
              || (ParkWay_NwkState == DEV_ROUTER)
              || (ParkWay_NwkState == DEV_END_DEVICE) )
          {
            // Start sending "the" message in a regular interval.
            osal_start_timerEx( ParkWay_TaskID,
                                ParkWay_SEND_MSG_EVT,
                                ParkWay_SEND_MSG_TIMEOUT );
          }
          break;

        default:
          break;
      }

      // Release the memory
      osal_msg_deallocate( (uint8 *)MSGpkt );

      // Next
      MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive( ParkWay_TaskID );
    }

    // return unprocessed events
    return (events ^ SYS_EVENT_MSG);
  }

  // Send to the coordinator the status of ultrasound
  if ( events & ParkWay_SEND_MSG_EVT )
  {
    // Send "the" message
    ParkWay_SendTheMessage();

    // Setup to send message again
    osal_start_timerEx( ParkWay_TaskID,
                        ParkWay_SEND_MSG_EVT,
                        ParkWay_SEND_MSG_TIMEOUT );

    // return unprocessed events
    return (events ^ ParkWay_SEND_MSG_EVT);
  }
  
  if(events & POLLING_OF_DEV_EVT) //This force the auto bind to the coordinator or Router
  {
    //HalLedSet ( HAL_LED_1, HAL_LED_MODE_ON);
    // Initiate a Match Description Request (Service Discovery)
    dstAddr.addrMode = AddrBroadcast;
    dstAddr.addr.shortAddr = NWK_BROADCAST_SHORTADDR;
    ZDP_MatchDescReq( &dstAddr, NWK_BROADCAST_SHORTADDR,
                      ParkWay_PROFID,
                      ParkWay_MAX_CLUSTERS, (cId_t *)ParkWay_ClusterList,
                      ParkWay_MAX_CLUSTERS, (cId_t *)ParkWay_ClusterList,
                      FALSE );
    osal_start_timerEx( ParkWay_TaskID,
                        POLLING_OF_DEV_EVT,
                        TIME_TO_CONNECT_COORD);
  } 


  HalLedSet( HAL_LED_ALL, HAL_LED_MODE_OFF ); //Shutdown all LEDS to reduce consumption of power
  
  // Discard unknown events
  return 0;
}

/*********************************************************************
 * Event Generation Functions
 */

/*********************************************************************
 */
//This function process the ZDO events to bind
static void ParkWay_ProcessZDOMsgs( zdoIncomingMsg_t *inMsg )
{
  switch ( inMsg->clusterID )
  {
    case End_Device_Bind_rsp:
      if ( ZDO_ParseBindRsp( inMsg ) == ZSuccess )
      {
        // Light LED
        //HalLedSet ( HAL_LED_2, HAL_LED_MODE_FLASH );
      }
      break;

    case Match_Desc_rsp:
      {
        ZDO_ActiveEndpointRsp_t *pRsp = ZDO_ParseEPListRsp( inMsg );
        if ( pRsp )
        {
          if ( pRsp->status == ZSuccess && pRsp->cnt )
          {
            ParkWay_DstAddr.addrMode = (afAddrMode_t)Addr16Bit;
            ParkWay_DstAddr.addr.shortAddr = pRsp->nwkAddr;
            // Take the first endpoint, Can be changed to search through endpoints
            ParkWay_DstAddr.endPoint = pRsp->epList[0];

            // Light LED
            //HalLedSet( HAL_LED_2, HAL_LED_MODE_ON );

            //This stop the peer from send request do the coordinator
            //HalLedSet ( HAL_LED_1, HAL_LED_MODE_OFF);
            osal_stop_timerEx( ParkWay_TaskID,POLLING_OF_DEV_EVT);
          }
          osal_mem_free( pRsp );
        }
      }
      break;
  }
}

/*********************************************************************
 * LOCAL FUNCTIONS
 */
//This function process the frames received by the application framework (source routers or the coordinator)

static void ParkWay_MessageMSGCB( afIncomingMSGPacket_t *pkt_1 )
{
  /************************************************* This was used to send some specific data do the last device connected to the COORDINATOR (HDA)**************************************/
  // uint8* pkt;
  // pkt=(pkt_1->cmd.Data);
  
  // uint8 pin_ultra=4;
  
  // P0SEL &= (~BV(2));
  // P0DIR |= (BV(2));
    
  // if(*pkt==48||*(pkt+1)==48||*(pkt+2)==48)
  // {
  //  P0&=~pin_ultra;
  // }
  // else
  // {
  //   P0|=pin_ultra;
  // }
  /************************************************* This was used to send some specific data do the last device connected to the COORDINATOR (HDA)**************************************/
}

//This function send the status of own park to the coordinator
static void ParkWay_SendTheMessage( void )
{
  uint8 pData[6], pin_ultra=4,vaga;
  P0SEL &= (~BV(2));
  P0DIR &= (~BV(2));
  vaga = (pin_ultra & P0);
  if(vaga)
  {
     vaga='1';
  }
  else
  {
     vaga='0';
  }

  pData[0] = 'E';
  pData[1] = 'D';
  pData[2] = DEVICE_PW_1;
  pData[3] = DEVICE_PW_2;
  pData[4] = '-';
  pData[5] = vaga;

  //Send data to the coordinator ADDRESS(0000)
  if ( AF_DataRequest( &ParkWay_DstAddr, &ParkWay_epDesc,
                       ParkWay_CLUSTERID,
                       (byte)osal_strlen( pData ) + 1,
                       (byte *)&pData,
                       &ParkWay_TransID,
                       AF_DISCV_ROUTE, AF_DEFAULT_RADIUS ) == afStatus_SUCCESS )
  {
    // Successfully requested to be sent.
  }
  else
  {
    osal_stop_timerEx( ParkWay_TaskID,
                        ParkWay_SEND_MSG_EVT);
    osal_start_timerEx( ParkWay_TaskID,
                        POLLING_OF_DEV_EVT,
                        200);
  }
}

static void UART_CallBack(uint8 port, uint8 event)
{
  (void)port;
  uint8 cmdbuff[1];
  if (event & (HAL_UART_RX_FULL | HAL_UART_RX_ABOUT_FULL | HAL_UART_RX_TIMEOUT))
  {
    HalUARTRead(port, cmdbuff, 1);
    SendUART(cmdbuff,1);
  }
}

//This function initializes the UART
void uart_init()
{
  halUARTCfg_t uartConfig;
  uartConfig.configured = TRUE; // 2x30 don't care - see uart driver.
  uartConfig.baudRate = HAL_UART_BR_9600;
  uartConfig.flowControl = TRUE;
  uartConfig.flowControlThreshold = 48; // 2x30 don't care - see uart driver.
  uartConfig.rx.maxBufSize = 128; // 2x30 don't care - see uart driver.
  uartConfig.tx.maxBufSize = 128; // 2x30 don't care - see uart driver.
  uartConfig.idleTimeout = 6; // 2x30 don't care - see uart driver.
  uartConfig.intEnable = TRUE; // 2x30 don't care - see uart driver.
  uartConfig.callBackFunc = UART_CallBack;
  HalUARTOpen (HAL_UART_PORT_0, &uartConfig);
}

void SendUART(uint8 *data,uint8 len)
{
  HalUARTWrite(HAL_UART_PORT_0, data, len);
}

static void ParkWay_HandleKeys( uint8 shift, uint8 keys )
{
}

/*********************************************************************/
