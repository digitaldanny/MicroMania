#ifndef INC_GAME2_H_
#define INC_GAME2_H_

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

/*********************************************** Game Functions *********************************************************************/

void game2_addHostThreads();    // called by the menu to initialize this game
void game2_addClientThreads();  // called by the menu to initialize this game

/*********************************************** Client Threads *********************************************************************/
/*
 * Thread for client to join game
 */
void game2_JoinGame();

/*
 * Thread that receives game state packets from host
 */
void game2_ReceiveDataFromHost();

/*
 * Thread that sends UDP packets to host
 */
void game2_SendDataToHost();

/*
 * Thread to read client's joystick
 */
void game2_ReadJoystickClient();

/*
 * End of game for the client
 */
void game2_EndOfGameClient();

/*********************************************** Client Threads *********************************************************************/


/*********************************************** Host Threads *********************************************************************/
/*
 * Thread for the host to create a game
 */
void game2_CreateGame();

/*
 * Thread that sends game state to client
 */
void game2_SendDataToClient();

/*
 * Thread that receives UDP packets from client
 */
void game2_ReceiveDataFromClient();

/*
 * Thread to read host's joystick
 */
void game2_ReadJoystickHost();

/*
 * End of game for the host
 */
void game2_EndOfGameHost();

/*********************************************** Host Threads *********************************************************************/


/*********************************************** Common Threads *********************************************************************/
/*
 * Idle thread
 */
void game2_IdleThread();

/*
 * Thread to draw all the objects in the game
 */
void game2_DrawObjects();

/*********************************************** Common Functions *********************************************************************/

/*
 * Initializes and prints initial game state
 */
void game2_InitBoardState();

/*********************************************** Aperiodic Threads *********************************************************************/
void game2_ButtonPress ( void );

#endif /* INC_GAME2_H_ */
