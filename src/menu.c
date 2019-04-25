#define SINGLE

/*
 *  menu.c
 *
 *  Created     : 4/20/2019
 *  Last Edit   : 4/20/2019
 *
 *  UPDATES     :
 *  4/18/2019   : File initialization.
 *  4/24/2019   : Fixed the sizing inconsistency issue for the menu blocks
 *
 *  TODO        :
 *
 */

#include "menu.h"

/*********************************************** Globals *********************************************************************/
HostToClient_t packet_HostToClient;
HostToClient_t packet_zipped;
ClientToHost_t packet_ClientToHost;
uint8_t numPlayers;

/*********************************************** Functions ********************************************************************/
// opening menu to choose between host or client
// B0 => HOST
// B1 => CLIENT
void menu_writeHostOrClient()
{
    LCD_Text(8*14, 16*6, "B0 -> HOST", LCD_WHITE);
    LCD_Text(8*14, 16*7, "B2 -> CLIENT", LCD_WHITE);
}

// Erases the opening menu to allow the main
// menu to open
void menu_eraseHostOrClient()
{
    LCD_Clear(LCD_BLACK);
}

void menu_addThreadsHost ( void )
{
#ifndef SINGLE
    G8RTOS_AddThread(&menu_SendDataToClient, 15, 0xFFFFFFFF, "SEND_DATA_2_CL");
#endif
    G8RTOS_AddThread(&menu_JoystickHost, 20, 0xFFFFFFFF, "JOYSTICK_HOST");
    G8RTOS_AddThread(&common_IdleThread, 255, 0xFFFFFFFF, "IDLE");
}

void menu_addThreadsClient ( void )
{
    G8RTOS_AddThread(&menu_ReceiveDataFromHost, 15, 0xFFFFFFFF, "SEND_DATA_2_HOST");
    G8RTOS_AddThread(&common_IdleThread, 255, 0xFFFFFFFF, "IDLE");
}

// initialize the main menu drawings for both the host AND client
void menu_initMenu ( void )
{
    // draw the menu quadrants
    LCD_DrawRectangle(MAX_SCREEN_X/2 - 1, MAX_SCREEN_X/2, 0, MAX_SCREEN_Y - 1, LCD_WHITE);
    LCD_DrawRectangle(0, MAX_SCREEN_X, MAX_SCREEN_Y/2 - 1, MAX_SCREEN_Y/2, LCD_WHITE);

    // draw the menu top, bottom, and side bars
    LCD_DrawRectangle(0, 1, 0, MAX_SCREEN_Y - 1, LCD_WHITE);
    LCD_DrawRectangle(MAX_SCREEN_X - 2, MAX_SCREEN_X - 1, 0, MAX_SCREEN_Y - 1, LCD_WHITE);
    LCD_DrawRectangle(0, MAX_SCREEN_X - 1, 0, 1, LCD_WHITE);
    LCD_DrawRectangle(0, MAX_SCREEN_X - 1, MAX_SCREEN_Y - 2, MAX_SCREEN_Y - 1, LCD_WHITE);

    // write the game options in each quadrant
    LCD_Text( (QUAD_1_MAX_X + QUAD_1_MIN_X)/2 - 8*3, (QUAD_1_MAX_Y + QUAD_1_MIN_Y)/2 - 16, "PAC-MAN", LCD_WHITE);
    LCD_Text( (QUAD_2_MAX_X + QUAD_2_MIN_X)/2 - 8*4, (QUAD_2_MAX_Y + QUAD_2_MIN_Y)/2 - 16, "DODGEBALL", LCD_WHITE);
    LCD_Text( (QUAD_3_MAX_X + QUAD_3_MIN_X)/2 - 8*3, (QUAD_3_MAX_Y + QUAD_3_MIN_Y)/2, "SNAKE", LCD_WHITE);
    LCD_Text( (QUAD_4_MAX_X + QUAD_4_MIN_X)/2 - 8*2, (QUAD_4_MAX_Y + QUAD_4_MIN_Y)/2, "SUMO", LCD_WHITE);
}

// redraws the menu with the correct tile hi-lited
void menu_updateMenu ( void )
{
    // only redraw the square if required
    if ( packet_HostToClient.game_number != packet_HostToClient.prev_game_number )
    {

        // update the current game quadrant
        if ( packet_HostToClient.game_number == 0 )
        {
            LCD_DrawRectangle(QUAD_1_MIN_X + 3, QUAD_1_MAX_X - 3, QUAD_1_MIN_Y + 3, QUAD_1_MAX_Y - 3, LCD_WHITE);
            LCD_Text( (QUAD_1_MAX_X + QUAD_1_MIN_X)/2 - 8*3, (QUAD_1_MAX_Y + QUAD_1_MIN_Y)/2 - 16, "PAC-MAN", LCD_BLACK);
        }
        else if ( packet_HostToClient.game_number == 1 )
        {
            LCD_DrawRectangle(QUAD_2_MIN_X + 2, QUAD_2_MAX_X - 4, QUAD_2_MIN_Y + 3, QUAD_2_MAX_Y - 3, LCD_WHITE);
            LCD_Text( (QUAD_2_MAX_X + QUAD_2_MIN_X)/2 - 8*4, (QUAD_2_MAX_Y + QUAD_2_MIN_Y)/2 - 16, "DODGEBALL", LCD_BLACK);
        }
        else if ( packet_HostToClient.game_number == 2 )
        {
            LCD_DrawRectangle(QUAD_3_MIN_X + 3, QUAD_3_MAX_X - 3, QUAD_3_MIN_Y + 2, QUAD_3_MAX_Y - 4, LCD_WHITE);
            LCD_Text( (QUAD_3_MAX_X + QUAD_3_MIN_X)/2 - 8*3, (QUAD_3_MAX_Y + QUAD_3_MIN_Y)/2, "SNAKE", LCD_BLACK);
        }
        else if ( packet_HostToClient.game_number == 3 )
        {
            LCD_DrawRectangle(QUAD_4_MIN_X + 2, QUAD_4_MAX_X - 4, QUAD_4_MIN_Y + 2, QUAD_4_MAX_Y - 4, LCD_WHITE);
            LCD_Text( (QUAD_4_MAX_X + QUAD_4_MIN_X)/2 - 8*2, (QUAD_4_MAX_Y + QUAD_4_MIN_Y)/2, "SUMO", LCD_BLACK);
        }

        // erase the previous game quadrant
        if ( packet_HostToClient.prev_game_number == 0 )
        {
            LCD_DrawRectangle(QUAD_1_MIN_X + 2, QUAD_1_MAX_X - 2, QUAD_1_MIN_Y + 2, QUAD_1_MAX_Y - 2, LCD_BLACK);
            LCD_Text( (QUAD_1_MAX_X + QUAD_1_MIN_X)/2 - 8*3, (QUAD_1_MAX_Y + QUAD_1_MIN_Y)/2 - 16, "PAC-MAN", LCD_WHITE);
        }
        else if ( packet_HostToClient.prev_game_number == 1 )
        {
            LCD_DrawRectangle(QUAD_2_MIN_X + 1, QUAD_2_MAX_X - 3, QUAD_2_MIN_Y + 2, QUAD_2_MAX_Y - 2, LCD_BLACK);
            LCD_Text( (QUAD_2_MAX_X + QUAD_2_MIN_X)/2 - 8*4, (QUAD_2_MAX_Y + QUAD_2_MIN_Y)/2 - 16, "DODGEBALL", LCD_WHITE);
        }
        else if ( packet_HostToClient.prev_game_number == 2 )
        {
            LCD_DrawRectangle(QUAD_3_MIN_X + 2, QUAD_3_MAX_X - 2, QUAD_3_MIN_Y + 1, QUAD_3_MAX_Y - 3, LCD_BLACK);
            LCD_Text( (QUAD_3_MAX_X + QUAD_3_MIN_X)/2 - 8*3, (QUAD_3_MAX_Y + QUAD_3_MIN_Y)/2, "SNAKE", LCD_WHITE);
        }
        else if ( packet_HostToClient.prev_game_number == 3 )
        {
            LCD_DrawRectangle(QUAD_4_MIN_X + 1, QUAD_4_MAX_X - 3, QUAD_4_MIN_Y + 1, QUAD_4_MAX_Y - 3, LCD_BLACK);
            LCD_Text( (QUAD_4_MAX_X + QUAD_4_MIN_X)/2 - 8*2, (QUAD_4_MAX_Y + QUAD_4_MIN_Y)/2, "SUMO", LCD_WHITE);

        }

        // update the previous game number so that the LCD only has to
        // update when a new game number is chosen.
        packet_HostToClient.prev_game_number = packet_HostToClient.game_number;
    }
}
/*********************************************** Host Threads *********************************************************************/
// This thread initializes wifi handshake then
// allows the user to control a menu.
void menu_MenuHost ( void )
{
    // packet initializations
    // Initialize the game number and previous game number to -1 so they
    // are INVALID game numbers so they will write on the first valid game number,
    // but they are also the same number, so it will not write over a corner.

    packet_HostToClient.game_number         = -1;
    packet_HostToClient.choice_made         = false;
    packet_HostToClient.prev_game_number    = -1;
    packet_HostToClient.client.IP_address   = 0;
    packet_HostToClient.client.acknowledge  = 0;
    packet_HostToClient.client.joined       = 0;
    packet_HostToClient.client.playerNumber = 0;
    numPlayers = 1; // counting the host as the first player

    // hardware initializations
    common_buttons_init();

    P2->OUT &= ~(BIT0 | BIT1 | BIT2); // initialize led's off
    P2->DIR |= (BIT0 | BIT1 | BIT2); // set R.G.B direction

#ifndef SINGLE
    // PUT THIS IN A LOOP LATER ***************************************************
    // HANDSHAKE HERE ----------------------------------------------
    // 3. Try to receive packet from the client until return SUCCESS
    while( ReceiveData((_u8*)&packet_HostToClient.client, sizeof(packet_HostToClient.client)) < 0 );

    // 4. Acknowledge client to tell them they joined the game.
    packet_HostToClient.client.joined = true;
    packet_HostToClient.client.playerNumber = numPlayers;
    numPlayers++;
    SendData( (_u8*)&packet_HostToClient, packet_HostToClient.client.IP_address, sizeof(packet_HostToClient) );

    // Wait for client to sync with host by acknowledging that
    // it received the host message.
    do
    {
        ReceiveData((uint8_t*)&packet_HostToClient.client, sizeof(packet_HostToClient.client));
    } while ( packet_HostToClient.client.acknowledge == false );
    // HANDSHAKE HERE -----------------------------------------------
#endif

    RED_ON;

    // enables sending menu state information to the
    // client and calculating joystick information from host.
    menu_initMenu();
    menu_addThreadsHost();

    while (1)
    {
        menu_updateMenu();

        // if the player enters the common button interrupt,
        // with the up button, the up_button boolean will get set.
        if ( buttonUP_pressed ) packet_HostToClient.choice_made = true;

#ifdef SINGLE
        // Check if the game choice has been made. If yes, add those threads
        if ( packet_HostToClient.choice_made == true )
            G8RTOS_AddThread(&ExitMenuHost, 0, 0xFFFFFFFF, "END_HOST");
#endif

        sleep(10);
    }
}

void menu_SendDataToClient ( void )
{
    while (1)
    {
        // 2. Send packet
        G8RTOS_WaitSemaphore(&CC3100_SEMAPHORE);
        SendData( (uint8_t*)&packet_HostToClient, packet_HostToClient.client.IP_address, sizeof(packet_HostToClient) );
        G8RTOS_SignalSemaphore(&CC3100_SEMAPHORE);

        // Check if the game choice has been made. If yes, add those threads
        if ( packet_HostToClient.choice_made == true )
            G8RTOS_AddThread(&ExitMenuHost, 0, 0xFFFFFFFF, "END_HOST");

        sleep(5);
    }
}

void menu_JoystickHost ( void )
{
    int16_t joystick_x = 0;
    int16_t joystick_y = 0;
    int16_t displacementX = 0;
    int16_t displacementY = 0;
    int16_t avgX = 0;
    int16_t avgY = 0;

    while(1)
    {
        GetJoystickCoordinates(&joystick_x, &joystick_y);
        avgX = (avgX + joystick_x + JOYSTICK_BIAS_X) >> 1;
        avgY = (avgY + joystick_y + JOYSTICK_BIAS_Y) >> 1;

        // The switch statement was causing about 500 ms of lag
        displacementX = (avgX >> 0);
        displacementY = -(avgY >> 0);

        // determine whether to hi-lite the top left, top right, bottom left,
        // or bottom right games in the main menu. 3000 is chosen to ignore
        // any times the joystick is resting in the center. The only time the
        // menu icon will move around is when the joystick is at extreme edges.

        // DIAGONAL MOVEMENT HANLDING -----------------------------------------
        // BOTTOM
        if ( displacementY < -3000 )
        {
            // RIGHT
            if (displacementX > 3000 )
                packet_HostToClient.game_number = 2;

            // LEFT
            else if ( displacementX < -3000 )
                packet_HostToClient.game_number = 3;
        }

        // TOP
        else if ( displacementY > 3000 )
        {
            // RIGHT
            if (displacementX > 3000 )
                packet_HostToClient.game_number = 0;

            // LEFT
            else if ( displacementX < -3000 )
                packet_HostToClient.game_number = 1;
        }

        // HORIZONTAL MOVEMENT HANLDING ----------------------------------------
        // in case the user is not pointing directly toward the bottom left,
        // bottom right, top left, or top right.. allow him to move from bottom
        // left to bottom right or top right to top left.
        if ( abs(displacementY) < 3000 && abs(displacementX) > 3000 )
        {
            // top left to top right
            if ( displacementX < -3000 && packet_HostToClient.game_number == 0 )
                packet_HostToClient.game_number = 1;

            // top right to top left
            else if ( displacementX > 3000 && packet_HostToClient.game_number == 1 )
                packet_HostToClient.game_number = 0;

            // bottom left to bottom right
            else if ( displacementX < -3000 && packet_HostToClient.game_number == 2 )
                packet_HostToClient.game_number = 3;

            // bottom right to botton left
            else if ( displacementX > 3000 && packet_HostToClient.game_number == 3 )
                packet_HostToClient.game_number = 2;
        }

        // VERTICAL MOVEMENT HANLDING ----------------------------------------
        // in case the user is not pointing directly toward the bottom left,
        // bottom right, top left, or top right.. allow him to move from bottom left
        // to top left or top right to bottom right
        if ( abs(displacementX) < 3000 && abs(displacementY) > 3000 )
        {
            // top right to bottom right
            if ( displacementY < -3000 && packet_HostToClient.game_number == 1 )
                packet_HostToClient.game_number = 3;

            // bottom right to top right
            if ( displacementY > 3000 && packet_HostToClient.game_number == 3 )
                packet_HostToClient.game_number = 1;

            // top left to bottom left
            if ( displacementY < -3000 && packet_HostToClient.game_number == 0 )
                packet_HostToClient.game_number = 2;

            // bottom left to top left
            if ( displacementY > 3000 && packet_HostToClient.game_number == 2 )
                packet_HostToClient.game_number = 0;
        }

        sleep(15);
    }
}

void ExitMenuHost ( void )
{
    // wait for semaphores
    G8RTOS_WaitSemaphore(&LCDREADY);
    G8RTOS_WaitSemaphore(&CC3100_SEMAPHORE);

    G8RTOS_KillAllOthers();

    // force semaphores to reset here..
    // shouldn't be required, but fixes semaphore block bug
    G8RTOS_InitSemaphore(&LCDREADY, 1);
    G8RTOS_InitSemaphore(&CC3100_SEMAPHORE, 1);

    buttonUP_pressed = false;
    buttonDOWN_pressed = false;
    buttonRIGHT_pressed = false;
    buttonLEFT_pressed = false;

    /*
    // use the particular game's gameX_addThreadsHost
    if ( packet_HostToClient.game_number == G_PAC_MAN )
        G8RTOS_AddThread(&game1_CreateGame, 16, 0xFFFFFFFF, "CREATE_PAC_MAN");
    else if ( packet_HostToClient.game_number == G_DODGEBALL )
        G8RTOS_AddThread(&game2_CreateGame, 16, 0xFFFFFFFF, "CREATE_DODGE");
    else if ( packet_HostToClient.game_number == G_SNAKE )
        G8RTOS_AddThread(&game3_CreateGame, 16, 0xFFFFFFFF, "CREATE_SNAKE");
    else if ( packet_HostToClient.game_number == G_SUMO )
        G8RTOS_AddThread(&game4_CreateGame, 16, 0xFFFFFFFF, "CREATE_SUMO");
     */
    //if ( packet_HostToClient.game_number == G_DODGEBALL )
        G8RTOS_AddThread(&Game2_CreateGame, 16, 0xFFFFFFFF, "CREATE_SNAKE");

    // reset so a choice can be made again later
    packet_HostToClient.choice_made = false;

    G8RTOS_KillSelf();
}

/*********************************************** Client Threads *********************************************************************/
void menu_MenuClient ( void )
{
    // packet initializations
    packet_ClientToHost.IP_address = getLocalIP();
    packet_ClientToHost.acknowledge = false;
    packet_ClientToHost.joined = false;
    packet_ClientToHost.playerNumber = 0;   // reassigned by host

    // hardware initializations
    common_buttons_init();

    P2->OUT &= ~(BIT0 | BIT1 | BIT2); // initialize led's off
    P2->DIR |= (BIT0 | BIT1 | BIT2); // set R.G.B direction

    // HANDSHAKE HERE ----------------------------------------------
    // wait for the host to receive message and notify
    // client that they joined the game.
    do
    {
        SendData(       (uint8_t*)&packet_ClientToHost, HOST_IP_ADDR, sizeof(packet_ClientToHost) );   // start handshake
        ReceiveData(    (uint8_t*)&packet_ClientToHost, sizeof(packet_ClientToHost) );   // check if host acknowledges
    } while( packet_ClientToHost.joined == false );

    // 4. Acknowledge client to tell them they have received
    // the message about joining the game and the game can begin.
    packet_ClientToHost.acknowledge = true;
    SendData( (uint8_t*)&packet_ClientToHost, HOST_IP_ADDR, sizeof(packet_ClientToHost) );
    // HANDSHAKE HERE -----------------------------------------------

    BLUE_ON;

    // enables sending menu state information to the
    // client and calculating joystick information from host.
    menu_addThreadsClient();
    menu_initMenu();

    while (1)
    {
        menu_updateMenu();
        sleep(10);
    }
}

void menu_ReceiveDataFromHost ( void )
{
    while(1)
    {
        // Receive packet from the host
        G8RTOS_WaitSemaphore(&CC3100_SEMAPHORE);
        ReceiveData( (_u8*)&packet_zipped, sizeof(packet_zipped));
        G8RTOS_SignalSemaphore(&CC3100_SEMAPHORE);

        // check that the data was correctly synchronized then
        // unpack the packet into the global gamestate.
        if ( packet_zipped.client.IP_address == getLocalIP() )
            packet_HostToClient = packet_zipped;

        // 3. Check if the game is done. Add EndOfGameHost thread if done.
        if ( packet_HostToClient.choice_made == true )
            G8RTOS_AddThread(ExitMenuClient, 0, 0xFFFFFFFF, "END_CLIENT_");

        sleep(2);
    }
}

void ExitMenuClient ( void )
{
    // wait for semaphores
    G8RTOS_WaitSemaphore(&LCDREADY);
    G8RTOS_WaitSemaphore(&CC3100_SEMAPHORE);

    G8RTOS_KillAllOthers();

    // force semaphores to reset here..
    // shouldn't be required, but fixes semaphore block bug
    G8RTOS_InitSemaphore(&LCDREADY, 1);
    G8RTOS_InitSemaphore(&CC3100_SEMAPHORE, 1);

    /*
    // use the particular game's gameX_addThreadsHost
    if ( packet_HostToClient.game_number == 0 )
    {
        game1_addClientThreads();
    }
    else if ( packet_HostToClient.game_number == 1 )
    {
        game2_addClientThreads();
    }
    else if ( packet_HostToClient.game_number == 2 )
    {
        game3_addClientThreads();
    }
    else if ( packet_HostToClient.game_number == 3 )
    {
        game4_addClientThreads();
    }
    */

    packet_HostToClient.choice_made = false;

    G8RTOS_KillSelf();
}
