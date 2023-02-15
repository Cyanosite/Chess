#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_ttf.h>

#include "board.h"
#include "data_structure.h"
#include "debugmalloc.h"
#include "logic.h"
#include "sdl_init.h"

int main(int argc, char *argv[])
{
	// Initializing variables
	SDL_Window *window = NULL;
	SDL_Renderer *renderer = NULL;
	int errorcode = SDL_Init_chess(argc, argv, &window, &renderer);
	if (errorcode != 0)
		return errorcode;
	Pieces board[8][8];	  // 2 dimensional array for the state of the board
	Color player = white; // active player's color
	int y1 = -1,
		x1 = -1;											   // mouse click coordinates / coordinates of the selected piece
															   // (-1 is default as the coordinates can never be negative)
	bool selected = false;									   // true if a Piece is selected
	int *valid = NULL;										   // dinamic array for valid moves (of selected piece)
	int size = 0;											   // number of elements in the valid array
	Move *lastmove = NULL;									   // Pointer to the last element of the dinamic list
															   // that stores previous moves
	board_init(board, &player);								   // Initializing the board
	screen_refresh(board, renderer, window, player, lastmove); // Refresh the screen for the pieces to appear
	SDL_Event event;
	// Event loop
	while (SDL_WaitEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT:
			TTF_Quit();
			free(valid);
			free_moves(lastmove);
			SDL_Quit();
			return 0;
			break;
		case SDL_MOUSEBUTTONDOWN:
		{
			if (selected)
			{
				int y2 = -1, x2 = -1; // destination of the selected piece
				get_field(&event, window, &y2, &x2);
				if (y2 != -1 && x2 != -1)
				{
					for (int i = 0; i < size / 2; ++i)
					{
						if (valid[i * 2] == y2 &&
							valid[i * 2 + 1] ==
								x2) // if the destination of the piece is a
									// valid move
						{
							lastmove =
								add_move(board, lastmove, y1, x1, y2, x2);
							if (board[y1][x1].type == king && abs(x1 - x2) > 1) // castling if the king is moved 2 positions
							{
								castling(board, y1, x1, y2, x2);
							}
							else
							{
								board[y2][x2] = board[y1][x1];
								board[y2][x2].hasmoved = true;
								board[y1][x1] = (Pieces){0};
								if ((y2 == 0 || y2 == 7) && board[y2][x2].type == pawn) // if the pawn reaches the end of the board make it transform
								{
									select_pawn(renderer, window, board, y2, x2);
								}
							}
							// resetting variables
							selected = false;
							y1 = -1;
							x1 = -1;
							player = invert_color(player);
							screen_refresh(board, renderer, window, player, lastmove);

							int tempsize = 0;
							int *tempvalid = is_valid(board, y2, x2, &tempsize);
							for (int j = 0; j < tempsize / 2; ++j)
							{
								if (board[tempvalid[j * 2]][tempvalid[j * 2 + 1]].type == king)
								{
									if (check_mate(board, player))
									{
										TTF_Font *Font = TTF_OpenFont("Font.ttf", 34);
										char string[14];
										player = invert_color(player);
										roundedRectangleRGBA(renderer, 1150, 450, 1410, 600, 5, 255, 255, 255, 255);
										roundedBoxRGBA(renderer, 1150, 450, 1410, 600, 5, 255, 255, 255, 100);
										sprintf(string, "%s WINS!", player == black ? "WHITE" : "WHITE");
										create_text(Font, renderer, string, 1200, 500);
										TTF_CloseFont(Font);
										SDL_RenderPresent(renderer);
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
													screen_refresh(board, renderer, window, player, lastmove);
													TTF_Quit();
													free(valid);
													free(tempvalid);
													free_moves(lastmove);
													SDL_Quit();
													return 0;
												}
												else if (y == 9 && x == 9) // New Game
												{
													screen_refresh(board, renderer, window, player, lastmove);
													free_moves(lastmove);
													lastmove = NULL;
													x1 = -1;
													y1 = -1;
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
						y1 = y2;
						x1 = x2;
						screen_refresh(board, renderer, window, player, lastmove);
					}
				}
			}
			if (y1 == -1 && x1 == -1)
				get_field(&event, window, &y1, &x1);
			if (y1 < 8 && x1 < 8) // if the player has clicked the board
			{
				if (board[y1][x1].color == player) // if the Piece on the click's coordinates matches the color of the player
				{
					add_coordinates_check(board, y1, x1, &valid, &size);
					if (valid != NULL)
					{
						draw_valid(renderer, board, valid, size);
						draw_selected(renderer, y1, x1);
						SDL_RenderPresent(renderer);
						selected = true;
					}
					else
					{
						y1 = -1;
						x1 = -1;
					}
				}
				else
				{
					y1 = -1;
					x1 = -1;
					selected = false;
				}
			}
			else
			{
				if (x1 == 8 && y1 == 8) // Exit button
				{
					TTF_Quit();
					free(valid);
					free_moves(lastmove);
					SDL_Quit();
					return 0;
				}
				else if (x1 == 9 && y1 == 9) // New Game button
				{
					player = white;
					reset_board(board);
					free_moves(lastmove);
					lastmove = NULL;
					screen_refresh(board, renderer, window, player, lastmove);
					x1 = -1;
					y1 = -1;
				}
				else if (x1 == 10 && y1 == 10) // Revert button
				{
					if (lastmove != NULL) // only invert if there are previous moves otherwise the player doesn't change
						player = invert_color(player);
					lastmove = revert(board, lastmove);
					screen_refresh(board, renderer, window, player, lastmove);
					x1 = -1;
					y1 = -1;
				}
			}
			break;
		}
		}
	}
	return 0;
}