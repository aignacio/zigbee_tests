/*****************************************************************************
  Project: Park Way (ZigBee)
  Header Firmware

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

*****************************************************************************/

#ifndef ParkWay_H
#define ParkWay_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
#include "ZComDef.h"

/*********************************************************************
 * CONSTANTS
 */

// These constants are only for example and should be changed to the
// device's needs
#define ParkWay_ENDPOINT           10

#define ParkWay_PROFID             0x0F04
#define ParkWay_DEVICEID           0x0001
#define ParkWay_DEVICE_VERSION     0
#define ParkWay_FLAGS              0

#define ParkWay_MAX_CLUSTERS       1
#define ParkWay_CLUSTERID          1


// Application Events (OSAL) - These are bit weighted definitions.
#define ParkWay_SEND_MSG_EVT       0x0001
#define POLLING_OF_DEV_EVT         0x0002 
#define POLL_PARWAY_EVT            0x0003
#define POLLING_ULTRASONIC         0x0004
/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * FUNCTIONS
 */

/*
 * Task Initialization for the Generic Application
 */
extern void ParkWay_Init( byte task_id );

/*
 * Task Event Processor for the Generic Application
 */
extern UINT16 ParkWay_ProcessEvent( byte task_id, UINT16 events );

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* ParkWay_H */
