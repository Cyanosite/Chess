#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "board.h"
#include "data_structure.h"
#include "debugmalloc.h"
#include "logic.h"
#include "display.h"

void quit()
{
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}

int main(int argc, char *argv[])
{
	// Initializing variables
	SDL_Window *window = NULL;
	SDL_Renderer *renderer = NULL;
	TTF_Font *font = NULL;
	int errorcode = SDL_Init_chess(argc, argv, &window, &renderer, &font);
	if (errorcode != 0)
		return errorcode;
	Pieces board[8][8];	  // 2 dimensional array for the state of the board
	Color player = white; // active player's color
	int selectionY = -1,
		selectionX = -1;											 // mouse click coordinates / coordinates of the selected piece
																	 // (-1 is default as the coordinates can never be negative)
	bool selected = false;											 // true if a Piece is selected
	int *valid = NULL;												 // dinamic array for valid moves (of selected piece)
	int size = 0;													 // number of elements in the valid array
	Move *lastmove = NULL;											 // Pointer to the last element of the dinamic list
																	 // that stores previous moves
	board_init(board, &player);										 // Initializing the board
	screen_refresh(board, renderer, window, font, player, lastmove); // Refresh the screen for the pieces to appear
	SDL_Event event;
	// Event loop
	while (SDL_WaitEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT:
			TTF_CloseFont(font);
			quit();
			return 0;
			break;
		case SDL_MOUSEBUTTONDOWN:
		{
			if (selected)
			{
				int destinationY = -1, destinationX = -1; // destination of the selected piece
				get_field(&event, window, &destinationY, &destinationX);
				if (destinationY == -1 && destinationX == -1)
					break;
				for (int i = 0; i < size / 2; ++i)
				{
					// if the destination of the piece is a valid move
					if (valid[i * 2] == destinationY && valid[i * 2 + 1] == destinationX)
					{
						lastmove = add_move(board, lastmove, selectionY, selectionX, destinationY, destinationX);
						if (board[selectionY][selectionX].type == king && abs(selectionX - destinationX) > 1) // castling if the king is moved 2 positions
							castling(board, selectionY, selectionX, destinationY, destinationX);
						else
						{
							board[destinationY][destinationX] = board[selectionY][selectionX];
							board[destinationY][destinationX].hasmoved = true;
							board[selectionY][selectionX] = (Pieces){0};
							if ((destinationY == 0 || destinationY == 7) && board[destinationY][destinationX].type == pawn) // if the pawn reaches the end of the board make it transform
							{
								select_pawn(renderer, window, board, destinationY, destinationX);
							}
						}
						// resetting variables
						selected = false;
						selectionY = -1;
						selectionX = -1;
						player = invert_color(player);
						screen_refresh(board, renderer, window, font, player, lastmove);

						int tempsize = 0;
						int *tempvalid = is_valid(board, destinationY, destinationX, &tempsize);
						for (int j = 0; j < tempsize / 2; ++j)
						{
							if (board[tempvalid[j * 2]][tempvalid[j * 2 + 1]].type == king)
							{
								if (check_mate(board, player))
								{
									display_win(renderer, font, player);
									reset_board(board);
									player = white;
									SDL_Event tempevent;
									while (SDL_WaitEvent(&tempevent))
									{
										if (tempevent.type == SDL_MOUSEBUTTONDOWN)
										{
											int y = 0, x = 0;
											get_field(&tempevent, window, &y, &x);
											if (y == 8 && x == 8) // Quit
											{
												quit();
												return 0;
											}
											else if (y == 9 && x == 9) // New Game
											{
												screen_refresh(board, renderer, window, font, player, lastmove);
												free_moves(lastmove);
												lastmove = NULL;
												selectionX = -1;
												selectionY = -1;
												break;
											}
										}
									}
								}
								break;
							}
						}
						free(tempvalid);
						break;
					}
				}
				if (selected)
				{
					selectionY = destinationY;
					selectionX = destinationX;
					screen_refresh(board, renderer, window, font, player, lastmove);
				}
			}
			if (selectionY == -1 && selectionX == -1)
				get_field(&event, window, &selectionY, &selectionX);
			if (selectionY < 8 && selectionX < 8) // if the player has clicked the board
			{
				if (board[selectionY][selectionX].color == player) // if the Piece on the click's coordinates matches the color of the player
				{
					add_coordinates_check(board, selectionY, selectionX, &valid, &size);
					if (valid != NULL)
					{
						draw_valid(renderer, board, valid, size);
						draw_selected(renderer, selectionY, selectionX);
						SDL_RenderPresent(renderer);
						selected = true;
					}
					else
					{
						selectionY = -1;
						selectionX = -1;
					}
				}
				else
				{
					selectionY = -1;
					selectionX = -1;
					selected = false;
				}
			}
			else
			{
				if (selectionX == 8 && selectionY == 8) // Exit button
				{
					quit();
					return 0;
				}
				else if (selectionX == 9 && selectionY == 9) // New Game button
				{
					player = white;
					reset_board(board);
					free_moves(lastmove);
					lastmove = NULL;
					screen_refresh(board, renderer, window, font, player, lastmove);
					selectionX = -1;
					selectionY = -1;
				}
				else if (selectionX == 10 && selectionY == 10) // Revert button
				{
					if (lastmove != NULL) // only invert if there are previous moves otherwise the player doesn't change
						player = invert_color(player);
					lastmove = revert(board, lastmove);
					screen_refresh(board, renderer, window, font, player, lastmove);
					selectionX = -1;
					selectionY = -1;
				}
			}
			break;
		}
		}
	}
	return 0;
}
