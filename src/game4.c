/*
 *  menu.c
 *
 *  Created     : 4/20/2019
 *  Last Edit   : 4/20/2019
 *
 *  UPDATES     :
 *  4/2/2019    : Initialized game functions.
 *
 *  TODO        :
 *  ~ Determine what variables the GameState packet should contain
 *      for sending from host to client.
 *  ~ Determine what variables the Client packet should contain for
 *      sending from client to host.
 *  ~ Write the single player HOST-SIDE version of the game with 2
 *      "dead" players who will be controlled by the client.
 *
 */

#include "game4.h"

//add host threads
void game4_addHostThreads(){
    G8RTOS_AddThread( &Game2_GenerateBall, 20, 0xFFFFFFFF,          "GENERATE_BALL___" );
    G8RTOS_AddThread( &game4_DrawObjects, 10, 0xFFFFFFFF,           "DRAW_OBJECTS____" );
    G8RTOS_AddThread( &game4_ReadJoystickHost, 20, 0xFFFFFFFF,      "READ_JOYSTICK___" );
    G8RTOS_AddThread( &game4_IdleThread, 255, 0xFFFFFFFF,           "IDLE____________" );
    G8RTOS_AddThread(&Game2_MoveBall, 11, 0xFFFFFFFF, "MOVE_BALL_______");
}

void game4_CreateGame(){
    game4_InitBoardState();
    game4_addHostThreads();

    G8RTOS_KillSelf();

}

void game4_InitBoardState(){
    G8RTOS_WaitSemaphore(&LCDREADY);
    //set inital lcd display
    //define inital values
    G8RTOS_SignalSemaphore(&LCDREADY);
}
void game4_ReadJoystickHost(){
    int16_t avgX = 0;
    int16_t avgY = 0;
    int16_t joystick_x = 0;
    int16_t joystick_y = 0;

    while(1)
    {

        GetJoystickCoordinates(&joystick_x, &joystick_y);
        avgX = (avgX + joystick_x + JOYSTICK_BIAS_HOST) >> 1;
        avgY = (avgY + joystick_y + JOYSTICK_BIAS_HOST) >> 1;
        if(avgX < 750 && avgX > -1000){
            avgX = 0;
        }
        if(avgY < 750 && avgY > -1000){
            avgY = 0;
        }
        // The switch statement was causing about 500 ms of lag
        Game2_displacementX = -(avgX >> 10);
        Game2_displacementY = (avgY >> 10);

        gamestate.players[0].currentCenterX += Game2_displacementX;
        gamestate.players[0].currentCenterY += Game2_displacementY;

        gamestate.players[1].currentCenterX += gamestate.player.displacementX;
        gamestate.players[1].currentCenterY += gamestate.player.displacementY;

        // PLAYER 1's X LIMITATIONS---------------------------------------------
        // player center is too far to the left - limit it.
        if ( gamestate.players[0].currentCenterX - PLAYER_OFFSET - 1 <= ARENA_MIN_X )
        {
            gamestate.players[0].currentCenterX = ARENA_MIN_X + PLAYER_OFFSET + 1;
        }

        // player center is too far to the right - limit it.
        else if ( gamestate.players[0].currentCenterX + PLAYER_OFFSET + 1 > ARENA_MAX_X - MARIO_COLUMNS )
        {
            gamestate.players[0].currentCenterX = ARENA_MAX_X - PLAYER_OFFSET - 1 - MARIO_COLUMNS;
        }

        // PLAYER 1's Y LIMITATIONS---------------------------------------------
        // player center is too far to the left - limit it.
        if ( gamestate.players[0].currentCenterY - PLAYER_OFFSET - 1 <= ARENA_MIN_Y )
        {
            gamestate.players[0].currentCenterY = ARENA_MIN_Y + PLAYER_OFFSET + 1;
        }

        // player center is too far to the right - limit it.
        else if ( gamestate.players[0].currentCenterY + PLAYER_OFFSET + 1 > ARENA_MAX_Y - 1 - MARIO_ROWS )
        {
            gamestate.players[0].currentCenterY = ARENA_MAX_Y - PLAYER_OFFSET - 1 - MARIO_ROWS;
        }



        // PLAYER 2's X LIMITATIONS---------------------------------------------
        // player center is too far to the left - limit it.
        if ( gamestate.players[1].currentCenterX - PLAYER_OFFSET - 1 <= ARENA_MIN_X )
        {
            gamestate.players[1].currentCenterX = ARENA_MIN_X + PLAYER_OFFSET + 1;
        }

        // player center is too far to the right - limit it.
        else if ( gamestate.players[1].currentCenterX + PLAYER_OFFSET + 1 > ARENA_MAX_X - MARIO_COLUMNS )
        {
            gamestate.players[1].currentCenterX = ARENA_MAX_X - PLAYER_OFFSET - 1 - MARIO_COLUMNS;
        }

        // PLAYER 1's Y LIMITATIONS---------------------------------------------
        // player center is too far to the left - limit it.
        if ( gamestate.players[1].currentCenterY - PLAYER_OFFSET - 1 <= ARENA_MIN_Y )
        {
            gamestate.players[1].currentCenterY = ARENA_MIN_Y + PLAYER_OFFSET + 1;
        }

        // player center is too far to the right - limit it.
        else if ( gamestate.players[1].currentCenterY + PLAYER_OFFSET + 1 > ARENA_MAX_Y - 1 - MARIO_ROWS )
        {
            gamestate.players[1].currentCenterY = ARENA_MAX_Y - PLAYER_OFFSET - 1 - MARIO_ROWS;
        }



        // player direction handling for the host
        if ( abs(Game2_displacementX) >= abs(Game2_displacementY) )
        {
            if (Game2_displacementX > 0 )
                gamestate.players[0].direction = 3;

            else
                gamestate.players[0].direction = 1;
        }
        else
        {

            if (Game2_displacementY > 0 )
                gamestate.players[0].direction = 0;
            else
                gamestate.players[0].direction = 2;
        }

        sleep(10);
    }
}
void game4_IdleThread(){
    while(1){

    }
}
