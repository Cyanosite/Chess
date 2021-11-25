#ifndef SDL_INIT_H
#define SDL_INIT_H
int SDL_Init_chess(int argc, char *argv[], SDL_Window **window, SDL_Renderer **renderer);
void create_text(TTF_Font *Font, SDL_Renderer *renderer, char *text, int w, int h);
void display_default(SDL_Renderer *renderer, SDL_Window *window);
#endif