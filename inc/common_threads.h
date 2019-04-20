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
#include "LCD_empty.h"
#include "time.h"
#include "math.h"
/*********************************************** Includes ********************************************************************/

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

/*********************************************** Background Threads *********************************************************************/
void common_IdleThread ( void );

/*********************************************** Aperiodic Threads *********************************************************************/
void common_ButtonPress ( void );

#endif /* INC_COMMON_THREADS_H_ */
