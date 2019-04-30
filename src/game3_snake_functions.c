/*
 * game3_snake_functions.c
 *
 *  Created on: Apr 26, 2019
 *      Author: Daniel Hamilton
 *
 *  DESCRIPTION:
 *  This file includes all snake linked list functions
 *  to allow the snake to move across the screen
 *
 *  TEST CODE:
 *  point_t tempPoint;
 *  tempPoint.x = 0;
 *  tempPoint.y = 0;
 *  game3_initSnake(&tempPoint, 0);
 *  for ( int i = 1; i < 20; i++ )
 *  {
 *      tempPoint.x = i;
 *      tempPoint.y = i;
 *      game3_addSnakeHead(&tempPoint, 0);
 *  }
 *
 *  tempPoint = game3_snakeAt(0, 0);
 *  tempPoint = game3_snakeAt(1, 0);
 *  tempPoint = game3_snakeAt(2, 0);
 *  tempPoint = game3_snakeAt(3, 0);
 *  tempPoint = game3_snakeAt(4, 0);
 *  tempPoint = game3_snakeAt(0, 0);
 *  tempPoint = game3_snakeAt(9, 0);
 *  // -----------------------------------------------------
 */

#include "game3_snake_functions.h"

// globals ----------------------------------
static snake_t * head;
static snake_t * tail;
static snake_t * body;
static int8_t * size;

snake_t snake_pl0[SN_SNAKE_MAX_LENGTH];
snake_t snake_pl1[SN_SNAKE_MAX_LENGTH];
snake_t snake_pl2[SN_SNAKE_MAX_LENGTH];

int8_t snake_pl0_size;
int8_t snake_pl1_size;
int8_t snake_pl2_size;

snake_t * snake_pl0_head;
snake_t * snake_pl0_tail;

snake_t * snake_pl1_head;
snake_t * snake_pl1_tail;

snake_t * snake_pl2_head;
snake_t * snake_pl2_tail;

snake_t prevSnakes[3][SN_SNAKE_MAX_LENGTH];
int8_t prevSnakesSize[3];

snake_t bufferSnakes[3][SN_SNAKE_MAX_LENGTH];
int8_t bufferSnakesSize[3];

// HELPER FUNCTIONS --------------------
// This function will point the head, tail, and
// body functions to the correct player properties
void game3_assignPointers(int8_t player_num)
{
    if ( player_num == 0 )
    {
        head = snake_pl0_head;
        tail = snake_pl0_tail;
        size = &snake_pl0_size;
    }
    else if ( player_num == 1 )
    {
        head = snake_pl1_head;
        tail = snake_pl1_tail;
        size = &snake_pl1_size;
    }
    else if ( player_num == 2 )
    {
        head = snake_pl2_head;
        tail = snake_pl2_tail;
        size = &snake_pl2_size;
    }
}

// returns the center value for the current
// tail
point_t game3_getTail(int8_t player_num)
{
    game3_assignPointers(player_num);
    return tail->center;
}

// returns the center value for the current
// head
point_t game3_getHead(int8_t player_num)
{
    game3_assignPointers(player_num);
    return head->center;
}

// returns "TRUE" if the element is equal to
// the tail
bool game3_isTail(int8_t index, int8_t player_num)
{
    game3_assignPointers(player_num);

    if ( tail->center.x == game3_snakeAt(index, player_num).x
            && tail->center.y == game3_snakeAt(index, player_num).y)
        return true;
    else
        return false;
}

// FUCTIONS ---------------------------------
// This function initializes the linked list
// for the snake structure
void game3_initSnake(point_t * head_and_tail, int8_t player_num)
{
    // determine which player's snake
    // array is being pointed to and
    // initialize that array so values
    // can be permanently stored
    if ( player_num == 0 )
    {
        for (int i = 0; i < SN_SNAKE_MAX_LENGTH; i++)
        {
            body = &snake_pl0[i];
            snake_pl0_size = 1;

            body->alive = false;
            body->center.x = -500;
            body->center.y = -500;
            body->next = NULL;
            body->next = NULL;
        }

        snake_pl0_head = &snake_pl0[0];
        snake_pl0_tail = snake_pl0_head;
    }
    else if ( player_num == 1 )
    {
        for (int i = 0; i < SN_SNAKE_MAX_LENGTH; i++)
        {
            body = &snake_pl1[i];
            snake_pl1_size = 1;

            body->alive = false;
            body->center.x = -500;
            body->center.y = -500;
            body->next = NULL;
            body->next = NULL;
        }

        snake_pl1_head = &snake_pl1[0];
        snake_pl1_tail = snake_pl1_head;
    }
    else
    {
        for (int i = 0; i < SN_SNAKE_MAX_LENGTH; i++)
        {
            body = &snake_pl2[i];
            snake_pl2_size = 1;

            body->alive = false;
            body->center.x = -500;
            body->center.y = -500;
            body->next = NULL;
            body->next = NULL;
        }

        snake_pl2_head = &snake_pl2[0];
        snake_pl2_tail = snake_pl2_head;
    }

    // initialize center values to invalid MAPPED
    // center values so erasing and drawing will happen
    // during the first iteration.
    for ( int i = 0; i < SN_SNAKE_MAX_LENGTH; i++ )
    {
        prevSnakes[player_num][i].center.x = -500;
        prevSnakesSize[player_num] = 0;

        bufferSnakes[player_num][i].center.x = -500;
        bufferSnakesSize[player_num] = 0;
    }

    // set up the pointers correctly first
    game3_assignPointers( player_num );

    // then assign the new head, tail, next and previous
    head->alive = true;
    head->center = *head_and_tail;
    head->next = head;
    head->prev = head;
}

// This function will adjust the head pointer
// for the player number's snake linked list
// and reassign to the next.
bool game3_addSnakeHead(point_t * new_head, int8_t player_num)
{
    bool head_added = true;

    // set up the pointers correctly first
    game3_assignPointers( player_num );

    // find the first "dead" snake structure and
    // initialize it as the new header
    for (int i = 0; i < SN_SNAKE_MAX_LENGTH; i++)
    {
        // determine which player's snake
        // array is being pointed to
        if ( player_num == 0 )
            body = &snake_pl0[i];
        else if ( player_num == 1 )
            body = &snake_pl1[i];
        else
            body = &snake_pl2[i];

        // initialize the new head
        if ( body->alive == false )
        {
            (*size)++;
            body->alive = true;
            body->center = *new_head;
            body->next = head;
            body->prev = tail;
            break;
        }

        // if the whole array has been searched through and no
        // living body segments were found, the program should
        // know that the head could not be added.
        if ( i == SN_SNAKE_MAX_LENGTH-1 && body->alive == true )
            head_added = false;
    }

    // reassign the old head pointer's previous as the
    // new head pointer
    head->prev = body;

    // reassign the tail pointer's next as the new
    // head pointer
    tail->next = body;

    // add the new head in
    if ( player_num == 0 )
        snake_pl0_head = body;
    else if ( player_num == 1 )
        snake_pl1_head = body;
    else
        snake_pl2_head = body;

    return head_added;
}

// This function will remove the tail pointer
// for the player number's snake linked list
// and reassign to the previous.
point_t game3_rmSnakeTail(int8_t player_num)
{
    point_t tail_center;

    if ( (*size) > 0 )
    {
        // determine which player's snake
        // array is being pointed to
        if ( player_num == 0 )
            tail = snake_pl0_tail;
        else if ( player_num == 1 )
            tail = snake_pl1_tail;
        else
            tail = snake_pl2_tail;

        // assign the returned tail center
        // value to be used in the main program
        tail_center = tail->center;

        // reassign the portions of the linked
        // list that are not being removed
        tail->prev->next = head;
        head->prev = tail->prev;

        // get rid of the previous tail
        // by reassigning it
        if ( player_num == 0 )
            snake_pl0_tail = tail->prev;
        else if ( player_num == 1 )
            snake_pl1_tail = tail->prev;
        else
            snake_pl2_tail = tail->prev;

        // set the tail as dead so that
        // it can be used later when the
        // snake grows
        tail->alive = false;
        (*size)--;
    }

    return tail_center;
}

// This function will search through the linked
// list until the correct snake structure is found
// and return the center value.
point_t game3_snakeAt(int8_t index, int8_t player_num)
{
    point_t index_point;
    index_point.x = -500;
    index_point.y = -500;

    // point to the correct snake information
    game3_assignPointers(player_num);

    // start the indexing at the head of the
    // linked list and work towards the tail
    body = head;

    // only try to access the snake information
    // if it is within a valid range
    // NOTE: This may not be the actual current
    // length of the snake.
    if ( index < SN_SNAKE_MAX_LENGTH )
    {
        // first go to the appropriate
        // index of the linked list.
        for (int8_t i = 0; i < index; i++)
            body = body->next;

        // after the correct index is located
        // return the center data
        index_point = body->center;
    }

    return index_point;
}

// This function returns "true" if the max size of
// the snake is reached.
bool game3_limitReached(int8_t player_num)
{
    // point to the correct snake information
    game3_assignPointers(player_num);

    if ( (*size) >= SN_SNAKE_MAX_LENGTH )
        return true;

    else
        return false;
}

// This function returns the current length of the snake
int8_t game3_snakeLength(int8_t player_num)
{
    // point to the correct snake information
    game3_assignPointers(player_num);

    return (*size);
}

// COMPARISON FUNCTIONS -------------------------------

// This function accesses the index of the linked
// list and determines whether the X/Y value of the
// NEXT/PREV node of the linked list has the same X/Y
bool game3_compAt (int8_t index, int8_t player_num, comp_t comparison, comp_t axis)
{
    bool isEqual = false;

    // point to the correct snake information
    game3_assignPointers(player_num);

    // locate the requested index of the linked list
    // for this player's snake
    if ( index < SN_SNAKE_MAX_LENGTH )
    {
        // first go to the appropriate
        // index of the linked list.
        for (int8_t i = 0; i < index; i++)
            body = body->next;

        // make the appropriate comparison for the
        // requested axis (X or Y)
        if ( axis == Y )
        {
            // make the appropriate comparison for
            // whether the user requested a comparison
            // for the NEXT element or the PREV element
            if ( comparison == NEXT )
            {
                if ( body->center.y == body->next->center.y )
                    isEqual = true;
            }
            else if ( comparison == PREV )
            {
                if ( body->center.y == body->prev->center.y )
                    isEqual = true;
            }
        }

        else if ( axis == X )
        {
            // make the appropriate comparison for
            // whether the user requested a comparison
            // for the NEXT element or the PREV element
            if ( comparison == NEXT )
            {
                if ( body->center.x == body->next->center.x )
                    isEqual = true;
            }
            else if ( comparison == PREV )
            {
                if ( body->center.x == body->prev->center.x )
                    isEqual = true;
            }
        }
    }

    return isEqual;
}

// PREVIOUS ANALYSIS ---------------------------------------------

// This function iterates through the whole previous
// snake array for the player
bool game3_iteratePrevSnakeCenters( point_t * center, int8_t player_num )
{
    // search through all previous snake center values
    for (int i = 0; i < prevSnakesSize[player_num]; i++)
    {
        if ( prevSnakes[player_num][i].center.x == center->x
                && prevSnakes[player_num][i].center.y == center->y )
            return true;
    }

    return false;
}

// This function stores the new MAPPED center value at the requested
// index of the snake buffer and increases the buffer size
void game3_storeToSnakeCenterBuffer( point_t * center, int8_t index, int8_t player_num )
{
    bufferSnakes[player_num][index].center = *center;
    bufferSnakesSize[player_num]++;
}

// This function resets the requested player number's snake buffer
// with invalid center values and a size of 0
void game3_resetSnakeBuffer( int8_t player_num )
{
    for (int i = 0; i < SN_SNAKE_MAX_LENGTH; i++ )
    {
        bufferSnakes[player_num][i].center.x = -500;
    }

    bufferSnakesSize[player_num] = 0;
}

// This function will transfer all the mapped center points to
// the array of previous center points and sets the previous
// array size to the center buffer size. Afterwards, it calls
// game3_resetSnakeBuffer to reset the buffer center points and
// size.
void game3_transferBufferToPrev ( int8_t player_num )
{
    // transfer all elements
    for (int i = 0; i < bufferSnakesSize[player_num]; i++)
    {
        prevSnakes[player_num][i].center = bufferSnakes[player_num][i].center;
    }

    // set the new size
    prevSnakesSize[player_num] = bufferSnakesSize[player_num];

    // reset the buffer
    game3_resetSnakeBuffer(player_num);
}

// CURRENT ANALYSIS --------------------------------------

// This function iterates through the whole current
// snake array buffer for the player
bool game3_iterateCurrentSnakeCenters( point_t * center, int8_t player_num )
{
    // search through all previous snake center values
    for (int i = 0; i < bufferSnakesSize[player_num]; i++)
    {
        if ( bufferSnakes[player_num][i].center.x == center->x
                && bufferSnakes[player_num][i].center.y == center->y )
            return true;
    }

    return false;
}
