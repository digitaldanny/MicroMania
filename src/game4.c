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

/*
 ************************************************************** Sprite Maps ********************************************************************************
 */


/*
 ******************************************************** LOCALS ************************************************************
 */

uint16_t matrix[4][4] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0};
uint16_t colorMatrix[8] = {LCD_BLUE,LCD_YELLOW,LCD_WHITE,LCD_PURPLE,LCD_RED,LCD_GREEN,LCD_ORANGE,LCD_PINK};

uint16_t xcoord = 0;
uint16_t ycoord = 0;

uint16_t xOffset = 80;
uint16_t yOffset = 60;
uint8_t tileScore = 0;


/*
 * Thread for client to join game
 */
void game4_JoinGame(){

}

/*
 * Thread that receives game state packets from host
 */
void game4_ReceiveDataFromHost(){

}

/*
 * Thread that sends UDP packets to host
 */
void game4_SendDataToHost(){

}

/*
 * Thread to read client's joystick
 */
void game4_ReadJoystickClient(){

}

/*
 * End of game for the client
 */
void game4_EndOfGameClient(){

}

/*********************************************** Client Threads *********************************************************************/


/*********************************************** Host Threads *********************************************************************/
/*
 * Thread for the host to create a game
 */
void game4_CreateGame(){
    // reset the matrix values for colors
    for(int i = 0; i < 4; i++){
       for(int j = 0; j < 4; j++){
           matrix[i][j] = 0;
       }
    }

    // reset the player moves
    tileScore = 0;

    // initialize the push buttons on board
    common_buttons_init();

    // randomize the map
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 2; j++){

            // come up with random column and row
            uint8_t row = rand()%4;
            uint8_t col = rand()%4;
            while(1){

                // see if the random row and col already have a valid color in the matrix
                if(matrix[row][col] == 0){
                    // matrix at row and col is empty so fill it with color
                    matrix[row][col] = colorMatrix[i];
                    break;
                }

                // get new random values
                row = rand()%4;
                col = rand()%4;
            }
        }
    }

    // clear the screen to black
    LCD_Clear(LCD_BLACK);

    // draw all the colors to the screen from the matrix 2d array
    for(int i = 0; i < 4; i++){
        for(int j = 0; j < 4; j++){
            LCD_DrawRectangle(j*xOffset, (j+1)*xOffset-1, yOffset*i, (i+1)*yOffset-1, matrix[j][i]);

        }
    }

    /*
     * Add necessary client threads for reading the buttons, moving the bird, and drawing
     */
    G8RTOS_AddThread(&game4_DrawObjects, 10, 0xFFFFFFFF, "DrawObjects");
    G8RTOS_AddThread(&game4_ReadJoystickHost, 15, 0xFFFFFFFF, "IdleThread");
    G8RTOS_AddThread(&game4_IdleThread, 0xff, 0xFFFFFFFF, "IdleThread");

    G8RTOS_KillSelf();

}

/*
 * Thread that sends game state to client
 */
void game4_SendDataToClient(){

}

/*
 * Thread that receives UDP packets from client
 */
void game4_ReceiveDataFromClient(){

}

/*
 * Thread to read host's joystick
 */
void game4_ReadJoystickHost(){

    // initialize all local variables
    int16_t joystick_x = 0;
    int16_t joystick_y = 0;
    int16_t displacementX = 0;
    int16_t displacementY = 0;
    int16_t avgX = 0;
    int16_t avgY = 0;

    while(1)
    {
        // retrieve the joystick values
        GetJoystickCoordinates(&joystick_x, &joystick_y);

        // The switch statement was causing about 500 ms of lag

        // see if joystick is pointing in an x direction, and see if the mag(x) > mag(y)
        if((joystick_x < -2000 || joystick_x > 2000) && abs(joystick_x) > abs(joystick_y))
            displacementX += (joystick_x >> 2);

        // see if joystick is pointing in a y direction, also from if statement above it is greater than x
        else if(joystick_y < -2000 || joystick_y > 2000)
            displacementY += -(joystick_y >> 2);

        // set the scale for sensitivity
        uint16_t scale = 15000;

        // set make sure X and Y are within boundaries of -2*scale to 2*scale
        if(displacementX < -2 * scale){
            displacementX = -2 * scale;
        }
        else if(displacementX > 2 * scale){
            displacementX = 2 * scale;
        }

        if(displacementY < -2 * scale){
            displacementY = -2 * scale;
        }
        else if(displacementY > 2 * scale){
            displacementY = 2 * scale;
        }

        // determine what square to hi-lite based off the joystick displacement

        // X at right
        if(displacementX < -1*scale){
            xcoord = 3;
        }

        // X at right Middle
        else if(displacementX < 0){
            xcoord = 2;
        }

        // X at left Middle
        else if(displacementX < scale){
            xcoord = 1;
        }

        // X at left
        else{
            xcoord = 0;
        }

        // Y at top
        if(displacementY < -1*scale){
            ycoord = 3;
        }

        // Y at Top Middle
        else if(displacementY < 0){
            ycoord = 2;
        }

        // Y at Bottom Middle
        else if(displacementY < scale){
            ycoord = 1;
        }

        // Y at bottom
        else{
            ycoord = 0;
        }

        sleep(20);
    }
}

/*
 * End of game for the host
 */
void game4_EndOfGameHost(){

    // not using semaphores, so just kill all other threads
    G8RTOS_KillAllOthers();

    // calculate score
    uint16_t overallScore = 100 - (tileScore-16)*((tileScore-15) >> 1);
    if(overallScore > 100){
        overallScore = 0;
    }

    // convert score to a string
    uint8_t str[2];
    sprintf(str, "%u", overallScore);

    // clear screen to white
    LCD_Clear(LCD_WHITE);

    // display final score
    LCD_Text((MAX_SCREEN_X>>1) - 60, (MAX_SCREEN_Y>>1) + 4, "Final Score:", LCD_BLACK);
    LCD_Text((MAX_SCREEN_X>>1) + 40, (MAX_SCREEN_Y>>1) + 4, str, LCD_BLACK);

    // wait 1 second
    delay_ms(1000000);

    // return to menu and kill self
    G8RTOS_AddThread(&menu_MenuHost, 10, 0xFFFFFFFF, "MenuHost");
    G8RTOS_KillSelf();
}

/*********************************************** Host Threads *********************************************************************/


/*********************************************** Common Threads *********************************************************************/
/*
 * Idle thread
 */
void game4_IdleThread(){
    while(1);

}

/*
 * Thread to draw all the objects in the game
 */
void game4_DrawObjects(){

    // wait 1 second to show user
    sleep(3000);

    // clear screen to black to hide the matrix of colors
    LCD_Clear(LCD_BLACK);

    // initialize all local variables
    uint16_t xcoord_Old = 5;
    uint16_t ycoord_Old = 5;

    uint8_t keepHiLited = 0;

    uint16_t first_xcoord = 5;
    uint16_t first_ycoord = 5;
    uint16_t second_xcoord = 5;
    uint16_t second_ycoord = 5;

    uint8_t valid[4][4] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

    uint8_t turn = 0;
    uint8_t decision = 0;

    uint8_t valid_cnt = 0;

    while(1){

        // user selected the box
        if(buttonUP_pressed){

            // help further debounce button
            sleep(50);
            // reset the button pressed to 0
            buttonUP_pressed = 0;

            // see if the square was already selected
            if(!valid[xcoord][ycoord]){
                // box not selected yet so show the color inside the selecting box
                LCD_DrawRectangle(xcoord*xOffset + 8, xcoord*xOffset + xOffset - 9, ycoord*yOffset + 8, ycoord*yOffset + yOffset - 9, matrix[xcoord][ycoord]);

                // incriment the user moves
                tileScore++;

                // see if it is the first box in a turn or second
                if(turn){
                    // second box in turn

                    // reset turn to first(0)
                    turn = 0;

                    // save coordinates for keeping or deletion
                    second_xcoord = xcoord;
                    second_ycoord = ycoord;

                    // set box in valid array to true so user cannot select box again
                    valid[xcoord][ycoord] = 1;

                    // set this parameter to true so we can see if this color matched the previous color from first choice
                    decision = 1;
                    valid_cnt++; //keeps track of progress towards end of game
                }
                else{
                    // first box selected
                    // record coordinates for decision
                    first_xcoord = xcoord;
                    first_ycoord = ycoord;

                    // set box in valid array to true so user cannot select box again
                    valid[xcoord][ycoord] = 1;
                    valid_cnt++;//keeps track of progress towards end of game

                    // set turn to second(1)
                    turn = 1;

                    // set this so that the box will stay highlighted
                    keepHiLited = 1;
                }
            }
        }

        // see if the user moved
        else if(xcoord != xcoord_Old || ycoord != ycoord_Old){
            // user moved, so we need to see if we need to hide the last box or show the color

            if(keepHiLited || valid[xcoord_Old][ycoord_Old]){
                // last box was a valid box or a keep highlighted box so show the color
                LCD_DrawRectangle(xcoord_Old*xOffset, xcoord_Old*xOffset + xOffset - 1, ycoord_Old*yOffset, ycoord_Old*yOffset + yOffset - 1, LCD_BLACK);
                LCD_DrawRectangle(xcoord_Old*xOffset + 2, xcoord_Old*xOffset + xOffset - 3, ycoord_Old*yOffset + 2, ycoord_Old*yOffset + yOffset - 3, matrix[xcoord_Old][ycoord_Old]);

                // reset keep highlighted
                keepHiLited = 0;
            }
            else{
                // last box should be kept hidden, so show as black
                LCD_DrawRectangle(xcoord_Old*xOffset, xcoord_Old*xOffset + xOffset - 1, ycoord_Old*yOffset, ycoord_Old*yOffset + yOffset - 1, LCD_BLACK);
            }

            // highlight the current box with white outline
            LCD_DrawRectangle(xcoord*xOffset, xcoord*xOffset + xOffset - 1, ycoord*yOffset, ycoord*yOffset + yOffset - 1, LCD_WHITE);
            // fill with black showing a hollow white outline around the box selected
            LCD_DrawRectangle(xcoord*xOffset + 4, xcoord*xOffset + xOffset - 5, ycoord*yOffset + 4, ycoord*yOffset + yOffset - 5, LCD_BLACK);

            // see if current box is valid
            if(valid[xcoord][ycoord] == 1){
                // is valid, so show the color to user inside the selection box
                LCD_DrawRectangle(xcoord*xOffset + 8, xcoord*xOffset + xOffset - 9, ycoord*yOffset + 8, ycoord*yOffset + yOffset - 9, matrix[xcoord][ycoord]);
            }

            // update the old x and y coordinates
            xcoord_Old = xcoord;
            ycoord_Old = ycoord;
        }

        // see if time for decision
        if(decision){

            // sleep for 300 to briefly show the user the color
            sleep(300);

            // test if the first selection color matches the second selection color
            if(matrix[first_xcoord][first_ycoord] != matrix[second_xcoord][second_ycoord]){
                // selection colors do not match
                // set both coordinates in valid to false
                valid[first_xcoord][first_ycoord] = 0;
                valid[second_xcoord][second_ycoord] = 0;

                // decrement valid count by 2
                valid_cnt -= 2;

                // hide both the first and second selection
                if(first_xcoord == xcoord && first_ycoord == ycoord)
                    // hide second selection
                    LCD_DrawRectangle(xcoord*xOffset + 4, xcoord*xOffset + xOffset - 5, ycoord*yOffset + 4, ycoord*yOffset + yOffset - 5, LCD_BLACK);
                else
                    // hide first selection
                    LCD_DrawRectangle(first_xcoord*xOffset, first_xcoord*xOffset + xOffset - 1, first_ycoord*yOffset, first_ycoord*yOffset + yOffset - 1, LCD_BLACK);

                if(second_xcoord == xcoord && second_ycoord == ycoord)
                    // hide second selection
                    LCD_DrawRectangle(xcoord*xOffset + 4, xcoord*xOffset + xOffset - 5, ycoord*yOffset + 4, ycoord*yOffset + yOffset - 5, LCD_BLACK);
                else
                    // hide first selection
                    LCD_DrawRectangle(second_xcoord*xOffset, second_xcoord*xOffset + xOffset - 1, second_ycoord*yOffset, second_ycoord*yOffset + yOffset - 1, LCD_BLACK);
            }
            // reset decision parameter to 0;
            decision = 0;
        }

        // see if at end of game
        if(valid_cnt == 16){
            // end of game, so add and game
            G8RTOS_AddThread(game4_EndOfGameHost, 0, 0xFFFFFFFF, "END_OF_GAME_HOST");
        }

        sleep(30);
    }
}

/*********************************************** Common Functions *********************************************************************/

/*
 * Initializes and prints initial game state
 */
void game4_InitBoardState(){

}

/*********************************************** Aperiodic Threads *********************************************************************/
void game4_ButtonPress ( void ){
}

