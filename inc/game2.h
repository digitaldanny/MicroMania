#ifndef INC_GAME2_H_
#define INC_GAME2_H_

/*********************************************** Includes ********************************************************************/
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include "G8RTOS.h"
#include "cc3100_usage.h"
#include "GLCD.h"
#include "time.h"
#include "math.h"
#include "common_threads.h"
/*********************************************** Includes ********************************************************************/
#define DEFAULT_PRIORITY    15
#define MAX_NUM_OF_BALLS 10
#define MAX_NUM_OF_PLAYERS 2
#define MAX_NUM_OF_SHROOMS 1
#define BALL_SIZE 3
#define SHROOM_SIZE 16
#define ARENA_MAX_X 320
#define ARENA_MAX_Y 240
#define ARENA_MIN_X 0
#define ARENA_MIN_Y 20
/* Maximum ball speed */
#define MAX_BALL_SPEED               3
#define MIN_BALL_SPEED               1
#define MAX_SHROOM_SPEED               3
#define MIN_SHROOM_SPEED               1
#define SHROOM_SLEEP_COUNT              5000
#define LIVES 5
#define BALL_GEN_SLEEP      200 // 10 second increments increasing linearly
#define PLAYER_OFFSET 5
#define JOYSTICK_BIAS_CLIENT 200
#define JOYSTICK_BIAS_HOST 200
#define STATUS_STRING_OFFSET 20

/* Background color - Black */
#define BACK_COLOR                   White
/*********************************************** Game Functions *********************************************************************/

void game2_addHostThreads();    // called by the menu to initialize this game
void game2_addClientThreads();  // called by the menu to initialize this game

/*********************************************** structs *********************************************************************/
#pragma pack ( push, 1)
/*
 * Struct to be sent from the client to the host
 */
typedef struct
{
    uint32_t IP_address;
    int8_t displacementX;
    int8_t displacementY;
    uint8_t playerNumber;
    uint8_t direction;
    bool ready;
    bool joined;
    bool acknowledge;
} Game2_SpecificPlayerInfo_t;

/*
 * General player info to be used by both host and client
 * Client responsible for translation
 */
typedef struct
{
    int16_t currentCenterX;
    int16_t currentCenterY;
    uint8_t direction;
    uint16_t num_lives;
    uint16_t RunTime;
    uint16_t color;
} Game2_GeneralPlayerInfo_t;
/*
 * Struct of all the balls, only changed by the host
 */
typedef struct
{
    int16_t currentCenterX;
    int16_t currentCenterY;
    int16_t xvel;
    int16_t yvel;
    uint16_t color;
    bool moving;
    bool alive;
    bool kill;
} Game2_Ball_t;
/*
 * Struct of all the balls, only changed by the host
 */
typedef struct
{
    int16_t currentCenterX;
    int16_t currentCenterY;
    int16_t xvel;
    int16_t yvel;
    bool alive;
    bool kill;
} Game2_Shroom_t;

/*
 * Struct to be sent from the host to the client
 */
typedef struct
{
    Game2_SpecificPlayerInfo_t player;
    Game2_GeneralPlayerInfo_t players[MAX_NUM_OF_PLAYERS];
    Game2_Ball_t balls[MAX_NUM_OF_BALLS];
    Game2_Shroom_t shroom[MAX_NUM_OF_SHROOMS];
    uint16_t numberOfBalls;
    bool winner;
    bool gameDone;
    uint8_t LEDScores[2];
    uint8_t overallScores[2];
} Game2_GameState_t;
#pragma pack ( pop )

/*
 * Struct of all the previous ball locations, only changed by self for drawing!
 */
typedef struct
{
    int16_t CenterX;
    int16_t CenterY;
}Game2_PrevBall_t;

/*
 * Struct of all the previous players locations, only changed by self for drawing
 */
typedef struct
{
    int16_t CenterX;
    int16_t CenterY;
}Game2_PrevPlayer_t;

/*
 * Struct of all the previous players locations, only changed by self for drawing
 */
typedef struct
{
    int16_t CenterX;
    int16_t CenterY;
}Game2_PrevShroom_t;


/*********************************************** Client Threads *********************************************************************/
/*
 * Thread for client to join game
 */
void Game2_JoinGame();

/*
 * Thread that receives game state packets from host
 */
void Game2_ReceiveDataFromHost();

/*
 * Thread that sends UDP packets to host
 */
void Game2_SendDataToHost();

/*
 * Thread to read client's joystick
 */
void Game2_ReadJoystickClient();

/*
 * End of game for the client
 */
void Game2_EndOfGameClient();

/*********************************************** Client Threads *********************************************************************/


/*********************************************** Host Threads *********************************************************************/
/*
 * Thread for the host to create a game
 */
void Game2_CreateGame();

/*
 * Thread that sends game state to client
 */
void Game2_SendDataToClient();

/*
 * Thread that receives UDP packets from client
 */
void Game2_ReceiveDataFromClient();

/*
 * Thread to read host's joystick
 */
void Game2_ReadJoystickHost();

/*
 * End of game for the host
 */
void Game2_EndOfGameHost();

void Game2_GenerateBall();

void Game2_GenerateShroom();


void Game2_MoveBall();

void Game2_MoveShroom();

void Game2_UpdateBallColors();

/*********************************************** Host Threads *********************************************************************/


/*********************************************** Common Threads *********************************************************************/
/*
 * Idle thread
 */
void Game2_IdleThread();

/*
 * Thread to draw all the objects in the game
 */
void Game2_DrawObjects();


/*********************************************** Common Functions *********************************************************************/

/*
 * Initializes and prints initial game state
 */
void Game2_InitBoardState();

void Game2_UpdatePlayerStatus();

void Game2_UpdatePlayerOnScreen(Game2_PrevPlayer_t * prevPlayerIn, Game2_GeneralPlayerInfo_t * outPlayer, int ID);

void Game2_UpdateBallOnScreen(Game2_PrevBall_t * previousBall, Game2_Ball_t * currentBall, uint16_t outColor);

void Game2_UpdateShroomOnScreen(Game2_PrevShroom_t * previousBall, Game2_Shroom_t * currentBall);


void Game2_DrawPlayer(Game2_GeneralPlayerInfo_t * player, uint16_t color, int ID);



/*********************************************** Aperiodic Threads *********************************************************************/
void Game2_ButtonPress ( void );

/*********************************************** image arrays *********************************************************************/


#endif /* INC_GAME2_H_ */
