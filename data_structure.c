#include "data_structure.h"
#include "logic.h"
#include "debugmalloc.h"

// swaps black to white and white to black
Color invert_color(Color color)
{
    switch (color)
    {
    case black:
        color = white;
        break;
    case white:
        color = black;
        break;
    default:
        color = empty;
    }
    return color;
}

// reverts a move
Move *revert(Pieces board[][8], Move *lastmove)
{
    if (lastmove == NULL)
        return lastmove;
    if (lastmove->capturer.type == king && abs(lastmove->x1 - lastmove->x2) > 1)
    {
        reverse_castling(board, lastmove->y1, lastmove->x1, lastmove->y2, lastmove->x2);
    }
    else
    {
        board[lastmove->y1][lastmove->x1] = lastmove->capturer;
        board[lastmove->y2][lastmove->x2] = lastmove->captured;
    }
    Move *temp = lastmove->next;
    free(lastmove);
    return temp;
}

// add the last move to the list
Move *add_move(Pieces board[][8], Move *lastmove, int y1, int x1, int y2, int x2)
{
    Move *temp = (Move *)malloc(sizeof(Move));
    *temp = (Move){board[y1][x1], board[y2][x2], y1, x1, y2, x2};
    if (lastmove == NULL)
    {
        temp->next = NULL;
    }
    else
    {
        temp->next = lastmove;
    }
    return temp;
}

// deallocates the moves list
void free_moves(Move *lastmove)
{
    while (lastmove != NULL)
    {
        Move *temp = lastmove->next;
        free(lastmove);
        lastmove = temp;
    }
}

// adds coordinates to an array
// param: pointer of array, pointer to the size of the array, coordinates to add
void add_coordinates(int **valid, int *size, int y, int x)
{
    *size += 2;
    *valid = (int *)realloc(*valid, (*size) * sizeof(int));
    (*valid)[*size - 2] = y;
    (*valid)[*size - 1] = x;
}
