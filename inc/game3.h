#ifndef INC_GAME3_H_
#define INC_GAME3_H_

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

void game3_addHostThreads();    // called by the menu to initialize this game
void game3_addClientThreads();  // called by the menu to initialize this game

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
 * End of game for the host
 */
void game3_EndOfGameHost();

/*********************************************** Host Threads *********************************************************************/


/*********************************************** Common Threads *********************************************************************/
/*
 * Idle thread
 */
void game3_IdleThread();

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
