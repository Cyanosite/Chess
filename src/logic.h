#ifndef LOGIC_H
#define LOGIC_H
void castling(Pieces board[][8], int y, int x, int y2, int x2);
void reverse_castling(Pieces board[][8], int y, int x, int y2, int x2);
bool check_check(Pieces board[][8], int y, int x, int y2, int x2);
int *is_valid(Pieces board[][8], int y, int x, int *size);
bool check_mate(Pieces board[][8], Color player);
void add_coordinates_check(Pieces board[][8], int y1, int x1, int **valid, int *size);
#endif