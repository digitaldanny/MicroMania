#ifndef INC_GAME1_H_
#define INC_GAME1_H_

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
/*********************************************** Includes ********************************************************************/

/*********************************************** Game Functions *********************************************************************/

void game1_addHostThreads();    // called by the menu to initialize this game
void game1_addClientThreads();  // called by the menu to initialize this game

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
