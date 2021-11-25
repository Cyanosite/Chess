#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include "debugmalloc.h"
#define len 130

// Initializing the SDL graphical engine, the window, the renderer,
// the image processing and font processing engines
// param: argc and argv from main, window and renderer
int SDL_Init_chess(int argc, char *argv[], SDL_Window **window, SDL_Renderer **renderer)
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("Error %s\n", SDL_GetError());
        return 1;
    }

    *window = SDL_CreateWindow("Chess", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1920, 1080, SDL_WINDOW_FULLSCREEN_DESKTOP);
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

    SDL_RenderClear(*renderer);
}

// Creates black text from the given string and puts it onto the
// renderer, but doesn't refresh the screen with the content
// param: Font, renderer, the text to display,
// and position coordinates of the text's starting point
void create_text(TTF_Font *Font, SDL_Renderer *renderer, char *text, int w, int h)
{
    SDL_Color Black = {0, 0, 0};
    SDL_Surface *surfaceLabel = TTF_RenderText_Blended(Font, text, Black);
    SDL_Texture *Label = SDL_CreateTextureFromSurface(renderer, surfaceLabel);
    SDL_Rect rectangle = {w, h, surfaceLabel->w, surfaceLabel->h};
    SDL_RenderCopy(renderer, Label, NULL, &rectangle);
    SDL_FreeSurface(surfaceLabel);
    SDL_DestroyTexture(Label);
}

// Creates the notations on the side of the chessboard, but doesn't display it
// param: Font, renderer, the text to display, swap (swap the coordinates)
void notations(TTF_Font *Font, SDL_Renderer *renderer, char *text, char swap)
{
    for (int i = 0; i < 8; ++i)
    {
        int x = 0, y = 0;
        switch (swap)
        {
        case 0:
            x = len * i + len / 2 - 10;
            y = 1040;
            break;
        case 1:
            y = len * i + len / 2 - 15;
            x = 1050;
            break;
        }
        create_text(Font, renderer, text, x, y);
        switch (swap)
        {
        case 0:
            ++(*text);
            break;
        case 1:
            --(*text);
            break;
        }
    }
}

// Displays the background of the game: the chessboard with the notations and
// the buttons on the right side
// param: renderer, window (to get the size of the screen for auto-scaling,
// this is not yet implemented in every function)
void display_default(SDL_Renderer *renderer, SDL_Window *window)
{
    TTF_Init();
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
    TTF_Font *Font = TTF_OpenFont("Font.ttf", 30);
    if (Font == NULL)
        printf("error: %s", SDL_GetError());

    int w, h;
    SDL_GetWindowSize(window, &w, &h);
    // Exit button with resolution scaling COORDINATE (8,8)
    boxRGBA(renderer, w - w * 0.05, 0, w, 50, 200, 200, 200, 255);
    create_text(Font, renderer, "EXIT", w - w * 0.041, 5);
    // New Game button with resolution scaling COORDINATE (9, 9)
    boxRGBA(renderer, w - w * 0.15, 0, w - w * 0.05, 50, 100, 100, 100, 255);
    create_text(Font, renderer, "NEW GAME", w - w * 0.14, 5);
    // Revert button with resolution scaling COORDINATE (10, 10)
    boxRGBA(renderer, w - w * 0.26, 0, w - w * 0.15, 50, 150, 150, 150, 255);
    create_text(Font, renderer, "REVERT", w - w * 0.23, 5);
    // Notations
    char text[2] = "A\0";
    notations(Font, renderer, text, 0);
    text[0] = '8';
    notations(Font, renderer, text, 1);
    TTF_CloseFont(Font);
    TTF_Quit();
}
