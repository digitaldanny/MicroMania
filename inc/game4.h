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
