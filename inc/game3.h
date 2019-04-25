#ifndef INC_GAME3_H_
#define INC_GAME3_H_

/*********************************************** Includes ********************************************************************/
#include "stdbool.h"
#include "stdint.h"
#include "stdlib.h"
#include "stdio.h"
#include "G8RTOS.h"
#include "cc3100_usage.h"
#include "GLCD.h"
#include "time.h"
#include "math.h"
#include "common_threads.h"
/*********************************************** Includes ********************************************************************/

// TIMER MACROS ---------------------
#define SN_UPDATE_PLAYER_POS    25

// GAME MACROS ----------------------
#define SN_MAX_FOOD_ON_MAP      1
#define SN_FOOD_SIZE            10
#define SN_FOOD_SPAWN_RANGE     10

#define SN_DIR_RIGHT        0
#define SN_DIR_LEFT         1
#define SN_DIR_UP           2
#define SN_DIR_DOWN         3

#define SN_FIFO_PL0         0
#define SN_FIFO_PL1         1
#define SN_FIFO_PL2         2

#define SN_SNAKE_SIZE       SN_FOOD_SIZE

#define SN_MAP_MAX_X        1 * MAX_SCREEN_X
#define SN_MAP_MIN_X        0
#define SN_MAP_CENTER_X     SN_MAP_MAX_X / 2
#define SN_MAP_MAX_Y        1 * MAX_SCREEN_Y
#define SN_MAP_MIN_Y        0
#define SN_MAP_CENTER_Y     SN_MAP_MAX_Y / 2

#define SN_BG_COLOR         LCD_WHITE
#define SN_FOOD_COLOR       LCD_PURPLE

#define SN_DRAW_RANGE_X     MAX_SCREEN_X
#define SN_DRAW_RANGE_Y     MAX_SCREEN_Y

// HARDWARE MACROS ------------------------
#define RED_ON              P2->OUT |= BIT0
#define RED_OFF             P2->OUT &= ~BIT0
#define GREEN_ON            P2->OUT |= BIT1
#define GREEN_OFF           P2->OUT &= ~BIT1
#define BLUE_ON             P2->OUT |= BIT2
#define BLUE_OFF            P2->OUT &= ~BIT2

/*********************************************** Enums ********************************************************************/


/*********************************************** Typedefs ********************************************************************/

typedef struct
{
    point_t center;
    dir_t dir;
    bool alive;
    bool kill;
} game3_Player_t;

typedef struct
{
    point_t center;
} prev_player_t;

typedef struct
{
    point_t center;
    bool alive;
    bool kill;
} game3_Food_t;

typedef struct
{
    point_t center;
} prev_Food_t;

// CLIENT TO HOST data structure
typedef struct
{
    uint32_t IP_address;    // return data information
    uint8_t playerNumber;   // return data information
    bool joined;            // handshaking data
    bool acknowledge;       // handshaking data
    point_t kill_food_center;
} game3_ClientToHost_t;

/*
 * HOST TO CLIENT data structure
*/
typedef struct
{
    game3_ClientToHost_t client;
    game3_Food_t new_food;
    game3_Player_t players[MAX_NUM_PLAYERS];

} game3_HostToClient_t;

/*********************************************** Game Functions *********************************************************************/

void game3_addHostThreads();    // called by the menu to initialize this game
void game3_addClientThreads();  // called by the menu to initialize this game

void game3_refreshFood();
bool withinPlayerRange(point_t * objectCenter, point_t * myCenter);
void mapObjectToMe(point_t * objectCenter, point_t * mappedCenter);

/*********************************************** Client Threads *********************************************************************/
/*
 * Thread for client to join game
 */
void game3_JoinGame();

/*
 * Thread that receives game state packets from host
 */
void game3_ReceiveDataFromHost();

/*
 * Thread that sends UDP packets to host
 */
void game3_SendDataToHost();

/*
 * Thread to read client's joystick
 */
void game3_ReadJoystickClient();

/*
 * End of game for the client
 */
void game3_EndOfGameClient();

/*********************************************** Client Threads *********************************************************************/


/*********************************************** Host Threads *********************************************************************/
/*
 * Thread for the host to create a game
 */
void game3_CreateGame();

/*
 * Thread that sends game state to client
 */
void game3_SendDataToClient();

/*
 * Thread that receives UDP packets from client
 */
void game3_ReceiveDataFromClient();

/*
 * Thread to read host's joystick
 */
void game3_ReadJoystickHost();

void game3_UpdateGamestateHost();

/*
 * End of game for the host
 */
void game3_EndOfGameHost();

/*********************************************** Host Threads *********************************************************************/


/*********************************************** Common Threads *********************************************************************/

/*
 * Thread to draw all the objects in the game
 */
void game3_DrawObjects();

/*********************************************** Common Functions *********************************************************************/

/*
 * Initializes and prints initial game state
 */
void game3_InitBoardState();

/*********************************************** Aperiodic Threads *********************************************************************/
void game3_ButtonPress ( void );

#endif /* INC_GAME3_H_ */
