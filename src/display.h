#ifndef SDL_INIT_H
#define SDL_INIT_H
int SDL_Init_chess(int argc, char *argv[], SDL_Window **window, SDL_Renderer **renderer, TTF_Font **font);
void create_text(TTF_Font *font, SDL_Renderer *renderer, char *text, int w, int h);
void display_default(SDL_Renderer *renderer, SDL_Window *window, TTF_Font *font);
void display_win(SDL_Renderer *renderer, TTF_Font *font, Color player);
#endif