#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "data_structure.h"
#include "debugmalloc.h"
#include "sdl_init.h"
#define len 130
const char typestring[6][7] = {{"bishop"}, {"king"}, {"knight"}, {"pawn"}, {"queen"}, {"rook"}};

// draws a rounded rectangle around the selected piece (only if it has valid
// moves) param: renderer, coordinates of the field to draw a circle around
void draw_selected(SDL_Renderer *renderer, int y, int x)
{
    roundedRectangleRGBA(renderer, x * len, y * len, (x + 1) * len, (y + 1) * len, 10, 0, 0, 0, 255);
}

// draws a yellow circle on the fields that are valid moves of a piece
// param: renderer, board, array of the valid moves, size of the array
void draw_valid(SDL_Renderer *renderer, Pieces board[][8], int *valid, int size)
{
    if (valid == NULL)
        return;
    for (int i = 0; i < size / 2; ++i)
    {
        if (board[valid[i * 2]][valid[i * 2 + 1]].type != king)
            filledCircleRGBA(renderer, len * valid[i * 2 + 1] + len / 2, len * valid[i * 2] + len / 2, 20, 255, 255, 158, 255);
    }
}

// select screen to transform the pawn when it reaches the end of the board
// param: renderer, board, destination coordinates of the pawn
void select_pawn(SDL_Renderer *renderer, SDL_Window *window, Pieces board[][8], int y, int x)
{
    boxRGBA(renderer, 1150, 130, 1280, 260, 210, 140, 69, 255);
    boxRGBA(renderer, 1280, 130, 1410, 260, 255, 206, 158, 255);
    boxRGBA(renderer, 1150, 260, 1280, 390, 255, 206, 158, 255);
    boxRGBA(renderer, 1280, 260, 1410, 390, 210, 140, 69, 255);
    Type pieces[2][2] = {{queen, rook}, {bishop, knight}};
    SDL_Surface *piece;
    for (int i = 0; i < 2; ++i)
    {
        for (int j = 0; j < 2; ++j)
        {
            char string[24] = {0};
            sprintf(string, "Pieces/%s_%s.png", board[y][x].color == black ? "black" : "white", typestring[pieces[i][j]]);
            piece = IMG_Load(string);
            SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, piece);
            SDL_Rect rectangle = {1150 + len * i, 130 + len * j, piece->w, piece->h};
            SDL_RenderCopy(renderer, texture, NULL, &rectangle);
            SDL_DestroyTexture(texture);
            SDL_FreeSurface(piece);
        }
    }
    roundedRectangleRGBA(renderer, 1150, 130, 1412, 392, 0, 255, 255, 255, 255);
    SDL_RenderPresent(renderer);
    int w, h;
    SDL_GetWindowSize(window, &w, &h);
    SDL_Event event;
    while (SDL_WaitEvent(&event))
    {
        if (event.type == SDL_MOUSEBUTTONDOWN)
        {
            int tempx = (event.button.x - 1150 * w / 1920.0) / len;
            int tempy = (event.button.y - 130 * h / 1080.0) / len;
            if (tempx >= 0 && tempx < 2 && tempy >= 0 && tempy < 2)
            {
                board[y][x].type = pieces[tempx][tempy];
                break;
            }
        }
    }
}

// Resets the pieces on the board to their initial positions
void reset_board(Pieces board[][8])
{
    for (int i = 0; i < 8; ++i)
    {
        for (int j = 0; j < 8; ++j)
        {
            board[i][j].hasmoved = false;
            if (i < 2)
                board[i][j].color = black;
            else if (i < 6)
            {
                board[i][j].color = empty;
                board[i][j].type = 0;
            }
            else
                board[i][j].color = white;
            if (i == 1 || i == 6)
                board[i][j].type = pawn;
            if (i == 0 || i == 7)
            {
                switch (j)
                {
                case 0:
                    board[i][j].type = rook;
                    break;
                case 1:
                    board[i][j].type = knight;
                    break;
                case 2:
                    board[i][j].type = bishop;
                    break;
                case 3:
                    board[i][j].type = queen;
                    break;
                case 4:
                    board[i][j].type = king;
                    break;
                case 5:
                    board[i][j].type = bishop;
                    break;
                case 6:
                    board[i][j].type = knight;
                    break;
                case 7:
                    board[i][j].type = rook;
                    break;
                }
            }
        }
    }
}

// Initializes the board at launch: reads the state of the board from the
// save.txt file, if this is empty then everything be set to default
// param: board, player (to load the saved active player)
void board_init(Pieces board[][8], Color *player)
{
    FILE *txt = fopen("save.txt", "r");
    int firstchar;
    fscanf(txt, "%d", &firstchar);
    if (txt == NULL || firstchar != 1)
    {
        reset_board(board);
    }
    else
    {
        fscanf(txt, "%d", player);
        for (int i = 0; i < 8; ++i)
        {
            for (int j = 0; j < 8; ++j)
            {
                fscanf(txt, "%d %d %d", &board[i][j].color, &board[i][j].type, &board[i][j].hasmoved);
            }
        }
    }
    fclose(txt);
}

// Saves the state of the board and the active player's color into the save.txt
// Refreshed the screen with the pieces on the board
// Displays 5 of the latest moves and also displays the active player
void screen_refresh(Pieces board[][8], SDL_Renderer *renderer, SDL_Window *window, Color player, Move *lastmove)
{
    FILE *txt = fopen("save.txt", "w");
    fprintf(txt, "%d %d \n", txt == NULL ? 0 : 1, player);
    SDL_SetRenderDrawColor(renderer, 186, 140, 99, 255);
    SDL_RenderClear(renderer);
    display_default(renderer, window);
    for (int i = 0; i < 8; ++i)
    {
        for (int j = 0; j < 8; ++j)
        {
            fprintf(txt, "%d %d %d %s", board[i][j].color, board[i][j].type, board[i][j].hasmoved, j == 7 ? "\n" : "");
            if (board[i][j].color != empty)
            {
                SDL_Surface *piece;
                char string[24] = {0};
                sprintf(string, "Pieces/%s_%s.png", board[i][j].color == black ? "black" : "white", typestring[board[i][j].type]);
                piece = IMG_Load(string);
                SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, piece);
                SDL_Rect rectangle = {130 * j, 130 * i, piece->w, piece->h};
                SDL_RenderCopy(renderer, texture, NULL, &rectangle);
                SDL_DestroyTexture(texture);
                SDL_FreeSurface(piece);
            }
        }
    }
    TTF_Font *Font = TTF_OpenFont("Font.ttf", 30);
    if (Font == NULL)
        printf("error: %s", SDL_GetError());
    char string[13];
    for (int i = 0; i < 5 && lastmove != NULL; ++i)
    {
        roundedRectangleRGBA(renderer, 1650, 530 - 100 * i, 1850, 605 - 100 * i, 5, 255, 255, 255, 255);
        roundedBoxRGBA(renderer, 1650, 530 - 100 * i, 1850, 605 - 100 * i, 5, 255, 255, 255, 100);
        sprintf(string, "%c%d -> %c%d", 65 + lastmove->x1, 8 - lastmove->y1, 65 + lastmove->x2, 8 - lastmove->y2);
        create_text(Font, renderer, string, 1710, 545 - 100 * i);
        lastmove = lastmove->next;
    }
    lineRGBA(renderer, 1080, 0, 1080, 1080, 0, 0, 0, 255);
    roundedRectangleRGBA(renderer, 1150, 900, 1410, 975, 5, 255, 255, 255, 255);
    roundedBoxRGBA(renderer, 1150, 900, 1410, 975, 5, 255, 255, 255, 100);
    sprintf(string, "%s MOVES!", player == black ? "BLACK" : "WHITE");
    create_text(Font, renderer, string, 1200, 915);
    TTF_CloseFont(Font);
    fclose(txt);
    SDL_RenderPresent(renderer);
}

// returns coordinates from the clicked pixel coordinates
// param: event, y, x (pointer to the coordinates to edit)
void get_field(SDL_Event *event, SDL_Window *window, int *y, int *x)
{
    int w, h;
    SDL_GetWindowSize(window, &w, &h);
    int tempx = event->button.x;
    int tempy = event->button.y;
    if (tempy < (len * 8) * h / 1080.0 && tempx < (len * 8) * w / 1920.0)
    {
        *y = tempy / (len * h / 1080.0);
        *x = tempx / (len * w / 1920.0);
        return;
    }
    if (tempx > 1150 * w / 1920.0 && tempx < 1410 * w / 1920.0)
    {
        if (tempy > 700 * h / 1080.0 && tempy < 775 * h / 1080.0)
        {
            *y = 9;
            *x = 9;
            return;
        }
        else if (tempy > 800 * h / 1080.0 && tempy < 875 * h / 1080.0)
        {
            *y = 10;
            *x = 10;
            return;
        }
    }
    if (tempx > 1830 * w / 1920.0 && tempy < 50 * h / 1080.0)
    {
        *y = 8;
        *x = 8;
        return;
    }
}
