//#include <SDL2/SDL2_gfxPrimitives.h>
#include "data_structure.h"
#include "logic.h" // compiler won't see some functions when needed if the header isn't included
#include "debugmalloc.h"
#define len 130

// execution of the castling move
// param: board, king's initial and destination coordinates
void castling(Pieces board[][8], int y, int x, int y2, int x2)
{
    board[y2][x2] = board[y][x];
    board[y][x] = (Pieces){0};
    if (x2 < x)
    {
        board[y][x - 1] = board[y][0];
        board[y][0] = (Pieces){0};
    }
    else
    {
        board[y][x + 1] = board[y][7];
        board[y][7] = (Pieces){0};
    }
}

// reverts the castling (may be used when the revert button is pressed)
// param: board, king's initial and destination coordinates
void reverse_castling(Pieces board[][8], int y, int x, int y2, int x2)
{
    board[y][x] = board[y2][x2];
    board[y2][x2] = (Pieces){0};
    if (x2 < x)
    {
        board[y][0] = board[y][x - 1];
        board[y][x - 1] = (Pieces){0};
    }
    else
    {
        board[y][7] = board[y][x + 1];
        board[y][x + 1] = (Pieces){0};
    }
}

// returns all the possible moves of the selected rook in a dynamically allocated array
// this doesn't check if this would cause the king to be in check!
// param: board, coordinates of the piece, pointer to the size of the array
int *rook_valid(Pieces board[][8], int y, int x, int *size)
{
    int *valid = NULL;
    const Color color = board[y][x].color;
    const Color othercolor = invert_color(color);
    for (int i = 1; y + i < 8; ++i)
    {
        if (board[y + i][x].color == color)
            break;
        else if (board[y + i][x].color == othercolor)
        {
            add_coordinates(&valid, size, y + i, x);
            break;
        }
        add_coordinates(&valid, size, y + i, x);
    }
    for (int i = 1; y - i >= 0; ++i)
    {
        if (board[y - i][x].color == color)
            break;
        else if (board[y - i][x].color == othercolor)
        {
            add_coordinates(&valid, size, y - i, x);
            break;
        }
        add_coordinates(&valid, size, y - i, x);
    }
    for (int i = 1; x + i < 8; ++i)
    {
        if (board[y][x + i].color == color)
            break;
        else if (board[y][x + i].color == othercolor)
        {
            add_coordinates(&valid, size, y, x + i);
            break;
        }
        add_coordinates(&valid, size, y, x + i);
    }
    for (int i = 1; x - i >= 0; ++i)
    {
        if (board[y][x - i].color == color)
            break;
        else if (board[y][x - i].color == othercolor)
        {
            add_coordinates(&valid, size, y, x - i);
            break;
        }
        add_coordinates(&valid, size, y, x - i);
    }
    return valid;
}

// returns all the possible moves of the selected bishop in a dynamically allocated array
// this doesn't check if this would cause the king to be in check!
// param: board, coordinates of the piece, pointer to the size of the array
int *bishop_valid(Pieces board[][8], int y, int x, int *size)
{
    int *valid = NULL;
    const Color color = board[y][x].color;
    const Color othercolor = invert_color(color);
    for (int i = 1; y - i >= 0 && x - i >= 0; ++i)
    {
        if (board[y - i][x - i].color == color)
            break;
        if (board[y - i][x - i].color == othercolor)
        {
            add_coordinates(&valid, size, y - i, x - i);
            break;
        }
        add_coordinates(&valid, size, y - i, x - i);
    }
    for (int i = 1; y - i >= 0 && x + i < 8; ++i)
    {
        if (board[y - i][x + i].color == color)
            break;
        if (board[y - i][x + i].color == othercolor)
        {
            add_coordinates(&valid, size, y - i, x + i);
            break;
        }
        add_coordinates(&valid, size, y - i, x + i);
    }
    for (int i = 1; y + i < 8 && x - i >= 0; ++i)
    {
        if (board[y + i][x - i].color == color)
            break;
        if (board[y + i][x - i].color == othercolor)
        {
            add_coordinates(&valid, size, y + i, x - i);
            break;
        }
        add_coordinates(&valid, size, y + i, x - i);
    }
    for (int i = 1; y + i < 8 && x + i < 8; ++i)
    {
        if (board[y + i][x + i].color == color)
            break;
        if (board[y + i][x + i].color == othercolor)
        {
            add_coordinates(&valid, size, y + i, x + i);
            break;
        }
        add_coordinates(&valid, size, y + i, x + i);
    }
    return valid;
}

// only returns the destination coordinates of the moves in which the pawn may capture a piece
// in a dinamically allocated array (used in check_check function)
// param: board, coordinates of the piece, pointer to the size of the array
int *pawn_capture(Pieces board[][8], int y, int x, int *size)
{
    int *valid = NULL;
    int dir = 0;
    Color color;

    if (board[y][x].color == black)
    {
        dir = 1;
        color = black;
    }
    else
    {
        dir = -1;
        color = white;
    }

    if (x - 1 >= 0 && board[y + dir][x - 1].color != color)
        add_coordinates(&valid, size, y + dir, x - 1);
    if (x + 1 < 8 && board[y + dir][x + 1].color != color)
        add_coordinates(&valid, size, y + dir, x + 1);

    return valid;
}

// returns all the possible moves of the selected pawn in a dynamically allocated array
// this doesn't check if this would cause the king to be in check!
// param: board, coordinates of the piece, pointer to the size of the array
int *pawn_valid(Pieces board[][8], int y, int x, int *size)
{
    int *valid = NULL;
    Color othercolor;
    int dir = 0;
    int startpos = -1;
    if (board[y][x].color == black)
    {
        startpos = 1;
        dir = 1;
        othercolor = white;
    }
    else
    {
        startpos = 6;
        dir = -1;
        othercolor = black;
    }
    if (board[y + dir][x].color == empty)
    {
        add_coordinates(&valid, size, y + dir, x);
        if (y == startpos && board[y + (2 * dir)][x].color == empty)
            add_coordinates(&valid, size, y + (2 * dir), x);
    }
    if (x - 1 >= 0 && board[y + dir][x - 1].color == othercolor)
    {
        add_coordinates(&valid, size, y + dir, x - 1);
    }
    if (x + 1 < 8 && board[y + dir][x + 1].color == othercolor)
    {
        add_coordinates(&valid, size, y + dir, x + 1);
    }
    return valid;
}

// returns all the possible moves of the selected knight in a dynamically allocated array
// this doesn't check if this would cause the king to be in check!
// param: board, coordinates of the piece, pointer to the size of the array
int *knight_valid(Pieces board[][8], int y, int x, int *size)
{
    int *valid = NULL;
    const Color color = board[y][x].color;
    int moves[8][2] = {{2, 1}, {2, -1}, {-2, 1}, {-2, -1}, {1, 2}, {1, -2}, {-1, 2}, {-1, -2}};
    for (int i = 0; i < 8; ++i)
    {
        if (y + moves[i][0] < 8 && y + moves[i][0] >= 0 && x + moves[i][1] < 8 && x + moves[i][1] >= 0 && board[y + moves[i][0]][x + moves[i][1]].color != color)
        {
            add_coordinates(&valid, size, y + moves[i][0], x + moves[i][1]);
        }
    }
    return valid;
}

// returns all the possible moves of the selected queen in a dynamically allocated array
// it does this by applying both the rook and bishop moves to the queen
// this doesn't check if this would cause the king to be in check!
// param: board, coordinates of the piece, pointer to the size of the array
int *queen_valid(Pieces board[][8], int y, int x, int *size)
{
    int *rook = rook_valid(board, y, x, size);
    int tempsize = 0;
    int *bishop = bishop_valid(board, y, x, &tempsize);
    *size += tempsize;
    rook = (int *)realloc(rook, *size * sizeof(int));
    for (int i = 0; i < tempsize; ++i)
    {
        rook[*size - tempsize + i] = bishop[i];
    }
    free(bishop);
    return rook;
}

// checks if the king of the same color will be in check if the piece would be moved
// to a different position
// param: board, initial and destination coordinates of the piece
bool check_check(Pieces board[][8], int y, int x, int y2, int x2)
{
    int kingy = 0, kingx = 0;
    const Color color = board[y][x].color;
    const Color othercolor = invert_color(color);

    Pieces tempboard[8][8];
    memcpy(&tempboard, board, sizeof(Pieces) * 64);
    Pieces temp2board[8][8];
    memcpy(&temp2board, board, sizeof(Pieces) * 64);

    if (board[y][x].type == king && abs(x - x2) > 1)
    {
        castling(temp2board, y, x, y2, x2);
        castling(tempboard, y, x, y2, x2);
    }
    else
    {
        temp2board[y][x] = (Pieces){0};
        temp2board[y2][x2] = board[y][x];
        tempboard[y][x] = (Pieces){0};
        tempboard[y2][x2] = board[y][x];
    }

    for (int i = 0; i < 8; ++i)
    {
        for (int j = 0; j < 8; ++j)
        {
            if (temp2board[i][j].type == king && temp2board[i][j].color == color)
            {
                kingy = i;
                kingx = j;
            }
        }
    }

    for (int i = 0; i < 8; ++i)
    {
        for (int j = 0; j < 8; ++j)
        {
            if (temp2board[i][j].color == othercolor)
            {
                int tempsize = 0;
                int *tempvalid = NULL;
                switch (temp2board[i][j].type)
                {
                case pawn:
                {
                    tempvalid = pawn_capture(temp2board, i, j, &tempsize);
                    break;
                }
                case king:
                {
                    int moves[8][2] = {{-1, 0}, {-1, 1}, {0, 1}, {1, 1}, {1, 0}, {1, -1}, {0, -1}, {-1, -1}};
                    for (int k = 0; k < 8; ++k)
                    {
                        if (i + moves[k][0] >= 0 && j + moves[k][1] >= 0 && i + moves[k][0] < 8 && j + moves[k][1] < 8)
                        {
                            if (board[i + moves[k][0]][j + moves[k][1]].color != color)
                                add_coordinates(&tempvalid, &tempsize, i + moves[k][0], j + moves[k][1]);
                        }
                    }
                    break;
                }
                default:
                {
                    tempvalid = is_valid(temp2board, i, j, &tempsize);
                    break;
                }
                }
                for (int k = 0; k < tempsize / 2; ++k)
                {
                    tempboard[tempvalid[k * 2]][tempvalid[k * 2 + 1]].color = othercolor;
                }
                free(tempvalid);
            }
        }
    }
    return tempboard[kingy][kingx].color == othercolor;
}

// returns all the possible moves of the selected king in a dynamically allocated array
// this doesn't check if this would cause the king to be in check!
// param: board, coordinates of the piece, pointer to the size of the array
int *king_valid(Pieces board[][8], int y, int x, int *size)
{
    int *valid = NULL;
    const Color color = board[y][x].color;
    const Color othercolor = invert_color(color);

    int moves[8][2] = {{-1, 0}, {-1, 1}, {0, 1}, {1, 1}, {1, 0}, {1, -1}, {0, -1}, {-1, -1}};
    for (int i = 0; i < 8; ++i)
    {
        if (y + moves[i][0] >= 0 && x + moves[i][1] >= 0 && y + moves[i][0] < 8 && x + moves[i][1] < 8)
        {
            if (board[y + moves[i][0]][x + moves[i][1]].color != color)
                add_coordinates(&valid, size, y + moves[i][0], x + moves[i][1]);
        }
    }

    if (board[y][x].hasmoved == false && !check_check(board, y, x, y, x))
    {
        if (board[y][0].hasmoved == false && board[y][0].type == rook && board[y][x - 1].color == empty && board[y][x - 2].color == empty && board[y][x - 3].color == empty) // && !check_check(board, y, x, y, x - 2))
            add_coordinates(&valid, size, y, x - 2);

        if (board[y][7].hasmoved == false && board[y][7].type == rook && board[y][x + 1].color == empty && board[y][x + 2].color == empty) // && !check_check(board, y, x, y, x + 2))
            add_coordinates(&valid, size, y, x + 2);
    }
    return valid;
}

// returns all the possible moves of the selected piece in a dinamically allocated array
// this doesn't check if this would cause the king to be in check!
// param: board, coordinates of the piece, pointer to the size of the array
int *is_valid(Pieces board[][8], int y, int x, int *size)
{
    int *array = NULL;
    switch (board[y][x].type)
    {
    case pawn:
    {
        array = pawn_valid(board, y, x, size);
        break;
    }
    case rook:
    {
        array = rook_valid(board, y, x, size);
        break;
    }
    case bishop:
    {
        array = bishop_valid(board, y, x, size);
        break;
    }
    case queen:
    {
        array = queen_valid(board, y, x, size);
        break;
    }
    case king:
    {
        array = king_valid(board, y, x, size);
        break;
    }
    case knight:
    {
        array = knight_valid(board, y, x, size);
        break;
    }
    }
    return array;
}

// checks whether or not a checkmate has happened
// if none of the Pieces can move (from the player's team) then it's checkmate
// param: board, color of the player
bool check_mate(Pieces board[][8], Color player)
{
    for (int i = 0; i < 8; ++i)
    {
        for (int j = 0; j < 8; ++j)
        {
            if (board[i][j].color == player)
            {
                int tempsize = 0;
                int *tempvalid = is_valid(board, i, j, &tempsize);
                for (int k = 0; k < tempsize / 2; ++k)
                {
                    if (!check_check(board, i, j, tempvalid[k * 2], tempvalid[k * 2 + 1]))
                    {
                        free(tempvalid);
                        return false;
                    }
                }
                free(tempvalid);
            }
        }
    }
    return true;
}

// makes the pointer it recieves point to the dinamically allocated array with all of the
// valid moves of a piece, this also checks for potential check
// param: board, coordinates of the piece, pointer to the valid moves, size of the array
void add_coordinates_check(Pieces board[][8], int y1, int x1, int **valid, int *size)
{
    int tempsize = 0;
    int *tempvalid = is_valid(board, y1, x1, &tempsize);
    *size = 0;
    free(*valid);
    *valid = NULL;
    for (int i = 0; i < tempsize / 2; ++i)
    {
        if (!check_check(board, y1, x1, tempvalid[i * 2], tempvalid[i * 2 + 1]))
        {
            add_coordinates(valid, size, tempvalid[i * 2], tempvalid[i * 2 + 1]);
        }
    }
    free(tempvalid);
}
