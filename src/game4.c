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

    G8RTOS_InitSemaphore(&LCD_Semaphore, 1);

    common_buttons_init();
    srand(0);
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 2; j++){
            uint8_t row = rand()%4;
            uint8_t col = rand()%4;
            while(1){
                if(matrix[row][col] == 0){
                    matrix[row][col] = colorMatrix[i];
                    break;
                }
                row = rand()%4;
                col = rand()%4;
            }
        }
    }
    LCD_Clear(LCD_BLACK);
    for(int i = 0; i < 4; i++){
        for(int j = 0; j < 4; j++){
            LCD_DrawRectangle(j*xOffset, (j+1)*xOffset-1, yOffset*i, (i+1)*yOffset-1, matrix[j][i]);

        }
    }

    /*
     * Add necessary host threads for reading the buttons, moving the bird, and drawing
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

int16_t joystick_x = 0;

int16_t joystick_y = 0;

int16_t displacementX = 0;

int16_t displacementY = 0;

int16_t avgX = 0;

int16_t avgY = 0;


    while(1)

    {

        GetJoystickCoordinates(&joystick_x, &joystick_y);



        // The switch statement was causing about 500 ms of lag

        if((joystick_x < -1000 || joystick_x > 1000) && abs(joystick_x) > abs(joystick_y))
            displacementX += (joystick_x >> 2);

        else if(joystick_y < -1000 || joystick_y > 1000)
        displacementY += -(joystick_y >> 2);

        uint16_t scale = 15000;

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
    G8RTOS_WaitSemaphore(&LCD_Semaphore);
    G8RTOS_KillAllOthers();
    G8RTOS_InitSemaphore(&LCD_Semaphore, 1);
    G8RTOS_WaitSemaphore(&LCD_Semaphore);
    LCD_Clear(LCD_WHITE);
    G8RTOS_SignalSemaphore(&LCD_Semaphore);

    delay_ms(1000000);

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

    sleep(1000);
    LCD_Clear(LCD_BLACK);
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

        if(buttonUP_pressed){
            sleep(50);
            buttonUP_pressed = 0;
            if(!valid[xcoord][ycoord]){
                LCD_DrawRectangle(xcoord*xOffset + 8, xcoord*xOffset + xOffset - 9, ycoord*yOffset + 8, ycoord*yOffset + yOffset - 9, matrix[xcoord][ycoord]);

                if(turn){
                    turn = 0;
                    second_xcoord = xcoord;
                    second_ycoord = ycoord;
                    valid[xcoord][ycoord] = 1;
                    decision = 1;
                    valid_cnt++; //keeps track of progress towards end of game
                }
                else{
                    first_xcoord = xcoord;
                    first_ycoord = ycoord;
                    valid[xcoord][ycoord] = 1;
                    valid_cnt++;//keeps track of progress towards end of game
                    turn = 1;
                    keepHiLited = 1;
                }
            }
        }

        else if(xcoord != xcoord_Old || ycoord != ycoord_Old){
            if(keepHiLited || valid[xcoord_Old][ycoord_Old]){
                LCD_DrawRectangle(xcoord_Old*xOffset, xcoord_Old*xOffset + xOffset - 1, ycoord_Old*yOffset, ycoord_Old*yOffset + yOffset - 1, LCD_BLACK);
                LCD_DrawRectangle(xcoord_Old*xOffset + 2, xcoord_Old*xOffset + xOffset - 3, ycoord_Old*yOffset + 2, ycoord_Old*yOffset + yOffset - 3, matrix[xcoord_Old][ycoord_Old]);
                keepHiLited = 0;
            }
            else{
                LCD_DrawRectangle(xcoord_Old*xOffset, xcoord_Old*xOffset + xOffset - 1, ycoord_Old*yOffset, ycoord_Old*yOffset + yOffset - 1, LCD_BLACK);
            }

            LCD_DrawRectangle(xcoord*xOffset, xcoord*xOffset + xOffset - 1, ycoord*yOffset, ycoord*yOffset + yOffset - 1, LCD_WHITE);
            LCD_DrawRectangle(xcoord*xOffset + 4, xcoord*xOffset + xOffset - 5, ycoord*yOffset + 4, ycoord*yOffset + yOffset - 5, LCD_BLACK);
            if(valid[xcoord][ycoord] == 1){
                LCD_DrawRectangle(xcoord*xOffset + 8, xcoord*xOffset + xOffset - 9, ycoord*yOffset + 8, ycoord*yOffset + yOffset - 9, matrix[xcoord][ycoord]);
            }
            xcoord_Old = xcoord;
            ycoord_Old = ycoord;
        }

        if(decision){
            sleep(300);

            if(matrix[first_xcoord][first_ycoord] != matrix[second_xcoord][second_ycoord]){
                valid[first_xcoord][first_ycoord] = 0;
                valid[second_xcoord][second_ycoord] = 0;

                if(first_xcoord == xcoord && first_ycoord == ycoord)
                    LCD_DrawRectangle(xcoord*xOffset + 4, xcoord*xOffset + xOffset - 5, ycoord*yOffset + 4, ycoord*yOffset + yOffset - 5, LCD_BLACK);
                else
                    LCD_DrawRectangle(first_xcoord*xOffset, first_xcoord*xOffset + xOffset - 1, first_ycoord*yOffset, first_ycoord*yOffset + yOffset - 1, LCD_BLACK);

                if(second_xcoord == xcoord && second_ycoord == ycoord)
                    LCD_DrawRectangle(xcoord*xOffset + 4, xcoord*xOffset + xOffset - 5, ycoord*yOffset + 4, ycoord*yOffset + yOffset - 5, LCD_BLACK);
                else
                    LCD_DrawRectangle(second_xcoord*xOffset, second_xcoord*xOffset + xOffset - 1, second_ycoord*yOffset, second_ycoord*yOffset + yOffset - 1, LCD_BLACK);

            }
            decision = 0;
        }
       if(valid_cnt == 16){
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
