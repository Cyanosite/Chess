#ifndef DATA_STRUCTURE_H
#define DATA_STRUCTURE_H
#include <stdbool.h>
typedef enum Color
{
    empty,
    black,
    white
} Color;

typedef enum Type
{
    bishop,
    king,
    knight,
    pawn,
    queen,
    rook
} Type;

char typestring[6][7] = {{"bishop"}, {"king"}, {"knight"}, {"pawn"}, {"queen"}, {"rook"}};

typedef struct Pieces
{
    Color color;
    Type type;
    bool hasmoved;
} Pieces;

typedef struct Move
{
    Pieces capturer;
    Pieces captured;
    int y1, x1, y2, x2;
    struct Move *next;
} Move;
Color invert_color(Color color);
Move *revert(Pieces board[][8], Move *lastmove);
Move *add_move(Pieces board[][8], Move *lastmove, int y1, int x1, int y2, int x2);
void free_moves(Move *lastmove);
void add_coordinates(int **valid, int *size, int y, int x);
#include "data_structure.c"
#endif