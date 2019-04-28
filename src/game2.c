
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
#define SINGLE
#include "game2.h"
#include "menu.h"


uint8_t ballCount = 0;
uint8_t ShroomCount = 0;
Game2_PrevShroom_t previousShrooms[MAX_NUM_OF_SHROOMS];
Game2_PrevBall_t previousBalls[MAX_NUM_OF_BALLS];
Game2_SpecificPlayerInfo_t client_player;
Game2_GameState_t gamestate;
Game2_GameState_t packet;
uint32_t Start_time = 0;
uint32_t prev_run_time = 0;
int16_t Game2_displacementX = 0;
int16_t Game2_displacementY = 0;
//image arrays
//12x16 image size
#define MARIO_COLUMNS 12
#define MARIO_ROWS 16
uint16_t mario_red_map[16][12] = {

  /*Pixel format: Red: 5 bit, Green: 6 bit, Blue: 5 bit*/
  0xffff, 0xffff, 0xffff, 0xF920, 0xF920, 0xF920, 0xF920, 0xF920, 0xffff, 0xffff, 0xffff, 0xffff,
  0xffff, 0xffff, 0xF920, 0xF920, 0xF920, 0xF920, 0xF920, 0xF920, 0xF920, 0xF920, 0xF920, 0xffff,
  0xffff, 0xffff, 0x9280, 0x9280, 0x9280, 0xFF75, 0xFF75, 0x0000, 0xFF75, 0xffff, 0xffff, 0xffff,
  0xffff, 0x9280, 0xFF75, 0x9280, 0xFF75, 0xFF75, 0xFF75, 0x0000, 0xFF75, 0xFF75, 0xFF75, 0xffff,
  0xffff, 0x9280, 0xFF75, 0x9280, 0x9280, 0xFF75, 0xFF75, 0xFF75, 0x0000, 0xFF75, 0xFF75, 0xFF75,
  0xffff, 0xffff, 0x9280, 0xFF75, 0xFF75, 0xFF75, 0xFF75, 0x0000, 0x0000, 0x0000, 0x0000, 0xffff,
  0xffff, 0xffff, 0xffff, 0xFF75, 0xFF75, 0xFF75, 0xFF75, 0xFF75, 0xFF75, 0xffff, 0xffff, 0xffff,
  0xffff, 0xffff, 0xF920, 0xF920, 0x019F, 0xF920, 0xF920, 0x019F, 0xF920, 0xF920, 0xffff, 0xffff,
  0xffff, 0xF920, 0xF920, 0xF920, 0x019F, 0xF920, 0xF920, 0x019F, 0xF920, 0xF920, 0xF920, 0xffff,
  0xF920, 0xF920, 0xF920, 0xF920, 0x019F, 0x019F, 0x019F, 0x019F, 0xF920, 0xF920, 0xF920, 0xF920,
  0xFF75, 0xFF75, 0xF920, 0x019F, 0x80ff, 0x019F, 0x019F, 0x80ff, 0x019F, 0xF920, 0xFF75, 0xFF75,
  0xFF75, 0xFF75, 0xFF75, 0x019F, 0x019F, 0x019F, 0x019F, 0x019F, 0x019F, 0xFF75, 0xFF75, 0xFF75,
  0xFF75, 0xFF75, 0x019F, 0x019F, 0x019F, 0x019F, 0x019F, 0x019F, 0x019F, 0x019F, 0xFF75, 0xFF75,
  0xffff, 0xffff, 0x019F, 0x019F, 0x019F, 0xffff, 0xffff, 0x019F, 0x019F, 0x019F, 0xffff, 0xffff,
  0xffff, 0x9080, 0x9080, 0x9080, 0xffff, 0xffff, 0xffff, 0xffff, 0x9080, 0x9080, 0x9080, 0xffff,
  0x9080, 0x9080, 0x9080, 0x9080, 0xffff, 0xffff, 0xffff, 0xffff, 0x9080, 0x9080, 0x9080, 0x9080
};
uint16_t mario_yellow_map[16][12] = {

  /*Pixel format: Red: 5 bit, Green: 6 bit, Blue: 5 bit*/
  0xffff, 0xffff, 0xffff, 0xFFC0, 0xFFC0, 0xFFC0, 0xFFC0, 0xFFC0, 0xffff, 0xffff, 0xffff, 0xffff,
  0xffff, 0xffff, 0xFFC0, 0xFFC0, 0xFFC0, 0xFFC0, 0xFFC0, 0xFFC0, 0xFFC0, 0xFFC0, 0xFFC0, 0xffff,
  0xffff, 0xffff, 0x9280, 0x9280, 0x9280, 0xFF75, 0xFF75, 0xFFFF, 0xFF75, 0xffff, 0xffff, 0xffff,
  0xffff, 0x9280, 0xFF75, 0x9280, 0xFF75, 0xFF75, 0xFF75, 0x07FF, 0xFF75, 0xFF75, 0xFF75, 0xffff,
  0xffff, 0x9280, 0xFF75, 0x9280, 0x9280, 0xFF75, 0xFF75, 0xFF75, 0x0000, 0xFF75, 0xFF75, 0xFF75,
  0xffff, 0xffff, 0x9280, 0xFF75, 0xFF75, 0xFF75, 0xFF75, 0x0000, 0x0000, 0x0000, 0x0000, 0xffff,
  0xffff, 0xffff, 0xffff, 0xFF75, 0xFF75, 0xFF75, 0xFF75, 0xFF75, 0xFF75, 0xffff, 0xffff, 0xffff,
  0xffff, 0xffff, 0xFFC0, 0xFFC0, 0xFA1F, 0xFFC0, 0xFFC0, 0xFA1F, 0xFFC0, 0xFFC0, 0xffff, 0xffff,
  0xffff, 0xFFC0, 0xFFC0, 0xFFC0, 0xFA1F, 0xFFC0, 0xFFC0, 0xFA1F, 0xFFC0, 0xFFC0, 0xFFC0, 0xffff,
  0xFFC0, 0xFFC0, 0xFFC0, 0xFFC0, 0xFA1F, 0xFA1F, 0xFA1F, 0xFA1F, 0xFFC0, 0xFFC0, 0xFFC0, 0xFFC0,
  0xFF75, 0xFF75, 0xF920, 0xFA1F, 0x80ff, 0xFA1F, 0xFA1F, 0x80ff, 0xFA1F, 0xFFC0, 0xFF75, 0xFF75,
  0xFF75, 0xFF75, 0xFF75, 0xFA1F, 0xFA1F, 0xFA1F, 0xFA1F, 0xFA1F, 0xFA1F, 0xFF75, 0xFF75, 0xFF75,
  0xFF75, 0xFF75, 0xFA1F, 0xFA1F, 0xFA1F, 0xFA1F, 0xFA1F, 0xFA1F, 0xFA1F, 0xFA1F, 0xFF75, 0xFF75,
  0xffff, 0xffff, 0xFA1F, 0xFA1F, 0xFA1F, 0xffff, 0xffff, 0xFA1F, 0xFA1F, 0xFA1F, 0xffff, 0xffff,
  0xffff, 0x9080, 0x9080, 0x9080, 0xffff, 0xffff, 0xffff, 0xffff, 0x9080, 0x9080, 0x9080, 0xffff,
  0x9080, 0x9080, 0x9080, 0x9080, 0xffff, 0xffff, 0xffff, 0xffff, 0x9080, 0x9080, 0x9080, 0x9080
};
uint16_t mario_green_map[16][12] = {

  /*Pixel format: Red: 5 bit, Green: 6 bit, Blue: 5 bit*/
  0xffff, 0xffff, 0xffff, 0x47E3, 0x47E3, 0x47E3, 0x47E3, 0x47E3, 0xffff, 0xffff, 0xffff, 0xffff,
  0xffff, 0xffff, 0x47E3, 0x47E3, 0x47E3, 0x47E3, 0x47E3, 0x47E3, 0x47E3, 0x47E3, 0x47E3, 0xffff,
  0xffff, 0xffff, 0x9280, 0x9280, 0x9280, 0xFF75, 0xFF75, 0x0000, 0xFF75, 0xffff, 0xffff, 0xffff,
  0xffff, 0x9280, 0xFF75, 0x9280, 0xFF75, 0xFF75, 0xFF75, 0x0000, 0xFF75, 0xFF75, 0xFF75, 0xffff,
  0xffff, 0x9280, 0xFF75, 0x9280, 0x9280, 0xFF75, 0xFF75, 0xFF75, 0x0000, 0xFF75, 0xFF75, 0xFF75,
  0xffff, 0xffff, 0x9280, 0xFF75, 0xFF75, 0xFF75, 0xFF75, 0x0000, 0x0000, 0x0000, 0x0000, 0xffff,
  0xffff, 0xffff, 0xffff, 0xFF75, 0xFF75, 0xFF75, 0xFF75, 0xFF75, 0xFF75, 0xffff, 0xffff, 0xffff,
  0xffff, 0xffff, 0x47E3, 0x47E3, 0x019F, 0x47E3, 0x47E3, 0x019F, 0x47E3, 0xF920, 0xffff, 0xffff,
  0xffff, 0x47E3, 0x47E3, 0x47E3, 0x019F, 0x47E3, 0x47E3, 0x019F, 0x47E3, 0x47E3, 0x47E3, 0xffff,
  0x47E3, 0x47E3, 0x47E3, 0x47E3, 0x019F, 0x019F, 0x019F, 0x019F, 0x47E3, 0x47E3, 0x47E3, 0x47E3,
  0xFF75, 0xFF75, 0x47E3, 0x019F, 0x80ff, 0x019F, 0x019F, 0x80ff, 0x019F, 0x47E3, 0xFF75, 0xFF75,
  0xFF75, 0xFF75, 0xFF75, 0x019F, 0x019F, 0x019F, 0x019F, 0x019F, 0x019F, 0xFF75, 0xFF75, 0xFF75,
  0xFF75, 0xFF75, 0x019F, 0x019F, 0x019F, 0x019F, 0x019F, 0x019F, 0x019F, 0x019F, 0xFF75, 0xFF75,
  0xffff, 0xffff, 0x019F, 0x019F, 0x019F, 0xffff, 0xffff, 0x019F, 0x019F, 0x019F, 0xffff, 0xffff,
  0xffff, 0x9080, 0x9080, 0x9080, 0xffff, 0xffff, 0xffff, 0xffff, 0x9080, 0x9080, 0x9080, 0xffff,
  0x9080, 0x9080, 0x9080, 0x9080, 0xffff, 0xffff, 0xffff, 0xffff, 0x9080, 0x9080, 0x9080, 0x9080
};
#define GOOMBA_SIZE 8
uint16_t goomba_map[8][8] = {
  /*Pixel format: Red: 5 bit, Green: 6 bit, Blue: 5 bit*/
  0xffff, 0xffff, 0xABC8, 0xABC8, 0xABC8, 0xABC8, 0xffff, 0xffff,
  0xffff, 0xABC8, 0xABC8, 0xABC8, 0xABC8, 0xABC8, 0xABC8, 0xffff,
  0xABC8, 0xABC8, 0xffff, 0xABC8, 0xABC8, 0xffff, 0xABC8, 0xABC8,
  0xABC8, 0xABC8, 0x0000, 0xABC8, 0xABC8, 0x0000, 0xABC8, 0xABC8,
  0xABC8, 0xABC8, 0xABC8, 0xABC8, 0xABC8, 0xABC8, 0xABC8, 0xABC8,
  0xffff, 0xffff, 0xFEAF, 0xFEAF, 0xFEAF, 0xFEAF, 0xffff, 0xffff,
  0xffff, 0xABC8, 0xFEAF, 0xFEAF, 0xFEAF, 0xFEAF, 0xABC8, 0xffff,
  0xffff, 0xABC8, 0xABC8, 0xFEAF, 0xFEAF, 0xABC8, 0xABC8, 0xffff
};
#define SHROOM_SIZE 16
uint16_t shroom_map[16][16] = {
  /*Pixel format: Red: 5 bit, Green: 6 bit, Blue: 5 bit*/
  0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
  0xffff, 0xffff, 0xffff, 0x0000, 0x0000, 0x0000, 0xffff, 0x07C0, 0x07C0, 0xffff, 0x0000, 0x0000, 0x0000, 0xffff, 0xffff, 0xffff,
  0xffff, 0xffff, 0x0000, 0x0000, 0xffff, 0xffff, 0xffff, 0x07C0, 0x07C0, 0xffff, 0xffff, 0xffff, 0x0000, 0x0000, 0xffff, 0xffff,
  0xffff, 0x0000, 0x0000, 0x07C0, 0xffff, 0xffff, 0x07C0, 0x07C0, 0x07C0, 0x07C0, 0xffff, 0xffff, 0x07C0, 0x0000, 0x0000, 0xffff,
  0xffff, 0x0000, 0xffff, 0x07C0, 0x07C0, 0x07C0, 0x07C0, 0x07C0, 0x07C0, 0x07C0, 0x07C0, 0x07C0, 0x07C0, 0xffff, 0x0000, 0xffff,
  0x0000, 0x0000, 0xffff, 0xffff, 0x07C0, 0x07C0, 0xffff, 0xffff, 0xffff, 0xffff, 0x07C0, 0x07C0, 0xffff, 0xffff, 0x0000, 0x0000,
  0x0000, 0xffff, 0xffff, 0xffff, 0x07C0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0x07C0, 0xffff, 0xffff, 0xffff, 0x0000,
  0x0000, 0xffff, 0xffff, 0xffff, 0x07C0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0x07C0, 0xffff, 0xffff, 0xffff, 0x0000,
  0x0000, 0xffff, 0xffff, 0xffff, 0x07C0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0x07C0, 0xffff, 0xffff, 0xffff, 0x0000,
  0x0000, 0x07C0, 0x07C0, 0x07C0, 0x07C0, 0x07C0, 0xffff, 0xffff, 0xffff, 0xffff, 0x8925, 0x07C0, 0x07C0, 0x07C0, 0x07C0, 0x0000,
  0x0000, 0x07C0, 0x07C0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x07C0, 0x07C0, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0xffff, 0xffff, 0x0000, 0xffff, 0xffff, 0x0000, 0xffff, 0xffff, 0x0000, 0x0000, 0x0000, 0x0000,
  0xffff, 0x0000, 0x0000, 0xffff, 0xffff, 0xffff, 0x0000, 0xffff, 0xffff, 0x0000, 0xffff, 0xffff, 0xffff, 0x0000, 0x0000, 0xffff,
  0xffff, 0xffff, 0x0000, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0x0000, 0xffff, 0xffff,
  0xffff, 0xffff, 0x0000, 0x0000, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0x0000, 0x0000, 0xffff, 0xffff,
  0xffff, 0xffff, 0xffff, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xffff, 0xffff, 0xffff
};
//add host threads
void Game2_addHostThreads(){
    G8RTOS_AddThread( &Game2_GenerateBall, 20, 0xFFFFFFFF,          "GENERATE_BALL___" );
    G8RTOS_AddThread( &Game2_GenerateShroom, 20, 0xFFFFFFFF,          "GENERATE_BALL___" );
    G8RTOS_AddThread( &Game2_DrawObjects, 10, 0xFFFFFFFF,           "DRAW_OBJECTS____" );
    G8RTOS_AddThread( &Game2_ReadJoystickHost, 20, 0xFFFFFFFF,      "READ_JOYSTICK___" );
    G8RTOS_AddThread( &Game2_UpdatePlayerStatus, 20, 0xFFFFFFFF,    "UpdatePlayerStatus" );
    G8RTOS_AddThread( &Game2_IdleThread, 255, 0xFFFFFFFF,           "IDLE____________" );
    G8RTOS_AddThread(&Game2_MoveBall, 10, 0xFFFFFFFF, "MOVE_BALL_______");
    G8RTOS_AddThread(&Game2_MoveShroom, 11, 0xFFFFFFFF, "MOVE_BALL_______");

    #ifndef SINGLE
        G8RTOS_AddThread( &Game2_ReceiveDataFromClient, DEFAULT_PRIORITY, 0xFFFFFFFF, "RECEIVE_DATA____" );
        G8RTOS_AddThread( &Game2_SendDataToClient, DEFAULT_PRIORITY, 0xFFFFFFFF,      "SEND_DATA_______" );
    #endif
}

void Game2_addClientThreads(){
    G8RTOS_AddThread( &Game2_ReadJoystickClient, DEFAULT_PRIORITY, 0xFFFFFFFF,    "READ_JOYSTICK___" );
    G8RTOS_AddThread( &Game2_SendDataToHost, DEFAULT_PRIORITY, 0xFFFFFFFF,        "SEND_DATA_______" );
    G8RTOS_AddThread( &Game2_ReceiveDataFromHost, DEFAULT_PRIORITY, 0xFFFFFFFF,   "RECEIVE_DATA____" );
    G8RTOS_AddThread( &Game2_DrawObjects, 10, 0xFFFFFFFF,                         "DRAW_OBJECTS____" );
    G8RTOS_AddThread( &Game2_UpdatePlayerStatus, 20, 0xFFFFFFFF,    "UpdatePlayerStatus" );
    G8RTOS_AddThread( &Game2_IdleThread, 255, 0xFFFFFFFF,                         "IDLE____________" );
}

void Game2_CreateGame(){
    Game2_InitBoardState();
    Game2_addHostThreads();
    G8RTOS_KillSelf();

}

void GAME2_JoinGame()
{
    // 1. Set initial SpecificPlayerInfo_t strict attributes ( getLocalIP() ).
    client_player.IP_address = getLocalIP();
    client_player.acknowledge = false;
    client_player.displacementX = 0;
    client_player.displacementY = 0;
    client_player.joined = false;
    client_player.playerNumber = 1;
    client_player.ready = false;


    Game2_InitBoardState();

    Game2_addClientThreads();

    // 6. Kill self.
    G8RTOS_KillSelf();
}

void Game2_InitBoardState(){
    uint8_t status_p1_str[30];
    LCD_Clear(White);

    for(int i = 0; i<MAX_NUM_OF_PLAYERS; i++){
    gamestate.players[i].num_lives = LIVES;
    gamestate.players[i].RunTime = 0;
    }
    if(MAX_NUM_OF_PLAYERS == 1){
        gamestate.players[0].currentCenterX = (ARENA_MAX_X - ARENA_MIN_X) >> 1;
        gamestate.players[0].currentCenterY = (ARENA_MAX_Y - ARENA_MIN_Y) >> 1;
        gamestate.players[0].color = Cyan;
    }
    if(MAX_NUM_OF_PLAYERS == 2){
        gamestate.players[0].currentCenterX = (ARENA_MAX_X - ARENA_MIN_X) >> 2;
        gamestate.players[0].currentCenterY = (ARENA_MAX_Y - ARENA_MIN_Y) >> 2;
        gamestate.players[1].currentCenterX = ((ARENA_MAX_X - ARENA_MIN_X) >> 1) + gamestate.players[0].currentCenterX;
        gamestate.players[1].currentCenterY = ((ARENA_MAX_Y - ARENA_MIN_Y) >> 1) + gamestate.players[0].currentCenterY;
        gamestate.players[0].color = Cyan;
        gamestate.players[1].color = Magenta;
    }
    if(MAX_NUM_OF_PLAYERS == 3){
        gamestate.players[0].currentCenterX = ((ARENA_MAX_X - ARENA_MIN_X) >> 1) - ((ARENA_MAX_X - ARENA_MIN_X) >> 3);
        gamestate.players[0].currentCenterY = ((ARENA_MAX_Y - ARENA_MIN_Y) >> 1) - ((ARENA_MAX_Y - ARENA_MIN_Y) >> 3);
        gamestate.players[1].currentCenterX = (ARENA_MAX_X - ARENA_MIN_X) >> 1;
        gamestate.players[1].currentCenterY = (ARENA_MAX_Y - ARENA_MIN_Y) >> 1;
        gamestate.players[2].currentCenterX = ((ARENA_MAX_X - ARENA_MIN_X) >> 3) + gamestate.players[1].currentCenterX;
        gamestate.players[2].currentCenterY = ((ARENA_MAX_Y - ARENA_MIN_Y) >> 3) + gamestate.players[1].currentCenterY;
        gamestate.players[0].color = Cyan;
        gamestate.players[1].color = Magenta;
        gamestate.players[2].color = Blue2;
    }

    LCD_DrawRectangle(ARENA_MIN_X, ARENA_MAX_X, ARENA_MIN_Y - 1, ARENA_MIN_Y - 1, Black);
}

/*
 * Generate Ball thread
 */
// Adds another ball thread. Sleeps proportional to the number of balls
// currently in play.
void Game2_GenerateBall()
{
    while(1)
    {
        // wake up the next available ball in the array if
        // the max number of balls have not been generated.
        if ( ballCount < MAX_NUM_OF_BALLS )
        {
            //G8RTOS_AddThread(&Game2_MoveBall, 10, 0xFFFFFFFF, "MOVE_BALL_______");
            //Game2_Ball_t * ball;
            //Game2_PrevBall_t * previousBall;

            for (int i = 0; i < MAX_NUM_OF_BALLS; i++)
            {
                if ( gamestate.balls[i].alive == 0 && gamestate.balls[i].kill == 0)
                {
                    //previousBall->CenterX = 0;
                    //previousBall->CenterY = 120;
                    gamestate.balls[i].alive = 1;
                    gamestate.balls[i].kill = 0;
                    gamestate.balls[i].color = BACK_COLOR;
                    gamestate.balls[i].currentCenterX = BALL_SIZE * (rand() % ((ARENA_MAX_X - ARENA_MIN_X - BALL_SIZE)/BALL_SIZE)) + ARENA_MIN_X + 1;
                    gamestate.balls[i].currentCenterY = BALL_SIZE * (rand() % (((ARENA_MAX_Y - 80 - ARENA_MIN_Y)/BALL_SIZE)) + 10);
                    previousBalls[i].CenterX = gamestate.balls[i].currentCenterX;
                    previousBalls[i].CenterY = gamestate.balls[i].currentCenterY;
                    gamestate.balls[i].xvel = rand() % MAX_BALL_SPEED + 1;
                    gamestate.balls[i].yvel = rand() % MAX_BALL_SPEED + 1;
                    if(gamestate.balls[i].currentCenterY > 120){
                        gamestate.balls[i].yvel = -gamestate.balls[i].yvel;
                    }
                    break;
                }
            }
            ballCount++;
        }
        sleep(ballCount * BALL_GEN_SLEEP);
    }
}

/*
 * Generate Shroom thread
 */
// Adds another ball thread. Sleeps proportional to the number of balls
// currently in play.
void Game2_GenerateShroom()
{
    while(1)
    {
        // wake up the next available ball in the array if
        // the max number of balls have not been generated.
        if ( ShroomCount < MAX_NUM_OF_SHROOMS )
        {

            for (int i = 0; i < MAX_NUM_OF_SHROOMS; i++)
            {
                if ( gamestate.shroom[i].alive == 0 && gamestate.shroom[i].kill == 0)
                {
                    gamestate.shroom[i].alive = 1;
                    gamestate.shroom[i].kill = 0;
                    gamestate.shroom[i].currentCenterX = (rand() % (ARENA_MAX_X - ARENA_MIN_X));
                    if(gamestate.shroom[i].currentCenterX < ARENA_MIN_X){
                        gamestate.shroom[i].currentCenterX += ARENA_MIN_X;
                    }
                    gamestate.shroom[i].currentCenterY = (rand() % (ARENA_MAX_Y - ARENA_MIN_Y));
                    if(gamestate.shroom[i].currentCenterY < ARENA_MIN_Y){
                        gamestate.shroom[i].currentCenterY += ARENA_MIN_Y;
                    }
                    previousShrooms[i].CenterX = gamestate.shroom[i].currentCenterX;
                    previousShrooms[i].CenterY = gamestate.shroom[i].currentCenterY;
                    gamestate.shroom[i].xvel = rand() % MAX_SHROOM_SPEED + 1;
                    gamestate.shroom[i].yvel = rand() % MAX_SHROOM_SPEED + 1;
                    if(gamestate.shroom[i].currentCenterY > 120){
                        gamestate.shroom[i].yvel = -gamestate.shroom[i].yvel;
                    }
                    break;
                }
            }
            ShroomCount++;
        }
        sleep(SHROOM_SLEEP_COUNT);
    }
}

/*
 * Thread to read host's joystick
 */
void Game2_ReadJoystickHost(){
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
        Game2_displacementX = -(avgX >> 12);
        Game2_displacementY = (avgY >> 12);

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
        if ( gamestate.players[1].currentCenterX - PLAYER_OFFSET - 1 <= MIN_SCREEN_X )
        {
            gamestate.players[1].currentCenterX = MIN_SCREEN_X + PLAYER_OFFSET + 1;
        }

        // player center is too far to the right - limit it.
        else if ( gamestate.players[1].currentCenterX + PLAYER_OFFSET + 1 > MAX_SCREEN_X - 1 )
        {
            gamestate.players[1].currentCenterX = MAX_SCREEN_X - PLAYER_OFFSET - 1;
        }

        // PLAYER 2's Y LIMITATIONS---------------------------------------------
        // player center is too far to the left - limit it.
        if ( gamestate.players[1].currentCenterY - PLAYER_OFFSET - 1 <= MIN_SCREEN_Y )
        {
            gamestate.players[1].currentCenterY = MIN_SCREEN_Y + PLAYER_OFFSET + 1;
        }

        // player center is too far to the right - limit it.
        else if ( gamestate.players[1].currentCenterY + PLAYER_OFFSET + 1 > MAX_SCREEN_Y - 1 )
        {
            gamestate.players[1].currentCenterY = MAX_SCREEN_Y - PLAYER_OFFSET - 1;
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

/*
 * Thread to read client's joystick
 */
void Game2_ReadJoystickClient(){
    int16_t avgX = 0;
    int16_t avgY = 0;
    int16_t joystick_x = 0;
    int16_t joystick_y = 0;

    while(1)
    {
        // moving average of the joystick inputs
        GetJoystickCoordinates(&joystick_x, &joystick_y);
        avgX = (avgX + joystick_x + 500) >> 1;
        avgY = (avgY + joystick_y + 750) >> 1;

        // The switch statement was causing about 500 ms of lag
        Game2_displacementX = -(avgX >> 12);
        Game2_displacementY = (avgY >> 12);

        // assign the displacement from the client to send to the host here
        client_player.displacementX = Game2_displacementX;
        client_player.displacementY = Game2_displacementY;

        // player direction handling
        if ( abs(client_player.displacementX) >= abs(client_player.displacementY) )
        {
            if (client_player.displacementX > 0 )
                client_player.direction = 3;

            else
                client_player.direction = 1;
        }
        else
        {

            if (client_player.displacementY > 0 )
                client_player.direction = 0;
            else
                client_player.direction = 2;
        }

        sleep(10);
    }
}

/*
 * SUMMARY: Thread to move a single ball
 *
 * DESCRIPTION: This thread is responsible for moving the ball at
 *              a constant velocity, handling collisions, and
 *              incrementing the score for whichever player scored.
 */
void Game2_MoveShroom()
{
    // initialize the ball properties for the ball
    // that will be used in this thread.

    while(1){
        for(int j = 0; j < MAX_NUM_OF_SHROOMS; j++){
            // test if hitting the right or left side wall
            if((gamestate.shroom[j].xvel > 0 && gamestate.shroom[j].currentCenterX + gamestate.shroom[j].xvel + SHROOM_SIZE + 1 >= ARENA_MAX_X) ||
               (gamestate.shroom[j].xvel < 0 && gamestate.shroom[j].currentCenterX + gamestate.shroom[j].xvel - 1 - SHROOM_SIZE<= ARENA_MIN_X)){
                gamestate.shroom[j].xvel = -gamestate.shroom[j].xvel;
            }

            // test if hitting the top or bottom paddle
            if(((gamestate.shroom[j].yvel > 0 && gamestate.shroom[j].currentCenterY + gamestate.shroom[j].yvel + SHROOM_SIZE + 1 >= ARENA_MAX_Y)) ||
               ((gamestate.shroom[j].yvel < 0 && gamestate.shroom[j].currentCenterY + gamestate.shroom[j].yvel - 1 <= ARENA_MIN_Y))){

                // reverse the y direction
                gamestate.shroom[j].yvel = -gamestate.shroom[j].yvel;

                // ball directions for top
                if(gamestate.shroom[j].yvel > 0){
                    if(gamestate.shroom[j].currentCenterX <= ARENA_MAX_X >>1){
                        gamestate.shroom[j].xvel -= 1;
                        if(gamestate.shroom[j].xvel > 0){
                            gamestate.shroom[j].yvel += 1;
                        }
                        else{
                            gamestate.shroom[j].yvel -= 1;
                        }
                    }
                    else if(gamestate.shroom[j].currentCenterX > ARENA_MAX_X>>1){
                        gamestate.shroom[j].xvel += 1;
                        if(gamestate.shroom[j].xvel < 0){
                            gamestate.shroom[j].yvel += 1;
                        }
                        else{
                            gamestate.shroom[j].yvel -= 1;
                        }
                    }
                    if(gamestate.shroom[j].yvel == 0){
                        gamestate.shroom[j].yvel = 1;
                    }
                }

                // ball directions for bottom
                else{
                    if(gamestate.shroom[j].currentCenterX <= ARENA_MAX_X>>1){
                        gamestate.shroom[j].xvel += 1;
                        if(gamestate.shroom[j].xvel > 0){
                            gamestate.shroom[j].yvel += 1;
                        }
                        else{
                            gamestate.shroom[j].yvel -= 1;
                        }
                    }
                    else if(gamestate.shroom[j].currentCenterX > ARENA_MAX_X>>1){
                        gamestate.shroom[j].xvel -= 1;
                        if(gamestate.shroom[j].xvel < 0){
                            gamestate.shroom[j].yvel += 1;
                        }
                        else{
                            gamestate.shroom[j].yvel -= 1;
                        }
                    }
                    if(gamestate.shroom[j].yvel == 0){  gamestate.shroom[j].yvel = -1;}
                }
            }

            for(int i = 0; i < MAX_NUM_OF_PLAYERS; i++){
                if(((gamestate.shroom[j].currentCenterX + gamestate.shroom[j].xvel + SHROOM_SIZE/2 >= gamestate.players[i].currentCenterX) &&
                        (gamestate.shroom[j].currentCenterX + gamestate.shroom[j].xvel + SHROOM_SIZE/2 <= gamestate.players[i].currentCenterX + MARIO_COLUMNS)) &&
                            ((gamestate.shroom[j].currentCenterY + gamestate.shroom[j].yvel + SHROOM_SIZE/2 >= gamestate.players[i].currentCenterY) &&
                                    (gamestate.shroom[j].currentCenterY + gamestate.shroom[j].yvel + SHROOM_SIZE/2  <= gamestate.players[i].currentCenterY + MARIO_ROWS)) &&
                                    gamestate.shroom[j].alive == true && gamestate.shroom[j].kill == false){
                        gamestate.players[i].num_lives++;
                        gamestate.shroom[j].kill = 1;
                    }
                }


            gamestate.shroom[j].currentCenterX       += gamestate.shroom[j].xvel;
            gamestate.shroom[j].currentCenterY       += gamestate.shroom[j].yvel;
        }
        sleep(35);
    }
}

/*
 * SUMMARY: Thread to move a single ball
 *
 * DESCRIPTION: This thread is responsible for moving the ball at
 *              a constant velocity, handling collisions, and
 *              incrementing the score for whichever player scored.
 */
void Game2_MoveBall()
{
    // initialize the ball properties for the ball
    // that will be used in this thread.

    while(1){
        for(int j = 0; j < MAX_NUM_OF_BALLS; j++){
            // test if hitting the right or left side wall
            if((gamestate.balls[j].xvel > 0 && gamestate.balls[j].currentCenterX + gamestate.balls[j].xvel + GOOMBA_SIZE + 1 >= ARENA_MAX_X) ||
               (gamestate.balls[j].xvel < 0 && gamestate.balls[j].currentCenterX + gamestate.balls[j].xvel - 1 - GOOMBA_SIZE<= ARENA_MIN_X)){
                gamestate.balls[j].xvel = -gamestate.balls[j].xvel;
            }

            // test if hitting the top or bottom paddle
            if(((gamestate.balls[j].yvel > 0 && gamestate.balls[j].currentCenterY + gamestate.balls[j].yvel + GOOMBA_SIZE + 1 >= ARENA_MAX_Y)) ||
               ((gamestate.balls[j].yvel < 0 && gamestate.balls[j].currentCenterY + gamestate.balls[j].yvel - 1 <= ARENA_MIN_Y))){

                // reverse the y direction
                gamestate.balls[j].yvel = -gamestate.balls[j].yvel;

                // ball directions for top
                if(gamestate.balls[j].yvel > 0){
                    if(gamestate.balls[j].currentCenterX <= ARENA_MAX_X >>1){
                        gamestate.balls[j].xvel -= 1;
                        if(gamestate.balls[j].xvel > 0){
                            gamestate.balls[j].yvel += 1;
                        }
                        else{
                            gamestate.balls[j].yvel -= 1;
                        }
                    }
                    else if(gamestate.balls[j].currentCenterX > ARENA_MAX_X>>1){
                        gamestate.balls[j].xvel += 1;
                        if(gamestate.balls[j].xvel < 0){
                            gamestate.balls[j].yvel += 1;
                        }
                        else{
                            gamestate.balls[j].yvel -= 1;
                        }
                    }
                    if(gamestate.balls[j].yvel == 0){  gamestate.balls[j].yvel = 1;}
                }

                // ball directions for bottom
                else{
                    if(gamestate.balls[j].currentCenterX <= ARENA_MAX_X>>1){
                        gamestate.balls[j].xvel += 1;
                        if(gamestate.balls[j].xvel > 0){
                            gamestate.balls[j].yvel += 1;
                        }
                        else{
                            gamestate.balls[j].yvel -= 1;
                        }
                    }
                    else if(gamestate.balls[j].currentCenterX > ARENA_MAX_X>>1){
                        gamestate.balls[j].xvel -= 1;
                        if(gamestate.balls[j].xvel < 0){
                            gamestate.balls[j].yvel += 1;
                        }
                        else{
                            gamestate.balls[j].yvel -= 1;
                        }
                    }
                    if(gamestate.balls[j].yvel == 0){  gamestate.balls[j].yvel = -1;}
                }
            }

            for(int i = 0; i < MAX_NUM_OF_PLAYERS; i++){
                if(((gamestate.balls[j].currentCenterX + gamestate.balls[j].xvel + GOOMBA_SIZE/2 >= gamestate.players[i].currentCenterX) &&
                        (gamestate.balls[j].currentCenterX + gamestate.balls[j].xvel + GOOMBA_SIZE/2 <= gamestate.players[i].currentCenterX + MARIO_COLUMNS)) &&
                            ((gamestate.balls[j].currentCenterY + gamestate.balls[j].yvel + GOOMBA_SIZE/2  >= gamestate.players[i].currentCenterY) &&
                                    (gamestate.balls[j].currentCenterY + gamestate.balls[j].yvel + GOOMBA_SIZE/2  <= gamestate.players[i].currentCenterY + MARIO_ROWS)) &&
                                    gamestate.balls[j].alive == true && gamestate.balls[j].kill == false){

                    if(gamestate.players[i].num_lives > 0){
                        gamestate.players[i].num_lives--;
                    }
                    if(MAX_NUM_OF_PLAYERS == 1){
                          if(gamestate.players[i].num_lives == 0){
                              gamestate.gameDone = true;
                              gamestate.winner = i;
                              #ifdef SINGLE
                                  G8RTOS_AddThread( &Game2_EndOfGameHost, 0, 0xFFFFFFFF,          "ENDGAMEHOST___" );
                              #endif

                          }
                    }
                    if(MAX_NUM_OF_PLAYERS == 2){
                          if(gamestate.players[0].num_lives == 0){
                              gamestate.winner = 1;
                          }
                          if(gamestate.players[1].num_lives == 0){
                              gamestate.winner = 0;
                          }
                          if(gamestate.players[0].num_lives == 0 && gamestate.players[1].num_lives == 0){
                              gamestate.gameDone = true;
                              #ifdef SINGLE
                                    G8RTOS_AddThread( &Game2_EndOfGameHost, 0, 0xFFFFFFFF,          "ENDGAMEHOST___" );
                              #endif
                          }
                    }
                    gamestate.balls[j].kill = 1;
                }
            }


            gamestate.balls[j].currentCenterX       += gamestate.balls[j].xvel;
            gamestate.balls[j].currentCenterY       += gamestate.balls[j].yvel;
        }
        sleep(35);
    }
}
//common threads
void Game2_DrawObjects()
{
    // store all previous locations of the players to see if
    // it needs to be updated.
    Game2_PrevPlayer_t prevPlayers[MAX_NUM_OF_PLAYERS];
    for (int i = 0; i < MAX_NUM_OF_PLAYERS; i++)
    {
        prevPlayers[i].CenterX = -1; // load fake values to determine first run
        prevPlayers[i].CenterY = -1; // load fake values to determine first run
    }

    while(1)
    {

        // Draw players --------------------
        for (int i = 0; i < MAX_NUM_OF_PLAYERS; i++)
        {
            // This player is on its first run. Draw
            // the entire paddle.
            if ( prevPlayers[i].CenterX == -1 || prevPlayers[i].CenterY == -1  ) {
                Game2_DrawPlayer( &gamestate.players[i], gamestate.players[i].color, i);
                prevPlayers[i].CenterX = gamestate.players[i].currentCenterX;
                prevPlayers[i].CenterY = gamestate.players[i].currentCenterY;
            }

            // if this player has already been drawn, only
            // update the parts that need to be redrawn.
            else
            {
                Game2_UpdatePlayerOnScreen( &prevPlayers[i], &gamestate.players[i], i);
            }
        }

        // Draw the ping pong balls ----------
        // STATE MACHINE..
        for(int i = 0; i < MAX_NUM_OF_BALLS; i++){

            // IF AT ORIGIN, THEN OFFSET Y TO NON OCCUPIED SPACE
            if(previousBalls[i].CenterX < ARENA_MIN_X){
                previousBalls[i].CenterY = 120;
            }

            // ALIVE && !KILL = REDRAW STATE
            if(gamestate.balls[i].alive && !gamestate.balls[i].kill){
                Game2_UpdateBallOnScreen(&previousBalls[i], &gamestate.balls[i], gamestate.balls[i].color);
            }
            // ALIVE && KILL = KILL HOST STATE
            else if(gamestate.balls[i].alive && gamestate.balls[i].kill){
                Game2_UpdateBallOnScreen(&previousBalls[i], &gamestate.balls[i], BACK_COLOR);
                gamestate.balls[i].alive = 0;
                ballCount--;
            }

            // !ALIVE && KILL = KILL CLIENT STATE
            // Because our sleep is 20 ms which is very high, our while loop
            // doesn't run again before the packet is sent. This allows the
            // client to enter this state before the host enters this state
            // and delete the ball on the client side.
            else if ( !gamestate.balls[i].alive && gamestate.balls[i].kill )
            {
                Game2_UpdateBallOnScreen(&previousBalls[i], &gamestate.balls[i], BACK_COLOR);
                gamestate.balls[i].kill = 0;
            }
        }
            //update shroom
            // ALIVE && !KILL = REDRAW STATE
            for(int i = 0; i < MAX_NUM_OF_SHROOMS; i++){
            if(gamestate.shroom[i].alive && !gamestate.shroom[i].kill){
                Game2_UpdateShroomOnScreen(&previousShrooms[i], &gamestate.shroom[i]);
            }
            // ALIVE && KILL = KILL HOST STATE
            else if(gamestate.shroom[i].alive && gamestate.shroom[i].kill){
                Game2_UpdateShroomOnScreen(&previousShrooms[i], &gamestate.shroom[i]);
                gamestate.shroom[i].alive = 0;
                ShroomCount--;
            }

            // !ALIVE && KILL = KILL CLIENT STATE
            // Because our sleep is 20 ms which is very high, our while loop
            // doesn't run again before the packet is sent. This allows the
            // client to enter this state before the host enters this state
            // and delete the ball on the client side.
            else if ( !gamestate.shroom[i].alive && gamestate.shroom[i].kill )
            {
                Game2_UpdateShroomOnScreen(&previousShrooms[i], &gamestate.shroom[i]);
                gamestate.shroom[i].kill = 0;
            }

            // !ALIVE && !KILL = NULL STATE
            // else do nothing .. (ALL 4 STATES ARE USED WITH THESE 2 BOOLEANS)
        }

        // Refresh rate --------------------
        sleep(50);
    }
}
/*
 * Function updates shroom position on screen
 */
void Game2_UpdateShroomOnScreen(Game2_PrevShroom_t * previousShroom, Game2_Shroom_t * currentShroom)
{

    G8RTOS_WaitSemaphore(&LCDREADY);
    LCD_DrawRectangle(previousShroom->CenterX, previousShroom->CenterX + 15, previousShroom->CenterY, previousShroom->CenterY + 15, White);
    G8RTOS_SignalSemaphore(&LCDREADY);

    if(currentShroom->kill != true){
        for(int i = 0; i < SHROOM_SIZE; i++){
            for(int j = 0; j < SHROOM_SIZE; j++){
                G8RTOS_WaitSemaphore(&LCDREADY);

                LCD_SetPoint(currentShroom->currentCenterX + j, currentShroom->currentCenterY + i, shroom_map[i][j]);

                G8RTOS_SignalSemaphore(&LCDREADY);
            }
        }
    }

    previousShroom->CenterX = currentShroom->currentCenterX;
    previousShroom->CenterY = currentShroom->currentCenterY;

    //G8RTOS_SignalSemaphore(&LCDREADY);
}
/*
 * Function updates ball position on screen
 */
void Game2_UpdateBallOnScreen(Game2_PrevBall_t * previousBall, Game2_Ball_t * currentBall, uint16_t outColor)
{

    G8RTOS_WaitSemaphore(&LCDREADY);

    LCD_DrawRectangle(previousBall->CenterX, previousBall->CenterX + 7, previousBall->CenterY, previousBall->CenterY + 7, White);

    G8RTOS_SignalSemaphore(&LCDREADY);

    if(currentBall->kill != true){
        for(int i = 0; i < GOOMBA_SIZE; i++){
            for(int j = 0; j < GOOMBA_SIZE; j++){
                G8RTOS_WaitSemaphore(&LCDREADY);

                LCD_SetPoint(currentBall->currentCenterX + j, currentBall->currentCenterY + i, goomba_map[i][j]);

                G8RTOS_SignalSemaphore(&LCDREADY);
            }
        }
    }

    previousBall->CenterX = currentBall->currentCenterX;
    previousBall->CenterY = currentBall->currentCenterY;

}

/*
 * Draw players given center X center coordinate
 */
uint16_t mario_red_map_left[MARIO_ROWS][MARIO_COLUMNS];
uint16_t mario_green_map_left[MARIO_ROWS][MARIO_COLUMNS];
uint16_t mario_yellow_map_left[MARIO_ROWS][MARIO_COLUMNS];

void Game2_DrawPlayer(Game2_GeneralPlayerInfo_t * player, uint16_t color, int ID)
{
    //G8RTOS_WaitSemaphore(&LCDREADY);
    for (int row = 0; row < MARIO_ROWS; ++row) {
     for (int col = 0; col < MARIO_COLUMNS; ++col) {
      mario_red_map_left[row][MARIO_COLUMNS-1-col] = mario_red_map[row][col];
     }
    }
    //G8RTOS_WaitSemaphore(&LCDREADY);
    for (int row = 0; row < MARIO_ROWS; ++row) {
     for (int col = 0; col < MARIO_COLUMNS; ++col) {
      mario_green_map_left[row][MARIO_COLUMNS-1-col] = mario_green_map[row][col];
     }
    }
    //G8RTOS_WaitSemaphore(&LCDREADY);
    for (int row = 0; row < MARIO_ROWS; ++row) {
     for (int col = 0; col < MARIO_COLUMNS; ++col) {
      mario_yellow_map_left[row][MARIO_COLUMNS-1-col] = mario_yellow_map[row][col];
     }
    }

    if(ID == 0){
        for(int i = 0; i < MARIO_ROWS; i++){
            for(int j = 0; j < MARIO_COLUMNS; j++){
                G8RTOS_WaitSemaphore(&LCDREADY);
                LCD_SetPoint(player->currentCenterX + j, player->currentCenterY + i, mario_red_map[i][j]);
                G8RTOS_SignalSemaphore(&LCDREADY);
            }
        }
    }
    if(ID == 1){
        for(int i = 0; i < MARIO_ROWS; i++){
            for(int j = 0; j < MARIO_COLUMNS; j++){
                G8RTOS_WaitSemaphore(&LCDREADY);
                LCD_SetPoint(player->currentCenterX + j, player->currentCenterY + i, mario_green_map[i][j]);
                G8RTOS_SignalSemaphore(&LCDREADY);
            }
        }
    }
    if(ID == 2) {
        for(int i = 0; i < MARIO_ROWS; i++){
            for(int j = 0; j < MARIO_COLUMNS; j++){
                G8RTOS_WaitSemaphore(&LCDREADY);
                LCD_SetPoint(player->currentCenterX + j, player->currentCenterY + i, mario_yellow_map[i][j]);
                G8RTOS_SignalSemaphore(&LCDREADY);
            }
        }
    }

    //G8RTOS_SignalSemaphore(&LCDREADY);
}

/*
 * Updates player based on current and new center
 *
 */
bool DrawRight;
void Game2_UpdatePlayerOnScreen(Game2_PrevPlayer_t * prevPlayerIn, Game2_GeneralPlayerInfo_t * outPlayer, int ID)
{
        // all smart common area saving was deleted here to generalize for the quiz.
        //G8RTOS_WaitSemaphore(&LCDREADY);
        if(outPlayer->currentCenterX > prevPlayerIn->CenterX){
            DrawRight = true;
        }
        else if (outPlayer->currentCenterX < prevPlayerIn->CenterX){
            DrawRight = false;
        }

        if(ID == 0){

            // // erase the UNCOMMON old player position first
            //LCD_DrawRectangle(prevPlayerIn->CenterX, prevPlayerIn->CenterX + MARIO_COLUMNS,
            //                  prevPlayerIn->CenterY, prevPlayerIn->CenterY + MARIO_ROWS, White);

            for(int i = 0; i < MARIO_ROWS; i++){
                for(int j = 0; j < MARIO_COLUMNS; j++){
                    if(prevPlayerIn->CenterX != outPlayer->currentCenterX || prevPlayerIn->CenterY != outPlayer->currentCenterY){
                        G8RTOS_WaitSemaphore(&LCDREADY);
                        LCD_SetPoint(prevPlayerIn->CenterX + j, prevPlayerIn->CenterY + i, White);
                        //LCD_SetPoint(outPlayer->currentCenterX + j, outPlayer->currentCenterY + i, mario_red_map[i][j]);
                        G8RTOS_SignalSemaphore(&LCDREADY);
                    }
                }
            }

            if(DrawRight == true){
                for(int i = 0; i < MARIO_ROWS; i++){
                    for(int j = 0; j < MARIO_COLUMNS; j++){
                            G8RTOS_WaitSemaphore(&LCDREADY);
                            LCD_SetPoint(outPlayer->currentCenterX + j, outPlayer->currentCenterY + i, mario_red_map[i][j]);
                            G8RTOS_SignalSemaphore(&LCDREADY);
                    }
                }
            }
            else {

                for(int i = 0; i < MARIO_ROWS; i++){
                    for(int j = 0; j < MARIO_COLUMNS; j++){
                            G8RTOS_WaitSemaphore(&LCDREADY);
                            LCD_SetPoint(outPlayer->currentCenterX + j, outPlayer->currentCenterY + i, mario_red_map_left[i][j]);
                            G8RTOS_SignalSemaphore(&LCDREADY);
                    }
                }
            }
            // before erasing the original
            prevPlayerIn->CenterX = outPlayer->currentCenterX;
            prevPlayerIn->CenterY = outPlayer->currentCenterY;
        }
        if(ID == 1){
            for(int i = 0; i < MARIO_ROWS; i++){
                for(int j = 0; j < MARIO_COLUMNS; j++){
                    if(prevPlayerIn->CenterX != outPlayer->currentCenterX || prevPlayerIn->CenterY != outPlayer->currentCenterY){
                        G8RTOS_WaitSemaphore(&LCDREADY);
                        LCD_SetPoint(prevPlayerIn->CenterX + j, prevPlayerIn->CenterY + i, White);
                        G8RTOS_SignalSemaphore(&LCDREADY);
                    }
                }
            }
            if(DrawRight == true){
                for(int i = 0; i < MARIO_ROWS; i++){
                    for(int j = 0; j < MARIO_COLUMNS; j++){
                            G8RTOS_WaitSemaphore(&LCDREADY);
                            LCD_SetPoint(outPlayer->currentCenterX + j, outPlayer->currentCenterY + i, mario_green_map[i][j]);
                            G8RTOS_SignalSemaphore(&LCDREADY);
                    }
                }
            }
            else {

                for(int i = 0; i < MARIO_ROWS; i++){
                    for(int j = 0; j < MARIO_COLUMNS; j++){
                            G8RTOS_WaitSemaphore(&LCDREADY);
                            LCD_SetPoint(outPlayer->currentCenterX + j, outPlayer->currentCenterY + i, mario_green_map_left[i][j]);
                            G8RTOS_SignalSemaphore(&LCDREADY);
                    }
                }
            }
            // before erasing the original
            prevPlayerIn->CenterX = outPlayer->currentCenterX;
            prevPlayerIn->CenterY = outPlayer->currentCenterY;
        }
        if(ID == 2) {
            for(int i = 0; i < MARIO_ROWS; i++){
                for(int j = 0; j < MARIO_COLUMNS; j++){
                    if(prevPlayerIn->CenterX != outPlayer->currentCenterX || prevPlayerIn->CenterY != outPlayer->currentCenterY){
                        G8RTOS_WaitSemaphore(&LCDREADY);
                        LCD_SetPoint(prevPlayerIn->CenterX + j, prevPlayerIn->CenterY + i, White);
                        G8RTOS_SignalSemaphore(&LCDREADY);
                    }
                }
            }
            if(DrawRight == true){
                for(int i = 0; i < MARIO_ROWS; i++){
                    for(int j = 0; j < MARIO_COLUMNS; j++){
                            G8RTOS_WaitSemaphore(&LCDREADY);
                            LCD_SetPoint(outPlayer->currentCenterX + j, outPlayer->currentCenterY + i, mario_yellow_map[i][j]);
                            G8RTOS_SignalSemaphore(&LCDREADY);
                    }
                }
            }
            else {

                for(int i = 0; i < MARIO_ROWS; i++){
                    for(int j = 0; j < MARIO_COLUMNS; j++){
                            G8RTOS_WaitSemaphore(&LCDREADY);
                            LCD_SetPoint(outPlayer->currentCenterX + j, outPlayer->currentCenterY + i, mario_yellow_map_left[i][j]);
                            G8RTOS_SignalSemaphore(&LCDREADY);
                    }
                }
            }
            // before erasing the original
            prevPlayerIn->CenterX = outPlayer->currentCenterX;
            prevPlayerIn->CenterY = outPlayer->currentCenterY;
        }


        //G8RTOS_SignalSemaphore(&LCDREADY);
}

/*
 * Function updates lives and run time
 */
void Game2_UpdatePlayerStatus()
{
    uint8_t status_str[20];
    uint8_t status_p1_str_prev[20];
    uint8_t status_p2_str_prev[20];
    uint8_t status_p3_str_prev[20];
    while(1){
        for(int i = 0; i < MAX_NUM_OF_PLAYERS; i++){
            if(gamestate.players[i].num_lives > 0){
                gamestate.players[i].RunTime = SystemTime/1000 - prev_run_time;
            }
            if(gamestate.players[i].num_lives > 0){
                sprintf(status_str,"L:%u RT:%u", gamestate.players[i].num_lives, gamestate.players[i].RunTime);
            }
            else {
                sprintf(status_str,"DEAD RT:%u", gamestate.players[i].RunTime);
            }
            uint16_t status_offset = i*STATUS_STRING_OFFSET*5.5 + 15;
            G8RTOS_WaitSemaphore(&LCDREADY);
            if(i == 0){
                    if(gamestate.players[i].RunTime < 20){
                        for(int i = 0; i < MARIO_ROWS; i++){
                            for(int j = 0; j < MARIO_COLUMNS; j++){
                                LCD_SetPoint(j, i, mario_red_map[i][j]);
                            }
                        }
                    }
                LCD_Text(status_offset, 0, status_p1_str_prev, White);
            }
            else if(i == 1){
                if(gamestate.players[i].RunTime < 20){
                    for(int i = 0; i < MARIO_ROWS; i++){
                        for(int j = 0; j < MARIO_COLUMNS; j++){
                            LCD_SetPoint(j, i, mario_green_map[i][j]);
                        }
                    }
                }
                LCD_Text(status_offset, 0, status_p2_str_prev, White);
            }
            else{
                if(gamestate.players[i].RunTime < 20){
                    for(int i = 0; i < MARIO_ROWS; i++){
                        for(int j = 0; j < MARIO_COLUMNS; j++){
                            LCD_SetPoint(j, i, mario_yellow_map[i][j]);
                        }
                    }
                }
                LCD_Text(status_offset, 0, status_p3_str_prev, White);
            }
            LCD_Text(status_offset, 0, status_str, Black);
            G8RTOS_SignalSemaphore(&LCDREADY);
            for(int j = 0; j < 20; j++){
                if(i == 0){
                    status_p1_str_prev[j] = status_str[j];
                }
                else if(i == 1){
                    status_p2_str_prev[j] = status_str[j];
                }
                else{
                    status_p3_str_prev[j] = status_str[j];
                }
            }
        }

        sleep(1000);
    }
}

void Game2_IdleThread(){
    while(1){

    }
}

void Game2_EndOfGameHost(){
    // wait for semaphores
    G8RTOS_WaitSemaphore(&LCDREADY);
    G8RTOS_WaitSemaphore(&CC3100_SEMAPHORE);

    G8RTOS_KillAllOthers();

    // force semaphores to reset here..
    // shouldn't be required, but fixes semaphore block bug
    G8RTOS_InitSemaphore(&LCDREADY, 1);
    G8RTOS_InitSemaphore(&CC3100_SEMAPHORE, 1);


    // send response to the client
    gamestate.gameDone = false;
    for(int i = 0; i < MAX_NUM_OF_BALLS; i++){
        gamestate.balls[i].alive = 0;
        gamestate.balls[i].kill = 0;
    }
    ballCount = 0;
    for(int i = 0; i < MAX_NUM_OF_SHROOMS; i++){
        gamestate.shroom[i].alive = 0;
        gamestate.shroom[i].kill = 0;
    }
    ShroomCount = 0;
    gamestate.winner = true;    // this notifies client kill all threads
    //SendData((uint8_t*)&gamestate, gamestate.player.IP_address, sizeof(gamestate));
    // delay for 1 secondish
    G8RTOS_WaitSemaphore(&LCDREADY);

    LCD_Clear(White);
    char status_str[30];
    if(MAX_NUM_OF_PLAYERS == 1){
        for(int i = 0; i < MARIO_ROWS; i++){
            for(int j = 0; j < MARIO_COLUMNS; j++){
                LCD_SetPoint(j, i, mario_red_map[i][j]);
            }
        }
        sprintf(status_str,"You had a run time of: %u", gamestate.players[0].RunTime);
        LCD_Text(15, 0, status_str, Black);
    }
    else {
        if(gamestate.players[0].RunTime > gamestate.players[1].RunTime){
            for(int i = 0; i < MARIO_ROWS; i++){
                for(int j = 0; j < MARIO_COLUMNS; j++){
                    LCD_SetPoint(j, i, mario_red_map[i][j]);
                }
            }
            sprintf(status_str,"Wins with a run time of: %u", gamestate.players[0].RunTime);
            LCD_Text(15, 0, status_str, Black);
        }
        else {
            for(int i = 0; i < MARIO_ROWS; i++){
                for(int j = 0; j < MARIO_COLUMNS; j++){
                    LCD_SetPoint(j, i, mario_green_map[i][j]);
                }
            }
            sprintf(status_str,"Wins with a run time of: %u", gamestate.players[1].RunTime);
            LCD_Text(15, 0, status_str, Black);
        }
    }
    for(int j = 0; j < MAX_NUM_OF_PLAYERS; j++){
        gamestate.players[j].num_lives = 5;
        if(gamestate.players[j].RunTime > prev_run_time){
            prev_run_time = gamestate.players[j].RunTime;
        }
        gamestate.players[j].RunTime = 0;
    }
    G8RTOS_SignalSemaphore(&LCDREADY);


    sleep(2000);
    LCD_Clear(Black);
    menu_initMenu();
    menu_updateMenu();

    // 6. Add GenerateBall, DrawObjects, ReadJoystickHost, SendDataToClient
    //      ReceiveDataFromClient, MoveLEDs (low priority), Idle
    G8RTOS_AddThread( &menu_MenuHost, 15, 0xFFFFFFFF, "MENU_HOST" ); // lowest priority

    // 7. Kill self.
    G8RTOS_KillSelf();
}

/*
 * End of game for the client
 */
void Game2_EndOfGameClient()
{
    while(1)
    {
        // wait for semaphores
        G8RTOS_WaitSemaphore(&LCDREADY);
        G8RTOS_WaitSemaphore(&CC3100_SEMAPHORE);

        G8RTOS_KillAllOthers();

        G8RTOS_InitSemaphore(&LCDREADY, 1);
        G8RTOS_InitSemaphore(&CC3100_SEMAPHORE, 1);


        for(int i = 0; i < MAX_NUM_OF_BALLS; i++){
            gamestate.balls[i].alive = 0;
            gamestate.balls[i].kill = 0;
        }

        ballCount = 0;
        for(int i = 0; i < MAX_NUM_OF_SHROOMS; i++){
            gamestate.shroom[i].alive = 0;
            gamestate.shroom[i].kill = 0;
        }
        ShroomCount = 0;
        LCD_Clear(White);
        char status_str[30];
        if(MAX_NUM_OF_PLAYERS == 1){
            G8RTOS_WaitSemaphore(&LCDREADY);
            for(int i = 0; i < MARIO_ROWS; i++){
                for(int j = 0; j < MARIO_COLUMNS; j++){
                    LCD_SetPoint(j, i, mario_red_map[i][j]);
                }
            }
            sprintf(status_str,"You had a run time of: %u", gamestate.players[0].RunTime);
            LCD_Text(15, 0, status_str, Black);
            G8RTOS_SignalSemaphore(&LCDREADY);
        }
        else {
            if(gamestate.players[0].RunTime > gamestate.players[1].RunTime){
                for(int i = 0; i < MARIO_ROWS; i++){
                    for(int j = 0; j < MARIO_COLUMNS; j++){
                        LCD_SetPoint(j, i, mario_red_map[i][j]);
                    }
                }
                sprintf(status_str,"Wins with a run time of: %u", gamestate.players[0].RunTime);
                LCD_Text(15, 0, status_str, Black);
            }
            else {
                for(int i = 0; i < MARIO_ROWS; i++){
                    for(int j = 0; j < MARIO_COLUMNS; j++){
                        LCD_SetPoint(j, i, mario_green_map[i][j]);
                    }
                }
                sprintf(status_str,"Wins with a run time of: %u", gamestate.players[1].RunTime);
                LCD_Text(15, 0, status_str, Black);
            }
        }
        sleep(2000);
        LCD_Clear(Black);
        menu_initMenu();
        menu_updateMenu();
        G8RTOS_KillSelf();


        // 6. Add GenerateBall, DrawObjects, ReadJoystickHost, SendDataToClient
        //      ReceiveDataFromClient, MoveLEDs (low priority), Idle
        G8RTOS_AddThread( &menu_MenuClient, 15, 0xFFFFFFFF, "MENU_HOST" ); // lowest priority

        // 7. Kill self.
        G8RTOS_KillSelf();
    }
}

/*
 * Thread that sends game state to client
 */
void Game2_SendDataToClient()
{
    while(1)
    {

        // 2. Send packet
        G8RTOS_WaitSemaphore(&CC3100_SEMAPHORE);
        SendData( (uint8_t*)&gamestate, gamestate.player.IP_address, sizeof(gamestate) );
        G8RTOS_SignalSemaphore(&CC3100_SEMAPHORE);

        // 3. Check if the game is done. Add endofgamehost thread if done.
        // This has to be wrapped in the semaphores because the gameDone
        // could otherwise be changed immediately after the data transfer
        // and the client wouldn't know the game ended.
        if ( gamestate.gameDone == true )
            G8RTOS_AddThread(Game2_EndOfGameHost, 0, 0xFFFFFFFF, "END_OF_GAME_HOST");

        sleep(5);
    }
}

/*
 * Thread that receives UDP packets from client
 */
void Game2_ReceiveDataFromClient()
{
    while(1)
    {
        // if the response is greater than 0, valid data was returned
        // to the gamestate. If not, no valid data was returned and
        // thread is put to sleep to avoid deadlock.
        G8RTOS_WaitSemaphore(&CC3100_SEMAPHORE);
        ReceiveData( (uint8_t*)&gamestate.player, sizeof(gamestate.player));
        G8RTOS_SignalSemaphore(&CC3100_SEMAPHORE);

        sleep(2);
    }
}

/*
 * Thread that receives game state packets from host
 */
void  Game2_ReceiveDataFromHost()
{
    while(1)
    {

        // 1. Receive packet from the host
        G8RTOS_WaitSemaphore(&CC3100_SEMAPHORE);
        ReceiveData( (_u8*)&gamestate, sizeof(gamestate));
        G8RTOS_SignalSemaphore(&CC3100_SEMAPHORE);

        // 3. Check if the game is done. Add EndOfGameHost thread if done.
        if ( gamestate.gameDone == true )
            G8RTOS_AddThread(Game2_EndOfGameClient, 0, 0xFFFFFFFF, "END_GAME_CLIENT_");

        sleep(2);
    }
}

/*
 * Thread that sends UDP packets to host
 */
void Game2_SendDataToHost()
{
    while(1)
    {
        G8RTOS_WaitSemaphore(&CC3100_SEMAPHORE);
        SendData( (_u8*)&client_player, HOST_IP_ADDR, sizeof(client_player) );
        G8RTOS_SignalSemaphore(&CC3100_SEMAPHORE);

        sleep(5);
    }
}


