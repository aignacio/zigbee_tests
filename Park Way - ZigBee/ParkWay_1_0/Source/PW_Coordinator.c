/*****************************************************************************
  Project: Park Way (ZigBee)
  Coordinator Firmware

  Ver.: 1.0
  Date: 26/06/14
  Developer:  �nderson Ign�cio da Silva
  Obs.:
    HAL_UART=TRUE - Enables UART on device

  Pre-processor directives:

      

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
#define   TIME_PARKWAY_POLL   2000
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
static uint8 DevListPW[20][6];        //Devices Vector for PARK WAY 

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
  HalLcdWriteString( "ParkWay-Coord", HAL_LCD_LINE_1 );
#endif

  ZDO_RegisterForZDOMsg( ParkWay_TaskID, End_Device_Bind_rsp );
  ZDO_RegisterForZDOMsg( ParkWay_TaskID, Match_Desc_rsp );

  osal_start_timerEx(   ParkWay_TaskID,
                        POLL_PARWAY_EVT,
                        TIME_PARKWAY_POLL);
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

        case AF_DATA_CONFIRM_CMD:
          break;

        case AF_INCOMING_MSG_CMD:
          ParkWay_MessageMSGCB( MSGpkt );
          break;

        case ZDO_STATE_CHANGE:
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

  if(events & POLLING_OF_DEV_EVT)
  {
    osal_start_timerEx( ParkWay_TaskID,
                        POLL_PARWAY_EVT,
                        TIME_PARKWAY_POLL);
    uart_init();
    SendUART("\n\rAP-0001",9);  
    uint8 i,i2;
    for(i=0;i<20;i++)
    {
      if(DevListPW[i][0])
      {
        SendUART(",",1);  
        for(i2=0;i2<6;i2++)
        {
          SendUART(&DevListPW[i][i2],1);  
          DevListPW[i][i2] = 0;
        }
      }
    } 
    SendUART("@",1);
  }  
 
  return 0;
}



static void ParkWay_ProcessZDOMsgs( zdoIncomingMsg_t *inMsg )
{
  switch ( inMsg->clusterID )
  {
    case End_Device_Bind_rsp:
      if ( ZDO_ParseBindRsp( inMsg ) == ZSuccess )
      {
        // Light LED
        HalLedSet( HAL_LED_1, HAL_LED_MODE_ON );
      }
#if defined( BLINK_LEDS )
      else
      {
        // Flash LED to show failure
        HalLedSet ( HAL_LED_1, HAL_LED_MODE_FLASH );
      }
#endif
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
            HalLedSet( HAL_LED_1, HAL_LED_MODE_ON );
          }
          osal_mem_free( pRsp );
        }
      }
      break;
  }
}

static void ParkWay_MessageMSGCB( afIncomingMSGPacket_t *pkt_1 )
{
  uint8* pkt;
  
  pkt=(pkt_1->cmd.Data);
  
  uint8 i,dev=(((*(pkt+2)-48)*10)+(*(pkt+3)-48));
  
  for(i=0;i<6;i++)
  {
    DevListPW[dev][i]=*(pkt+i);
  } 
}

static void ParkWay_SendTheMessage( void )
{
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
