#ifndef LOGIC_H
#define LOGIC_H
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "data_structure.h"
void castling(Pieces board[][8], int y, int x, int y2, int x2);
void reverse_castling(Pieces board[][8], int y, int x, int y2, int x2);
bool check_check(Pieces board[][8], int y, int x, int y2, int x2);
int *is_valid(Pieces board[][8], int y, int x, int *size);
bool check_mate(Pieces board[][8], Color player);
void add_coordinates_check(Pieces board[][8], int y1, int x1, int **valid, int *size);
#include "logic.c"
#endif