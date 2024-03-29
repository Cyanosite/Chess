#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include "debugmalloc.h"
#include "data_structure.h"
#define len 130

// Initializing the SDL graphical engine, the window, the renderer,
// the image processing and font processing engines
// param: argc and argv from main, window and renderer
int SDL_Init_chess(int argc, char *argv[], SDL_Window **window, SDL_Renderer **renderer, TTF_Font **font)
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("Error %s\n", SDL_GetError());
        return 1;
    }

    *window = SDL_CreateWindow("Chess", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1920, 1080, SDL_WINDOW_FULLSCREEN);
    if (*window == NULL)
    {
        printf("Error %s\n", SDL_GetError());
        return 2;
    }

    *renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED);
    if (*renderer == NULL)
    {
        printf("Error %s\n", SDL_GetError());
        return 3;
    }

    // Set to png as the image files are png
    if (IMG_Init(IMG_INIT_PNG) == 0)
    {
        printf("Error %s\n", SDL_GetError());
        return 4;
    }

    if (TTF_Init() == -1)
    {
        printf("Error %s\n", SDL_GetError());
        return 5;
    }
    *font = TTF_OpenFont("Font.ttf", 30);
    if (*font == NULL)
        printf("error: %s", SDL_GetError());

    SDL_Surface *icon = IMG_Load("Pieces/icon.png");
    if (icon == NULL)
    {
        printf("Error %s\n", SDL_GetError());
        return 6;
    }
    SDL_SetWindowIcon(*window, icon);
    SDL_FreeSurface(icon);
    int w, h;
    SDL_GetWindowSize(*window, &w, &h);
    SDL_RenderSetScale(*renderer, w / 1920.0, h / 1080.0);
    SDL_RenderClear(*renderer);
    return 0;
}

// Creates black text from the given string and puts it onto the
// renderer, but doesn't refresh the screen with the content
// param: Font, renderer, the text to display,
// and position coordinates of the text's starting point
void create_text(TTF_Font *font, SDL_Renderer *renderer, char *text, int w, int h)
{
    SDL_Color Black = {0, 0, 0};
    SDL_Surface *surfaceLabel = TTF_RenderText_Blended(font, text, Black);
    SDL_Texture *Label = SDL_CreateTextureFromSurface(renderer, surfaceLabel);
    SDL_Rect rectangle = {w, h, surfaceLabel->w, surfaceLabel->h};
    SDL_RenderCopy(renderer, Label, NULL, &rectangle);
    SDL_FreeSurface(surfaceLabel);
    SDL_DestroyTexture(Label);
}

// Creates the notations on the side of the chessboard, but doesn't display it
// param: Font, renderer, the text to display, swap (swap the coordinates)
void notations(TTF_Font *font, SDL_Renderer *renderer, char *text, char swap)
{
    for (int i = 0; i < 8; ++i)
    {
        int x = 0, y = 0;
        switch (swap)
        {
        case 0:
            x = len * i + len / 2 - 10;
            y = 1038;
            break;
        case 1:
            y = len * i + len / 2 - 20;
            x = 1052;
            break;
        }
        create_text(font, renderer, text, x, y);
        *text = swap ? --(*text) : ++(*text);
    }
}

// Displays the background of the game: the chessboard with the notations and
// the buttons on the right side
// param: renderer, window (to get the size of the screen for auto-scaling,
// this is not yet implemented in every function)
void display_default(SDL_Renderer *renderer, SDL_Window *window, TTF_Font *font)
{
    boxRGBA(renderer, 0, 0, 1080, 1080, 186, 140, 99, 255);

    // Create the checkerboard
    for (int i = 0; i < 8; ++i)
    {
        for (int j = 0; j < 8; ++j)
        {
            if (i % 2 == 0)
            {
                if (j % 2 == 1)
                    boxRGBA(renderer, len * j, len * i, len * (j + 1), len * (i + 1), 210, 140, 69, 255);
                else
                    boxRGBA(renderer, len * j, len * i, len * (j + 1), len * (i + 1), 255, 206, 158, 255);
            }
            else
            {
                if (j % 2 == 0)
                    boxRGBA(renderer, len * j, len * i, len * (j + 1), len * (i + 1), 210, 140, 69, 255);
                else
                    boxRGBA(renderer, len * j, len * i, len * (j + 1), len * (i + 1), 255, 206, 158, 255);
            }
        }
    }

    // Exit button COORDINATE (8,8)
    roundedRectangleRGBA(renderer, 1830, 5, 1915, 50, 5, 255, 255, 255, 255);
    roundedBoxRGBA(renderer, 1830, 5, 1915, 50, 5, 255, 255, 255, 100);
    create_text(font, renderer, "EXIT", 1850, 5);
    // New Game button COORDINATE (9, 9)
    roundedRectangleRGBA(renderer, 1150, 700, 1410, 775, 5, 255, 255, 255, 255);
    roundedBoxRGBA(renderer, 1150, 700, 1410, 775, 5, 255, 255, 255, 100);
    create_text(font, renderer, "NEW GAME", 1220, 715);
    // Revert button COORDINATE (10, 10)
    roundedRectangleRGBA(renderer, 1150, 800, 1410, 875, 5, 255, 255, 255, 255);
    roundedBoxRGBA(renderer, 1150, 800, 1410, 875, 5, 255, 255, 255, 100);
    create_text(font, renderer, "REVERT", 1235, 815);
    // Notations
    char text[2] = "A\0";
    notations(font, renderer, text, 0);
    text[0] = '8';
    notations(font, renderer, text, 1);
}

void display_win(SDL_Renderer *renderer, TTF_Font *font, Color player)
{
    char string[14];
    player = invert_color(player);
    roundedRectangleRGBA(renderer, 1150, 450, 1410, 600, 5, 255, 255, 255, 255);
    roundedBoxRGBA(renderer, 1150, 450, 1410, 600, 5, 255, 255, 255, 100);
    sprintf(string, "%s WINS!", player == black ? "WHITE" : "WHITE");
    create_text(font, renderer, string, 1200, 500);
    SDL_RenderPresent(renderer);
}
