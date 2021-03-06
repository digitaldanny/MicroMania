/*
 *  menu.c
 *
 *  Created     : 4/20/2019
 *  Last Edit   : 4/20/2019
 *
 *  UPDATES     :
 *  4/2/2019    : Initialized menu functions.
 *
 *  DESCRIPTION :
 *  This document includes the functions and background threads that are
 *  common between all 4 games and the main menu.
 *
 *  TODO        :
 *
 */

#include "common_threads.h"

/*********************************************** Types ********************************************************************/

/*********************************************** Globals ********************************************************************/
bool buttonUP_pressed;
bool buttonRIGHT_pressed;
bool buttonDOWN_pressed;
bool buttonLEFT_pressed;
uint8_t afterInit = 0;

playerType myPlayerType;

semaphore_t CC3100_SEMAPHORE;
semaphore_t LCDREADY;

/*********************************************** Common Functions *********************************************************************/
void common_buttons_init( void )
{
    // Initialize the common globals for which buttons are pressed
    myPlayerType = None;
    buttonUP_pressed = false;
    buttonRIGHT_pressed = false;
    buttonDOWN_pressed = false;
    buttonLEFT_pressed = false;

    // Initialize the button interrupt properties
    // B0 = P4.4, B1 = P4.5
    P4->DIR &= ~(BIT4 | BIT5);
    P4->IFG &= ~(BIT4 | BIT5);      // P4.4 IFG cleared
    P4->IE  |= BIT4 | BIT5;         // Enable interrupt on P4.4
    P4->IES |= BIT4 | BIT5;         // high-to-low transition
    P4->REN |= BIT4 | BIT5;         // Pull-up resister
    P4->OUT |= BIT4 | BIT5;         // Sets res to pull-up
    NVIC_EnableIRQ(PORT4_IRQn);     // enable interrupts on PORT4
    __NVIC_SetPriority(PORT4_IRQn, 0);

    // B2 = P5.4, B3 = P5.5
    P5->DIR &= ~(BIT4 | BIT5);
    P5->IFG &= ~(BIT4 | BIT5);      // P4.4 IFG cleared
    P5->IE  |= BIT4 | BIT5;         // Enable interrupt on P4.4
    P5->IES |= BIT4 | BIT5;         // high-to-low transition
    P5->REN |= BIT4 | BIT5;         // Pull-up resister
    P5->OUT |= BIT4 | BIT5;         // Sets res to pull-up
    NVIC_EnableIRQ(PORT5_IRQn);     // enable interrupts on PORT4
    NVIC_SetPriority(PORT5_IRQn, 0);
}

// This function checks that the LCD is not being written past its
// boundaries ( x > 320 or x < 0, y > 240 or y < 0 )
void common_checkLCDBoundaries( point_t * point )
{
    if ( point->x >= MAX_SCREEN_X )         point->x = MAX_SCREEN_X - 1;
    else if ( point->x <= MIN_SCREEN_X )    point->x = MIN_SCREEN_X;

    if ( point->y >= MAX_SCREEN_Y )         point->y = MAX_SCREEN_Y - 1;
    else if ( point->y <= MIN_SCREEN_Y )    point->y = MIN_SCREEN_Y;
}

/*********************************************** Common Aperiodic Threads *********************************************************************/
void common_ButtonPress ( void )
{
     // PORT 4 INTERRUPT ROUTINES ---------------------------------
     // determine the next game mode
     if ( (P4->IFG & BIT4))
     {
         myPlayerType = Host;
     }
     else if ( (P4->IFG & BIT5))
     {
         myPlayerType = Client;
     }

     // PORT 5 INTERRUPT ROUTINE ---------------------------------------
     // MAIN MENU NAVIGATION MODE --------------------------------------
     if ( (P5->IFG & BIT4 ) )
     {
         myPlayerType = Client;
     }
     else if ( (P5->IFG & BIT5) )
     {
         myPlayerType = Client;
     }


     // disable interrupts for the debounce thread
     NVIC_DisableIRQ(PORT5_IRQn);
     NVIC_DisableIRQ(PORT4_IRQn);

     G8RTOS_AddThread(&common_debounceThread, 0, 0xFFFFFFFF, "DEBOUNCE");
}

// Debouncing thread waits for the button to stop bouncing
// then re-enables interrupts for that port.
void common_debounceThread ( void )
{
    sleep(10); // ms
        // PORT 4 INTERRUPT ROUTINES ---------------------------------
        // determine the next game mode
        if ( (~P4->IN & BIT4))
        {
            buttonUP_pressed = true;
        }
        else if ( (~(P4->IN) & BIT5))
        {
            buttonRIGHT_pressed = true;
        }

        // PORT 5 INTERRUPT ROUTINE ---------------------------------------
        // MAIN MENU NAVIGATION MODE --------------------------------------
        if ( (~(P5->IN) & BIT4 ) )
        {
            buttonDOWN_pressed = true;
        }
        else if ( (~(P5->IN) & BIT5) )
        {
            buttonLEFT_pressed = true;
        }
    // clear flags
    P4->IFG &= ~(BIT4 | BIT5);
    P5->IFG &= ~(BIT4 | BIT5);

    NVIC_EnableIRQ(PORT5_IRQn);
    NVIC_EnableIRQ(PORT4_IRQn);

    G8RTOS_KillSelf();
}


/*********************************************** Background Threads *********************************************************************/
void common_IdleThread ( void ) { while(1); }
