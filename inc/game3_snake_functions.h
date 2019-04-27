/*
 * game3_snake_functions.h
 *
 *  Created on: Apr 26, 2019
 *      Author: Daniel Hamilton
 */

#ifndef INC_GAME3_SNAKE_FUNCTIONS_H_
#define INC_GAME3_SNAKE_FUNCTIONS_H_

#include "common_threads.h"

#define SN_SNAKE_MAX_LENGTH     50

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

// HELPER FUNCTIONS --------------------
// This function will point the head, tail, and
// body functions to the correct player properties
void game3_assignPointers(int8_t player_num);

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

#endif /* INC_GAME3_SNAKE_FUNCTIONS_H_ */
