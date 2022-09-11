#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string>
#include <cmath>
#include "Mgame.h"
#include "Mgraphics.h"

//Initial screen dimension constants
const int SCREEN_WIDTH = 1100;
const int SCREEN_HEIGHT = 600;

//Function prototypes
static bool init(); //Starts up SDL and creates window
static bool loadMedia(SDL_Renderer *renderer); //Loads media
static bool loadMenuMedia(SDL_Renderer *renderer);
static void close(); //Frees media and shuts down SDL
static Tiles givetoTile(int no); //Takes number -1, 0-8 and returns enum for tile
static void displayDigit(int no, SDL_Rect *dest, SDL_Renderer *renderer); //Draws a number no to dest of renderer
static int getDigit(double input, int digit); //Returns digit of input from right

//Global variables
SDL_Rect gTiles[15];
SDL_Rect gDestination;
SDL_Rect gInterface[5];
SDL_Rect gMenu[4];

//Scene global variables
LTexture gtileSheetTexture;
LTexture gInterfaceTextures;
LTexture gMenuTexture;

int main(int argc, char *args[])
{
	//Grid size variables & game
	int grid_x = 20, grid_y = 10, mine_n = 25, tile_size = 50, size_UI = 50, tile_size_x = 50, tile_size_y = 50, x_offset = 50, y_offset = 50;
	//most recent tile
	int xTile = -1, yTile = -1;
	bool clickReverse = false;
	Mgame newGame;
	newGame.init(grid_x, grid_y, mine_n);
	LWindow mainWindow, menuWindow;
	LTimer gameTimer;

	if (!init()) //Setup SDL
	{
		printf("Failed to initialise!\n");
	}
	//Create main window
	else if (mainWindow.init("Minesweeper", SCREEN_WIDTH, SCREEN_HEIGHT, true, true) && menuWindow.init("Menu", SCREEN_HEIGHT /2, SCREEN_HEIGHT/2, false, false, true, true))
	{
		if (!loadMedia(mainWindow.getRenderer())) //Load media
		{
			printf("Failed to load media!\n");
		}
		else if (!loadMenuMedia(menuWindow.getRenderer())) //Load menu media
		{
			printf("Failed to load media!\n");
		}
		else
		{
			bool quit = false; //Main loop flag
			SDL_Event e; //Event handler
			int uX, uY, mX, mY; //Mouse position variables
			MButton mainMEvent, menuMEvent; //Mouse event 
			int resetCounter = 0, cRevCounter = 0, menuCounter = 0; //Counter for press of button
			int xUCounter = 0, xDCounter = 0, yUCounter = 0, yDCounter = 0, mUCounter = 0, mDCounter = 0; //Counter for menu button presses
			SDL_Rect resetButton, cRevButton, menuButton; //Reset button position, click reverse button position
			bool firstGo = true; //True until first tile has been selected
			SDL_Rect timerPos, mineCount, menuRender; //Some areas for defined objects

			while (!quit)
			{
				while (SDL_PollEvent(&e) != 0) //Handle queue
				{
					//If user requests to quit
					if (e.type == SDL_QUIT)
					{
						quit = true;
					}
					//Handle window events
					mainWindow.handleEvent(e);
					menuWindow.handleEvent(e);
					//Handle mouse events for main window
					if (e.window.windowID == mainWindow.getWindowID() && !menuWindow.isShown())
					{
						mainMEvent.handleEvent(&e);
					}

					if (e.window.windowID == menuWindow.getWindowID() && menuWindow.isShown())
					{
						menuMEvent.handleEvent(&e);
					}
				}
				//Render main window & main window functionality
				if (!mainWindow.isMinimised() && mainWindow.isShown())
				{
					//Set minimum window size
					mainWindow.setMinimum(440, 290);
					
					//Calculate tile sizes & offsets
					tile_size_x = (mainWindow.getWidth() / (grid_x + 2));
					tile_size_y = ((mainWindow.getHeight() - size_UI) / (grid_y + 2));

					//If tiles would be wider in y than x
					if (tile_size_x <= tile_size_y)
					{
						tile_size = tile_size_x;
						x_offset = (mainWindow.getWidth() - (tile_size * grid_x)) / 2;
						y_offset = (mainWindow.getHeight() - size_UI - (tile_size * grid_y)) / 2;
					}

					else if (tile_size_x > tile_size_y) //If tiles would be greater in x than y
					{
						tile_size = tile_size_y;
						y_offset = (mainWindow.getHeight() - size_UI - (tile_size * grid_y)) / 2;
						x_offset = (mainWindow.getWidth() - (tile_size * grid_x)) / 2;
					}

					//Give space for UI
					y_offset += size_UI;

					//Clear screen
					SDL_SetRenderDrawColor(mainWindow.getRenderer(), 180, 180, 180, 0xFF);
					SDL_RenderClear(mainWindow.getRenderer());

					//Position reset button
					resetButton.w = size_UI * 4 / 5;
					resetButton.h = size_UI * 4 / 5;
					resetButton.x = (int)(x_offset + (grid_x / 2.0) * tile_size - resetButton.w / 2.0);
					resetButton.y = (y_offset - resetButton.h) / 2;

					//Position click reverse button
					cRevButton.w = size_UI * 4 / 5;
					cRevButton.h = size_UI * 4 / 5;
					cRevButton.y = (y_offset - cRevButton.h) / 2;
					cRevButton.x = x_offset + grid_x * tile_size - cRevButton.w;

					//Position menu button
					menuButton.w = size_UI * 4 / 5;
					menuButton.h = size_UI * 4 / 5;
					menuButton.y = (y_offset - menuButton.h) / 2;
					menuButton.x = x_offset;

					//Gain mouse click data
					MButtonType uType = mainMEvent.getMState(&uX, &uY);

					//Check for gameover and win
					bool gameWon = newGame.testWin();
					bool gOver = newGame.testOver();
					int mRem = newGame.getRemMine();
					if (gOver == false)
						{
							//Convert button type to type passable to game function
							int gameType = 0;
							if (!clickReverse)
							{
								switch (uType)
								{
								case(BUTTON_LEFT_DOWN):
									gameType = 1;
									break;
								case(BUTTON_RIGHT_DOWN):
									gameType = 2;
									break;
								case(BLANK):
									gameType = 0;
									break;
								default:
									break;
								}
							}
							else if (clickReverse)
							{
								switch (uType)
								{
								case(BUTTON_LEFT_DOWN):
									gameType = 2;
									break;
								case(BUTTON_RIGHT_DOWN):
									gameType = 1;
									break;
								case(BLANK):
									gameType = 0;
									break;
								default:
									break;
								}
							}
							//Equate mouse position to tile number
							xTile = -1;
							yTile = -1;
							if (uX >= x_offset && uY >= y_offset)
							{
								xTile = (uX - x_offset) / tile_size;
								yTile = (uY - y_offset) / tile_size;
							}
							if (xTile >= 0 && xTile < grid_x && yTile >= 0 && yTile < grid_y)
							{
								//Have go of game
								newGame.gameTurn(xTile, yTile, gameType);
								//Start timer on first tile selection
								if (firstGo)
								{
									gameTimer.start();
									firstGo = false;
								}
							}
						}

					//Check for press of reset button
					bool resetPressed = false;
					if (uX >= resetButton.x && uX <= resetButton.x + resetButton.w && uY >= resetButton.y && uY <= resetButton.y + resetButton.h && resetCounter == 0)
					{
						resetPressed = true;
						resetCounter = 10;
						newGame.free();
						newGame.init(grid_x, grid_y, mine_n);
						gameTimer.stop();
						firstGo = true;
						clickReverse = false;
						cRevCounter = 0;
					}

					//Check for press of click reverse button
					bool cRevPressed = false;
					if (uX >= cRevButton.x && uX <= cRevButton.x + cRevButton.w && uY >= cRevButton.y && uY <= cRevButton.y + cRevButton.h && cRevCounter == 0)
					{
						cRevPressed = true;
						cRevCounter = 10;
						if (clickReverse) clickReverse = false;
						else if (!clickReverse) clickReverse = true;
					}

					//Check for press of menu button
					bool menuPressed = false;
					if (uX >= menuButton.x && uX <= menuButton.x + menuButton.w && uY >= menuButton.y && uY <= menuButton.y + menuButton.h && menuCounter == 0)
					{
						menuPressed = true;
						menuCounter = 10;
					}

					//Open menu
					if (menuCounter == 1)
					{

						//Open mneu
						menuWindow.focus();
					}

					//Pause timer when menu open
					if (!gameTimer.isPaused() && menuWindow.isShown())
					{
						gameTimer.pause();
					}

					//Start timer when menu is closed
					if (gameTimer.isPaused() && !menuWindow.isShown())
					{
						gameTimer.unpause();
					}

					//Draw grid of tiles
					for (int ii = 0; ii < grid_x; ii++)
						{
							for (int jj = 0; jj < grid_y; jj++)
							{
								//Set tile position
								gDestination.x = (ii * tile_size + x_offset);
								gDestination.y = (jj * tile_size + y_offset);
								gDestination.w = tile_size;
								gDestination.h = tile_size;
								//enum to store tile type
								Tiles t_type;
								//Find tile type
								int t_reveal = newGame.getRevealed(ii, jj);
								if (gOver == false) //Only whilst game is being played
								{
									switch (t_reveal)
									{
									case(0):
										t_type = TILE_BLANK; //An unrevealed tile
										break;
									case(1):
										t_type = givetoTile(newGame.getTile(ii, jj)); //A revealed tile
										break;
									case(2):
										t_type = TILE_FLAG; //A flagged tile
										break;
									default:
										t_type = TILE_ERROR; //Unexpected number handled with error tile
										break;
									}
								}
								if (gOver == true) //Render full grid showing missed mines and incorrect mines
								{
									t_type = givetoTile(newGame.getTile(ii, jj));
									if (t_reveal == 2 && t_type != TILE_MINE)
									{
										t_type = TILE_NOMINE;
									}
									//Highlight the mine clicked
									if (xTile == ii && yTile == jj)
									{
										t_type = TILE_BADMINE;
									}
								}

								//Render the tile to mainWindow.getRenderer()
								gtileSheetTexture.render(mainWindow.getRenderer(), &gDestination, &gTiles[t_type]);

								//Render line at edge for vertical tiles
								if ((t_type != TILE_BLANK) && (t_type != TILE_FLAG) && (ii == grid_x - 1))
								{
									//Set line at edge of tile
									gDestination.x = ((ii + 1) * tile_size + x_offset - tile_size / 20);
									gDestination.y = (jj * tile_size + y_offset);
									gDestination.w = tile_size / 20;
									gDestination.h = tile_size;
									//Set colour
									SDL_SetRenderDrawColor(mainWindow.getRenderer(), 110, 110, 110, 0xFF);
									//Rednder in rectangle
									SDL_RenderFillRect(mainWindow.getRenderer(), &gDestination);
								}
								//Render line at bottom of horisontal tiles
								if ((t_type != TILE_BLANK) && (t_type != TILE_FLAG) && (jj == grid_y - 1))
								{
									//Set line at edge of tile
									gDestination.x = (ii * tile_size + x_offset);
									gDestination.y = ((jj + 1) * tile_size + y_offset - tile_size / 20);
									gDestination.w = tile_size;
									gDestination.h = tile_size / 20;
									//Set colour
									SDL_SetRenderDrawColor(mainWindow.getRenderer(), 110, 110, 110, 0xFF);
									//Rednder in rectangle
									SDL_RenderFillRect(mainWindow.getRenderer(), &gDestination);
								}
							}
						}

					//Draw reset button
					if (resetPressed || resetCounter >= 1)
					{
						//Render face
						gInterfaceTextures.render(mainWindow.getRenderer(), &resetButton, &gInterface[0]);
						resetCounter--;
						//Grey out face
						SDL_SetRenderDrawColor(mainWindow.getRenderer(), 150, 150, 150, 180);
						SDL_RenderFillRect(mainWindow.getRenderer(), &resetButton);
					}

					else if (gOver == true && gameWon == false)
					{
						//Render sad face
						gInterfaceTextures.render(mainWindow.getRenderer(), &resetButton, &gInterface[1]);
					}

					else
					{
						gInterfaceTextures.render(mainWindow.getRenderer(), &resetButton, &gInterface[0]);
					}

					//Draw click reverse button
					if (cRevPressed || cRevCounter >= 1)
					{
						//Render button
						gInterfaceTextures.render(mainWindow.getRenderer(), &cRevButton, &gInterface[2]);
						cRevCounter--;
						SDL_SetRenderDrawColor(mainWindow.getRenderer(), 150, 150, 150, 180);
						SDL_RenderFillRect(mainWindow.getRenderer(), &cRevButton);
					}
					else if(!clickReverse)
					{
						gInterfaceTextures.render(mainWindow.getRenderer(), &cRevButton, &gInterface[3]);
					}
					else
					{
						gInterfaceTextures.render(mainWindow.getRenderer(), &cRevButton, &gInterface[2]);
					}


					//Draw menu button
					if (menuPressed || menuCounter >= 1)
					{
						//Render button
						gInterfaceTextures.render(mainWindow.getRenderer(), &menuButton, &gInterface[4]);
						menuCounter--;
						SDL_SetRenderDrawColor(mainWindow.getRenderer(), 150, 150, 150, 180);
						SDL_RenderFillRect(mainWindow.getRenderer(), &menuButton);
					}
					else
					{
						gInterfaceTextures.render(mainWindow.getRenderer(), &menuButton, &gInterface[4]);
					}

					//Check gameover
					if (gOver)
					{
						gameTimer.pause();
					}

					//Display timer
					float time = (float)gameTimer.getTicks();
					int ti_ti = (int)(time / 1000.f);
					int ti_s = ti_ti % 60;
					int ti_m = ti_ti / 60;
					timerPos.h = size_UI * 4 / 5;
					timerPos.w = timerPos.h * 2 / 3;
					timerPos.x = (grid_x * tile_size) * 3 / 4 + x_offset - (2 * timerPos.w + timerPos.w / 6);
					timerPos.y = (y_offset - timerPos.h) / 2;
					
					int posw = timerPos.w;
					//Display Minutes
					displayDigit(getDigit(ti_m, 1), &timerPos, mainWindow.getRenderer());
					timerPos.x += timerPos.w;
					displayDigit(getDigit(ti_m, 0), &timerPos, mainWindow.getRenderer());
					timerPos.x += timerPos.w;
					//Display divider
					timerPos.w = timerPos.w / 3 + 1;
					displayDigit(-20, &timerPos, mainWindow.getRenderer());
					timerPos.x += timerPos.w;
					timerPos.w = posw;
					//Display seconds
					displayDigit(getDigit(ti_s, 1), &timerPos, mainWindow.getRenderer());
					timerPos.x += timerPos.w;
					displayDigit(getDigit(ti_s, 0), &timerPos, mainWindow.getRenderer());

					//Display mine counter
					int mine_count = newGame.getRemMine();
					if (mine_count < 0) mine_count = 0;
					mineCount.h = size_UI * 4 / 5;
					mineCount.w = mineCount.h * 2 / 3;
					mineCount.x = (grid_x * tile_size) * 1 / 4 + x_offset - mineCount.w / 2;
					mineCount.y = (y_offset - resetButton.h) / 2;
					//Display first two digits
					displayDigit(getDigit(mine_count, 1), &mineCount, mainWindow.getRenderer());
					mineCount.x += mineCount.w;
					displayDigit(getDigit(mine_count, 0), &mineCount, mainWindow.getRenderer());
					//Display third digit
					mineCount.x -= mineCount.w * 2;
					displayDigit(getDigit(mine_count, 2), &mineCount, mainWindow.getRenderer());

					//Reset mouse event data
					mainMEvent.reset();

					//Render to screen
					SDL_RenderPresent(mainWindow.getRenderer());
				}

				//Render menu window & menu window functionality
				if (!menuWindow.isMinimised() && menuWindow.isShown())
				{
					SDL_SetRenderDrawColor(menuWindow.getRenderer(), 180, 180, 180, 0xFF);
					SDL_RenderClear(menuWindow.getRenderer());

					//Handle mouse input
					MButtonType mType = menuMEvent.getMState(&mX, &mY);

					int mWidth = menuWindow.getHeight();
					int mHeight = menuWindow.getWidth();
					menuRender.h = (int)(mHeight / 5);
					menuRender.w = (int)(mWidth / 5);
					mWidth = menuRender.h;
					mHeight = menuRender.h;
					if (menuRender.w < menuRender.h) menuRender.h = menuRender.w;
					else if (menuRender.w > menuRender.h) menuRender.w = menuRender.h;
					
					//X + Y size & adjustment
					//X
					int offset = (menuRender.w / 2);
					menuRender.x = offset;
					menuRender.y = offset;
					gMenuTexture.render(menuWindow.getRenderer(), &menuRender, &gMenu[0]);
					//Numbers
					menuRender.x += menuRender.w;
					menuRender.x += (int)(menuRender.w / 5);
					menuRender.w = (int)(menuRender.w * 2 / 3);
					displayDigit(getDigit(grid_x, 2), &menuRender, menuWindow.getRenderer());
					menuRender.x += menuRender.w;
					displayDigit(getDigit(grid_x, 1), &menuRender, menuWindow.getRenderer());
					menuRender.x += menuRender.w;
					displayDigit(getDigit(grid_x, 0), &menuRender, menuWindow.getRenderer());
					//Arrows
					menuRender.x += menuRender.w;
					menuRender.x += (int)(menuRender.w / 5);
					menuRender.w = menuRender.h;
					gMenuTexture.render(menuWindow.getRenderer(), &menuRender, &gMenu[2]);
					//Check button press
					bool xUPPress = false;
					if (mX >= menuRender.x && mX <= menuRender.x + menuRender.w && mY >= menuRender.y && mY <= menuRender.y + (menuRender.h /2) && xUCounter == 0)
					{
						xUPPress = true;
						xUCounter = 10;
						grid_x++;
						newGame.free();
						newGame.init(grid_x, grid_y, mine_n);
					}
					if (xUCounter > 0) //Grey out button
					{
						int heightTmp = menuRender.h;
						menuRender.h = (menuRender.h / 2);
						//Set colour
						SDL_SetRenderDrawColor(menuWindow.getRenderer(), 150, 150, 150, 180);
						//Rednder in rectangle
						SDL_RenderFillRect(menuWindow.getRenderer(), &menuRender);
						menuRender.h = heightTmp;
						xUCounter--;
					}
					//Check x_down press
					bool xDPress = false;
					if (mX >= menuRender.x && mX <= menuRender.x + menuRender.w && mY >= menuRender.y + (menuRender.h / 2) && mY <= menuRender.y + (menuRender.h) && xDCounter == 0)
					{
						xDPress = true;
						xDCounter = 10;
						if(grid_x > 5) grid_x--;
						newGame.free();
						newGame.init(grid_x, grid_y, mine_n);
					}
					if (xDCounter > 0) //Grey out button
					{
						int heightTmp = menuRender.h;
						menuRender.h = (menuRender.h / 2);
						menuRender.y += menuRender.h;
						//Set colour
						SDL_SetRenderDrawColor(menuWindow.getRenderer(), 150, 150, 150, 180);
						//Rednder in rectangle
						SDL_RenderFillRect(menuWindow.getRenderer(), &menuRender);
						menuRender.y -= menuRender.h;
						menuRender.h = heightTmp;
						xDCounter--;
					}

					//Y
					menuRender.y = (offset *4);
					menuRender.x = offset;
					gMenuTexture.render(menuWindow.getRenderer(), &menuRender, &gMenu[1]);
					//Numbers
					menuRender.x += menuRender.w;
					menuRender.x += (int)(menuRender.w / 5);
					menuRender.w = (int)(menuRender.w * 2 / 3);
					displayDigit(getDigit(grid_y, 2), &menuRender, menuWindow.getRenderer());
					menuRender.x += menuRender.w;
					displayDigit(getDigit(grid_y, 1), &menuRender, menuWindow.getRenderer());
					menuRender.x += menuRender.w;
					displayDigit(getDigit(grid_y, 0), &menuRender, menuWindow.getRenderer());
					//Arrows
					menuRender.x += menuRender.w;
					menuRender.x += (int)(menuRender.w / 5);
					menuRender.w = menuRender.h;
					gMenuTexture.render(menuWindow.getRenderer(), &menuRender, &gMenu[2]);
					//Check button press
					bool yUPPress = false;
					if (mX >= menuRender.x && mX <= menuRender.x + menuRender.w && mY >= menuRender.y && mY <= menuRender.y + (menuRender.h / 2) && yUCounter == 0)
					{
						yUPPress = true;
						yUCounter = 10;
						grid_y++;
						newGame.free();
						newGame.init(grid_x, grid_y, mine_n);
					}
					if (yUCounter > 0) //Grey out button
					{
						int heightTmp = menuRender.h;
						menuRender.h = (menuRender.h / 2);
						//Set colour
						SDL_SetRenderDrawColor(menuWindow.getRenderer(), 150, 150, 150, 180);
						//Rednder in rectangle
						SDL_RenderFillRect(menuWindow.getRenderer(), &menuRender);
						menuRender.h = heightTmp;
						yUCounter--;
					}
					//Check y_down press
					bool yDPress = false;
					if (mX >= menuRender.x && mX <= menuRender.x + menuRender.w && mY >= menuRender.y + (menuRender.h / 2) && mY <= menuRender.y + (menuRender.h) && yDCounter == 0)
					{
						yDPress = true;
						yDCounter = 10;
						if (grid_y > 5) grid_y--;
						newGame.free();
						newGame.init(grid_x, grid_y, mine_n);
					}
					if (yDCounter > 0) //Grey out button
					{
						int heightTmp = menuRender.h;
						menuRender.h = (menuRender.h / 2);
						menuRender.y += menuRender.h;
						//Set colour
						SDL_SetRenderDrawColor(menuWindow.getRenderer(), 150, 150, 150, 180);
						//Rednder in rectangle
						SDL_RenderFillRect(menuWindow.getRenderer(), &menuRender);
						menuRender.y -= menuRender.h;
						menuRender.h = heightTmp;
						yDCounter--;
					}

					//Mines
					menuRender.y = (offset * 7);
					menuRender.x = offset;
					gMenuTexture.render(menuWindow.getRenderer(), &menuRender, &gMenu[3]);
					//Numbers
					menuRender.x += menuRender.w;
					menuRender.x += (int)(menuRender.w / 5);
					menuRender.w = (int)(menuRender.w * 2 / 3);
					displayDigit(getDigit(mine_n, 2), &menuRender, menuWindow.getRenderer());
					menuRender.x += menuRender.w;
					displayDigit(getDigit(mine_n, 1), &menuRender, menuWindow.getRenderer());
					menuRender.x += menuRender.w;
					displayDigit(getDigit(mine_n, 0), &menuRender, menuWindow.getRenderer());
					//Arrows
					menuRender.x += menuRender.w;
					menuRender.x += (int)(menuRender.w / 5);
					menuRender.w = menuRender.h;
					gMenuTexture.render(menuWindow.getRenderer(), &menuRender, &gMenu[2]);
					//Check button press
					bool mUPPress = false;
					if (mX >= menuRender.x && mX <= menuRender.x + menuRender.w && mY >= menuRender.y && mY <= menuRender.y + (menuRender.h / 2) && mUCounter == 0)
					{
						mUPPress = true;
						mUCounter = 10;
						mine_n++;
						newGame.free();
						newGame.init(grid_x, grid_y, mine_n);
					}
					if (mUCounter > 0) //Grey out button
					{
						int heightTmp = menuRender.h;
						menuRender.h = (menuRender.h / 2);
						//Set colour
						SDL_SetRenderDrawColor(menuWindow.getRenderer(), 150, 150, 150, 180);
						//Rednder in rectangle
						SDL_RenderFillRect(menuWindow.getRenderer(), &menuRender);
						menuRender.h = heightTmp;
						mUCounter--;
					}
					//Check y_down press
					bool mDPress = false;
					if (mX >= menuRender.x && mX <= menuRender.x + menuRender.w && mY >= menuRender.y + (menuRender.h / 2) && mY <= menuRender.y + (menuRender.h) && mDCounter == 0)
					{
						mDPress = true;
						mDCounter = 10;
						if (mine_n > 5) mine_n--;
						newGame.free();
						newGame.init(grid_x, grid_y, mine_n);
					}
					if (mDCounter > 0) //Grey out button
					{
						int heightTmp = menuRender.h;
						menuRender.h = (menuRender.h / 2);
						menuRender.y += menuRender.h;
						//Set colour
						SDL_SetRenderDrawColor(menuWindow.getRenderer(), 150, 150, 150, 180);
						//Rednder in rectangle
						SDL_RenderFillRect(menuWindow.getRenderer(), &menuRender);
						menuRender.y -= menuRender.h;
						menuRender.h = heightTmp;
						mDCounter--;
					}

					//Reset mouse event
					menuMEvent.reset();

					SDL_RenderPresent(menuWindow.getRenderer());
				}

				//Quit when all windows are closed
				if (!mainWindow.isShown() && !menuWindow.isShown())
				{
					quit = true;
				}
			}
		}
	}
	//Free resources and close SDL
	newGame.free();
	mainWindow.free();
	menuWindow.free();
	close();
	return 0;
}

bool init()
{
	bool success = true; //Init flag
	
	if (SDL_Init(SDL_INIT_VIDEO) < 0) //Init SDL
	{
		printf("SDL could not initialise! SDL Error: %s\n", SDL_GetError());
		return false;
	}

	if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")) //Set texture filtering to linear
	{
		printf("Warning: Linear texture filtering not enabled!");
	}

	//Initialise PNG loading
	int imgFlags = IMG_INIT_PNG;
	if (!(IMG_Init(imgFlags) & imgFlags))
	{
		printf("SDL_image could not initialise! SDL_image Error: %s\n", IMG_GetError());
		return false;
	}
	return success;
}

//Loads main window media for *renderer
bool loadMedia(SDL_Renderer *renderer)
{
	bool success = true;
	//Load tile sheet texture
	if (!gtileSheetTexture.loadFromFile("TILES.png", renderer))
	{
		printf("Failed to load sprite sheet texture!\n");
		success = false;
	}
	//Set all sprites in TILES.png
	else
	{
		for (int ii = 0; ii < 15; ii++)
		{
			gTiles[ii].w = 100;
			gTiles[ii].h = 100;
		}

		gTiles[TILE_0].x = 600;
		gTiles[TILE_0].y = 0;

		gTiles[TILE_1].x = 100;
		gTiles[TILE_1].y = 0;

		gTiles[TILE_2].x = 200;
		gTiles[TILE_2].y = 0;
		
		gTiles[TILE_3].x = 300;
		gTiles[TILE_3].y = 0;

		gTiles[TILE_4].x = 400;
		gTiles[TILE_4].y = 0;

		gTiles[TILE_5].x = 100;
		gTiles[TILE_5].y = 100;

		gTiles[TILE_6].x = 200;
		gTiles[TILE_6].y = 100;

		gTiles[TILE_7].x = 300;
		gTiles[TILE_7].y = 100;

		gTiles[TILE_8].x = 400;
		gTiles[TILE_8].y = 100;

		gTiles[TILE_BLANK].x = 0;
		gTiles[TILE_BLANK].y = 100;

		gTiles[TILE_FLAG].x = 0;
		gTiles[TILE_FLAG].y = 0;

		gTiles[TILE_NOMINE].x = 500;
		gTiles[TILE_NOMINE].y = 0;

		gTiles[TILE_MINE].x = 500;
		gTiles[TILE_MINE].y = 100;

		gTiles[TILE_BADMINE].x = 600;
		gTiles[TILE_BADMINE].y = 100;

		gTiles[TILE_ERROR].x = 50;
		gTiles[TILE_ERROR].y = 50;
	}

	//Load face sprite sheet
	if (!gInterfaceTextures.loadFromFile("UI.png", renderer))
	{
		printf("Failed to load sprite sheet texture! \n");
		success = false;
	}
	
	//Set sprites in UI.png
	else
	{
		for (int ii = 0; ii < 5; ii++)
		{
			gInterface[ii].h = 100;
			gInterface[ii].w = 100;
		}
		//Happy Face
		gInterface[0].x = 0;
		gInterface[0].y = 0;
		//Sad face
		gInterface[1].x = 100;
		gInterface[1].y = 0;
		//Flag select
		gInterface[2].x = 0;
		gInterface[2].y = 100;
		//Remove select
		gInterface[3].x = 100;
		gInterface[3].y = 100;
		//Menu
		gInterface[4].x = 200;
		gInterface[4].y = 0;

	}
	return success;
}

//Loads media for the menu for *renderer
bool loadMenuMedia(SDL_Renderer *renderer)
{
	bool success = true;
	//Load menu texture here
	if (!gMenuTexture.loadFromFile("M_UI.png", renderer))
	{
		printf("Failed to load sprite sheet texture! \n");
		success = false;
	}
	
	//Set sprites for M_UI.png
	else
	{
		gMenu[0].x = 0;
		gMenu[0].y = 0;
		gMenu[0].h = 100;
		gMenu[0].w = 100;

		gMenu[1].x = 100;
		gMenu[1].y = 0;
		gMenu[1].h = 100;
		gMenu[1].w = 100;

		gMenu[2].x = 0;
		gMenu[2].y = 100;
		gMenu[2].h = 100;
		gMenu[2].w = 100;

		gMenu[3].x = 100;
		gMenu[3].y = 100;
		gMenu[3].h = 100;
		gMenu[3].w = 100;

	}

	return success;
}

//Frees memory
void close()
{
	//Free textures
	gtileSheetTexture.free();
	gInterfaceTextures.free();

	//Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();
}

//Takes a number and returns coresponding enum
Tiles givetoTile(int no)
{
	switch (no)
	{
	case(0):
		return TILE_0;
		break;
	case(1):
		return TILE_1;
		break;
	case(2):
		return TILE_2;
		break;
	case(3):
		return TILE_3;
		break;
	case(4):
		return TILE_4;
		break;
	case(5):
		return TILE_5;
		break;
	case(6):
		return TILE_6;
		break;
	case(7):
		return TILE_7;
		break;
	case(8):
		return TILE_8;
		break;
	case(-1):
		return TILE_MINE;
		break;
	default:
		return TILE_ERROR;
		break;
	}
}

//Displays digit no inside *dest of *renderer
void displayDigit(int no, SDL_Rect *dest, SDL_Renderer *renderer)
{
	if ((no >= 10 || no < 0) && no != -20)
	{
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderFillRect(renderer, dest);
	}
	else if (no == -20) //For :
	{
		//Fill black
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderFillRect(renderer, dest);
		//Position variable
		SDL_Rect position;
		position.w = (int)(dest->w * 0.5);
		position.h = (int)(0.1 * dest->h);
		position.x = (int)(dest->x + dest->w * 0.25);
		position.y = (int)(dest->y + dest->h * 0.25);
		//Render top dot
		SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
		SDL_RenderFillRect(renderer, &position);
		//Render lower dot
		position.y = (int)(dest->y + dest->h * 0.65);
		SDL_RenderFillRect(renderer, &position);
	}
	else
	{
		//Local variables
		SDL_Rect position[2];

		//Set to size of destination
		position[0].w = (int)(dest->w * 0.7);
		position[0].h = (int)(dest->h * 0.05);
		position[1].w = (int)(dest->w * 0.1);
		position[1].h = (int)(dest->h * 0.375);

		position[0].x = dest->x + (int)(dest->w * 0.15);
		position[0].y = dest->y + (int)(dest->h * 0.05);
		position[1].x = dest->x + (int)(dest->w * 0.05);
		position[1].y = dest->y + (int)(dest->h * 0.1);
		//w & h increment
		int hIncr = (int)(0.4 * (dest->h * 0.9));
		int wIncr = (int)(0.4 * (dest->w * 0.9));
		//Render black background
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderFillRect(renderer, dest);

		//Render numbers

		switch (no)
		{
		case(0):
			SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
			SDL_RenderFillRect(renderer, &position[0]); //Top
			SDL_RenderFillRect(renderer, &position[1]); //Top left
			position[0].y += (position[1].h + position[0].h);
			SDL_SetRenderDrawColor(renderer, 50, 40, 40, 255);
			SDL_RenderFillRect(renderer, &position[0]); //Middle
			position[0].y += (position[1].h + position[0].h);
			SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
			SDL_RenderFillRect(renderer, &position[0]); //Bottom
			position[1].y += (position[1].h + position[0].h);
			SDL_RenderFillRect(renderer, &position[1]); //Bottom left
			position[1].y -= (position[1].h + position[0].h);
			position[1].x += (position[0].w + position[1].w);
			SDL_RenderFillRect(renderer, &position[1]); //Top right
			position[1].y += (position[1].h + position[0].h);
			SDL_RenderFillRect(renderer, &position[1]); //Bottom Right
			break;
		case(1):
			SDL_SetRenderDrawColor(renderer, 50, 40, 40, 255);
			SDL_RenderFillRect(renderer, &position[0]); //Top
			SDL_RenderFillRect(renderer, &position[1]); //Top left
			position[0].y += (position[1].h + position[0].h);
			SDL_RenderFillRect(renderer, &position[0]); //Middle
			position[0].y += (position[1].h + position[0].h);
			SDL_RenderFillRect(renderer, &position[0]); //Bottom
			position[1].y += (position[1].h + position[0].h);
			SDL_RenderFillRect(renderer, &position[1]); //Bottom left
			position[1].y -= (position[1].h + position[0].h);
			position[1].x += (position[0].w + position[1].w);
			SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
			SDL_RenderFillRect(renderer, &position[1]); //Top right
			position[1].y += (position[1].h + position[0].h);
			SDL_RenderFillRect(renderer, &position[1]); //Bottom Right
			break;
		case(2):
			SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
			SDL_RenderFillRect(renderer, &position[0]); //Top
			SDL_SetRenderDrawColor(renderer, 50, 40, 40, 255);
			SDL_RenderFillRect(renderer, &position[1]); //Top left
			position[0].y += (position[1].h + position[0].h);
			SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
			SDL_RenderFillRect(renderer, &position[0]); //Middle
			position[0].y += (position[1].h + position[0].h);
			SDL_RenderFillRect(renderer, &position[0]); //Bottom
			position[1].y += (position[1].h + position[0].h);
			SDL_RenderFillRect(renderer, &position[1]); //Bottom left
			position[1].y -= (position[1].h + position[0].h);
			position[1].x += (position[0].w + position[1].w);
			SDL_RenderFillRect(renderer, &position[1]); //Top right
			position[1].y += (position[1].h + position[0].h);
			SDL_SetRenderDrawColor(renderer, 50, 40, 40, 255);
			SDL_RenderFillRect(renderer, &position[1]); //Bottom Right
			break;
		case(3):
			SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
			SDL_RenderFillRect(renderer, &position[0]); //Top
			SDL_SetRenderDrawColor(renderer, 50, 40, 40, 255);
			SDL_RenderFillRect(renderer, &position[1]); //Top left
			position[0].y += (position[1].h + position[0].h);
			SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
			SDL_RenderFillRect(renderer, &position[0]); //Middle
			position[0].y += (position[1].h + position[0].h);
			SDL_RenderFillRect(renderer, &position[0]); //Bottom
			position[1].y += (position[1].h + position[0].h);
			SDL_SetRenderDrawColor(renderer, 50, 40, 40, 255);
			SDL_RenderFillRect(renderer, &position[1]); //Bottom left
			position[1].y -= (position[1].h + position[0].h);
			position[1].x += (position[0].w + position[1].w);
			SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
			SDL_RenderFillRect(renderer, &position[1]); //Top right
			position[1].y += (position[1].h + position[0].h);
			SDL_RenderFillRect(renderer, &position[1]); //Bottom Right
			break;
		case(4):
			SDL_SetRenderDrawColor(renderer, 50, 40, 40, 255);
			SDL_RenderFillRect(renderer, &position[0]); //Top
			SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
			SDL_RenderFillRect(renderer, &position[1]); //Top left
			position[0].y += (position[1].h + position[0].h);
			SDL_RenderFillRect(renderer, &position[0]); //Middle
			position[0].y += (position[1].h + position[0].h);
			SDL_SetRenderDrawColor(renderer, 50, 40, 40, 255);
			SDL_RenderFillRect(renderer, &position[0]); //Bottom
			position[1].y += (position[1].h + position[0].h);
			SDL_RenderFillRect(renderer, &position[1]); //Bottom left
			position[1].y -= (position[1].h + position[0].h);
			position[1].x += (position[0].w + position[1].w);
			SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
			SDL_RenderFillRect(renderer, &position[1]); //Top right
			position[1].y += (position[1].h + position[0].h);
			SDL_RenderFillRect(renderer, &position[1]); //Bottom Right
			break;
		case(5):
			SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
			SDL_RenderFillRect(renderer, &position[0]); //Top
			SDL_RenderFillRect(renderer, &position[1]); //Top left
			position[0].y += (position[1].h + position[0].h);
			SDL_RenderFillRect(renderer, &position[0]); //Middle
			position[0].y += (position[1].h + position[0].h);
			SDL_RenderFillRect(renderer, &position[0]); //Bottom
			position[1].y += (position[1].h + position[0].h);
			SDL_SetRenderDrawColor(renderer, 50, 40, 40, 255);
			SDL_RenderFillRect(renderer, &position[1]); //Bottom left
			position[1].y -= (position[1].h + position[0].h);
			position[1].x += (position[0].w + position[1].w);
			SDL_RenderFillRect(renderer, &position[1]); //Top right
			position[1].y += (position[1].h + position[0].h);
			SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
			SDL_RenderFillRect(renderer, &position[1]); //Bottom Right
			break;
		case(6):
			SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
			SDL_RenderFillRect(renderer, &position[0]); //Top
			SDL_RenderFillRect(renderer, &position[1]); //Top left
			position[0].y += (position[1].h + position[0].h);
			SDL_RenderFillRect(renderer, &position[0]); //Middle
			position[0].y += (position[1].h + position[0].h);
			SDL_RenderFillRect(renderer, &position[0]); //Bottom
			position[1].y += (position[1].h + position[0].h);
			SDL_RenderFillRect(renderer, &position[1]); //Bottom left
			position[1].y -= (position[1].h + position[0].h);
			position[1].x += (position[0].w + position[1].w);
			SDL_SetRenderDrawColor(renderer, 50, 40, 40, 255);
			SDL_RenderFillRect(renderer, &position[1]); //Top right
			position[1].y += (position[1].h + position[0].h);
			SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
			SDL_RenderFillRect(renderer, &position[1]); //Bottom Right
			break;
		case(7):
			SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
			SDL_RenderFillRect(renderer, &position[0]); //Top
			SDL_SetRenderDrawColor(renderer, 50, 40, 40, 255);
			SDL_RenderFillRect(renderer, &position[1]); //Top left
			position[0].y += (position[1].h + position[0].h);
			SDL_RenderFillRect(renderer, &position[0]); //Middle
			position[0].y += (position[1].h + position[0].h);
			SDL_RenderFillRect(renderer, &position[0]); //Bottom
			position[1].y += (position[1].h + position[0].h);
			SDL_RenderFillRect(renderer, &position[1]); //Bottom left
			position[1].y -= (position[1].h + position[0].h);
			position[1].x += (position[0].w + position[1].w);
			SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
			SDL_RenderFillRect(renderer, &position[1]); //Top right
			position[1].y += (position[1].h + position[0].h);
			SDL_RenderFillRect(renderer, &position[1]); //Bottom Right
			break;
		case(8):
			SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
			SDL_RenderFillRect(renderer, &position[0]); //Top
			SDL_RenderFillRect(renderer, &position[1]); //Top left
			position[0].y += (position[1].h + position[0].h);
			SDL_RenderFillRect(renderer, &position[0]); //Middle
			position[0].y += (position[1].h + position[0].h);
			SDL_RenderFillRect(renderer, &position[0]); //Bottom
			position[1].y += (position[1].h + position[0].h);
			SDL_RenderFillRect(renderer, &position[1]); //Bottom left
			position[1].y -= (position[1].h + position[0].h);
			position[1].x += (position[0].w + position[1].w);
			SDL_RenderFillRect(renderer, &position[1]); //Top right
			position[1].y += (position[1].h + position[0].h);
			SDL_RenderFillRect(renderer, &position[1]); //Bottom Right
			break;
		case(9):
			SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
			SDL_RenderFillRect(renderer, &position[0]); //Top
			SDL_RenderFillRect(renderer, &position[1]); //Top left
			position[0].y += (position[1].h + position[0].h);
			SDL_RenderFillRect(renderer, &position[0]); //Middle
			position[0].y += (position[1].h + position[0].h);
			SDL_RenderFillRect(renderer, &position[0]); //Bottom
			position[1].y += (position[1].h + position[0].h);
			SDL_SetRenderDrawColor(renderer, 50, 40, 40, 255);
			SDL_RenderFillRect(renderer, &position[1]); //Bottom left
			SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
			position[1].y -= (position[1].h + position[0].h);
			position[1].x += (position[0].w + position[1].w);
			SDL_RenderFillRect(renderer, &position[1]); //Top right
			position[1].y += (position[1].h + position[0].h);
			SDL_RenderFillRect(renderer, &position[1]); //Bottom Right
			break;
		case(-1):
			SDL_SetRenderDrawColor(renderer, 50, 40, 40, 255);
			SDL_RenderFillRect(renderer, &position[0]); //Top
			SDL_RenderFillRect(renderer, &position[1]); //Top left
			position[0].y += (position[1].h + position[0].h);
			SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
			SDL_RenderFillRect(renderer, &position[0]); //Middle
			position[0].y += (position[1].h + position[0].h);
			SDL_SetRenderDrawColor(renderer, 50, 40, 40, 255);
			SDL_RenderFillRect(renderer, &position[0]); //Bottom
			position[1].y += (position[1].h + position[0].h);
			SDL_RenderFillRect(renderer, &position[1]); //Bottom left
			position[1].y -= (position[1].h + position[0].h);
			position[1].x += (position[0].w + position[1].w);
			SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
			SDL_RenderFillRect(renderer, &position[1]); //Top right
			position[1].y += (position[1].h + position[0].h);
			SDL_RenderFillRect(renderer, &position[1]); //Bottom Right
			break;
		default:
			break;
		}
	}
}

//Returns digit from right of input
int getDigit(double input, int digit)
{
	//input = abcde  digit 0 = e, 1 = d etc...
	int value = (int)(input / pow(10, digit));
	return value % 10;
}

/*
	Next:
	- Add congratulations message when the user succesfully completes the game - Face added & win test fixed
	- Add game over message when the user fails - Sad face
	- Add timer at top & counter for mines less flags - done
	- Add highscore function
	- Add reset button at top -Face
	- Change reset button to appear depressed when clicked - dark area and shrink face rect?
	- Add toggle at top to switch L_CLICK to flag / reveal - done
	- Add menu to select no of mines and size of grid: EASY / MEDIUM / HARD / CUSTOM
	- Allow window to be rezied & scale everything accordingly - done
	- Fix mine counter for negative numbers - done
	- Add minimum window size? - done
	- Fix first move defeat

	- Highscore
	- Menu
	- Confetti for winning?

*/