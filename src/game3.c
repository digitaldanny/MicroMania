#define SINGLE

/*
 *  menu.c
 *
 *  Created     : 4/20/2019
 *  Last Edit   : 4/20/2019
 *
 *  UPDATES     :
 *  4/18/2019    : Initialized game functions.
 *  4/20/2019   : Snake head draws and map movement added
 *  4/24/2019   : Map movement stabilized and added boundaries to the map
 *
 *  DESCRIPTION : SNAKE.IO
 *
 *  TODO        :
 *  ~ Write the initializers for multiple clients and the handshake
 *  ~ Make players spawn at a random location
 *  ~ Food should spawn near a random player
 *  ~ Update "me" for client players
 *  ~ Update withinPlayerRange function when ing is implemented
 *  ~ Possibly remove ExitMenu threads if they are never going to be used
 *  ~ Send new local food center data to client every time it is generated in create game
 */

#include "game3.h"

// game global ---------------
game3_HostToClient_t game3_HostToClient;
game3_HostToClient_t game3_zipped;
game3_ClientToHost_t game3_ClientToHost;

uint8_t game3_numPlayers;

prev_player_t prevPlayers[MAX_NUM_PLAYERS];
prev_Food_t prevFood[SN_MAX_FOOD_ON_MAP];
game3_Food_t local_food[SN_MAX_FOOD_ON_MAP];

game3_Player_t * me;
prev_player_t * my_prev;

// other globals --------------
int16_t displacementX = 0;
int16_t displacementY = 0;

/*********************************************** Common Functions *********************************************************************/

/*
 * Initializes and prints initial game state
 */
void game3_InitBoardState()
{
    game3_Player_t * player;
    game3_Food_t * food;
    point_t mappedCenter;
    uint16_t color;

    // Initialize the FIFOs to be used as snake bodies
    G8RTOS_InitFIFO(SN_FIFO_PL0);
    G8RTOS_InitFIFO(SN_FIFO_PL1);
    G8RTOS_InitFIFO(SN_FIFO_PL2);

    // Draw all the players on the map that are alive
    for (int i = 0; i < game3_numPlayers; i++)
    {
        player = &game3_HostToClient.players[i];

        if ( player->alive )
        {
            mapObjectToMe(&player->center, &mappedCenter);

            // determine what color the player's snake will be
            if ( i == 0 ) color = LCD_BLUE;
            else if ( i == 1 ) color = LCD_GREEN;
            else if ( i == 2 ) color = LCD_RED;
            else color = LCD_PURPLE;

            LCD_DrawRectangle(MAX_SCREEN_X / 2 - SN_SNAKE_SIZE / 2,
                              MAX_SCREEN_X / 2 + SN_SNAKE_SIZE / 2,
                              MAX_SCREEN_Y / 2 - SN_SNAKE_SIZE / 2,
                              MAX_SCREEN_Y / 2 + SN_SNAKE_SIZE / 2,
                              color);
        }
    }

    // Draw all food near players
    for (int i = 0; i < SN_MAX_FOOD_ON_MAP; i++)
    {
        food = &local_food[i];

        // UPDATE THE FOOD DRAWING IF IT IS WITHIN RANGE OF THE PLAYER
        if ( food->alive && !food->kill )
        {
            mapObjectToMe(&food->center, &mappedCenter);

            // only draw the food if it is within range after being mapped to the
            // player's center
            if (withinPlayerRange(&mappedCenter, &me->center))
            {
                LCD_DrawRectangle(mappedCenter.x - SN_FOOD_SIZE / 2,
                                  mappedCenter.x + SN_FOOD_SIZE / 2,
                                  mappedCenter.y - SN_FOOD_SIZE / 2,
                                  mappedCenter.y + SN_FOOD_SIZE / 2,
                                  SN_FOOD_COLOR);
            }
        }
    }
}

/*********************************************** Game Functions *********************************************************************/

void game3_addHostThreads()
{
    // Add background threads ------------------------------------
#ifndef SINGLE
    G8RTOS_AddThread(&game3_SendDataToClient, 20, 0xFFFFFFFF, "SEND_TO_CLIENT");
    G8RTOS_AddThread(&game3_ReceiveDataFromClient, 20, 0xFFFFFFFF, "RECEIVE_FROM_CLIENT");
#endif

    G8RTOS_AddThread(&game3_UpdateGamestateHost, 20, 0xFFFFFFFF, "GAMESTATE_UPDATE");
    G8RTOS_AddThread(&game3_ReadJoystickHost, 25, 0xFFFFFFFF, "JOYSTICK_HOST");
    G8RTOS_AddThread(&game3_DrawObjects, 20, 0xFFFFFFFF, "DRAW_OBJECTS");
    G8RTOS_AddThread(&common_IdleThread, 255, 0xFFFFFFFF, "IDLE");
}

void game3_addClientThreads()
{

}

// This function loops through all food structures and checks
// if the food center received is equal to the current food.
// If yes, generate a new center for that food and set the
// received gamestate center to an invalid center (-1, -1)
void game3_refreshFood()
{
    game3_Food_t * food;

    for (int i = 0; i < SN_MAX_FOOD_ON_MAP; i++)
    {
        food = &local_food[i];

        if ( food->center.x == game3_HostToClient.client.kill_food_center.x
                && food->center.y == game3_HostToClient.client.kill_food_center.y )
        {
            food->center.x = rand() % SN_MAP_MAX_X + 1;
            food->center.y = rand() % SN_MAP_MAX_Y + 1;
            food->kill = false;
            food->alive = true;
            break;  // only one piece of food can be eaten at a time
        }
    }
}

// This function checks if an object is within a predefined RANGE and returns
// true if it is
bool withinPlayerRange(point_t * objectCenter, point_t * myCenter)
{
    if ( abs(objectCenter->x - myCenter->x) <= SN_DRAW_RANGE_X
           && abs(objectCenter->y - myCenter->y) <= SN_DRAW_RANGE_Y )
    {
        return true;
    }

    else
        return false;
}

// This function maps objects in the map relative to the currently selected
// player "me" with "me" being centered on the LCD at 320/2, 240/2
void mapObjectToMe(point_t * objectCenter, point_t * mappedCenter)
{
    mappedCenter->x = (objectCenter->x - me->center.x + MAX_SCREEN_X / 2);
    mappedCenter->y = (objectCenter->y - me->center.y + MAX_SCREEN_X / 2);
}

/*********************************************** Client Threads *********************************************************************/
/*
 * Thread for client to join game
 */
void game3_JoinGame()
{

}

/*
 * Thread that receives game state packets from host
 */
void game3_ReceiveDataFromHost()
{

}

/*
 * Thread that sends UDP packets to host
 */
void game3_SendDataToHost()
{

}

/*
 * Thread to read client's joystick
 */
void game3_ReadJoystickClient()
{

}

/*
 * End of game for the client
 */
void game3_EndOfGameClient()
{

}

/*********************************************** Client Threads *********************************************************************/


/*********************************************** Host Threads *********************************************************************/
/*
 * Thread for the host to create a game
 */
void game3_CreateGame()
{
    srand( time(NULL) );
    LCD_Clear(LCD_GREEN);
    LCD_Text(8*10, 16*10, "Connecting to SNAKE clients..", LCD_WHITE);

    // packet initializations
    // Initialize the game number and previous game number to -1 so they
    // are INVALID game numbers so they will write on the first valid game number,
    // but they are also the same number, so it will not write over a corner.

    // initialize all clients and host info here..

    for (int i = 0; i < MAX_NUM_PLAYERS; i++)
    {
        game3_HostToClient.players[i].alive = false;
        game3_HostToClient.players[i].dir = UP;
        game3_HostToClient.players[i].kill = false;

        // UPDATE THIS TO BE RANDOM
        game3_HostToClient.players[i].center.x = SN_SNAKE_SIZE * (rand() % (SN_MAP_MAX_X/SN_SNAKE_SIZE)) + SN_MAP_MIN_X;
        game3_HostToClient.players[i].center.y = SN_SNAKE_SIZE * (rand() % (SN_MAP_MAX_Y/SN_SNAKE_SIZE)) + SN_MAP_MIN_Y;

        // INIT PREVIOUS PLAYER CENTERS TO INVALID DATA
        prevPlayers[i].center.x = -1;
        prevPlayers[i].center.y = -1;
    }

    my_prev = &prevPlayers[0];
    me = &game3_HostToClient.players[0];    // every player should define its "me" based on gamestate

    game3_HostToClient.players[0].alive = true;
    game3_numPlayers++; // counting the host as the first player

    // Initialize the food centers ----------------------------
    game3_Food_t * food;
    for (int i = 0; i < SN_MAX_FOOD_ON_MAP; i++)
    {
        food = &local_food[i];

        // SPAWNING NEAR A RANDOM PLAYER ONLY TO SAVE MEMORY
        // Choose a random ALIVE player to spawn new food near
        // while ( player->alive != true )
        // {
        //     player = &game3_HostToClient.players[rand() % MAX_NUM_PLAYERS];
        // }

        // food->center.x = -SN_FOOD_SPAWN_RANGE/2 + (rand() % SN_FOOD_SPAWN_RANGE + 1) + player->center.x;
        // food->center.y = -SN_FOOD_SPAWN_RANGE/2 + (rand() % SN_FOOD_SPAWN_RANGE + 1) + player->center.y;

        int16_t tempX;
        int16_t tempY;
        game3_Food_t * tempFoodPtr;
        game3_Player_t * tempPlayerPtr;

        // search for a center value that is not currently taken
        while ( 1 )
        {
            tempX = SN_FOOD_SIZE * (rand() % (SN_MAP_MAX_X/SN_FOOD_SIZE)) + SN_MAP_MIN_X;
            tempY = SN_FOOD_SIZE * (rand() % (SN_MAP_MAX_Y/SN_FOOD_SIZE)) + SN_MAP_MIN_Y;

            // make sure the center does not match any of the
            // current player centers
            for (int i = 0; i < MAX_NUM_PLAYERS; i++)
            {
                tempPlayerPtr = &game3_HostToClient.players[i];

                // if X is between any center X positions OR if Y is between any
                // center Y positions, regenerate
                if (    (tempX < tempPlayerPtr->center.x - SN_FOOD_SPAWN_RANGE / 2
                        && tempX > tempPlayerPtr->center.x + SN_FOOD_SPAWN_RANGE / 2)

                                                ||

                        (tempY < tempPlayerPtr->center.y - SN_FOOD_SPAWN_RANGE / 2
                        && tempY > tempPlayerPtr->center.y + SN_FOOD_SPAWN_RANGE / 2)   )
                {
                    continue; // don't spawn this iteration
                }
            }

            // make sure the center does not match any of the
            // current food centers
            for (int i = 0; i < SN_MAX_FOOD_ON_MAP; i++)
            {
                tempFoodPtr = &local_food[i];

                // if X is between any center X positions OR if Y is between any
                // center Y positions, regenerate
                if (    (tempX < tempFoodPtr->center.x - SN_FOOD_SPAWN_RANGE / 2
                        && tempX > tempFoodPtr->center.x + SN_FOOD_SPAWN_RANGE / 2)

                                                ||

                        (tempY < tempFoodPtr->center.y - SN_FOOD_SPAWN_RANGE / 2
                        && tempY > tempFoodPtr->center.y + SN_FOOD_SPAWN_RANGE / 2)   )
                {
                    continue; // don't spawn this iteration
                }
            }

            break; // exit the infinite loop because a valid food center has been found
        }

        // assign the new food values
        food->center.x = tempX;
        food->center.y = tempY;
        food->kill = false;
        food->alive = true;

        // SEND THE NEW CENTER FOOD DATA TO CLIENTS ***************************
    }

    RED_OFF;

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

    LCD_Clear(SN_BG_COLOR);
    RED_ON;

    // enables sending menu state information to the
    // client and calculating joystick information from host.
    game3_InitBoardState();
    game3_addHostThreads();

    G8RTOS_KillSelf();
}

/*
 *  This thread updates all gamestate components for host and client to make sure
 *  timing is the same for everything.
 */
void game3_UpdateGamestateHost()
{
    uint8_t updateJoystickCount = 0;

    // OTHER VARIABLES
    int16_t offsetX = 0;
    int16_t offsetY = -1;   // init to be moving upward

    while(1)
    {

        // INCREMENT COUNTERS HERE TO UPDATE GAMESTATE -------
        updateJoystickCount++;

        // HOST ADJUSTMENTS ----------------------------------
        if ( updateJoystickCount >= SN_UPDATE_PLAYER_POS )
        {

            // Player should only move one block at a time. Determine
            // which direction to move based on which joystick value is
            // highest.
            if ( abs(displacementX) > abs(displacementY) )
            {
                if ( displacementX >= 0 )   { offsetX = 1;    me->dir = RIGHT; }
                else                        { offsetX = -1;   me->dir = LEFT;  }

                offsetY = 0;
            }

            else if ( abs(displacementX) < abs(displacementY) )
            {
                if ( displacementY >= 0 )   { offsetY = 1; me->dir = DOWN; }
                else                        { offsetY = -1; me->dir = UP;  }

                offsetX = 0;
            }

            // update the player center by the snake head square size
            me->center.x += offsetX * SN_SNAKE_SIZE;
            me->center.y += offsetY * SN_SNAKE_SIZE;

            // Map boundaries to make sure the player does not travel too far
            if ( me->center.x >= SN_MAP_MAX_X - SN_SNAKE_SIZE / 2 )
            {
                me->center.x = SN_MAP_MAX_X - SN_SNAKE_SIZE / 2;
            }
            else if ( me->center.x <= SN_SNAKE_SIZE / 2 )
            {
                me->center.x = SN_SNAKE_SIZE / 2;
            }
            if ( me->center.y >= SN_MAP_MAX_Y - SN_SNAKE_SIZE / 2 )
            {
                me->center.y = SN_MAP_MAX_Y - SN_SNAKE_SIZE / 2;
            }
            else if ( me->center.y <= SN_SNAKE_SIZE / 2 )
            {
                me->center.y = SN_SNAKE_SIZE / 2;
            }

            updateJoystickCount = 0;
        }

        // CLIENT ADJUSTMENTS --------------------------------

        sleep(10);
    }
}

/*
 * Thread that sends game state to client
 */
void game3_SendDataToClient()
{
    while(1)
    {
        // Move the player in increments of the size of the snake
        me->center.x += displacementX * SN_SNAKE_SIZE;
        me->center.y += displacementY * SN_SNAKE_SIZE;

        sleep(5);
    }
}

/*
 * Thread that receives UDP packets from client
 */
void game3_ReceiveDataFromClient()
{
    while(1)
    {
        sleep(5);
    }
}

/*
 * Thread to read host's joystick
 */
void game3_ReadJoystickHost()
{
    int16_t avgX = 0;
    int16_t avgY = 0;
    int16_t joystick_x = 0;
    int16_t joystick_y = 0;

    while(1)
    {
        // moving average of the joystick inputs
        GetJoystickCoordinates(&joystick_x, &joystick_y);
        avgX = (avgX + joystick_x + JOYSTICK_BIAS_X) >> 1;
        avgY = (avgY + joystick_y + JOYSTICK_BIAS_Y) >> 1;

        // The switch statement was causing about 500 ms of lag
        displacementX = -(avgX >> 12);
        displacementY = (avgY >> 12);

        sleep(20);  // sleep before updating host's position to make it fair for client

    }
}

/*
 * End of game for the host
 */
void game3_EndOfGameHost()
{
    while(1)
    {
        sleep(5);
    }
}

/*********************************************** Host Threads *********************************************************************/


/*********************************************** Common Threads *********************************************************************/

/*
 * Thread to draw all the objects in the game
 *
 * DESCRIPTION:
 * This thread will loop through all objects in the gamestate and
 * check what the difference between
 */
void game3_DrawObjects()
{
    game3_Player_t * player;
    game3_Food_t * food;
    point_t mappedCenter;

    point_t startPoint;
    point_t endPoint;

    for ( int i = 0; i < SN_MAX_FOOD_ON_MAP; i++ )
    {
        prevFood[i].center.x = -1;
        prevFood[i].center.y = -1;
    }

    while(1)
    {
        /*
         * ======================================================
         * =                                                    =
         * =                   PLAYER UPDATES                   =
         * =                                                    =
         * ======================================================
         */
        for (int i = 0; i < MAX_NUM_PLAYERS; i++)
        {
            player = &game3_HostToClient.players[i];

            if ( player->alive )
            {
                // If this is another player in the map, update their position
                // relative the current player's center.
                if ( i != game3_ClientToHost.playerNumber )
                {

                }

                // If this is the current player, only update the player based
                // on the new  value.
                else
                {

                }
            }
        }

        /*
         * ======================================================
         * =                                                    =
         * =                    FOOD UPDATES                    =
         * =                                                    =
         * ======================================================
         */
        // loop through all food items -----------------
        // Draw all food near players
        for (int i = 0; i < SN_MAX_FOOD_ON_MAP; i++)
        {
            food = &local_food[i];

            // UPDATE THE FOOD DRAWING IF IT IS WITHIN RANGE OF THE PLAYER
            if ( food->alive && !food->kill )
            {
                mapObjectToMe(&food->center, &mappedCenter);

                // only draw the food if it is within range after being mapped to the
                // player's center
                if (withinPlayerRange(&mappedCenter, &me->center))
                {
                    // erase the old position if this is not an initialization
                    if ( prevFood[i].center.x != -1 )
                    {
                        LCD_DrawRectangle(prevFood[i].center.x - SN_FOOD_SIZE / 2,
                                          prevFood[i].center.x + SN_FOOD_SIZE / 2,
                                          prevFood[i].center.y - SN_FOOD_SIZE / 2,
                                          prevFood[i].center.y + SN_FOOD_SIZE / 2,
                                          SN_BG_COLOR);
                    }

                    // update the previous array
                    prevFood[i].center = mappedCenter;

                    // draw the new position
                    LCD_DrawRectangle(mappedCenter.x - SN_FOOD_SIZE / 2,
                                      mappedCenter.x + SN_FOOD_SIZE / 2,
                                      mappedCenter.y - SN_FOOD_SIZE / 2,
                                      mappedCenter.y + SN_FOOD_SIZE / 2,
                                      SN_FOOD_COLOR);
                }

                // if the food is not currently within range of the player, check
                // if the previous position was in the range. If it was within range,
                // make sure to erase that square.
                else if ( withinPlayerRange(&prevFood[i].center, &me->center) )
                {
                    LCD_DrawRectangle( prevFood[i].center.x - SN_FOOD_SIZE / 2,
                                       prevFood[i].center.x + SN_FOOD_SIZE / 2,
                                       prevFood[i].center.y - SN_FOOD_SIZE / 2,
                                       prevFood[i].center.y + SN_FOOD_SIZE / 2,
                                       SN_BG_COLOR);
                }
            }

            // // ERASE THE FOOD
            else if ( !food->alive && food->kill )
            {
                prevFood[i].center.x = -1;
                prevFood[i].center.y = -1;
                food->kill = false;

                LCD_DrawRectangle(mappedCenter.x - SN_FOOD_SIZE / 2,
                                  mappedCenter.x + SN_FOOD_SIZE / 2,
                                  mappedCenter.y - SN_FOOD_SIZE / 2,
                                  mappedCenter.y + SN_FOOD_SIZE / 2,
                                  SN_BG_COLOR);
            }

            // SPAWN NEW FOOD *****************************************8
            else if ( !food->alive && !food->kill )
            {
                // game3_refreshFood();
            }

        }

        /*
         * ======================================================
         * =                                                    =
         * =                 MAP BORDER UPDATES                 =
         * =                                                    =
         * ======================================================
         */

        // mapObjectToMe(&me->center, &mappedCenter);

        // LEFT BORDER UPDATE -----------------------------------
        // This center.x must be compared to the actual map
        // dimensions.
        if ( me->center.x < SN_DRAW_RANGE_X / 2 )
        {
            startPoint.x    = MAX_SCREEN_X / 2 - my_prev->center.x;
            startPoint.y    = MAX_SCREEN_Y / 2 - my_prev->center.y;
            endPoint.x      = MAX_SCREEN_X / 2 - my_prev->center.x;
            endPoint.y      = MAX_SCREEN_Y - 1 + my_prev->center.y;

            common_checkLCDBoundaries(&startPoint);
            common_checkLCDBoundaries(&endPoint);

            // erase the previous line
            LCD_DrawRectangle(startPoint.x,
                              endPoint.x,
                              startPoint.y,
                              endPoint.y,
                              LCD_WHITE);

            startPoint.x    = MAX_SCREEN_X / 2 - me->center.x;
            startPoint.y    = MAX_SCREEN_Y / 2 - me->center.y;
            endPoint.x      = MAX_SCREEN_X / 2 - me->center.x;
            endPoint.y      = MAX_SCREEN_Y / 2 - (MAX_SCREEN_Y - me->center.y);

            common_checkLCDBoundaries(&startPoint);
            common_checkLCDBoundaries(&endPoint);

            // write the new line
            LCD_DrawRectangle(startPoint.x,
                              endPoint.x,
                              startPoint.y,
                              endPoint.y,
                              LCD_BLACK);
        }

        // RIGHT BORDER UPDATE ----------------------------------

        // TOP BORDER UPDATE ------------------------------------

        // BOTTOM BORDER UPDATE ---------------------------------

        // reassign the local player's center to be used in border updates next cycle
        my_prev->center = me->center;

        sleep(20);
    }
}
