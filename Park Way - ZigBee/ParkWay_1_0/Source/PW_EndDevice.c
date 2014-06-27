/*****************************************************************************
  Project: Park Way (ZigBee)
  End Device Firmware

  Ver.: 1.0
  Date: 26/06/14
  Developer:  Ânderson Ignácio da Silva
  Obs.:
    HAL_UART=TRUE - Enables UART on device

  Pre-processor directives:

  Changed files:

      

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

/* RTOS */
#if defined( IAR_ARMCM3_LM )
#include "RTOS_App.h"
#endif  

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */
#define   TIME_AUTO_POLL_ED           200
#define   park                        '0'
#define   DEVICE_PW_1                 '0'
#define   DEVICE_PW_2                 '2'
#define   ParkWay_SEND_MSG_TIMEOUT   1500
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

/*********************************************************************
 * @fn      ParkWay_Init
 *
 * @brief   Initialization function for the Generic App Task.
 *          This is called during initialization and should contain
 *          any application specific initialization (ie. hardware
 *          initialization/setup, table initialization, power up
 *          notificaiton ... ).
 *
 * @param   task_id - the ID assigned by OSAL.  This ID should be
 *                    used to send messages and set timers.
 *
 * @return  none
 */
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
                        2000);
}

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
        case ZDO_CB_MSG:
          ParkWay_ProcessZDOMsgs( (zdoIncomingMsg_t *)MSGpkt );
          break;

        case KEY_CHANGE:
          ParkWay_HandleKeys( ((keyChange_t *)MSGpkt)->state, ((keyChange_t *)MSGpkt)->keys );
          osal_start_timerEx( ParkWay_TaskID,
                              POLLING_OF_DEV_EVT,
                              2000);
          break;

        case AF_DATA_CONFIRM_CMD:
          // This message is received as a confirmation of a data packet sent.
          // The status is of ZStatus_t type [defined in ZComDef.h]
          // The message fields are defined in AF.h
          afDataConfirm = (afDataConfirm_t *)MSGpkt;
          sentEP = afDataConfirm->endpoint;
          sentStatus = afDataConfirm->hdr.status;
          sentTransID = afDataConfirm->transID;
          (void)sentEP;
          (void)sentTransID;

          // Action taken when confirmation is received.
          if ( sentStatus != ZSuccess )
          {
            // The data wasn't delivered -- Do something
          }
          break;

        case AF_INCOMING_MSG_CMD:
          ParkWay_MessageMSGCB( MSGpkt );
          break;

        case ZDO_STATE_CHANGE:
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

  // Send a message out - This event is generated by a timer
  //  (setup in ParkWay_Init()).
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
  
  if(events & POLLING_OF_DEV_EVT)
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
                        2000 );
  } 


  HalLedSet( HAL_LED_ALL, HAL_LED_MODE_OFF );
  
  // Discard unknown events
  return 0;
}

/*********************************************************************
 * Event Generation Functions
 */

/*********************************************************************
 */
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

static void ParkWay_MessageMSGCB( afIncomingMSGPacket_t *pkt )
{
}

static void ParkWay_SendTheMessage( void )
{
  uint8 pData[6], pin_ultra=4;
  // P0SEL &= (~BV(2));
  // P0DIR &= (~BV(2));
  // vaga = (pin_ultra & P0);
  // if(vaga)
  // {
  //    vaga='1';
  // }
  // else
  // {
  //    vaga='0';
  // }

  pData[0] = 'E';
  pData[1] = 'D';
  pData[2] = DEVICE_PW_1;
  pData[3] = DEVICE_PW_2;
  pData[4] = '-';
  pData[5] = park;

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
  uartConfig.baudRate = HAL_UART_BR_115200;
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
