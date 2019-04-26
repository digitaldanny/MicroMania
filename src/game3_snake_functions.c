/*
 * game3_snake_functions.c
 *
 *  Created on: Apr 26, 2019
 *      Author: Daniel Hamilton
 *
 *  DESCRIPTION:
 *  This file includes all snake linked list functions
 *  to allow the snake to move across the screen
 */

#include "game3_snake_functions.h"

// globals ----------------------------------
snake_t * head;
snake_t * tail;
snake_t * body;

snake_t snake_pl0[SN_SNAKE_MAX_LENGTH];
snake_t snake_pl1[SN_SNAKE_MAX_LENGTH];
snake_t snake_pl2[SN_SNAKE_MAX_LENGTH];

snake_t * snake_pl0_head;
snake_t * snake_pl0_tail;

snake_t * snake_pl1_head;
snake_t * snake_pl1_tail;

snake_t * snake_pl2_head;
snake_t * snake_pl2_tail;

// HELPER FUNCTIONS --------------------
// This function will point the head, tail, and
// body functions to the correct player properties
void game3_assignPointers(int8_t player_num)
{
    if ( player_num == 0 )
    {
        head = snake_pl0_head;
        tail = snake_pl0_tail;
        body = &snake_pl0[0];
    }
    else if ( player_num == 1 )
    {
        head = snake_pl1_head;
        tail = snake_pl1_tail;
        body = &snake_pl1[0];
    }
    else if ( player_num == 2 )
    {
        head = snake_pl2_head;
        tail = snake_pl2_tail;
        body = &snake_pl2[0];
    }
}

// FUCTIONS ---------------------------------
// This function initializes the linked list
// for the snake structure
void game3_initSnake(point_t * head_and_tail, int8_t player_num)
{
    // set up the pointers correctly first
    game3_assignPointers( player_num );

    // then assign the new head, tail, next and previous
    head->alive = true;
    head->center = *head_and_tail;
    head->next = head;
    head->prev = head;
    tail = head;
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
        if ( body->alive == true )
        {
            // initialize this one as the new head
        }
        else
        {

        }

        // if the whole array has been searched through and no
        // living body segments were found, the program should
        // know that the head could not be added.
        if ( i == SN_SNAKE_MAX_LENGTH-1 && body->alive == true )
            head_added = false;
    }

    // reassign the old head pointer's previous as the
    // new head pointer

    // reassign the tail pointer's next as the new
    // head pointer
    return head_added;
}

// This function will remove the tail pointer
// for the player number's snake linked list
// and reassign to the previous.
point_t game3_rmSnakeTail(int8_t player_num)
{

}

// This function will search through the linked
// list until the correct snake structure is found
// and return the center value.
point_t game3_viewAt(int8_t i, int8_t player_num)
{

}
