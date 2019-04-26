
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

#include "game2.h"

uint8_t ballCount = 0;
Game2_PrevBall_t previousBalls[MAX_NUM_OF_BALLS];
Game2_SpecificPlayerInfo_t client_player;
Game2_GameState_t gamestate;
Game2_GameState_t packet;
uint32_t Start_time = 0;
int16_t Game2_displacementX = 0;
int16_t Game2_displacementY = 0;
//image arrays
//12x16 image size
#define MARIO_COLUMNS 12
#define MARIO_ROWS 16
uint16_t mario_red_map[16][12] = {

  /*Pixel format: Red: 5 bit, Green: 6 bit, Blue: 5 bit*/
  0xffff, 0xffff, 0xffff, 0xe4f0, 0xe4f0, 0xe4f0, 0xe4f0, 0xe4f0, 0xffff, 0xffff, 0xffff, 0xffff,
  0xffff, 0xffff, 0xe4f0, 0xe4f0, 0xe4f0, 0xe4f0, 0xe4f0, 0xe4f0, 0xe4f0, 0xe4f0, 0xe4f0, 0xffff,
  0xffff, 0xffff, 0xcbbb, 0xcbbb, 0xcbbb, 0x36f7, 0x36f7, 0x0000, 0x36f7, 0xffff, 0xffff, 0xffff,
  0xffff, 0xcbbb, 0x36f7, 0xcbbb, 0x36f7, 0x36f7, 0x36f7, 0x0000, 0x36f7, 0x36f7, 0x36f7, 0xffff,
  0xffff, 0xcbbb, 0x36f7, 0xcbbb, 0xcbbb, 0x36f7, 0x36f7, 0x36f7, 0x0000, 0x36f7, 0x36f7, 0x36f7,
  0xffff, 0xffff, 0xcbbb, 0x36f7, 0x36f7, 0x36f7, 0x36f7, 0x0000, 0x0000, 0x0000, 0x0000, 0xffff,
  0xffff, 0xffff, 0xffff, 0x36f7, 0x36f7, 0x36f7, 0x36f7, 0x36f7, 0x36f7, 0xffff, 0xffff, 0xffff,
  0xffff, 0xffff, 0xe4f0, 0xe4f0, 0x5942, 0xe4f0, 0xe4f0, 0x5942, 0xe4f0, 0xe4f0, 0xffff, 0xffff,
  0xffff, 0xe4f0, 0xe4f0, 0xe4f0, 0x5942, 0xe4f0, 0xe4f0, 0x5942, 0xe4f0, 0xe4f0, 0xe4f0, 0xffff,
  0xe4f0, 0xe4f0, 0xe4f0, 0xe4f0, 0x5942, 0x5942, 0x5942, 0x5942, 0xe4f0, 0xe4f0, 0xe4f0, 0xe4f0,
  0x36f7, 0x36f7, 0xe4f0, 0x5942, 0x80ff, 0x5942, 0x5942, 0x80ff, 0x5942, 0xe4f0, 0x36f7, 0x36f7,
  0x36f7, 0x36f7, 0x36f7, 0x5942, 0x5942, 0x5942, 0x5942, 0x5942, 0x5942, 0x36f7, 0x36f7, 0x36f7,
  0x36f7, 0x36f7, 0x5942, 0x5942, 0x5942, 0x5942, 0x5942, 0x5942, 0x5942, 0x5942, 0x36f7, 0x36f7,
  0xffff, 0xffff, 0x5942, 0x5942, 0x5942, 0xffff, 0xffff, 0x5942, 0x5942, 0x5942, 0xffff, 0xffff,
  0xffff, 0x0388, 0x0388, 0x0388, 0xffff, 0xffff, 0xffff, 0xffff, 0x0388, 0x0388, 0x0388, 0xffff,
  0x0388, 0x0388, 0x0388, 0x0388, 0xffff, 0xffff, 0xffff, 0xffff, 0x0388, 0x0388, 0x0388, 0x0388
};
//add host threads
void Game2_addHostThreads(){
    G8RTOS_AddThread( &Game2_GenerateBall, 20, 0xFFFFFFFF,          "GENERATE_BALL___" );
    G8RTOS_AddThread( &Game2_DrawObjects, 10, 0xFFFFFFFF,           "DRAW_OBJECTS____" );
    G8RTOS_AddThread( &Game2_UpdateBallColors, 20, 0xFFFFFFFF,           "DRAW_OBJECTS____" );
    G8RTOS_AddThread( &Game2_ReadJoystickHost, 20, 0xFFFFFFFF,      "READ_JOYSTICK___" );
    G8RTOS_AddThread( &Game2_UpdatePlayerStatus, 20, 0xFFFFFFFF,              "MOVE_LEDS_______" );
    G8RTOS_AddThread( &Game2_IdleThread, 255, 0xFFFFFFFF,           "IDLE____________" );
}

void Game2_CreateGame(){
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
        gamestate.players[0].currentCenterX = (ARENA_MAX_X - ARENA_MIN_X) >> 3;
        gamestate.players[0].currentCenterY = (ARENA_MAX_Y - ARENA_MIN_Y) >> 3;
        gamestate.players[1].currentCenterX = (ARENA_MAX_X - ARENA_MIN_X) >> 1;
        gamestate.players[1].currentCenterY = (ARENA_MAX_Y - ARENA_MIN_Y) >> 1;
        gamestate.players[2].currentCenterX = ((ARENA_MAX_X - ARENA_MIN_X) >> 1) + gamestate.players[1].currentCenterX;
        gamestate.players[2].currentCenterY = ((ARENA_MAX_Y - ARENA_MIN_Y) >> 1) + gamestate.players[1].currentCenterY;
        gamestate.players[0].color = Cyan;
        gamestate.players[1].color = Magenta;
        gamestate.players[2].color = Blue2;
    }

    Game2_addHostThreads();
    LCD_Clear(BACK_COLOR);
    LCD_DrawRectangle(ARENA_MIN_X, ARENA_MAX_X, ARENA_MIN_Y - 1, ARENA_MIN_Y - 1, White);
    //Game2_InitBoardState(LIVES, 0, 0);
    G8RTOS_KillSelf();

}

void Game2_InitBoardState(uint8_t num_lives, uint16_t P1_runTime, uint16_t P2_runTime){
    uint8_t status_p1_str[30];
    uint8_t status_p2_str[30];
    // draw the current score of the players
    LCD_DrawRectangle(ARENA_MIN_X, ARENA_MAX_X, ARENA_MIN_Y - 2, ARENA_MIN_Y - 5, White);
    if(MAX_NUM_OF_PLAYERS == 1){
        sprintf(status_p1_str,"P1 Lives: %u Run Time: %u", num_lives, P1_runTime);
        LCD_Text(0, 0, status_p1_str, White);
    }
    else {
        //LCD_Text(8*14, 16*6, "B0 -> HOST", LCD_WHITE);
    }
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
            G8RTOS_AddThread(&Game2_MoveBall, 10, 0xFFFFFFFF, "MOVE_BALL_______");
            ballCount++;
        }
        sleep(ballCount * BALL_GEN_SLEEP);
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
        else if ( gamestate.players[0].currentCenterX + PLAYER_OFFSET + 1 > ARENA_MAX_X - 1 )
        {
            gamestate.players[0].currentCenterX = ARENA_MAX_X - PLAYER_OFFSET - 1;
        }

        // PLAYER 1's Y LIMITATIONS---------------------------------------------
        // player center is too far to the left - limit it.
        if ( gamestate.players[0].currentCenterY - PLAYER_OFFSET - 1 <= ARENA_MIN_Y )
        {
            gamestate.players[0].currentCenterY = ARENA_MIN_Y + PLAYER_OFFSET + 1;
        }

        // player center is too far to the right - limit it.
        else if ( gamestate.players[0].currentCenterY + PLAYER_OFFSET + 1 > ARENA_MAX_Y - 1 )
        {
            gamestate.players[0].currentCenterY = ARENA_MAX_Y - PLAYER_OFFSET - 1;
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
void Game2_MoveBall()
{
    // initialize the ball properties for the ball
    // that will be used in this thread.

    Game2_Ball_t * ball;
    Game2_PrevBall_t * previousBall;

    for (int i = 0; i < MAX_NUM_OF_BALLS; i++)
    {
        ball = &gamestate.balls[i];
        previousBall = &previousBalls[i];
        if ( ball->alive == 0 )
        {
            //previousBall->CenterX = 0;
            //previousBall->CenterY = 120;
            ball->alive = 1;
            ball->kill = 0;
            ball->color = Black;
            ball->currentCenterX = BALL_SIZE * (rand() % ((ARENA_MAX_X - ARENA_MIN_X - BALL_SIZE)/BALL_SIZE)) + ARENA_MIN_X + 1;
            ball->currentCenterY = BALL_SIZE * (rand() % (((ARENA_MAX_Y - 80 - ARENA_MIN_Y)/BALL_SIZE)) + 10);
            previousBall->CenterX = ball->currentCenterX;
            previousBall->CenterY = ball->currentCenterY;
            break;
        }
    }

    int16_t xvel = rand() % MAX_BALL_SPEED + 1;
    int16_t yvel = rand() % MAX_BALL_SPEED + 1;

    if(ball->currentCenterY > 120){
        yvel = -yvel;
    }

    while(1){

        // test if hitting the right or left side wall
        if((xvel > 0 && ball->currentCenterX + xvel + BALL_SIZE + 1 >= ARENA_MAX_X) ||
           (xvel < 0 && ball->currentCenterX + xvel - 1 <= ARENA_MIN_X)){
            xvel = -xvel;
        }

        // test if hitting the top or bottom paddle
        if(((yvel > 0 && ball->currentCenterY + yvel + BALL_SIZE + 1 >= ARENA_MAX_Y)) ||
           ((yvel < 0 && ball->currentCenterY + yvel - 1 <= ARENA_MIN_Y))){

            // reverse the y direction
            yvel = -yvel;

            // ball directions for top
            if(yvel > 0){
                if(ball->currentCenterX <= ARENA_MAX_X >>1){
                    xvel -= 1;
                    if(xvel > 0){
                        yvel += 1;
                    }
                    else{
                        yvel -= 1;
                    }
                }
                else if(ball->currentCenterX > ARENA_MAX_X>>1){
                    xvel += 1;
                    if(xvel < 0){
                        yvel += 1;
                    }
                    else{
                        yvel -= 1;
                    }
                }
                if(yvel == 0){  yvel = 1;}
            }

            // ball directions for bottom
            else{
                if(ball->currentCenterX <= ARENA_MAX_X>>1){
                    xvel += 1;
                    if(xvel > 0){
                        yvel += 1;
                    }
                    else{
                        yvel -= 1;
                    }
                }
                else if(ball->currentCenterX > ARENA_MAX_X>>1){
                    xvel -= 1;
                    if(xvel < 0){
                        yvel += 1;
                    }
                    else{
                        yvel -= 1;
                    }
                }
                if(yvel == 0){  yvel = -1;}
            }
        }

        /*
        else if((yvel > 0 && ball->currentCenterY + yvel + BALL_SIZE + 1 >= ARENA_MAX_Y - PADDLE_WID) ||
                (yvel < 0 && ball->currentCenterY + yvel <= ARENA_MIN_Y + PADDLE_WID)){
            // kill this ball, and score the point
            // set ball to dead
            ball->alive = 1;
            // score points
            if(yvel > 0 && (ball->color == LCD_BLUE || ball->color == LCD_RED)){
                gamestate.LEDScores[1] += 1;
            }
            else if(ball->color == LCD_BLUE || ball->color == LCD_RED){
                gamestate.LEDScores[0] += 1;
            }
            if(gamestate.LEDScores[0] == 8 || gamestate.LEDScores[1] == 8){
   #ifdef SINGLE
                G8RTOS_AddThread( &EndOfGameHost, 0, 0xFFFFFFFF,          "KillAll___" );
   #endif
              gamestate.gameDone = true;
            }
            ball->kill = 1;
            G8RTOS_KillSelf();
        }
        */

        ball->currentCenterX       += xvel;
        ball->currentCenterY       += yvel;

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
                Game2_DrawPlayer( &gamestate.players[i], gamestate.players[i].color );
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
                Game2_UpdateBallOnScreen(&previousBalls[i], &gamestate.balls[i], LCD_BLACK);
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
                Game2_UpdateBallOnScreen(&previousBalls[i], &gamestate.balls[i], LCD_BLACK);
                gamestate.balls[i].kill = 0;
            }

            // !ALIVE && !KILL = NULL STATE
            // else do nothing .. (ALL 4 STATES ARE USED WITH THESE 2 BOOLEANS)
        }

        // Refresh rate --------------------
        sleep(40);
    }
}

void Game2_UpdateBallColors(){

    while(1){
            for(int i = 0; i < MAX_NUM_OF_BALLS; i++){
                gamestate.balls[i].color = Orange;
            }
            sleep(2000);
            for(int i = 0; i < MAX_NUM_OF_BALLS; i++){
                gamestate.balls[i].color = Blue;
            }
            sleep(2000);
    }
}

/*
 * Function updates ball position on screen
 */
void Game2_UpdateBallOnScreen(Game2_PrevBall_t * previousBall, Game2_Ball_t * currentBall, uint16_t outColor)
{

    G8RTOS_WaitSemaphore(&LCDREADY);

    // erase the old ball first
    LCD_fillCircle(previousBall->CenterX, previousBall->CenterY, BALL_SIZE, Black);
    //LCD_DrawRectangle(previousBall->CenterX,previousBall->CenterX + BALL_SIZE, previousBall->CenterY, previousBall->CenterY + BALL_SIZE, Black);
    // before erasing the original
    previousBall->CenterX = currentBall->currentCenterX;
    previousBall->CenterY = currentBall->currentCenterY;
    // draw the new ball next

    LCD_fillCircle(currentBall->currentCenterX, currentBall->currentCenterY, BALL_SIZE, outColor);
    //LCD_DrawRectangle(currentBall->currentCenterX,currentBall->currentCenterX + BALL_SIZE, currentBall->currentCenterY, currentBall->currentCenterY + BALL_SIZE, Blue);


    G8RTOS_SignalSemaphore(&LCDREADY);
}

/*
 * Draw players given center X center coordinate
 */
void Game2_DrawPlayer(Game2_GeneralPlayerInfo_t * player, uint16_t color)
{
    G8RTOS_WaitSemaphore(&LCDREADY);

    LCD_DrawRectangle(player->currentCenterX - PLAYER_OFFSET, player->currentCenterX + PLAYER_OFFSET,
                      player->currentCenterY - PLAYER_OFFSET, player->currentCenterY + PLAYER_OFFSET, color);

    G8RTOS_SignalSemaphore(&LCDREADY);
}

/*
 * Updates player based on current and new center
 *
 */
void Game2_UpdatePlayerOnScreen(Game2_PrevPlayer_t * prevPlayerIn, Game2_GeneralPlayerInfo_t * outPlayer, int ID)
{
    // only update the player if it's center moved
    if ( prevPlayerIn->CenterX != outPlayer->currentCenterX || prevPlayerIn->CenterY != outPlayer->currentCenterY )
    {
        // all smart common area saving was deleted here to generalize for the quiz.
        G8RTOS_WaitSemaphore(&LCDREADY);


        if(ID == 0){
            for(int i = 0; i < MARIO_ROWS; i++){
                for(int j = 0; j < MARIO_ROWS; j++){
                    LCD_SetPoint(prevPlayerIn->CenterX, prevPlayerIn->CenterY, BACK_COLOR);
                    //LCD_SetPoint(outPlayer->currentCenterX, outPlayer->currentCenterY, mario_red_map[i][j]);
                }
            }

            for(int i = 0; i < MARIO_ROWS; i++){
                for(int j = 0; j < MARIO_ROWS; j++){
                    //LCD_SetPoint(prevPlayerIn->CenterX, prevPlayerIn->CenterY, BACK_COLOR);
                    LCD_SetPoint(outPlayer->currentCenterX, outPlayer->currentCenterY, mario_red_map[i][j]);
                }
            }
        }
        else if(ID == 1){
            for(int i = 0; i < MARIO_ROWS; i++){
                for(int j = 0; j < MARIO_ROWS; j++){
                    LCD_SetPoint(prevPlayerIn->CenterX, prevPlayerIn->CenterY, BACK_COLOR);
                    //LCD_SetPoint(outPlayer->currentCenterX, outPlayer->currentCenterY, mario_red_map[i][j]);
                }
            }

            for(int i = 0; i < MARIO_ROWS; i++){
                for(int j = 0; j < MARIO_ROWS; j++){
                    //LCD_SetPoint(prevPlayerIn->CenterX, prevPlayerIn->CenterY, BACK_COLOR);
                    LCD_SetPoint(outPlayer->currentCenterX, outPlayer->currentCenterY, mario_red_map[i][j]);
                }
            }
        }
        else {
            for(int i = 0; i < MARIO_ROWS; i++){
                for(int j = 0; j < MARIO_ROWS; j++){
                    LCD_SetPoint(prevPlayerIn->CenterX, prevPlayerIn->CenterY, BACK_COLOR);
                    //LCD_SetPoint(outPlayer->currentCenterX, outPlayer->currentCenterY, mario_red_map[i][j]);
                }
            }

            for(int i = 0; i < MARIO_ROWS; i++){
                for(int j = 0; j < MARIO_ROWS; j++){
                    //LCD_SetPoint(prevPlayerIn->CenterX, prevPlayerIn->CenterY, BACK_COLOR);
                    LCD_SetPoint(outPlayer->currentCenterX, outPlayer->currentCenterY, mario_red_map[i][j]);
                }
            }
        }

        /*
        // // erase the UNCOMMON old player position first
        LCD_DrawRectangle(prevPlayerIn->CenterX - PLAYER_OFFSET, prevPlayerIn->CenterX + PLAYER_OFFSET,
                          prevPlayerIn->CenterY - PLAYER_OFFSET, prevPlayerIn->CenterY + PLAYER_OFFSET, LCD_BLACK);



        // // draw the UNCOMMON updated player position
        LCD_DrawRectangle(outPlayer->currentCenterX - PLAYER_OFFSET, outPlayer->currentCenterX + PLAYER_OFFSET,
                          outPlayer->currentCenterY - PLAYER_OFFSET, outPlayer->currentCenterY + PLAYER_OFFSET, outPlayer->color);
         */
        // wrapping the data update doesn't allow the players to update twice
        // before erasing the original
        prevPlayerIn->CenterX = outPlayer->currentCenterX;
        prevPlayerIn->CenterY = outPlayer->currentCenterY;

        G8RTOS_SignalSemaphore(&LCDREADY);
    }
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
                gamestate.players[i].RunTime = SystemTime/1000;
            }
            if(gamestate.players[i].num_lives > 0){
                sprintf(status_str,"P%u L: %u RT: %u", i+1, gamestate.players[i].num_lives, gamestate.players[i].RunTime);
            }
            else {
                sprintf(status_str,"P%u DEAD RT: %u", i+1, gamestate.players[i].RunTime);
            }
            uint16_t status_offset = i*STATUS_STRING_OFFSET*8;
            G8RTOS_WaitSemaphore(&LCDREADY);
            if(i == 0){
                LCD_Text(0, status_offset, status_p1_str_prev, Black);
            }
            if(i == 1){
                LCD_Text(0, status_offset, status_p2_str_prev, Black);
            }
            else{
                LCD_Text(0, status_offset, status_p3_str_prev, Black);
            }
            LCD_Text(0, status_offset, status_str, gamestate.players[i].color);
            G8RTOS_SignalSemaphore(&LCDREADY);
            for(int j = 0; j < 20; j++){
                if(i == 0){
                    status_p1_str_prev[j] = status_str[j];
                }
                if(i == 1){
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
