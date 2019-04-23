#define MAIN
#define SINGLE

// To run game with different routers, go to the
// cc3100_usage.h and sl_common.h header files and
// adjust the preprocessor directive.

/*
 *
 *  main.c
 *
 *  UPDATES :
 *  3/29/2019   : File initialization.
 *  4/2/2019    : Connect to game as Host or Client.
 *	4/10/2019	: LCD and LED semaphores initialized.
 *
 *  TODO    :
 *  ~  Remove BUTTON_BUG directive once Daniel's board has been
 *      resoldered.
 *
 */

#include "G8RTOS.h"
#include "cc3100_usage.h"
#include "GLCD.h"
#include "common_threads.h"
#include "game1.h"
#include "game2.h"
#include "game3.h"
#include "game4.h"
#include "menu.h"

// ================================== MAIN PROGRAM ===============================
void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer

    // Initialize and launch RTOS
    G8RTOS_Init();
    common_buttons_init();
    LCD_Initializtion();

    G8RTOS_AddAperiodicEvent_Priority(&common_ButtonPress, 0, PORT4_IRQn);
    G8RTOS_AddAperiodicEvent_Priority(&common_ButtonPress, 0, PORT5_IRQn);

    // Initialize semaphores
    G8RTOS_InitSemaphore(&CC3100_SEMAPHORE, 1);
	G8RTOS_InitSemaphore(&LCDREADY, 1);
  
    // write the menu text
    menu_writeHostOrClient();

    // Do not initialize any of the threads until the user decides
    // whether to host the game or to be on the client side.
    while (1)
    {
        if (myPlayerType == Host)
        {
            // Initialize HOST-side threads
#ifndef SINGLE
            initCC3100(Host);
#endif

            G8RTOS_AddThread( &menu_MenuHost, 15, 0xFFFFFFFF, "MENU_HOST" ); // lowest priority
            buttonUP_pressed = false;
            break;
        }

        else if ( myPlayerType == Client )
        {
            // Initialize CLIENT-side threads
            initCC3100(Client);
            G8RTOS_AddThread( &menu_MenuClient, 15, 0xFFFFFFFF, "MENU_CLIENT" ); // lowest priority
            buttonRIGHT_pressed = false;
            break;
        }
    }

    // Erase the main menu before beginning the game
    menu_eraseHostOrClient();

    // launch RTOS
    G8RTOS_Launch_Priority();
}
