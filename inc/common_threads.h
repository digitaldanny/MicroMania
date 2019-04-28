/*
 * common_threads.h
 *
 *  Created on: Apr 20, 2019
 *      Author: dhami
 */

#ifndef INC_COMMON_THREADS_H_
#define INC_COMMON_THREADS_H_

/*********************************************** Includes ********************************************************************/
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include "G8RTOS.h"
#include "cc3100_usage.h"
#include "GLCD.h"
#include "time.h"
#include "math.h"
/*********************************************** Includes ********************************************************************/

#define JOYSTICK_BIAS_X     500
#define JOYSTICK_BIAS_Y     350

#define MAX_NUM_PLAYERS     2

typedef enum
{
    UP,
    DOWN,
    RIGHT,
    LEFT
} dir_t;

typedef struct
{
    int16_t x;
    int16_t y;
} point_t;

/*********************************************** Semaphores ********************************************************************/
extern semaphore_t CC3100_SEMAPHORE;
extern semaphore_t LCDREADY;
/*********************************************** Semaphores ********************************************************************/

/*********************************************** Globals ********************************************************************/
extern bool buttonUP_pressed;
extern bool buttonRIGHT_pressed;
extern bool buttonDOWN_pressed;
extern bool buttonLEFT_pressed;
extern playerType myPlayerType;
/*********************************************** Globals ********************************************************************/

/*********************************************** Functions ********************************************************************/
void common_buttons_init( void );
void common_checkLCDBoundaries( point_t * point );

/*********************************************** Background Threads *********************************************************************/
void common_IdleThread ( void );
void common_debounceThread ( void );

/*********************************************** Aperiodic Threads *********************************************************************/
void common_ButtonPress ( void );

#endif /* INC_COMMON_THREADS_H_ */
