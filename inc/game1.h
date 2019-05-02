#ifndef INC_GAME1_H_
#define INC_GAME1_H_

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
#include "G8RTOS_Semaphores.h"
#include "menu.h"
/*********************************************** Includes ********************************************************************/

/*********************************************** Semaphores ******************************************************************/
semaphore_t LCD_Semaphore;



/*********************************************** Game Functions *********************************************************************/

void game1_addHostThreads();    // called by the menu to initialize this game
void game1_addClientThreads();  // called by the menu to initialize this game

/*********************************************** Structures     *********************************************************************/

// GAMESTATE data for both players
typedef struct
{

    uint16_t BC_Color;
    uint16_t Pipe_Color;

    // bird locations
    uint16_t Flappy_X;
    uint16_t Flappy_Y;
    int8_t  Flappy_Y_Velocity;

    // pipe locations
    uint16_t Pipe_Width;
    uint8_t  Pipe_Gap;
    int16_t Pipe_X;
    uint16_t Pipe_Y;

    uint8_t score;
} game1_gamestate_t;



// CLIENT TO HOST data structure
typedef struct
{
    uint32_t IP_address;    // return data information
    uint8_t playerNumber;   // return data information
    bool joined;            // handshaking data
    bool acknowledge;       // handshaking data
    bool buttonPressed;     // push button data
} game1_ClientToHost_t;

/*
 * HOST TO CLIENT data structure
 * Game Number : 0 => top left
 *               1 => top right
 *               2 => bottom left
 *               3 => bottom right
*/
typedef struct
{
    game1_ClientToHost_t client;
    int8_t game_number;
    int8_t prev_game_number;
    bool choice_made;
    game1_gamestate_t gamestate;
    bool game1_endgame;
} game1_HostToClient_t;



/*********************************************** Client Threads *********************************************************************/
/*
 * Thread for client to join game
 */
void game1_JoinGame();

/*
 * Thread that receives game state packets from host
 */
void game1_ReceiveDataFromHost();

/*
 * Thread that sends UDP packets to host
 */
void game1_SendDataToHost();

/*
 * Thread to read client's joystick
 */
void game1_ReadJoystickClient();

/*
 * End of game for the client
 */
void game1_EndOfGameClient();

/*********************************************** Client Threads *********************************************************************/


/*********************************************** Host Threads *********************************************************************/
/*
 * Thread for the host to create a game
 */
void game1_CreateGame();

/*
 * Thread for moving the bird on host side
 */
void game1_MoveBird();

/*
 * Thread that sets new location of pipes.
 */
void game1_MovePipes();

/*
 * Thread that sends game state to client
 */
void game1_SendDataToClient();

/*
 * Thread that receives UDP packets from client
 */
void game1_ReceiveDataFromClient();

/*
 * Thread to read host's joystick
 */
void game1_ReadJoystickHost();

/*
 * End of game for the host
 */
void game1_EndOfGameHost();

/*********************************************** Host Threads *********************************************************************/


/*********************************************** Common Threads *********************************************************************/
/*
 * Idle thread
 */
void game1_IdleThread();

/*
 * Thread to draw all the objects in the game
 */
void game1_DrawObjects();

/*********************************************** Common Functions *********************************************************************/

/*
 * Initializes and prints initial game state
 */
void game1_InitBoardState();

/*********************************************** Aperiodic Threads *********************************************************************/
void game1_ButtonPress ( void );

#endif /* INC_GAME1_H_ */
