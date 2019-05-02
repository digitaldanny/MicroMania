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

#include "game1.h"

/*
 ************************************************************** Sprite Maps ********************************************************************************
 */

const uint16_t Flappy_map[12][17] = {

  /*Pixel format: Red: 5 bit, Green: 6 bit, Blue: 5 bit*/
  0x07FF, 0x07FF, 0x07FF, 0x07FF, 0x07FF, 0x07FF, 0x5A08, 0x5A08, 0x5A08, 0x5A08, 0x5A08, 0x5A08, 0x07FF, 0x07FF, 0x07FF, 0x07FF, 0x07FF,
  0x07FF, 0x07FF, 0x07FF, 0x07FF, 0x5A08, 0x5A08, 0xFFE9, 0xFFE9, 0xFFE9, 0x5A08, 0xffff, 0xffff, 0x5A08, 0x07FF, 0x07FF, 0x07FF, 0x07FF,
  0x07FF, 0x07FF, 0x07FF, 0x5A08, 0xFFE9, 0xFFE9, 0xFFA2, 0xFFA2, 0x5A08, 0xffff, 0xffff, 0xffff, 0xffff, 0x5A08, 0x07FF, 0x07FF, 0x07FF,
  0x07FF, 0x5A08, 0x5A08, 0x5A08, 0x5A08, 0xFFA2, 0xFFA2, 0xFFA2, 0x5A08, 0xffff, 0xffff, 0xffff, 0x5A08, 0xffff, 0x5A08, 0x07FF, 0x07FF,
  0x5A08, 0xffff, 0xffff, 0xffff, 0xffff, 0x5A08, 0xFFA2, 0xFFA2, 0x5A08, 0xffff, 0xffff, 0xffff, 0x5A08, 0xffff, 0x5A08, 0x07FF, 0x07FF,
  0x5A08, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0x5A08, 0xFFA2, 0xFFA2, 0x5A08, 0xffff, 0xffff, 0xffff, 0xffff, 0x5A08, 0x07FF, 0x07FF,
  0x5A08, 0xFFE9, 0xffff, 0xffff, 0xffff, 0xFFE9, 0x5A08, 0xFFA2, 0xFFA2, 0xFFA2, 0x5A08, 0x5A08, 0x5A08, 0x5A08, 0x5A08, 0x5A08, 0x07FF,
  0x07FF, 0x5A08, 0xFFE9, 0xFFE9, 0xFFE9, 0x5A08, 0xFFA2, 0xFFA2, 0xFFA2, 0x5A08, 0xFAC1, 0xFAC1, 0xFAC1, 0xFAC1, 0xFAC1, 0xFAC1, 0x5A08,
  0x07FF, 0x07FF, 0x5A08, 0x5A08, 0x5A08, 0xFE01, 0xFE01, 0xFE01, 0x5A08, 0xFAC1, 0x5A08, 0x5A08, 0x5A08, 0x5A08, 0x5A08, 0x5A08, 0x5A08,
  0x07FF, 0x07FF, 0x5A08, 0xFE01, 0xFE01, 0xFE01, 0xFE01, 0xFE01, 0xFE01, 0x5A08, 0xFAC1, 0xFAC1, 0xFAC1, 0xFAC1, 0xFAC1, 0x5A08, 0x07FF,
  0x07FF, 0x07FF, 0x07FF, 0x5A08, 0x5A08, 0xFE01, 0xFE01, 0xFE01, 0xFE01, 0xFE01, 0x5A08, 0x5A08, 0x5A08, 0x5A08, 0x5A08, 0x07FF, 0x07FF,
  0x07FF, 0x07FF, 0x07FF, 0x07FF, 0x07FF, 0x5A08, 0x5A08, 0x5A08, 0x5A08, 0x5A08, 0x07FF, 0x07FF, 0x07FF, 0x07FF, 0x07FF, 0x07FF, 0x07FF,
};


/*
 ******************************************************** LOCALS ************************************************************
 */

game1_gamestate_t gamestate;
game1_HostToClient_t HostToClient;
game1_ClientToHost_t ClientToHost;
bool clientButtonPressed = 0;

uint8_t playerTurn = 0;
uint8_t host_score;
uint8_t client_score;



uint16_t Flappy_Y_Old       = MAX_SCREEN_Y>>1;
int16_t Pipe_X_Old         = MAX_SCREEN_X - 1 + 20;
uint8_t score_Old = 0;




/*
 * Thread for client to join game
 */
void game1_JoinGame(){

    // initialize all local IP and Client to Host data
    ClientToHost.IP_address = getLocalIP();
    ClientToHost.acknowledge = false;
    ClientToHost.joined = false;
    ClientToHost.playerNumber = 1;
    ClientToHost.buttonPressed = false;

    // initialize buttons
    common_buttons_init();

    // initialize all parameters to start new game
    HostToClient.gamestate.BC_Color           = 0x07FF;
    HostToClient.gamestate.Pipe_Color         = 0x5EA6;
    HostToClient.gamestate.Flappy_X           = MAX_SCREEN_X>>2;
    HostToClient.gamestate.Flappy_Y           = MAX_SCREEN_Y>>1;
    HostToClient.gamestate.Flappy_Y_Velocity   = 0;
    Flappy_Y_Old       = MAX_SCREEN_Y>>1;

    // initialize pipe locations
    HostToClient.gamestate.Pipe_Width         = 40;
    HostToClient.gamestate.Pipe_Gap           = 100;
    HostToClient.gamestate.Pipe_X             = MAX_SCREEN_X - 1 + 20;
    HostToClient.gamestate.Pipe_Y             = MAX_SCREEN_Y>>1;

    // initialize old parameters for memory
    Flappy_Y_Old       = MAX_SCREEN_Y>>1;
    Pipe_X_Old         = MAX_SCREEN_X - 1 + 20;
    score_Old = 0;

    // reset the score
    HostToClient.gamestate.score = 0;

    // clear screen to back ground color
    LCD_Clear(HostToClient.gamestate.BC_Color);

    // show initial position of the bird
    for(int i = 0; i < 12; i++){
        for(int j = 0; j < 17; j++){
            LCD_SetPoint( HostToClient.gamestate.Flappy_X + j , HostToClient.gamestate.Flappy_Y + i , Flappy_map[i][j]);

        }
    }

    // add all client threads
    G8RTOS_AddThread(&game1_DrawObjects, 10, 0xFFFFFFFF, "DrawObjects");
    G8RTOS_AddThread(&game1_IdleThread, 0xff, 0xFFFFFFFF, "IdleThread");
    G8RTOS_AddThread(&game1_ReceiveDataFromHost, 15, 0xFFFFFFFF, "RecieveData");
    G8RTOS_AddThread(&game1_SendDataToHost, 15, 0xFFFFFFFF, "SendData");

    G8RTOS_KillSelf();
}

/*
 * Thread that receives game state packets from host
 */
void game1_ReceiveDataFromHost(){
    while(1)
    {
        // Receive packet from the host
        G8RTOS_WaitSemaphore(&CC3100_SEMAPHORE);
        ReceiveData( (_u8*)&HostToClient, sizeof(HostToClient));
        G8RTOS_SignalSemaphore(&CC3100_SEMAPHORE);

        // Check if the game is done. Add EndOfGameClient thread if done.
        if ( HostToClient.game1_endgame > 0 )
            G8RTOS_AddThread(&game1_EndOfGameClient, 0, 0xFFFFFFFF, "END_CLIENT_");

        sleep(2);
    }
}

/*
 * Thread that sends UDP packets to host
 */
void game1_SendDataToHost(){
    while(1)
    {
        // set the button parameter to the buttonUp value
        ClientToHost.buttonPressed = buttonUP_pressed;
        // reset buttonUp pressed
        buttonUP_pressed = 0;

        // send client data to host
        G8RTOS_WaitSemaphore(&CC3100_SEMAPHORE);
        SendData( (_u8*)&ClientToHost, HOST_IP_ADDR, sizeof(ClientToHost) );
        G8RTOS_SignalSemaphore(&CC3100_SEMAPHORE);

        sleep(5);
    }
}

/*
 * Thread to read client's joystick
 */
void game1_ReadJoystickClient(){
}

/*
 * End of game for the client
 */
void game1_EndOfGameClient(){

    // reset the old positions of the pipes
    Pipe_X_Old         = MAX_SCREEN_X - 1 + 20;
    Flappy_Y_Old       = MAX_SCREEN_Y>>1;
    Pipe_X_Old         = MAX_SCREEN_X - 1 + 20;

    // check for debouncing, if not supposed to be here, exit
    if(HostToClient.game1_returnToMenu == 0 && playerTurn == 1){
        HostToClient.game1_endgame = false;
        G8RTOS_KillSelf();
    }

    // wait for semaphores, then kill all others
    G8RTOS_WaitSemaphore(&CC3100_SEMAPHORE);
    G8RTOS_KillAllOthers();
    G8RTOS_InitSemaphore(&CC3100_SEMAPHORE, 1);


    // clear the screen to black and wait
    LCD_Clear(LCD_BLACK);
    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < 1000000; j++);
    }


    // if the client has played, then return to menu and show the winner on the screen
    if(HostToClient.game1_returnToMenu == 1){
        if(host_score > HostToClient.gamestate.score){
            // host won
            LCD_Clear(LCD_RED);
            LCD_Text((MAX_SCREEN_X>>1) - 20, (MAX_SCREEN_Y>>1), "PLAYER 1 WINS", LCD_WHITE);
            for (int i = 0; i < 2; i++)
            {
                for (int j = 0; j < 1000000; j++);
            }
        }
        else{
            // client won
            LCD_Clear(LCD_BLUE);
            LCD_Text((MAX_SCREEN_X>>1) - 20, (MAX_SCREEN_Y>>1), "PLAYER 2 WINS", LCD_WHITE);
            for (int i = 0; i < 2; i++)
            {
                for (int j = 0; j < 1000000; j++);
            }
        }

        // reset the client parameters
        ClientToHost.acknowledge = false;
        ClientToHost.joined = false;
        HostToClient.game1_endgame = false;
        playerTurn = 0;

        // add the menu for client
        G8RTOS_AddThread(&menu_MenuClient, 10, 0xFFFFFFFF, "MenuClient");
    }

    // host has finished session, its clients turn now
    else{
        // save host score, and add restart the flappy bird, but this time read the button pressed and send to host
        host_score = HostToClient.gamestate.score;
        G8RTOS_AddThread(&game1_JoinGame, 10, 0xFFFFFFFF, "CreateGame");

        // reset the end game parameter
        HostToClient.game1_endgame = false;

        // set player turn to 1
        playerTurn = 1;
    }

    G8RTOS_KillSelf();
}

/*********************************************** Client Threads *********************************************************************/


/*********************************************** Host Threads *********************************************************************/
/*
 * Thread for the host to create a game
 */
void game1_CreateGame(){

    // initialize the buttons on board
    common_buttons_init();

    // reset all the gamestate variables
    HostToClient.gamestate.BC_Color           = 0x07FF;
    HostToClient.gamestate.Pipe_Color         = 0x5EA6;
    HostToClient.gamestate.Flappy_X           = MAX_SCREEN_X>>2;
    HostToClient.gamestate.Flappy_Y           = MAX_SCREEN_Y>>1;
    HostToClient.gamestate.Flappy_Y_Velocity   = 0;
    Flappy_Y_Old       = MAX_SCREEN_Y>>1;

    // pipe locations
    HostToClient.gamestate.Pipe_Width         = 40;
    HostToClient.gamestate.Pipe_Gap           = 100;
    HostToClient.gamestate.Pipe_X             = MAX_SCREEN_X - 1 + 20;
    HostToClient.gamestate.Pipe_Y             = MAX_SCREEN_Y>>1;
    Pipe_X_Old         = MAX_SCREEN_X - 1 + 20;

    // reset the score and old score
    HostToClient.gamestate.score = 0;
    score_Old = 0;


    // write the background color to the screen
    LCD_Clear(HostToClient.gamestate.BC_Color);

    // write initial bird location to screen
    for(int i = 0; i < 12; i++){
        for(int j = 0; j < 17; j++){
            LCD_SetPoint( HostToClient.gamestate.Flappy_X + j , HostToClient.gamestate.Flappy_Y + i , Flappy_map[i][j]);

        }
    }

    /*
     * Add necessary host threads for reading the buttons, moving the bird, and drawing
     */
    G8RTOS_AddThread(&game1_MoveBird, 15, 0xFFFFFFFF, "MoveBird");
    G8RTOS_AddThread(&game1_MovePipes, 15, 0xFFFFFFFF, "MovePipes");
    G8RTOS_AddThread(&game1_DrawObjects, 10, 0xFFFFFFFF, "DrawObjects");
    G8RTOS_AddThread(&game1_IdleThread, 0xff, 0xFFFFFFFF, "IdleThread");
    G8RTOS_AddThread(&game1_SendDataToClient, 15, 0xFFFFFFFF, "SendData");
    G8RTOS_AddThread(&game1_ReceiveDataFromClient, 15, 0xFFFFFFFF, "RecieveData");

    G8RTOS_KillSelf();

}

/*
 * Thread that moves the location of the bird based on gravity and button press
 */
void game1_MoveBird(){
    // sleep 500 to let players get ready to move
    sleep(500);

    while(1){
        // add the up/down velocity to the bird
        HostToClient.gamestate.Flappy_Y += HostToClient.gamestate.Flappy_Y_Velocity;

        // see if the bird fell off the top or bottom of screen, if true then end game
        if(HostToClient.gamestate.Flappy_Y >= MAX_SCREEN_Y - 12 || HostToClient.gamestate.Flappy_Y <= MIN_SCREEN_Y ||
          ((HostToClient.gamestate.Flappy_X + 17 >= HostToClient.gamestate.Pipe_X - (HostToClient.gamestate.Pipe_Width>>1) && HostToClient.gamestate.Flappy_X <= HostToClient.gamestate.Pipe_X + (HostToClient.gamestate.Pipe_Width>>1)) &&
           (HostToClient.gamestate.Flappy_Y + 12 >= HostToClient.gamestate.Pipe_Y + (HostToClient.gamestate.Pipe_Gap>>1) || HostToClient.gamestate.Flappy_Y <= HostToClient.gamestate.Pipe_Y - (HostToClient.gamestate.Pipe_Gap>>1)))){
            HostToClient.game1_endgame = 1;
            G8RTOS_AddThread(&game1_EndOfGameHost, 1, 0xFFFFFFFF, "EndGame");
        }

        // increment velocity downward for gravity
        if(HostToClient.gamestate.Flappy_Y_Velocity < 20)
            HostToClient.gamestate.Flappy_Y_Velocity += 2;

        // set max upward limit to abs(10)
        if(HostToClient.gamestate.Flappy_Y_Velocity < -10)
            HostToClient.gamestate.Flappy_Y_Velocity = -10;

        // if playerTurn is 1, then client is playing, if 0 then host is playing
        // client turn
        if(playerTurn){

            // see if client pressed button
            if(clientButtonPressed){

                // reset the button pressed parameter
                clientButtonPressed = false;
                // incriment the upward velocity
                HostToClient.gamestate.Flappy_Y_Velocity = -10;
            }
        }

        // host turn
        else{

            // see if host pressed button
            if(buttonUP_pressed){

                // reset the button pressed parameter
                buttonUP_pressed = 0;
                // increment the upward velocity
                HostToClient.gamestate.Flappy_Y_Velocity = -10;
            }
        }

        sleep(30);
    }
}

void game1_MovePipes(){

    // initialize the random number generation
    uint16_t pipe_velocity = 4;
    while(1){
        //fell off left side of screen
        if(HostToClient.gamestate.Pipe_X + HostToClient.gamestate.Pipe_Width>>1 < MIN_SCREEN_X){

            // create new pipe
            HostToClient.gamestate.Pipe_X = MAX_SCREEN_X - 1 + (HostToClient.gamestate.Pipe_Width>>1);

            // if the score is less than 9, decrease the height between pipes
            if(HostToClient.gamestate.score < 9){
                HostToClient.gamestate.Pipe_Gap -= 4;
            }

            // limit max velocity, and increase with every pipe up to 10
            if(pipe_velocity < 10){
                pipe_velocity += 1;
            }

            // get new random location for the opening in pipe
            HostToClient.gamestate.Pipe_Y = (rand()%100) + ((MAX_SCREEN_Y - 1 - 100)>>1);

            // update score
            HostToClient.gamestate.score += 1;
        }

        // set the velocity
        HostToClient.gamestate.Pipe_X -= pipe_velocity;
        sleep(30);
    }
}

/*
 * Thread that sends game state to client
 */
void game1_SendDataToClient(){
    while (1)
    {
        // send host data and gamestate to client
        G8RTOS_WaitSemaphore(&CC3100_SEMAPHORE);
        SendData( (uint8_t*)&HostToClient, HostToClient.client.IP_address, sizeof(HostToClient) );
        G8RTOS_SignalSemaphore(&CC3100_SEMAPHORE);

        // reset the endgame
        HostToClient.game1_endgame = false;

        sleep(5);
    }
}

/*
 * Thread that receives UDP packets from client
 */
void game1_ReceiveDataFromClient(){
    while(1)
    {
        // if the response is greater than 0, valid data was returned
        // to the gamestate. If not, no valid data was returned and
        // thread is put to sleep to avoid deadlock.
        G8RTOS_WaitSemaphore(&CC3100_SEMAPHORE);
        ReceiveData( (uint8_t*)&HostToClient.client, sizeof(HostToClient.client));
        G8RTOS_SignalSemaphore(&CC3100_SEMAPHORE);

        // see if the client pushed the button
        if(HostToClient.client.buttonPressed){
            // reset the client button pressed parameter
            HostToClient.client.buttonPressed = 0;
            // set the local client button pressed parameter to 1
            clientButtonPressed = 1;
        }

        sleep(2);
    }
}

/*
 * Thread to read host's joystick
 */
void game1_ReadJoystickHost(){

}

/*
 * End of game for the host
 */
void game1_EndOfGameHost(){

    // if client has gone, set the return to menu parameter to true and sleep for sending to client
    if(playerTurn){
        HostToClient.game1_returnToMenu = 1;
    }
    sleep(5);

    // wait for semaphores and kill all other threads
    G8RTOS_WaitSemaphore(&CC3100_SEMAPHORE);
    G8RTOS_KillAllOthers();
    G8RTOS_InitSemaphore(&CC3100_SEMAPHORE, 1);

    // reset the endgame parameter
    HostToClient.game1_endgame = false;

    // set screen to black and delay 1 second
    LCD_Clear(LCD_BLACK);
    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < 1000000; j++);
    }

    // host finished turn
    if(playerTurn == 0){
        // remember the host score
        host_score = HostToClient.gamestate.score;
        G8RTOS_AddThread(&game1_CreateGame, 10, 0xFFFFFFFF, "CreateGame");
        // set the turn to client
        playerTurn = 1;
    }

    // client finished turn
    else{
        // reset the return to menu parameter
        HostToClient.game1_returnToMenu = 0;

        // see who won, HostToClient.gamestate.score is client score
        if(host_score > HostToClient.gamestate.score){
            // host won, display winner
            LCD_Clear(LCD_RED);
            LCD_Text((MAX_SCREEN_X>>1) - 20, (MAX_SCREEN_Y>>1), "PLAYER 1 WINS", LCD_WHITE);
            for (int i = 0; i < 2; i++)
            {
                for (int j = 0; j < 1000000; j++);
            }
        }
        else{
            // client won, display winner
            LCD_Clear(LCD_BLUE);
            LCD_Text((MAX_SCREEN_X>>1) - 20, (MAX_SCREEN_Y>>1), "PLAYER 2 WINS", LCD_WHITE);
            for (int i = 0; i < 2; i++)
            {
                for (int j = 0; j < 1000000; j++);
            }
        }

        // reset the player turn to host
        playerTurn = 0;
        // return to menu
        G8RTOS_AddThread(&menu_MenuHost, 10, 0xFFFFFFFF, "MenuHost");
    }

    G8RTOS_KillSelf();
}

/*********************************************** Host Threads *********************************************************************/


/*********************************************** Common Threads *********************************************************************/
/*
 * Idle thread
 */
void game1_IdleThread(){
    while(1);

}

/*
 * Thread to draw all the objects in the game
 */
void game1_DrawObjects(){
    // wait until a correct background color is retrieved
    while(HostToClient.gamestate.BC_Color == 0x0000);

    while(1){

        // Pipes erasing and drawing
        if((HostToClient.gamestate.Pipe_X + (HostToClient.gamestate.Pipe_Width>>1)) < MIN_SCREEN_X){
            // erase the pipe falling off left of screen
            LCD_DrawRectangle(MIN_SCREEN_X, Pipe_X_Old + (HostToClient.gamestate.Pipe_Width>>1), MIN_SCREEN_Y, MAX_SCREEN_Y - 1, HostToClient.gamestate.BC_Color);
        }
        else if((Pipe_X_Old + (HostToClient.gamestate.Pipe_Width>>1)) < MAX_SCREEN_X){
            //Erasing the Pipe moving across the screen
            LCD_DrawRectangle(HostToClient.gamestate.Pipe_X + (HostToClient.gamestate.Pipe_Width>>1) + 1, Pipe_X_Old + (HostToClient.gamestate.Pipe_Width>>1), MIN_SCREEN_Y, MAX_SCREEN_Y - 1, HostToClient.gamestate.BC_Color);
            LCD_DrawRectangle(HostToClient.gamestate.Pipe_X + (HostToClient.gamestate.Pipe_Width>>1) - 1, HostToClient.gamestate.Pipe_X + (HostToClient.gamestate.Pipe_Width>>1), MIN_SCREEN_Y, HostToClient.gamestate.Pipe_Y - (HostToClient.gamestate.Pipe_Gap>>1) - 21, HostToClient.gamestate.BC_Color);
            LCD_DrawRectangle(HostToClient.gamestate.Pipe_X + (HostToClient.gamestate.Pipe_Width>>1) - 1, HostToClient.gamestate.Pipe_X + (HostToClient.gamestate.Pipe_Width>>1), HostToClient.gamestate.Pipe_Y + (HostToClient.gamestate.Pipe_Gap>>1) + 21, MAX_SCREEN_Y - 1, HostToClient.gamestate.BC_Color);
        }
        else if((HostToClient.gamestate.Pipe_X + (HostToClient.gamestate.Pipe_Width>>1)) < MAX_SCREEN_X){
            // Erase the pipe just coming onto the screen
            LCD_DrawRectangle(HostToClient.gamestate.Pipe_X + (HostToClient.gamestate.Pipe_Width>>1), MAX_SCREEN_X - 1, MIN_SCREEN_Y, MAX_SCREEN_Y - 1, HostToClient.gamestate.BC_Color);
        }

        // draw the pipes
        if((HostToClient.gamestate.Pipe_X + (HostToClient.gamestate.Pipe_Width>>1)) >= MAX_SCREEN_X){

            // first see if the old x value is on the screen, if it is then draw the front of the pipe, if not then
            // draw the horizontal lines and the green in the middle
            if((Pipe_X_Old - (HostToClient.gamestate.Pipe_Width>>1)) >= MAX_SCREEN_X){
                // draw pipe coming onto the right side of screen
                LCD_DrawRectangle(HostToClient.gamestate.Pipe_X - (HostToClient.gamestate.Pipe_Width>>1), MAX_SCREEN_X - 1, MIN_SCREEN_Y, HostToClient.gamestate.Pipe_Y - (HostToClient.gamestate.Pipe_Gap>>1), HostToClient.gamestate.Pipe_Color);
                LCD_DrawRectangle(HostToClient.gamestate.Pipe_X - (HostToClient.gamestate.Pipe_Width>>1), MAX_SCREEN_X - 1, HostToClient.gamestate.Pipe_Y + (HostToClient.gamestate.Pipe_Gap>>1), MAX_SCREEN_Y - 1, HostToClient.gamestate.Pipe_Color);

                // draw top middle black
                LCD_DrawRectangle(HostToClient.gamestate.Pipe_X - (HostToClient.gamestate.Pipe_Width>>1), Pipe_X_Old - (HostToClient.gamestate.Pipe_Width>>1) + 1, HostToClient.gamestate.Pipe_Y - (HostToClient.gamestate.Pipe_Gap>>1) - 1 , HostToClient.gamestate.Pipe_Y - (HostToClient.gamestate.Pipe_Gap>>1), LCD_BLACK);
                LCD_DrawRectangle(HostToClient.gamestate.Pipe_X - (HostToClient.gamestate.Pipe_Width>>1), Pipe_X_Old - (HostToClient.gamestate.Pipe_Width>>1) + 1, HostToClient.gamestate.Pipe_Y + (HostToClient.gamestate.Pipe_Gap>>1)     , HostToClient.gamestate.Pipe_Y + (HostToClient.gamestate.Pipe_Gap>>1) + 1, LCD_BLACK);

                // draw top middle black
                LCD_DrawRectangle(HostToClient.gamestate.Pipe_X - (HostToClient.gamestate.Pipe_Width>>1), Pipe_X_Old - (HostToClient.gamestate.Pipe_Width>>1) + 1, HostToClient.gamestate.Pipe_Y - (HostToClient.gamestate.Pipe_Gap>>1) - 20, HostToClient.gamestate.Pipe_Y - (HostToClient.gamestate.Pipe_Gap>>1) - 19, LCD_BLACK);
                LCD_DrawRectangle(HostToClient.gamestate.Pipe_X - (HostToClient.gamestate.Pipe_Width>>1), Pipe_X_Old - (HostToClient.gamestate.Pipe_Width>>1) + 1, HostToClient.gamestate.Pipe_Y + (HostToClient.gamestate.Pipe_Gap>>1) + 19, HostToClient.gamestate.Pipe_Y + (HostToClient.gamestate.Pipe_Gap>>1) + 20, LCD_BLACK);
            }
            else{
                // draw edge green
                LCD_DrawRectangle(HostToClient.gamestate.Pipe_X - (HostToClient.gamestate.Pipe_Width>>1) + 4, Pipe_X_Old -       (HostToClient.gamestate.Pipe_Width>>1) + 3, MIN_SCREEN_Y, HostToClient.gamestate.Pipe_Y - (HostToClient.gamestate.Pipe_Gap>>1) - 21, HostToClient.gamestate.Pipe_Color);
                LCD_DrawRectangle(HostToClient.gamestate.Pipe_X - (HostToClient.gamestate.Pipe_Width>>1) + 4, Pipe_X_Old -       (HostToClient.gamestate.Pipe_Width>>1) + 3, HostToClient.gamestate.Pipe_Y + (HostToClient.gamestate.Pipe_Gap>>1) + 21, MAX_SCREEN_Y - 1, HostToClient.gamestate.Pipe_Color);

                // draw middle green
                LCD_DrawRectangle(HostToClient.gamestate.Pipe_X - (HostToClient.gamestate.Pipe_Width>>1) + 2, Pipe_X_Old -       (HostToClient.gamestate.Pipe_Width>>1) + 1, HostToClient.gamestate.Pipe_Y - (HostToClient.gamestate.Pipe_Gap>>1) - 18, HostToClient.gamestate.Pipe_Y - (HostToClient.gamestate.Pipe_Gap>>1) - 2, HostToClient.gamestate.Pipe_Color);
                LCD_DrawRectangle(HostToClient.gamestate.Pipe_X - (HostToClient.gamestate.Pipe_Width>>1) + 2, Pipe_X_Old -       (HostToClient.gamestate.Pipe_Width>>1) + 1, HostToClient.gamestate.Pipe_Y + (HostToClient.gamestate.Pipe_Gap>>1) + 2, HostToClient.gamestate.Pipe_Y + (HostToClient.gamestate.Pipe_Gap>>1) + 18, HostToClient.gamestate.Pipe_Color);

                // draw leading edge black edge
                LCD_DrawRectangle(HostToClient.gamestate.Pipe_X - (HostToClient.gamestate.Pipe_Width>>1) + 2, HostToClient.gamestate.Pipe_X - (HostToClient.gamestate.Pipe_Width>>1) + 3, MIN_SCREEN_Y, HostToClient.gamestate.Pipe_Y - (HostToClient.gamestate.Pipe_Gap>>1) - 19, LCD_BLACK);
                LCD_DrawRectangle(HostToClient.gamestate.Pipe_X - (HostToClient.gamestate.Pipe_Width>>1) + 2, HostToClient.gamestate.Pipe_X - (HostToClient.gamestate.Pipe_Width>>1) + 3, HostToClient.gamestate.Pipe_Y + (HostToClient.gamestate.Pipe_Gap>>1) + 19, MAX_SCREEN_Y - 1, LCD_BLACK);

                // draw leading edge black middle
                LCD_DrawRectangle(HostToClient.gamestate.Pipe_X - (HostToClient.gamestate.Pipe_Width>>1)    , HostToClient.gamestate.Pipe_X - (HostToClient.gamestate.Pipe_Width>>1) + 1, HostToClient.gamestate.Pipe_Y - (HostToClient.gamestate.Pipe_Gap>>1) - 18, HostToClient.gamestate.Pipe_Y - (HostToClient.gamestate.Pipe_Gap>>1), LCD_BLACK);
                LCD_DrawRectangle(HostToClient.gamestate.Pipe_X - (HostToClient.gamestate.Pipe_Width>>1)    , HostToClient.gamestate.Pipe_X - (HostToClient.gamestate.Pipe_Width>>1) + 1, HostToClient.gamestate.Pipe_Y + (HostToClient.gamestate.Pipe_Gap>>1), HostToClient.gamestate.Pipe_Y + (HostToClient.gamestate.Pipe_Gap>>1) + 18, LCD_BLACK);

                // draw top middle black
                LCD_DrawRectangle(HostToClient.gamestate.Pipe_X - (HostToClient.gamestate.Pipe_Width>>1)    , Pipe_X_Old -       (HostToClient.gamestate.Pipe_Width>>1) + 1, HostToClient.gamestate.Pipe_Y - (HostToClient.gamestate.Pipe_Gap>>1) - 1, HostToClient.gamestate.Pipe_Y - (HostToClient.gamestate.Pipe_Gap>>1), LCD_BLACK);
                LCD_DrawRectangle(HostToClient.gamestate.Pipe_X - (HostToClient.gamestate.Pipe_Width>>1)    , Pipe_X_Old -       (HostToClient.gamestate.Pipe_Width>>1) + 1, HostToClient.gamestate.Pipe_Y + (HostToClient.gamestate.Pipe_Gap>>1), HostToClient.gamestate.Pipe_Y + (HostToClient.gamestate.Pipe_Gap>>1) + 1, LCD_BLACK);

                // draw top middle black
                LCD_DrawRectangle(HostToClient.gamestate.Pipe_X - (HostToClient.gamestate.Pipe_Width>>1)    , Pipe_X_Old -       (HostToClient.gamestate.Pipe_Width>>1) + 1, HostToClient.gamestate.Pipe_Y - (HostToClient.gamestate.Pipe_Gap>>1) - 20, HostToClient.gamestate.Pipe_Y - (HostToClient.gamestate.Pipe_Gap>>1) - 19, LCD_BLACK);
                LCD_DrawRectangle(HostToClient.gamestate.Pipe_X - (HostToClient.gamestate.Pipe_Width>>1)    , Pipe_X_Old -       (HostToClient.gamestate.Pipe_Width>>1) + 1, HostToClient.gamestate.Pipe_Y + (HostToClient.gamestate.Pipe_Gap>>1) + 19, HostToClient.gamestate.Pipe_Y + (HostToClient.gamestate.Pipe_Gap>>1) + 20, LCD_BLACK);
            }
        }
        else if((HostToClient.gamestate.Pipe_X - (HostToClient.gamestate.Pipe_Width>>1)) >= MIN_SCREEN_X){
            // draw the pipe going across the screen

            // draw edge green
            LCD_DrawRectangle(HostToClient.gamestate.Pipe_X - (HostToClient.gamestate.Pipe_Width>>1) + 4, Pipe_X_Old - (HostToClient.gamestate.Pipe_Width>>1) + 3, MIN_SCREEN_Y, HostToClient.gamestate.Pipe_Y - (HostToClient.gamestate.Pipe_Gap>>1) - 21, HostToClient.gamestate.Pipe_Color);
            LCD_DrawRectangle(HostToClient.gamestate.Pipe_X - (HostToClient.gamestate.Pipe_Width>>1) + 4, Pipe_X_Old - (HostToClient.gamestate.Pipe_Width>>1) + 3, HostToClient.gamestate.Pipe_Y + (HostToClient.gamestate.Pipe_Gap>>1) + 21, MAX_SCREEN_Y - 1, HostToClient.gamestate.Pipe_Color);

            // draw middle green
            LCD_DrawRectangle(HostToClient.gamestate.Pipe_X - (HostToClient.gamestate.Pipe_Width>>1) + 2, Pipe_X_Old - (HostToClient.gamestate.Pipe_Width>>1) + 1, HostToClient.gamestate.Pipe_Y - (HostToClient.gamestate.Pipe_Gap>>1) - 18, HostToClient.gamestate.Pipe_Y - (HostToClient.gamestate.Pipe_Gap>>1) - 2, HostToClient.gamestate.Pipe_Color);
            LCD_DrawRectangle(HostToClient.gamestate.Pipe_X - (HostToClient.gamestate.Pipe_Width>>1) + 2, Pipe_X_Old - (HostToClient.gamestate.Pipe_Width>>1) + 1, HostToClient.gamestate.Pipe_Y + (HostToClient.gamestate.Pipe_Gap>>1) + 2, HostToClient.gamestate.Pipe_Y + (HostToClient.gamestate.Pipe_Gap>>1) + 18, HostToClient.gamestate.Pipe_Color);

            // draw leading edge black edge
            LCD_DrawRectangle(HostToClient.gamestate.Pipe_X - (HostToClient.gamestate.Pipe_Width>>1) + 2, HostToClient.gamestate.Pipe_X - (HostToClient.gamestate.Pipe_Width>>1) + 3, MIN_SCREEN_Y, HostToClient.gamestate.Pipe_Y - (HostToClient.gamestate.Pipe_Gap>>1) - 19, LCD_BLACK);
            LCD_DrawRectangle(HostToClient.gamestate.Pipe_X - (HostToClient.gamestate.Pipe_Width>>1) + 2, HostToClient.gamestate.Pipe_X - (HostToClient.gamestate.Pipe_Width>>1) + 3, HostToClient.gamestate.Pipe_Y + (HostToClient.gamestate.Pipe_Gap>>1) + 19, MAX_SCREEN_Y - 1, LCD_BLACK);

            // draw leading edge black middle
            LCD_DrawRectangle(HostToClient.gamestate.Pipe_X - (HostToClient.gamestate.Pipe_Width>>1), HostToClient.gamestate.Pipe_X - (HostToClient.gamestate.Pipe_Width>>1) + 1, HostToClient.gamestate.Pipe_Y - (HostToClient.gamestate.Pipe_Gap>>1) - 18, HostToClient.gamestate.Pipe_Y - (HostToClient.gamestate.Pipe_Gap>>1), LCD_BLACK);
            LCD_DrawRectangle(HostToClient.gamestate.Pipe_X - (HostToClient.gamestate.Pipe_Width>>1), HostToClient.gamestate.Pipe_X - (HostToClient.gamestate.Pipe_Width>>1) + 1, HostToClient.gamestate.Pipe_Y + (HostToClient.gamestate.Pipe_Gap>>1), HostToClient.gamestate.Pipe_Y + (HostToClient.gamestate.Pipe_Gap>>1) + 18, LCD_BLACK);

            // draw falling edge black edge
            LCD_DrawRectangle(HostToClient.gamestate.Pipe_X + (HostToClient.gamestate.Pipe_Width>>1) - 3, HostToClient.gamestate.Pipe_X + (HostToClient.gamestate.Pipe_Width>>1) - 2, MIN_SCREEN_Y, HostToClient.gamestate.Pipe_Y - (HostToClient.gamestate.Pipe_Gap>>1) - 19, LCD_BLACK);
            LCD_DrawRectangle(HostToClient.gamestate.Pipe_X + (HostToClient.gamestate.Pipe_Width>>1) - 3, HostToClient.gamestate.Pipe_X + (HostToClient.gamestate.Pipe_Width>>1) - 2, HostToClient.gamestate.Pipe_Y + (HostToClient.gamestate.Pipe_Gap>>1) + 19, MAX_SCREEN_Y - 1, LCD_BLACK);

            // draw falling edge black middle
            LCD_DrawRectangle(HostToClient.gamestate.Pipe_X + (HostToClient.gamestate.Pipe_Width>>1) - 1, HostToClient.gamestate.Pipe_X + (HostToClient.gamestate.Pipe_Width>>1), HostToClient.gamestate.Pipe_Y - (HostToClient.gamestate.Pipe_Gap>>1) - 18, HostToClient.gamestate.Pipe_Y - (HostToClient.gamestate.Pipe_Gap>>1) - 2, LCD_BLACK);
            LCD_DrawRectangle(HostToClient.gamestate.Pipe_X + (HostToClient.gamestate.Pipe_Width>>1) - 1, HostToClient.gamestate.Pipe_X + (HostToClient.gamestate.Pipe_Width>>1), HostToClient.gamestate.Pipe_Y + (HostToClient.gamestate.Pipe_Gap>>1) + 2, HostToClient.gamestate.Pipe_Y + (HostToClient.gamestate.Pipe_Gap>>1) + 18, LCD_BLACK);

            // draw top center black
            LCD_DrawRectangle(HostToClient.gamestate.Pipe_X - (HostToClient.gamestate.Pipe_Width>>1), Pipe_X_Old - (HostToClient.gamestate.Pipe_Width>>1) + 1, HostToClient.gamestate.Pipe_Y - (HostToClient.gamestate.Pipe_Gap>>1) - 1, HostToClient.gamestate.Pipe_Y - (HostToClient.gamestate.Pipe_Gap>>1), LCD_BLACK);
            LCD_DrawRectangle(HostToClient.gamestate.Pipe_X - (HostToClient.gamestate.Pipe_Width>>1), Pipe_X_Old - (HostToClient.gamestate.Pipe_Width>>1) + 1, HostToClient.gamestate.Pipe_Y + (HostToClient.gamestate.Pipe_Gap>>1), HostToClient.gamestate.Pipe_Y + (HostToClient.gamestate.Pipe_Gap>>1) + 1, LCD_BLACK);

            // draw top middle black
            LCD_DrawRectangle(HostToClient.gamestate.Pipe_X - (HostToClient.gamestate.Pipe_Width>>1), Pipe_X_Old - (HostToClient.gamestate.Pipe_Width>>1) + 1, HostToClient.gamestate.Pipe_Y - (HostToClient.gamestate.Pipe_Gap>>1) - 20, HostToClient.gamestate.Pipe_Y - (HostToClient.gamestate.Pipe_Gap>>1) - 19, LCD_BLACK);
            LCD_DrawRectangle(HostToClient.gamestate.Pipe_X - (HostToClient.gamestate.Pipe_Width>>1), Pipe_X_Old - (HostToClient.gamestate.Pipe_Width>>1) + 1, HostToClient.gamestate.Pipe_Y + (HostToClient.gamestate.Pipe_Gap>>1) + 19, HostToClient.gamestate.Pipe_Y + (HostToClient.gamestate.Pipe_Gap>>1) + 20, LCD_BLACK);
        }
        else if((HostToClient.gamestate.Pipe_X + (HostToClient.gamestate.Pipe_Width>>1)) > MIN_SCREEN_X){
            // draw falling edge black middle
            LCD_DrawRectangle(HostToClient.gamestate.Pipe_X + (HostToClient.gamestate.Pipe_Width>>1) - 1, HostToClient.gamestate.Pipe_X + (HostToClient.gamestate.Pipe_Width>>1), HostToClient.gamestate.Pipe_Y - (HostToClient.gamestate.Pipe_Gap>>1) - 18, HostToClient.gamestate.Pipe_Y - (HostToClient.gamestate.Pipe_Gap>>1) - 2, LCD_BLACK);
            LCD_DrawRectangle(HostToClient.gamestate.Pipe_X + (HostToClient.gamestate.Pipe_Width>>1) - 1, HostToClient.gamestate.Pipe_X + (HostToClient.gamestate.Pipe_Width>>1), HostToClient.gamestate.Pipe_Y + (HostToClient.gamestate.Pipe_Gap>>1) + 2 , HostToClient.gamestate.Pipe_Y + (HostToClient.gamestate.Pipe_Gap>>1) + 18, LCD_BLACK);

            // draw falling edge black edge
            LCD_DrawRectangle(HostToClient.gamestate.Pipe_X + (HostToClient.gamestate.Pipe_Width>>1) - 3, HostToClient.gamestate.Pipe_X + (HostToClient.gamestate.Pipe_Width>>1) - 2, MIN_SCREEN_Y, HostToClient.gamestate.Pipe_Y - (HostToClient.gamestate.Pipe_Gap>>1) - 19, LCD_BLACK);
            LCD_DrawRectangle(HostToClient.gamestate.Pipe_X + (HostToClient.gamestate.Pipe_Width>>1) - 3, HostToClient.gamestate.Pipe_X + (HostToClient.gamestate.Pipe_Width>>1) - 2, HostToClient.gamestate.Pipe_Y + (HostToClient.gamestate.Pipe_Gap>>1) + 19, MAX_SCREEN_Y - 1, LCD_BLACK);

            if(Pipe_X_Old >= MIN_SCREEN_X){

                // draw edge green
                LCD_DrawRectangle(MIN_SCREEN_X, Pipe_X_Old - (HostToClient.gamestate.Pipe_Width>>1) + 3, MIN_SCREEN_Y, HostToClient.gamestate.Pipe_Y - (HostToClient.gamestate.Pipe_Gap>>1) - 21, HostToClient.gamestate.Pipe_Color);
                LCD_DrawRectangle(MIN_SCREEN_X, Pipe_X_Old - (HostToClient.gamestate.Pipe_Width>>1) + 3, HostToClient.gamestate.Pipe_Y + (HostToClient.gamestate.Pipe_Gap>>1) + 21, MAX_SCREEN_Y - 1, HostToClient.gamestate.Pipe_Color);

                // draw middle green
                LCD_DrawRectangle(MIN_SCREEN_X, Pipe_X_Old - (HostToClient.gamestate.Pipe_Width>>1) + 1, HostToClient.gamestate.Pipe_Y - (HostToClient.gamestate.Pipe_Gap>>1) - 18, HostToClient.gamestate.Pipe_Y - (HostToClient.gamestate.Pipe_Gap>>1) - 2, HostToClient.gamestate.Pipe_Color);
                LCD_DrawRectangle(MIN_SCREEN_X, Pipe_X_Old - (HostToClient.gamestate.Pipe_Width>>1) + 1, HostToClient.gamestate.Pipe_Y + (HostToClient.gamestate.Pipe_Gap>>1) + 2, HostToClient.gamestate.Pipe_Y + (HostToClient.gamestate.Pipe_Gap>>1) + 18, HostToClient.gamestate.Pipe_Color);

                // draw top center black
                LCD_DrawRectangle(MIN_SCREEN_X, Pipe_X_Old - (HostToClient.gamestate.Pipe_Width>>1) + 1, HostToClient.gamestate.Pipe_Y - (HostToClient.gamestate.Pipe_Gap>>1) - 1, HostToClient.gamestate.Pipe_Y - (HostToClient.gamestate.Pipe_Gap>>1), LCD_BLACK);
                LCD_DrawRectangle(MIN_SCREEN_X, Pipe_X_Old - (HostToClient.gamestate.Pipe_Width>>1) + 1, HostToClient.gamestate.Pipe_Y + (HostToClient.gamestate.Pipe_Gap>>1), HostToClient.gamestate.Pipe_Y + (HostToClient.gamestate.Pipe_Gap>>1) + 1, LCD_BLACK);

                // draw top middle black
                LCD_DrawRectangle(MIN_SCREEN_X, Pipe_X_Old - (HostToClient.gamestate.Pipe_Width>>1) + 1, HostToClient.gamestate.Pipe_Y - (HostToClient.gamestate.Pipe_Gap>>1) - 20, HostToClient.gamestate.Pipe_Y - (HostToClient.gamestate.Pipe_Gap>>1) - 19, LCD_BLACK);
                LCD_DrawRectangle(MIN_SCREEN_X, Pipe_X_Old - (HostToClient.gamestate.Pipe_Width>>1) + 1, HostToClient.gamestate.Pipe_Y + (HostToClient.gamestate.Pipe_Gap>>1) + 19, HostToClient.gamestate.Pipe_Y + (HostToClient.gamestate.Pipe_Gap>>1) + 20, LCD_BLACK);

            }
        }

        // save old pipe location
        Pipe_X_Old = HostToClient.gamestate.Pipe_X;

        // FlappyBird erasing and drawing
        if(HostToClient.gamestate.Flappy_Y - Flappy_Y_Old > 0){
            // flappy moved down
            LCD_DrawRectangle(HostToClient.gamestate.Flappy_X, HostToClient.gamestate.Flappy_X + 16, Flappy_Y_Old, HostToClient.gamestate.Flappy_Y - 1, HostToClient.gamestate.BC_Color);
        }
        else if(HostToClient.gamestate.Flappy_Y - Flappy_Y_Old < 0){
            // flappy moved up
            LCD_DrawRectangle(HostToClient.gamestate.Flappy_X, HostToClient.gamestate.Flappy_X + 16, HostToClient.gamestate.Flappy_Y + 12, Flappy_Y_Old + 12, HostToClient.gamestate.BC_Color);
        }

        // test writing the bird
        for(int i = 0; i < 12; i++){
            for(int j = 0; j < 17; j++){
                LCD_SetPoint( HostToClient.gamestate.Flappy_X + j , HostToClient.gamestate.Flappy_Y + i , Flappy_map[i][j]);
            }
        }

        // remember old flappy location
        Flappy_Y_Old = HostToClient.gamestate.Flappy_Y;

        // idek it did something
        if(HostToClient.gamestate.Pipe_X > MAX_SCREEN_X){
            LCD_DrawRectangle(MIN_SCREEN_X, MIN_SCREEN_X + 10, MIN_SCREEN_Y, MIN_SCREEN_Y + 12, HostToClient.gamestate.BC_Color);
            uint8_t str[2];
            sprintf(str, "%u", HostToClient.gamestate.score);

            LCD_Text(0, 0, str, LCD_RED);
        }

        sleep(30);
    }
}

/*********************************************** Common Functions *********************************************************************/

/*
 * Initializes and prints initial game state
 */
void game1_InitBoardState(){

}

/*********************************************** Aperiodic Threads *********************************************************************/
void game1_ButtonPress ( void ){
}

