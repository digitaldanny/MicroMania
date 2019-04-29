#define MULTI

/*
 *  menu.c
 *
 *  Created     : 4/20/2019
 *  Last Edit   : 4/29/2019
 *
 *  UPDATES     :
 *  4/20/2019   : Snake head draws and map movement added
 *  4/24/2019   : Map movement stabilized and added boundaries to the map
 *  4/25/2019   : Added snake head animation drawing
 *  4/26/2019   : Block borders to map with clean erasing for snake at edges
 *  4/27/2019   : Snake linked list added so snake can grow and shrink
 *  4/28/2019   : Snake offset at border fixed. Clean erase bug fixed.
 *                initial Snake/Food drawing on both boards works.
 *  4/29/2019   : Finished multiplayer threads (send/receives)
 *
 *  DESCRIPTION : SLITHER.IO
 *
 *  TODO        :
 *  ~ Write the initializers for multiple clients and the handshake
 *  ~ Food should spawn near a random player
 *  ~ Update "me" for client players
 *  ~ Update withinPlayerRange function when ing is implemented
 *  ~ Possibly remove ExitMenu threads if they are never going to be used
 *  ~ Send new local food center data to client every time it is generated in create game
 */

#include "game3.h"

static uint8_t numPlayers;

// semaphores ----------------
semaphore_t CENTER_SEMAPHORE;

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

point_t mappedCenterStartDraw;
point_t mappedCenterEndDraw;
point_t mappedCenterStartErase;
point_t mappedCenterEndErase;

bool do_delete;
bool do_draw;

// variables used to map each part of the map to the current
// center of the snake head.
point_t mappedCenter;
point_t prevMappedCenter;
point_t mappedBodyCenter;
point_t prevMappedBodyCenter;

// left border init ----------------------------------------
point_t startPointLeft;             point_t endPointLeft;

// right border init ----------------------------------------
point_t startPointRight;             point_t endPointRight;

// top border init ----------------------------------------
point_t startPointTop;             point_t endPointTop;

// bottom border init ----------------------------------------
point_t startPointBottom;             point_t endPointBottom;

// other globals --------------
int16_t avgX = 0;
int16_t avgY = 0;

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

    LCD_Clear(SN_BG_COLOR);

    // Draw all the players on the map that are alive
    for (int i = 0; i < MAX_NUM_PLAYERS; i++)
    {

        player = &game3_HostToClient.players[i];

        if ( player->alive )
        {
            mapObjectToMe(&player->center, &mappedCenter);

            // determine what color the player's snake will be
            if ( i == 0 ) color = SN_PLAYER1_COLOR;
            else if ( i == 1 ) color = SN_PLAYER2_COLOR;
            else if ( i == 2 ) color = SN_PLAYER3_COLOR;
            else color = LCD_PURPLE;

            if (withinPlayerRange(&mappedCenter))
            {
                game3_drawSnakeHead(prevPlayers[i].dir,
                                    player->dir,
                                    mappedCenter.x,
                                    mappedCenter.y,
                                    player->animation_count,
                                    color);
            }
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
            if (withinPlayerRange(&mappedCenter))
            {
                LCD_DrawRectangle(mappedCenter.x - SN_FOOD_SIZE / 2,
                                  mappedCenter.x + SN_FOOD_SIZE / 2,
                                  mappedCenter.y - SN_FOOD_SIZE / 2,
                                  mappedCenter.y + SN_FOOD_SIZE / 2,
                                  SN_FOOD_COLOR);
            }
        }
    }

    // INITIALIZE THE GAME BORDERS --------------------------------------------
    // Draw left border ----------------------
    mapObjectToMe(&endPointLeft, &mappedCenterEndDraw);
    common_checkLCDBoundaries(&mappedCenterEndDraw);

    LCD_DrawRectangle(0,
                      mappedCenterEndDraw.x,
                      0,
                      MAX_SCREEN_Y - 1,
                      LCD_BLACK);

    // Draw right border ----------------------
    mapObjectToMe(&endPointRight, &mappedCenterEndDraw);
    common_checkLCDBoundaries(&mappedCenterEndDraw);

    LCD_DrawRectangle(mappedCenterEndDraw.x,
                      MAX_SCREEN_X - 1,
                      0,
                      MAX_SCREEN_Y - 1,
                      LCD_BLACK);

    // Draw top border ------------------------
    mapObjectToMe(&endPointTop, &mappedCenterEndDraw);
    common_checkLCDBoundaries(&mappedCenterEndDraw);

    LCD_DrawRectangle(0,
                      MAX_SCREEN_X - 1,
                      0,
                      mappedCenterEndDraw.y,
                      LCD_BLACK);

    // Draw bottom border ---------------------
    mapObjectToMe(&endPointBottom, &mappedCenterEndDraw);
    common_checkLCDBoundaries(&mappedCenterEndDraw);

    LCD_DrawRectangle(0,
                      MAX_SCREEN_X - 1,
                      mappedCenterEndDraw.y,
                      MAX_SCREEN_Y - 1,
                      LCD_BLACK);
}

/*********************************************** Game Functions *********************************************************************/

void game3_addHostThreads()
{
#ifndef SINGLE
    G8RTOS_AddThread(&game3_SendDataToClient, 20, 0xFFFFFFFF, "SEND_TO_CLIENT");
    G8RTOS_AddThread(&game3_ReceiveDataFromClient, 20, 0xFFFFFFFF, "RECEIVE_FROM_CLIENT");
#endif

    G8RTOS_AddThread(&game3_UpdateGamestateHost, 20, 0xFFFFFFFF, "GAMESTATE_UPDATE");
    G8RTOS_AddThread(&game3_ReadJoystickHost, 25, 0xFFFFFFFF, "JOYSTICK_HOST");
    G8RTOS_AddThread(&game3_DrawObjects, 22, 0xFFFFFFFF, "DRAW_OBJECTS");
    G8RTOS_AddThread(&common_IdleThread, 255, 0xFFFFFFFF, "IDLE");
}

void game3_addClientThreads()
{
    G8RTOS_AddThread(&game3_SendDataToHost, 20, 0xFFFFFFFF, "SEND_TO_HOST");
    G8RTOS_AddThread(&game3_ReceiveDataFromHost, 20, 0xFFFFFFFF, "RECEIVE_FROM_HOST");
    G8RTOS_AddThread(&game3_ReadJoystickClient, 25, 0xFFFFFFFF, "JOYSTICK_CLIENT");
    G8RTOS_AddThread(&game3_DrawObjects, 22, 0xFFFFFFFF, "DRAW_OBJECTS");
    G8RTOS_AddThread(&common_IdleThread, 255, 0xFFFFFFFF, "IDLE");
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
            food->center.x = rand() % (SN_MAP_MAX_X + SN_FOOD_SIZE/2 + 1);
            food->center.y = rand() % (SN_MAP_MAX_Y + SN_FOOD_SIZE/2 + 1);
            food->kill = false;
            food->alive = true;
            break;  // only one piece of food can be eaten at a time
        }
    }
}

// This function checks if an object is within a predefined RANGE and returns
// true if it is
bool withinPlayerRange(point_t * objectCenter)
{
    if (        objectCenter->x <= SN_DRAW_RANGE_X
           &&   objectCenter->x > MIN_SCREEN_X
           &&   objectCenter->y <= SN_DRAW_RANGE_Y
           &&   objectCenter->y > MIN_SCREEN_Y       )
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
    mappedCenter->y = (objectCenter->y - me->center.y + MAX_SCREEN_Y / 2);
}

// This function does the same as mapObjectToMe above except it maps it
// to the "my_prev" pointer instead of the "me" pointer.
void mapObjectToPrev(int8_t num, point_t * objectCenter, point_t * mappedCenter)
{
    point_t center;

    // this if branch is used to handle errors where the LCD draws the borders
    // at different cycles where the my->center changes and the border cannot
    // erase the correct border line.
    if ( num == 1 )
        center = my_prev->er_center1;
    else if ( num == 2 )
        center = my_prev->er_center2;
    else if ( num == 3 )
        center = my_prev->er_center3;
    else if ( num == 4 )
        center = my_prev->er_center4;
    else
        center = my_prev->center;

    mappedCenter->x = (objectCenter->x - center.x + MAX_SCREEN_X / 2);
    mappedCenter->y = (objectCenter->y - center.y + MAX_SCREEN_Y / 2);
}

// This function draws the snake head animation with an increment value
// that should be sent between gamestates. This function will only redraw
// for every frame if drawing for the enemy player.
void game3_drawSnakeHead(dir_t prevDir, dir_t dir, int16_t x, int16_t y, int8_t count, int16_t color)
{
    int16_t eyeX_left;
    int16_t eyeY_left;
    int16_t eyeX_right;
    int16_t eyeY_right;

    int16_t pupilX_left;
    int16_t pupilY_left;
    int16_t pupilX_right;
    int16_t pupilY_right;

    int16_t frameNum = 0;

    bool isMe = x == MAX_SCREEN_X / 2 && y == MAX_SCREEN_Y / 2;

    // only redraw the snake image if the direction has changed
    // OR if the animation needs to play OR if drawing an enemy player
    if ( prevDir != dir || !(count > 0 && count < SN_SNAKE_FRAME_1) || !isMe )
    {
        // Base box
        LCD_DrawRectangle(x - SN_SNAKE_SIZE / 2,
                          x + SN_SNAKE_SIZE / 2,
                          y - SN_SNAKE_SIZE / 2,
                          y + SN_SNAKE_SIZE / 2,
                          color);

        // Rotation handling
        if ( dir == UP )
        {
            eyeX_left = -2;     // X
            eyeY_left = -2;     // X
            eyeX_right = 2;     // X
            eyeY_right = -2;    // X

            pupilX_left = -1;   // X
            pupilY_left = -3;   // X
            pupilX_right = 1;   // X
            pupilY_right = -3;  // X
        }
        else if ( dir == DOWN )
        {
            eyeX_left = -2;     // X
            eyeY_left = 2;      // X
            eyeX_right = 2;     // X
            eyeY_right = 2;     // X

            pupilX_left = -1;   // X
            pupilY_left = 3;    // X
            pupilX_right = 1;   // X
            pupilY_right = 3;   // X
        }
        else if ( dir == RIGHT )
        {
            eyeX_left = 2;      // X
            eyeY_left = -2;     // X
            eyeX_right = 2;     // X
            eyeY_right = 2;     // X

            pupilX_left = 3;    // X
            pupilY_left = -1;   // X
            pupilX_right = 3;   // X
            pupilY_right = 1;   // X
        }
        else if ( dir == LEFT )
        {
            eyeX_left = -2;      // X
            eyeY_left = -2;     // X
            eyeX_right = -2;     // X
            eyeY_right = 2;     // X

            pupilX_left = -3;    // X
            pupilY_left = -1;   // X
            pupilX_right = -3;   // X
            pupilY_right = 1;   // X
        }

        // animation frame number handling
        if ( count < SN_SNAKE_FRAME_0 )         frameNum = 0;
        else if ( count < SN_SNAKE_FRAME_1 )    frameNum = 1;
        else if ( count < SN_SNAKE_FRAME_2 )    frameNum = 2;
        else if ( count < SN_SNAKE_FRAME_3 )    frameNum = 3;
        else if ( count < SN_SNAKE_FRAME_4 )    frameNum = 4;
        else if ( count < SN_SNAKE_FRAME_5 )    frameNum = 3;
        else if ( count < SN_SNAKE_FRAME_6 )    frameNum = 2;
        else if ( count < SN_SNAKE_FRAME_7 )    frameNum = 1;
        else                                    frameNum = 0;

        // left eye
        LCD_fillCircle(x + eyeX_left, y + eyeY_left, 2, LCD_WHITE);
        LCD_fillCircle(x + pupilX_left, y + pupilY_left, 1, LCD_BLACK);

        // right eye
        LCD_fillCircle(x + eyeX_right, y + eyeY_right, 2, LCD_WHITE);
        LCD_fillCircle(x + pupilX_right, y + pupilY_right, 1, LCD_BLACK);

        // Eye closing animation
        if ( (dir == DOWN) && frameNum > 0 )
        {
            LCD_DrawRectangle(x - SN_SNAKE_SIZE / 2,
                              x + SN_SNAKE_SIZE / 2,
                              y,
                              y + frameNum,
                              color);
        }
        else if ( (dir == UP) && frameNum > 0 )
        {
            LCD_DrawRectangle(x - SN_SNAKE_SIZE / 2,
                              x + SN_SNAKE_SIZE / 2,
                              y - frameNum,
                              y,
                              color);
        }
        else if ( (dir == RIGHT) && frameNum > 0 )
        {
            LCD_DrawRectangle(x,
                              x + frameNum,
                              y - SN_SNAKE_SIZE / 2,
                              y + SN_SNAKE_SIZE / 2,
                              color);
        }
        else if ( (dir == LEFT) && frameNum > 0 )
        {
            LCD_DrawRectangle(x - frameNum,
                              x,
                              y - SN_SNAKE_SIZE / 2,
                              y + SN_SNAKE_SIZE / 2,
                              color);
        }
    }
}

// This function draws all the map borders based on the size of the
// predefined max and min sizes
void game3_updateBorders()
{
    for (int ii = 0; ii < 4; ii++)
    {
        // LEFT BORDER --------------------------------
        if ( ii == 0 )
        {
            mapObjectToPrev(1, &startPointLeft, &mappedCenterStartErase);
            mapObjectToPrev(1, &endPointLeft, &mappedCenterEndErase);

            common_checkLCDBoundaries(&mappedCenterStartErase);
            common_checkLCDBoundaries(&mappedCenterEndErase);

            mapObjectToMe(&startPointLeft, &mappedCenterStartDraw);
            mapObjectToMe(&endPointLeft, &mappedCenterEndDraw);

            common_checkLCDBoundaries(&mappedCenterStartDraw);
            common_checkLCDBoundaries(&mappedCenterEndDraw);

            my_prev->er_center1 = me->center;

            // DRAW BLOCKS OF NEW BORDER ------------------------------
            // erase the previous left border line --------------------
            if ( me->dir == RIGHT )
            {
                LCD_DrawRectangle(mappedCenterStartDraw.x,
                                  mappedCenterEndErase.x,
                                  mappedCenterStartErase.y,
                                  mappedCenterEndErase.y,
                                  LCD_WHITE);
            }

            // write the new left border line --------------------------
            else if ( me->dir == LEFT )
            {
                LCD_DrawRectangle(mappedCenterStartErase.x,
                                  mappedCenterEndDraw.x,
                                  mappedCenterStartDraw.y,
                                  mappedCenterEndDraw.y,
                                  LCD_BLACK);
            }
        }

        // RIGHT BORDER UPDATE ----------------------------------
        else if ( ii == 1 )
        {
            mapObjectToPrev(2, &startPointRight, &mappedCenterStartErase);
            mapObjectToPrev(2, &endPointRight, &mappedCenterEndErase);

            common_checkLCDBoundaries(&mappedCenterStartErase);
            common_checkLCDBoundaries(&mappedCenterEndErase);

            mapObjectToMe(&startPointRight, &mappedCenterStartDraw);
            mapObjectToMe(&endPointRight, &mappedCenterEndDraw);

            common_checkLCDBoundaries(&mappedCenterStartDraw);
            common_checkLCDBoundaries(&mappedCenterEndDraw);

            my_prev->er_center2 = me->center;

            // DRAW BLOCKS OF NEW BORDER ------------------------------
            // erase the previous left border line --------------------
            if ( me->dir == LEFT )
            {
                LCD_DrawRectangle(mappedCenterStartErase.x,
                                  mappedCenterEndDraw.x,
                                  mappedCenterStartErase.y,
                                  mappedCenterEndErase.y,
                                  LCD_WHITE);
            }

            // write the new left border line --------------------------
            else if ( me->dir == RIGHT )
            {
                LCD_DrawRectangle(mappedCenterStartDraw.x,
                                  mappedCenterEndErase.x,
                                  mappedCenterStartDraw.y,
                                  mappedCenterEndDraw.y,
                                  LCD_BLACK);
            }
        }

        // TOP BORDER UPDATE ------------------------------------
        else if ( ii == 2 )
        {
            mapObjectToPrev(3, &startPointTop, &mappedCenterStartErase);
            mapObjectToPrev(3, &endPointTop, &mappedCenterEndErase);

            common_checkLCDBoundaries(&mappedCenterStartErase);
            common_checkLCDBoundaries(&mappedCenterEndErase);

            mapObjectToMe(&startPointTop, &mappedCenterStartDraw);
            mapObjectToMe(&endPointTop, &mappedCenterEndDraw);

            common_checkLCDBoundaries(&mappedCenterStartDraw);
            common_checkLCDBoundaries(&mappedCenterEndDraw);

            my_prev->er_center3 = me->center;

            // DRAW BLOCKS OF NEW BORDER ------------------------------
            // erase the previous left border line --------------------
            if ( me->dir == DOWN )
            {
                LCD_DrawRectangle(mappedCenterStartDraw.x,
                                  mappedCenterEndErase.x,
                                  mappedCenterStartDraw.y,
                                  mappedCenterEndErase.y,
                                  LCD_WHITE);
            }

            // write the new left border line --------------------------
            else if ( me->dir == UP )
            {
                LCD_DrawRectangle(mappedCenterStartDraw.x,
                                  mappedCenterEndDraw.x,
                                  mappedCenterStartErase.y,
                                  mappedCenterEndDraw.y,
                                  LCD_BLACK);
            }
        }

        // BOTTOM BORDER UPDATE ---------------------------------
        else if ( ii == 3 )
        {
            mapObjectToPrev(4, &startPointBottom, &mappedCenterStartErase);
            mapObjectToPrev(4, &endPointBottom, &mappedCenterEndErase);

            common_checkLCDBoundaries(&mappedCenterStartErase);
            common_checkLCDBoundaries(&mappedCenterEndErase);

            mapObjectToMe(&startPointBottom, &mappedCenterStartDraw);
            mapObjectToMe(&endPointBottom, &mappedCenterEndDraw);

            common_checkLCDBoundaries(&mappedCenterStartDraw);
            common_checkLCDBoundaries(&mappedCenterEndDraw);

            my_prev->er_center4 = me->center;

            // DRAW BLOCKS OF NEW BORDER ------------------------------
            // write the new left border line --------------------------
            if ( me->dir == UP )
            {
                LCD_DrawRectangle(mappedCenterStartDraw.x,
                                  mappedCenterEndDraw.x,
                                  mappedCenterStartErase.y,
                                  mappedCenterEndDraw.y,
                                  LCD_WHITE);
            }

            // erase the previous left border line --------------------
            else if ( me->dir == DOWN )
            {
                LCD_DrawRectangle(mappedCenterStartDraw.x,
                                  mappedCenterEndDraw.x,
                                  mappedCenterStartDraw.y,
                                  mappedCenterEndErase.y,
                                  LCD_BLACK);
            }
        }
    }
}

/*********************************************** Client Threads *********************************************************************/
/*
 * Thread for client to join game
 */
void game3_JoinGame()
{
    // 1. Set initial SpecificPlayerInfo_t strict attributes ( getLocalIP() ).
    game3_ClientToHost.IP_address = getLocalIP();
    game3_ClientToHost.acknowledge = false;
    game3_ClientToHost.joined = false;
    game3_ClientToHost.playerNumber = 1;

    srand( time(NULL) );
    LCD_Clear(LCD_GREEN);
    LCD_Text(8*6, 16*6, "Connecting to SLITHER host..", LCD_WHITE);

    G8RTOS_InitSemaphore(&CENTER_SEMAPHORE, 1);
    G8RTOS_InitSemaphore(&CC3100_SEMAPHORE, 1);
    G8RTOS_InitSemaphore(&LCDREADY, 1);

    common_buttons_init();

    // INITIALIZE BORDERS TO BE DRAWN IN THE INIT FUNCTION
    // left border init ----------------------------------------
    startPointLeft.x = SN_MAP_MIN_X;    startPointLeft.y = SN_MAP_MIN_Y;
    endPointLeft.x = SN_MAP_MIN_X;      endPointLeft.y = SN_MAP_MAX_Y;

    // right border init ----------------------------------------
    startPointRight.x = SN_MAP_MAX_X;    startPointRight.y = SN_MAP_MIN_Y;
    endPointRight.x = SN_MAP_MAX_X;      endPointRight.y = SN_MAP_MAX_Y;

    // top border init ----------------------------------------
    startPointTop.x = SN_MAP_MIN_X;    startPointTop.y = SN_MAP_MIN_Y;
    endPointTop.x = SN_MAP_MAX_X;      endPointTop.y = SN_MAP_MIN_Y;

    // bottom border init ----------------------------------------
    startPointBottom.x = SN_MAP_MIN_X;    startPointBottom.y = SN_MAP_MAX_Y;
    endPointBottom.x = SN_MAP_MAX_X;      endPointBottom.y = SN_MAP_MAX_Y;

    do_delete = false;
    do_draw = false;

    // initialize the snake linked lists locally
    for (int i = 0; i < MAX_NUM_PLAYERS; i++)
    {
        // initialize the snakes for each function
        game3_initSnake(&game3_HostToClient.players[i].center, i);
    }

    my_prev = &prevPlayers[1];
    me = &game3_HostToClient.players[1];    // every player should define its "me" based on gamestate
    game3_numPlayers++; // counting the host as the first player

    BLUE_OFF;

    // wait for the host to receive message and notify
    // client that they joined the game.
    do
    {
        SendData(       (uint8_t*)&game3_ClientToHost, HOST_IP_ADDR, sizeof(game3_ClientToHost) );   // start handshake
        ReceiveData(    (uint8_t*)&game3_HostToClient, sizeof(game3_HostToClient) );   // check if host acknowledges
    } while( game3_HostToClient.client.joined == false );

    // 4. Acknowledge client to tell them they have received
    // the message about joining the game and the game can begin.
    game3_ClientToHost.acknowledge = true;
    SendData( (uint8_t*)&game3_ClientToHost, HOST_IP_ADDR, sizeof(game3_ClientToHost) );

    // 4. If you've joined the game, acknowledge you've joined to the host
    //      and show connection through LED.
    BLUE_ON;

    // 5. Initialize board state, semaphores, and add the following threads..
    //      ReadJoystickClient, SendDataToHost, ReceiveDataFromHost, DrawObjects,
    //      MoveLEDs, Idle
    game3_InitBoardState();
    game3_addClientThreads();

    // 6. Kill self.
    G8RTOS_KillSelf();
}

/*
 * Thread that receives game state packets from host
 */
void game3_ReceiveDataFromHost()
{
    while(1)
    {
        // Receive packet from the host
        G8RTOS_WaitSemaphore(&CC3100_SEMAPHORE);
        ReceiveData( (_u8*)&game3_HostToClient, sizeof(game3_HostToClient));
        G8RTOS_SignalSemaphore(&CC3100_SEMAPHORE);

        // 3. Check if the game is done. Add EndOfGameHost thread if done.
        // if ( game3_HostToClient.choice_made == true )
        //     G8RTOS_AddThread(ExitMenuClient, 0, 0xFFFFFFFF, "END_CLIENT_");

        sleep(2);
    }
}

/*
 * Thread that sends UDP packets to host
 */
void game3_SendDataToHost()
{
    while(1)
    {
        sleep(5);
    }
}

/*
 * Thread to read client's joystick
 */
void game3_ReadJoystickClient()
{
    while(1)
    {
        sleep(10);
    }
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
    LCD_Text(8*8, 16*6, "Connecting to SNAKE clients..", LCD_WHITE);

    G8RTOS_InitSemaphore(&CENTER_SEMAPHORE, 1);
    G8RTOS_InitSemaphore(&CC3100_SEMAPHORE, 1);
    G8RTOS_InitSemaphore(&LCDREADY, 1);

    common_buttons_init();

    // INITIALIZE BORDERS TO BE DRAWN IN THE INIT FUNCTION
    // left border init ----------------------------------------
    startPointLeft.x = SN_MAP_MIN_X - SN_SNAKE_SIZE / 2;    startPointLeft.y = SN_MAP_MIN_Y - SN_SNAKE_SIZE / 2;
    endPointLeft.x = SN_MAP_MIN_X - SN_SNAKE_SIZE / 2;      endPointLeft.y = SN_MAP_MAX_Y + SN_SNAKE_SIZE / 2;

    // right border init ----------------------------------------
    startPointRight.x = SN_MAP_MAX_X + SN_SNAKE_SIZE / 2;    startPointRight.y = SN_MAP_MIN_Y - SN_SNAKE_SIZE / 2;
    endPointRight.x = SN_MAP_MAX_X + SN_SNAKE_SIZE / 2;      endPointRight.y = SN_MAP_MAX_Y + SN_SNAKE_SIZE / 2;

    // top border init ----------------------------------------
    startPointTop.x = SN_MAP_MIN_X - SN_SNAKE_SIZE / 2;    startPointTop.y = SN_MAP_MIN_Y - SN_SNAKE_SIZE / 2;
    endPointTop.x = SN_MAP_MAX_X + SN_SNAKE_SIZE / 2;      endPointTop.y = SN_MAP_MIN_Y - SN_SNAKE_SIZE / 2;

    // bottom border init ----------------------------------------
    startPointBottom.x = SN_MAP_MIN_X - SN_SNAKE_SIZE / 2;    startPointBottom.y = SN_MAP_MAX_Y + SN_SNAKE_SIZE / 2;
    endPointBottom.x = SN_MAP_MAX_X + SN_SNAKE_SIZE / 2;      endPointBottom.y = SN_MAP_MAX_Y + SN_SNAKE_SIZE / 2;

    do_delete = false;
    do_draw = false;

    // packet initializations
    // Initialize the game number and previous game number to -1 so they
    // are INVALID game numbers so they will write on the first valid game number,
    // but they are also the same number, so it will not write over a corner.

    // initialize all clients and host info here..

    for (int i = 0; i < MAX_NUM_PLAYERS; i++)
    {
        game3_HostToClient.players[i].alive = true;
        game3_HostToClient.players[i].dir = UP;
        game3_HostToClient.players[i].kill = false;
        game3_HostToClient.players[i].animation_count = 0;

        // UPDATE THIS TO BE RANDOM
        game3_HostToClient.players[i].center.x = SN_SNAKE_SIZE * (rand() % ((SN_MAP_MAX_X + 1)/SN_SNAKE_SIZE)) + SN_MAP_MIN_X ;
        game3_HostToClient.players[i].center.y = SN_SNAKE_SIZE * (rand() % ((SN_MAP_MAX_Y + 1)/SN_SNAKE_SIZE)) + SN_MAP_MIN_Y ;

        // Initialize previous center data to be different than the current center
        // so the borders are initialized approximately correct
        prevPlayers[i].center.x = game3_HostToClient.players[i].center.x - 1;
        prevPlayers[i].center.y = game3_HostToClient.players[i].center.y - 1;
        prevPlayers[i].dir = DOWN;

        // initialize the snakes for each function
        game3_initSnake(&game3_HostToClient.players[i].center, i);
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

        int16_t tempX;
        int16_t tempY;
        game3_Food_t * tempFoodPtr;
        game3_Player_t * tempPlayerPtr;

        // search for a center value that is not currently taken
        while ( 1 )
        {
            tempX = SN_FOOD_SIZE * (rand() % ((SN_MAP_MAX_X - SN_FOOD_SIZE)/SN_FOOD_SIZE)) + SN_MAP_MIN_X + SN_FOOD_SIZE;
            tempY = SN_FOOD_SIZE * (rand() % ((SN_MAP_MAX_Y - SN_FOOD_SIZE)/SN_FOOD_SIZE)) + SN_MAP_MIN_Y + SN_FOOD_SIZE;

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
    while( ReceiveData((_u8*)&game3_HostToClient.client, sizeof(game3_HostToClient.client)) < 0 );

    // 4. Acknowledge client to tell them they joined the game.
    game3_HostToClient.client.joined = true;
    game3_HostToClient.client.playerNumber = numPlayers;
    SendData( (_u8*)&game3_HostToClient, game3_HostToClient.client.IP_address, sizeof(game3_HostToClient) );

    // Wait for client to sync with host by acknowledging that
    // it received the host message.
    do
    {
        ReceiveData((uint8_t*)&game3_HostToClient.client, sizeof(game3_HostToClient.client));
    } while ( game3_HostToClient.client.acknowledge == false );
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

        G8RTOS_WaitSemaphore(&CENTER_SEMAPHORE);

        // HOST ADJUSTMENTS ----------------------------------
        if ( updateJoystickCount >= SN_UPDATE_PLAYER_POS )
        {

            // Player should only move one block at a time. Determine
            // which direction to move based on which joystick value is
            // highest.
            if ( abs(avgX) > abs(avgY) && abs(avgX) > 1000 )
            {
                if ( avgX >= 0 )   { offsetX = 1;    me->dir = LEFT; }
                else               { offsetX = -1;   me->dir = RIGHT;  }

                offsetY = 0;
            }

            else if ( abs(avgX) < abs(avgY) && abs(avgY) > 1000 )
            {
                if ( avgY >= 0 )   { offsetY = 1; me->dir = DOWN; }
                else               { offsetY = -1; me->dir = UP;  }

                offsetX = 0;
            }

            // update the player center by the snake head square size
            me->center.x -= offsetX * SN_SNAKE_SIZE;
            me->center.y += offsetY * SN_SNAKE_SIZE;

            // Map boundaries to make sure the player does not travel too far
            if ( me->center.x >= SN_MAP_MAX_X) // - SN_SNAKE_SIZE / 2)
            {
                me->center.x = SN_MAP_MAX_X; // - SN_SNAKE_SIZE / 2;
            }
            else if ( me->center.x <= SN_MAP_MIN_X ) //SN_SNAKE_SIZE / 2)
            {
                me->center.x = SN_MAP_MIN_X; //SN_SNAKE_SIZE / 2;
            }
            if ( me->center.y >= SN_MAP_MAX_Y ) // - SN_SNAKE_SIZE / 2)
            {
                me->center.y = SN_MAP_MAX_Y; // - SN_SNAKE_SIZE / 2;
            }
            else if ( me->center.y <= SN_MAP_MIN_Y ) // SN_SNAKE_SIZE / 2)
            {
                me->center.y = SN_MAP_MIN_Y; // SN_SNAKE_SIZE / 2;
            }

            // DEBUGGGGGGGGGGGG ****************************************************
            game3_HostToClient.players[0].size_up = true;

            // add the new center value to the player snake and remove
            // the old tail to be drawn by the draw objects function if
            // the snake is not supposed to size up OR if the max snake
            // length has already been reached.
            if ( !game3_HostToClient.players[0].size_up || game3_limitReached(0) )
            {
                G8RTOS_WaitSemaphore(&LCDREADY);
                point_t eraseCenter;

                point_t erasePoint = game3_rmSnakeTail(0);
                mapObjectToPrev(1, &erasePoint, &eraseCenter);

                // Erase the previous tail here if the tail is deleted..
                if (withinPlayerRange(&eraseCenter))
                {
                    LCD_DrawRectangle( eraseCenter.x - SN_SNAKE_SIZE / 2,
                                       eraseCenter.x + SN_SNAKE_SIZE / 2,
                                       eraseCenter.y - SN_SNAKE_SIZE / 2,
                                       eraseCenter.y + SN_SNAKE_SIZE / 2,
                                       SN_BG_COLOR);
                }

                G8RTOS_SignalSemaphore(&LCDREADY);
            }

            game3_addSnakeHead(&me->center, 0);

            updateJoystickCount = 0;
        }

        G8RTOS_SignalSemaphore(&CENTER_SEMAPHORE);

        // CLIENT ADJUSTMENTS --------------------------------

        sleep(15);
    }
}

/*
 * Thread that sends game state to client
 */
void game3_SendDataToClient()
{
    while (1)
    {
        G8RTOS_WaitSemaphore(&CC3100_SEMAPHORE);
        SendData( (uint8_t*)&game3_HostToClient, game3_HostToClient.client.IP_address, sizeof(game3_HostToClient) );
        G8RTOS_SignalSemaphore(&CC3100_SEMAPHORE);

        // Check if the game choice has been made. If yes, add those threads
        // if ( game3_HostToClient.choice_made == true )
        //     G8RTOS_AddThread(&ExitMenuHost, 0, 0xFFFFFFFF, "END_HOST");

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
    int16_t joystick_x = 0;
    int16_t joystick_y = 0;

    while(1)
    {
        // moving average of the joystick inputs
        GetJoystickCoordinates(&joystick_x, &joystick_y);
        avgX = (avgX + joystick_x + JOYSTICK_BIAS_X) >> 1;
        avgY = (avgY + joystick_y + JOYSTICK_BIAS_Y) >> 1;

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
    prev_player_t * prevPlayer;
    game3_Food_t * food;

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
        G8RTOS_WaitSemaphore(&CENTER_SEMAPHORE);
        for (int i = 0; i < MAX_NUM_PLAYERS; i++)
        {

            player = &game3_HostToClient.players[i];
            prevPlayer = &prevPlayers[i];

            if ( player->alive )
            {
                // Update each player based on their center relative to "me" center
                mapObjectToMe(&player->center, &mappedCenter);
                mapObjectToPrev(0, &prevPlayer->center, &prevMappedCenter);

                // erase player's previous head position, only used for the other
                // player's snake. If playing as host, the host's head will not ever
                // be redraw because it is always on the center of the map. The enemy
                // player's snake head WILL be redrawn because it should always be changing
                // position.

                if ( withinPlayerRange(&prevMappedCenter)
                        && !(player->center.x == me->center.x && player->center.y == me->center.y))
                {
                    G8RTOS_WaitSemaphore(&LCDREADY);
                    LCD_DrawRectangle(prevMappedCenter.x - SN_SNAKE_SIZE / 2,
                                      prevMappedCenter.x + SN_SNAKE_SIZE / 2,
                                      prevMappedCenter.y - SN_SNAKE_SIZE / 2,
                                      prevMappedCenter.y + SN_SNAKE_SIZE / 2,
                                      LCD_YELLOW); // SN_BG_COLOR);
                    G8RTOS_SignalSemaphore(&LCDREADY);
                }

                if ( withinPlayerRange(&mappedCenter) )
                {
                    // determine the snake color based on the player number
                    int16_t color;
                    if ( i == 0 )       color = SN_PLAYER1_COLOR;
                    else if ( i == 1 )  color = SN_PLAYER2_COLOR;
                    else                color = SN_PLAYER3_COLOR;

                    // play the next frame of the snake head animation
                    point_t snakeBodyCenter;
                    if ( mappedCenter.x > 0 && mappedCenter.y > 0 )
                    {
                        G8RTOS_WaitSemaphore(&LCDREADY);
                        game3_drawSnakeHead(prevPlayer->dir,
                                            player->dir,
                                            mappedCenter.x,
                                            mappedCenter.y,
                                            player->animation_count,
                                            color);
                        G8RTOS_SignalSemaphore(&LCDREADY);

                        // update each part of the snake's body relative
                        // to the current snake head
                        for (int j = 0; j < game3_snakeLength(i); j++)
                        {
                            // always skip the head
                            if ( j == 0 )
                                continue;

                            // erase the previous snake body information
                            // if the player has moved
                            snakeBodyCenter = game3_snakeAt(j, i);
                            mapObjectToPrev(0, &snakeBodyCenter, &prevMappedBodyCenter);

                            // draw the new snake body information if the player
                            mapObjectToMe(&snakeBodyCenter, &mappedBodyCenter);

                            // ALGORITHM TO REDRAW REQUIRED BLOCKS GOES HERE!!!
                            // ************************************************
                            do_delete = true;
                            do_draw = true;

                            // read the currently mapped LCD value..
                            // if the color matches the player color, do not
                            // erase OR redraw
                            // if ( player->dir == LEFT )
                            // {
                            //     if ( LCD_GetPoint(mappedBodyCenter.x, mappedBodyCenter.y) == color)
                            //     {
                            //         do_draw = false;
                            //
                            //         if ( game3_compAt(j, i, PREV, Y) )
                            //             do_delete = false;
                            //     }
                            // }

                            // Read the mapped center value
                            // ************************************************
                            // ALGORITHM TO REDRAW REQUIRED BLOCKS GOES HERE!!!

                            // Only erase the snake body if the center data is
                            // valid information ( NOT -500, -500 )
                            if ( withinPlayerRange(&prevMappedBodyCenter)
                                    && !(mappedCenter.x == prevMappedBodyCenter.x && mappedCenter.y == prevMappedBodyCenter.y)
                                    && snakeBodyCenter.x != -500 && snakeBodyCenter.y != -500
                                    && do_delete == true )
                            {
                                int16_t delete_color = SN_BG_COLOR;

                                // if the player is up against the border, the erase function
                                // can delete portions of the off-map because it writes in
                                // white instead of black.

                                // CHECKING LEFT BORDER ----------------------
                                if ( snakeBodyCenter.x <= SN_MAP_MIN_X // + SN_SNAKE_SIZE / 2 + 1
                                        && player->dir == LEFT )
                                    delete_color = LCD_BLACK;

                                // CHECKING RIGHT BORDER ---------------------
                                else if ( snakeBodyCenter.x >= SN_MAP_MAX_X // - 1 - SN_SNAKE_SIZE / 2
                                        && player->dir == RIGHT)
                                    delete_color = LCD_BLACK;

                                // CHECKING TOP BORDER -----------------------
                                else if ( snakeBodyCenter.y <= SN_MAP_MIN_Y // + SN_SNAKE_SIZE / 2 + 1
                                        && player->dir == UP)
                                    delete_color = LCD_BLACK;

                                // CHECKING BOTTOM BORDER --------------------
                                else if ( snakeBodyCenter.y >= SN_MAP_MAX_Y // - 1 - SN_SNAKE_SIZE / 2
                                        && player->dir == DOWN)
                                    delete_color = LCD_BLACK;

                                G8RTOS_WaitSemaphore(&LCDREADY);
                                LCD_DrawRectangle(prevMappedBodyCenter.x - SN_SNAKE_SIZE / 2,
                                                  prevMappedBodyCenter.x + SN_SNAKE_SIZE / 2,
                                                  prevMappedBodyCenter.y - SN_SNAKE_SIZE / 2,
                                                  prevMappedBodyCenter.y + SN_SNAKE_SIZE / 2,
                                                  delete_color);
                                G8RTOS_SignalSemaphore(&LCDREADY);
                            }

                            /*
                            // DEBUGGING *********
                            if ( j == 1 )
                                color = LCD_RED;
                            else if ( j == 2 )
                                color = LCD_PURPLE;
                            else if ( j == 3 )
                                color = LCD_BLUE;
                            else if ( j == 4 )
                                color = LCD_PINK;
                            // DEBUGGING **********
                             */

                            if ( withinPlayerRange(&mappedBodyCenter)
                                    // && !(mappedCenter.x == mappedBodyCenter.x && mappedCenter.y == mappedBodyCenter.y)
                                    && do_draw == true )
                            {
                                G8RTOS_WaitSemaphore(&LCDREADY);
                                LCD_DrawRectangle(mappedBodyCenter.x - SN_SNAKE_SIZE / 2,
                                                  mappedBodyCenter.x + SN_SNAKE_SIZE / 2,
                                                  mappedBodyCenter.y - SN_SNAKE_SIZE / 2,
                                                  mappedBodyCenter.y + SN_SNAKE_SIZE / 2,
                                                  color);
                                G8RTOS_SignalSemaphore(&LCDREADY);
                            }
                        }
                    }
                }

                // update the player's animation count so the snake's eyes
                // will open and close. If the count exceeds the animation
                // count, reset the count so the animation can loop.
                player->animation_count++;
                if ( player->animation_count >= SN_SNAKE_FRAME_7 )
                    player->animation_count = 0;

            }

            prevPlayers[i].center = player->center;
            prevPlayers[i].dir = player->dir;
        }

        /*
         * ======================================================
         * =                                                    =
         * =                    FOOD UPDATES                    =
         * =                                                    =
         * ======================================================
         */
        // loop through all food items -----------------
        for (int i = 0; i < SN_MAX_FOOD_ON_MAP; i++)
        {
            food = &local_food[i];

            // UPDATE THE FOOD DRAWING IF IT IS WITHIN RANGE OF THE PLAYER
            if ( food->alive && !food->kill )
            {

                mapObjectToMe(&food->center, &mappedCenter);

                // only draw the food if it is within range BEFORE being mapped to the
                // player's center
                if (withinPlayerRange(&mappedCenter))
                {

                    // erase the old position if this is not an initialization
                    // if ( (prevFood->center.x != -500) && (prevFood->center.y != -500) )
                    if ( withinPlayerRange(&prevFood[i].center) )
                    {
                        G8RTOS_WaitSemaphore(&LCDREADY);
                        LCD_DrawRectangle(prevFood[i].center.x - SN_FOOD_SIZE / 2,
                                          prevFood[i].center.x + SN_FOOD_SIZE / 2,
                                          prevFood[i].center.y - SN_FOOD_SIZE / 2,
                                          prevFood[i].center.y + SN_FOOD_SIZE / 2,
                                          SN_BG_COLOR);
                        G8RTOS_SignalSemaphore(&LCDREADY);
                    }

                    // draw the new position
                    G8RTOS_WaitSemaphore(&LCDREADY);
                    LCD_DrawRectangle(mappedCenter.x - SN_FOOD_SIZE / 2,
                                      mappedCenter.x + SN_FOOD_SIZE / 2,
                                      mappedCenter.y - SN_FOOD_SIZE / 2,
                                      mappedCenter.y + SN_FOOD_SIZE / 2,
                                      SN_FOOD_COLOR);
                    G8RTOS_SignalSemaphore(&LCDREADY);

                    // update the previous array so it cannot be forgotten for erase
                    mapObjectToMe(&food->center, &prevMappedCenter);
                    prevFood[i].center = prevMappedCenter;
                }

                // if the food is not currently within range of the player, check
                // if the previous position was in the range. If it was within range,
                // make sure to erase that square.
                else if ( withinPlayerRange(&prevFood[i].center) )
                {
                    G8RTOS_WaitSemaphore(&LCDREADY);
                    LCD_DrawRectangle( prevFood[i].center.x - SN_FOOD_SIZE / 2,
                                       prevFood[i].center.x + SN_FOOD_SIZE / 2,
                                       prevFood[i].center.y - SN_FOOD_SIZE / 2,
                                       prevFood[i].center.y + SN_FOOD_SIZE / 2,
                                       SN_BG_COLOR);
                    G8RTOS_SignalSemaphore(&LCDREADY);

                    // define the previous center as invalid data so the
                    // block is not constantly rewritten
                    prevFood[i].center.x = -500;
                    prevFood[i].center.y = -500;
                }
            }

            // THE FOOD WAS EATEN
            // else if ( !food->alive && food->kill && withinPlayerRange(&food->center))
            // {
            //     mapObjectToMe(&prevFood[i].center, &mappedCenter);
            //
            //     prevFood[i].center.x = -1;
            //     prevFood[i].center.y = -1;
            //     food->kill = false;
            //
            //     LCD_DrawRectangle(mappedCenter.x - SN_FOOD_SIZE / 2,
            //                       mappedCenter.x + SN_FOOD_SIZE / 2,
            //                       mappedCenter.y - SN_FOOD_SIZE / 2,
            //                       mappedCenter.y + SN_FOOD_SIZE / 2,
            //                       SN_BG_COLOR);
            // }

            // // SPAWN NEW FOOD *****************************************8
            // else if ( !food->alive && !food->kill )
            // {
            //     // game3_refreshFood();
            // }

        }

        /*
         * ======================================================
         * =                                                    =
         * =                 MAP BORDER UPDATES                 =
         * =                                                    =
         * ======================================================
         */

        game3_updateBorders();

        G8RTOS_SignalSemaphore(&CENTER_SEMAPHORE);

        // reassign the local player's center to be used in border updates next cycle
        // my_prev->center = me->center;

        sleep(20);
    }
}
