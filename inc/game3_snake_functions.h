/*
 * game3_snake_functions.h
 *
 *  Created on: Apr 26, 2019
 *      Author: Daniel Hamilton
 */

#ifndef INC_GAME3_SNAKE_FUNCTIONS_H_
#define INC_GAME3_SNAKE_FUNCTIONS_H_

#include "common_threads.h"

#define SN_SNAKE_MAX_LENGTH     20

// This represents one block of
// the snake. Head and tail are of
// this type.
typedef struct snake
{
    point_t center;
    struct snake * next;
    struct snake * prev;
    bool alive;
} snake_t;

typedef enum
{
    PREV,
    NEXT,
    X,
    Y
} comp_t;

// HELPER FUNCTIONS --------------------

// This function will point the head, tail, and
// body functions to the correct player properties
void game3_assignPointers(int8_t player_num);

// returns the center value for the current
// tail
point_t game3_getTail(int8_t player_num);

// returns the center value for the current
// head
point_t game3_getHead(int8_t player_num);

// returns "TRUE" if the element is equal to
// the tail
bool game3_isTail(int8_t index, int8_t player_num);

// FUNCTIONS ---------------------------

// This function initializes the linked list
// for the snake structure
void game3_initSnake(point_t * head_and_tail, int8_t player_num);

// This function will adjust the head pointer
// for the player number's snake linked list
// and reassign to the next.
bool game3_addSnakeHead(point_t * new_head, int8_t player_num);

// This function will remove the tail pointer
// for the player number's snake linked list
// and reassign to the previous.
point_t game3_rmSnakeTail(int8_t player_num);

// This function will search through the linked
// list until the correct snake structure is found
// and return the center value.
point_t game3_snakeAt(int8_t index, int8_t player_num);

// This function returns "true" if the max size of
// the snake is reached.
bool game3_limitReached(int8_t player_num);

// This function returns the current length of the snake
int8_t game3_snakeLength(int8_t player_num);

// COMPARISION FUNCTIONS ----------------

// This function accesses the index of the linked
// list and determines whether the Y value of the
// NEXT node of the linked list has the same Y
bool game3_compAt (int8_t index, int8_t player_num,
                    comp_t compare, comp_t axis);

// PREVIOUS ANALYSIS ---------------------

// This function iterates through the whole previous
// snake array for the player
bool game3_iteratePrevSnakeCenters( point_t * center, int8_t player_num );

// This function stores the new MAPPED center value at the requested
// index of the snake buffer and increases the buffer size
void game3_storeToSnakeCenterBuffer( point_t * center, int8_t index, int8_t player_num );

// This function resets the requested player number's snake buffer
// with invalid center values and a size of 0
void game3_resetSnakeBuffer( int8_t player_num );

// This function will transfer all the mapped center points to
// the array of previous center points and sets the previous
// array size to the center buffer size. Afterwards, it calls
// game3_resetSnakeBuffer to reset the buffer center points and
// size.
void game3_transferBufferToPrev (int8_t player_num);

// CURRENT ANALYSIS -----------------------

// This function iterates through the whole current
// snake array buffer for the player
bool game3_iterateCurrentSnakeCenters( point_t * center, int8_t player_num );

#endif /* INC_GAME3_SNAKE_FUNCTIONS_H_ */
