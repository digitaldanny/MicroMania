#ifndef INC_MENU_H_
#define INC_MENU_H_

/*********************************************** Includes ********************************************************************/
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include "G8RTOS.h"
#include "cc3100_usage.h"
#include "LCD_empty.h"
#include "time.h"
#include "math.h"
#include "common_threads.h"
#include "game1.h"
#include "game2.h"
#include "game3.h"
#include "game4.h"
/*********************************************** Includes ********************************************************************/

#define RED_ON              P2->OUT |= BIT0
#define RED_OFF             P2->OUT &= ~BIT0
#define GREEN_ON            P2->OUT |= BIT1
#define GREEN_OFF           P2->OUT &= ~BIT1
#define BLUE_ON             P2->OUT |= BIT2
#define BLUE_OFF            P2->OUT &= ~BIT2

/*
 * ======================
 * =    Q1   |    Q2    =
 * ======================
 * =    Q3   |    Q4    =
 * ======================
 */
#define QUAD_1_MAX_X        MAX_SCREEN_X / 2
#define QUAD_1_MIN_X        0
#define QUAD_1_MAX_Y        MAX_SCREEN_Y / 2
#define QUAD_1_MIN_Y        0

#define QUAD_2_MAX_X        MAX_SCREEN_X
#define QUAD_2_MIN_X        MAX_SCREEN_X / 2
#define QUAD_2_MAX_Y        MAX_SCREEN_Y / 2
#define QUAD_2_MIN_Y        0

#define QUAD_3_MAX_X        MAX_SCREEN_X / 2
#define QUAD_3_MIN_X        0
#define QUAD_3_MAX_Y        MAX_SCREEN_Y
#define QUAD_3_MIN_Y        MAX_SCREEN_Y / 2

#define QUAD_4_MAX_X        MAX_SCREEN_X
#define QUAD_4_MIN_X        MAX_SCREEN_X / 2
#define QUAD_4_MAX_Y        MAX_SCREEN_Y
#define QUAD_4_MIN_Y        MAX_SCREEN_Y / 2

/*********************************************** Typedefs ********************************************************************/

// CLIENT TO HOST data structure
typedef struct
{
    uint32_t IP_address;    // return data information
    uint8_t playerNumber;   // return data information
    bool joined;            // handshaking data
    bool acknowledge;       // handshaking data
} ClientToHost_t;

/*
 * HOST TO CLIENT data structure
 * Game Number : 0 => top left
 *               1 => top right
 *               2 => bottom left
 *               3 => bottom right
*/
typedef struct
{
    ClientToHost_t client;
    int8_t game_number;
    int8_t prev_game_number;
    bool choice_made;
} HostToClient_t;
/*********************************************** Typedefs ********************************************************************/


/*********************************************** Semaphores ********************************************************************/
extern semaphore_t CC3100_SEMAPHORE;
extern semaphore_t LCDREADY;
/*********************************************** Semaphores ********************************************************************/

/*********************************************** Functions ********************************************************************/
void menu_writeHostOrClient();
void menu_eraseHostOrClient();
void menu_addThreadsHost ( void );
void menu_addThreadsClient ( void );
void menu_initMenu ( void );
/*********************************************** Functions ********************************************************************/

/*********************************************** Host Threads *********************************************************************/
void menu_MenuHost ( void );
void menu_SendDataToClient ( void );
void menu_JoystickHost ( void );
void ExitMenuHost ( void );

/*********************************************** Host Threads *********************************************************************/
void menu_MenuClient ( void );
void menu_ReceiveDataFromHost ( void );
void ExitMenuClient ( void );

/*********************************************** Aperiodic Threads *********************************************************************/
void menu_ButtonPress ( void );

#endif /* INC_MENU_H_ */
