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

/*********************************************** Game Functions *********************************************************************/

void game4_addHostThreads();    // called by the menu to initialize this game
void game4_addClientThreads();  // called by the menu to initialize this game

/*********************************************** Client Threads *********************************************************************/
/*
 * Thread for client to join game
 */
void game4_JoinGame();

/*
 * Thread that receives game state packets from host
 */
void game4_ReceiveDataFromHost();

/*
 * Thread that sends UDP packets to host
 */
void game4_SendDataToHost();

/*
 * Thread to read client's joystick
 */
void game4_ReadJoystickClient();

/*
 * End of game for the client
 */
void game4_EndOfGameClient();

/*********************************************** Client Threads *********************************************************************/


/*********************************************** Host Threads *********************************************************************/
/*
 * Thread for the host to create a game
 */
void game4_CreateGame();

/*
 * Thread that sends game state to client
 */
void game4_SendDataToClient();

/*
 * Thread that receives UDP packets from client
 */
void game4_ReceiveDataFromClient();

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
