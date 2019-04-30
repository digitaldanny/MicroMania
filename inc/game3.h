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
#include "game3_snake_functions.h"
/*********************************************** Includes ********************************************************************/

// SEMAPHORES -----------------------
extern semaphore_t CENTER_SEMAPHORE;

// TIMER MACROS ---------------------
#define SN_UPDATE_PLAYER_POS    5

// GAME MACROS ----------------------
#define SN_MAX_FOOD_ON_MAP      10
#define SN_FOOD_SIZE            10
#define SN_FOOD_SPAWN_RANGE     30

#define SN_DIR_RIGHT        0
#define SN_DIR_LEFT         1
#define SN_DIR_UP           2
#define SN_DIR_DOWN         3

#define SN_PLAYER1_COLOR    LCD_GREEN
#define SN_PLAYER2_COLOR    LCD_BLUE
#define SN_PLAYER3_COLOR    LCD_RED

#define SN_SNAKE_FRAME_0    20 // fully open
#define SN_SNAKE_FRAME_1    21
#define SN_SNAKE_FRAME_2    22
#define SN_SNAKE_FRAME_3    23
#define SN_SNAKE_FRAME_4    24 // fully closed
#define SN_SNAKE_FRAME_5    25
#define SN_SNAKE_FRAME_6    26
#define SN_SNAKE_FRAME_7    27

#define SN_FIFO_PL0         0
#define SN_FIFO_PL1         1
#define SN_FIFO_PL2         2

#define SN_SNAKE_SIZE       SN_FOOD_SIZE

#define SN_MAP_MAX_X        1 * (MAX_SCREEN_X) + SN_SNAKE_SIZE / 2
#define SN_MAP_MIN_X        0 + SN_SNAKE_SIZE / 2
#define SN_MAP_CENTER_X     SN_MAP_MAX_X / 2
#define SN_MAP_MAX_Y        1 * (MAX_SCREEN_Y) + SN_SNAKE_SIZE / 2
#define SN_MAP_MIN_Y        0 + SN_SNAKE_SIZE / 2
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
    int8_t animation_count;
    bool alive;
    bool kill;
    bool size_up;
} game3_Player_t;

typedef struct
{
    point_t center;
    dir_t dir;

    // points used for erasing the borders
    // at different times
    point_t er_center1;
    point_t er_center2;
    point_t er_center3;
    point_t er_center4;

    // points used for erasing food at different
    // times
    point_t er_center5;
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
    dir_t dir;
} game3_ClientToHost_t;

/*
 * HOST TO CLIENT data structure
*/
typedef struct
{
    game3_ClientToHost_t client;
    game3_Food_t new_food;
    game3_Player_t players[MAX_NUM_PLAYERS];
    int8_t winner;
} game3_HostToClient_t;

/*********************************************** Game Functions *********************************************************************/

void game3_addHostThreads();    // called by the menu to initialize this game
void game3_addClientThreads();  // called by the menu to initialize this game

dir_t game3_oppositeDir ( dir_t dir );
void game3_checkDeleteColor( point_t * center, dir_t * dir, int16_t * color );
void game3_forceBorderTurn( point_t * center, int16_t * x_off, int16_t * y_off, dir_t * dir );
void game3_updateBorders();
point_t game3_spawnFood();
void game3_drawSnakeHead(dir_t prevDir, dir_t dir, int16_t x, int16_t y, int8_t count, int16_t color);
bool withinPlayerRange(point_t * objectCenter);
void mapObjectToMe(point_t * objectCenter, point_t * mappedCenter);
void mapObjectToPrev(int8_t num, point_t * objectCenter, point_t * mappedCenter);
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

/*
 *  Thread to update gamestate to be sent
 *  back to the client.
 */
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
