#define DANNY_HOTSPOT

// PREPROCESSOR DIRECTIVES
// Configurations dependent on which router the
// device needs to connect to.
// ----------------------------------------------------
// DANNY_HOTSPOT :
// JAKE_HOTSPOT  :
// DANNY_ROUTER  :
// ----------------------------------------------------

/*
 * cc3100_usage.h
 *
 *  Created on: Feb 16, 2017
 *      Author: Danny
 */

#ifndef CC3100_USAGE_H_
#define CC3100_USAGE_H_

/********************** Includes *********************/
#include "simplelink.h"
#include "sl_common.h"
/********************** Includes *********************/

/*
 * Description of CC3100 Usage:
 *
 * TODO: fill this in
 *
 *
 */

/**************************** Defines ******************************/
/*
 * Determines whether player is the host or client
 */
typedef enum
{
    Client = 0,
    Host = 1,
    None = 2
}playerType;

/* IP addressed of server side socket.
 * Should be in long format, E.g: 0xc0a8010a == 192.168.1.10
 */

#ifdef MATT_HOTSPOT
#define HOST_IP_ADDR           0xAC140A09               // IP address of server to connect to
#define PORT_NUM               5001                     // Port number to be used
#define NO_OF_PACKETS          1                        // Number of packets to send out

/*
 * Static IP address for host
 */
#define CONFIG_IP       SL_IPV4_VAL(172,20,10,9)       /* Static IP to be configured */
#define AP_MASK         SL_IPV4_VAL(255,255,255,240)      /* Subnet Mask for the station */
#define AP_GATEWAY      SL_IPV4_VAL(172,20,10,1)        /* Default Gateway address */
#define AP_DNS          SL_IPV4_VAL(172,20,10,1)            /* DNS Server Address */
#define SL_STOP_TIMEOUT        0xFF
#endif

// #define HOST_IP_ADDR           0xC0A80102               // IP address of server to connect to
#ifdef DANNY_HOTSPOT
#define HOST_IP_ADDR           0xAC140A02               // IP address of server to connect to
#define PORT_NUM               5001                     // Port number to be used
#define NO_OF_PACKETS          1                        // Number of packets to send out

/*
 * Static IP address for host
 */
#define CONFIG_IP       SL_IPV4_VAL(172,20,10,2)       /* Static IP to be configured */
#define AP_MASK         SL_IPV4_VAL(255,255,255,240)      /* Subnet Mask for the station */
#define AP_GATEWAY      SL_IPV4_VAL(172,20,10,1)        /* Default Gateway address */
#define AP_DNS          SL_IPV4_VAL(172,20,10,1)            /* DNS Server Address */
#define SL_STOP_TIMEOUT        0xFF
#endif

#ifdef DANNY_COMP
#define HOST_IP_ADDR           0xC0A889F8               // IP address of server to connect to
#define PORT_NUM               5001                     // Port number to be used
#define NO_OF_PACKETS          1                        // Number of packets to send out

/*
 * Static IP address for host
 */
#define CONFIG_IP       SL_IPV4_VAL(192,168,137,248)       /* Static IP to be configured */
#define AP_MASK         SL_IPV4_VAL(255,255,255,0)      /* Subnet Mask for the station */
#define AP_GATEWAY      SL_IPV4_VAL(192,168,137,1)        /* Default Gateway address */
#define AP_DNS          SL_IPV4_VAL(192,168,137,1)            /* DNS Server Address */
#define SL_STOP_TIMEOUT        0xFF
#endif

#ifdef JAKE_HOTSPOT
#define HOST_IP_ADDR           0xAC140A07               // IP address of server to connect to
#define PORT_NUM               5001                     // Port number to be used
#define NO_OF_PACKETS          1                        // Number of packets to send out

#define CONFIG_IP       SL_IPV4_VAL(172,20,10,7)       /* Static IP to be configured */
#define AP_MASK         SL_IPV4_VAL(255,255,255,240)      /* Subnet Mask for the station */
#define AP_GATEWAY      SL_IPV4_VAL(172,20,10,1)        /* Default Gateway address */
#define AP_DNS          SL_IPV4_VAL(172,20,10,1)            /* DNS Server Address */
#define SL_STOP_TIMEOUT        0xFF
#endif

#ifdef DANNY_ROUTER
#define HOST_IP_ADDR           0xC0A801A9               // IP address of server to connect to
#define PORT_NUM               5001                     // Port number to be used
#define NO_OF_PACKETS          1                        // Number of packets to send out

/*
 * Static IP address for host
 */
#define CONFIG_IP       SL_IPV4_VAL(192,168,1,169)       /* Static IP to be configured */
#define AP_MASK         SL_IPV4_VAL(255,255,255,0)      /* Subnet Mask for the station */
#define AP_GATEWAY      SL_IPV4_VAL(192,168,1,1)        /* Default Gateway address */
#define AP_DNS          SL_IPV4_VAL(192,168,1,1)            /* DNS Server Address */
#define SL_STOP_TIMEOUT        0xFF
#endif

/* Application specific status/error codes */
typedef enum{
    DEVICE_NOT_IN_STATION_MODE = -0x7D0,        /* Choosing this number to avoid overlap w/ host-driver's error codes */
    BSD_UDP_CLIENT_FAILED = DEVICE_NOT_IN_STATION_MODE - 1,

    STATUS_CODE_MAX = -0xBB8
}e_AppStatusCodes;

#define NOTHING_RECEIVED -1
/**************************** Defines ******************************/


/*********************** User Functions ************************/
void SendData(_u8 *data, _u32 IP, _u16 BUF_SIZE);
_i32 ReceiveData(_u8 *data, _u16 BUF_SIZE);
void initCC3100(playerType playerRole);
_u32 getLocalIP();
/*********************** User Functions ************************/


/*********************** Event Handlers ************************/
void SimpleLinkSockEventHandler(SlSockEvent_t *pSock);
void SimpleLinkGeneralEventHandler(SlDeviceEvent_t *pDevEvent);
void SimpleLinkHttpServerCallback(SlHttpServerEvent_t *pHttpEvent,
                                  SlHttpServerResponse_t *pHttpResponse);
void SimpleLinkNetAppEventHandler(SlNetAppEvent_t *pNetAppEvent);
void SimpleLinkWlanEventHandler(SlWlanEvent_t *pWlanEvent);
/*********************** Event Handlers ************************/



#endif /* CC3100_USAGE_H_ */
