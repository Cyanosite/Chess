#ifndef BOARD_REFRESH_H
#define BOARD_REFRESH_H
void draw_selected(SDL_Renderer *renderer, int y, int x);
void draw_valid(SDL_Renderer *renderer, Pieces board[][8], int *valid, int size);
void select_pawn(SDL_Renderer *renderer, Pieces board[][8], int y, int x);
void reset_board(Pieces board[][8]);
void board_init(Pieces board[][8], Color *player);
void screen_refresh(Pieces board[][8], SDL_Renderer *renderer, SDL_Window *window, Color player, Move *lastmove);
void get_field(SDL_Event *event, int *y, int *x);
#endif