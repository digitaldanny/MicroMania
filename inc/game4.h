#ifndef INC_GAME4_H_
#define INC_GAME4_H_

/*********************************************** Includes ********************************************************************/
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include "G8RTOS.h"
#include "cc3100_usage.h"
#include "GLCD.h"
#include "time.h"
#include "math.h"
#include "common_threads.h"
/*********************************************** Includes ********************************************************************/
*
 * Struct to be sent from the client to the host
 */
typedef struct
{
    uint32_t IP_address;
    int8_t displacementX;
    int8_t displacementY;
    uint8_t playerNumber;
    uint8_t direction;
    bool ready;
    bool joined;
    bool acknowledge;
} Game2_SpecificPlayerInfo_t;

/*
 * General player info to be used by both host and client
 * Client responsible for translation
 */
typedef struct
{
    int16_t currentCenterX;
    int16_t currentCenterY;
    uint8_t direction;
    bool drawRight;
    uint16_t num_lives;
    uint16_t RunTime;
    bool kill;
} Game2_GeneralPlayerInfo_t;
/*
 * Struct of all the balls, only changed by the host
 */
typedef struct
{
    int16_t currentCenterX;
    int16_t currentCenterY;
    int16_t xvel;
    int16_t yvel;
    bool alive;
    bool kill;
} Game2_Ball_t;
/*
 * Struct of all the balls, only changed by the host
 */
typedef struct
{
    int16_t currentCenterX;
    int16_t currentCenterY;
    int16_t xvel;
    int16_t yvel;
    bool alive;
    bool kill;
} Game2_Shroom_t;

/*
 * Struct to be sent from the host to the client
 */
typedef struct
{
    Game2_SpecificPlayerInfo_t player;
    Game2_GeneralPlayerInfo_t players[MAX_NUM_OF_PLAYERS];
    Game2_Ball_t balls[MAX_NUM_OF_BALLS];
    Game2_Shroom_t shroom[MAX_NUM_OF_SHROOMS];
    bool winner;
    bool gameDone;
} Game2_GameState_t;
#pragma pack ( pop )

/*
 * Struct of all the previous ball locations, only changed by self for drawing!
 */
typedef struct
{
    int16_t CenterX;
    int16_t CenterY;
}Game2_PrevBall_t;

/*
 * Struct of all the previous players locations, only changed by self for drawing
 */
typedef struct
{
    int16_t CenterX;
    int16_t CenterY;
}Game2_PrevPlayer_t;

/*
 * Struct of all the previous players locations, only changed by self for drawing
 */
typedef struct
{
    int16_t CenterX;
    int16_t CenterY;
}Game2_PrevShroom_t;

/*********************************************** Game Functions *********************************************************************/

void game4_addHostThreads();    // called by the menu to initialize this game


/*********************************************** Host Threads *********************************************************************/
/*
 * Thread for the host to create a game
 */
void game4_CreateGame();

/*
 * Thread that sends game state to client
 */

/*
 * Thread to read host's joystick
 */
void game4_ReadJoystickHost();

/*
 * End of game for the host
 */
void game4_EndOfGameHost();

/*********************************************** Host Threads *********************************************************************/


/*********************************************** Common Threads *********************************************************************/
/*
 * Idle thread
 */
void game4_IdleThread();

/*
 * Thread to draw all the objects in the game
 */
void game4_DrawObjects();

/*********************************************** Common Functions *********************************************************************/

/*
 * Initializes and prints initial game state
 */
void game4_InitBoardState();

/*********************************************** Aperiodic Threads *********************************************************************/
void game4_ButtonPress ( void );

#endif /* INC_GAME4_H_ */
