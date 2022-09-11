//Include needed libs
#include <cstdlib>
#include <time.h>
#include "Mgame.h"

//Function prototypes
static int **giveArray(int x, int y);
static int fillMines(int **grid, int x, int y, int n);
static int gridNum(int **grid, int x, int y);
static void destroyArray(int **arr, int x, int y);
static int revealSurround(int **gameArr, int **revealArr, int xpos, int ypos, int xmax, int ymax);
static void addMine(int **grid, int xmax, int ymax);

Mgame::Mgame() //Constructor
{
	gameOver = false;
	x = 5;
	y = 5;
	mineNo = 0;
	revealCount = 0;
	flagCount = 0;
	turns = 0;
}

Mgame::~Mgame() //Destructor
{
	//Free memory
	free();
}

//Frees memory used by the game
void Mgame::free()
{
	destroyArray(gGrid, x, y);
	gGrid = NULL;
	destroyArray(usrGrid, x, y);
	usrGrid = NULL;
	x = 0, y = 0, mineNo = 0;
}

//Returns the value of the tile at xpos, ypos
int Mgame::getTile(int xpos, int ypos)
{
	return gGrid[ypos][xpos];
}

//Returns the value at xpos, ypos: 0 for unrevealed; 1 for unrevealed; 2 for flagged
int Mgame::getRevealed(int xpos, int ypos)
{
	return usrGrid[ypos][xpos];
}

//Initialise the game with size xSize by ySize and mines no. of mines
bool Mgame::init(int xSize, int ySize, int mines)
{
	//Reset game over flag
	gameOver = false;
	//Store board sizes
	x = xSize;
	y = ySize;
	mineNo = mines;
	//Create array for board and for user input
	gGrid = giveArray(x, y);
	if (gGrid == NULL) return false;
	usrGrid = giveArray(x, y);
	if (usrGrid == NULL) return false;
	//Fill gGrid with desired mines then add numbers
	fillMines(gGrid, x, y, mineNo);
	gridNum(gGrid, x, y);
	//Reset variables
	gameOver = false;
	gameWin = false;
	flagCount = 0;
	revealCount = 0;

	return true;
}

//Turn of the game from user input
//Type 1 = reveal, 2 = flag
void Mgame::gameTurn(int xpos, int ypos, int type) 
{
	if (type == 1)
	{
		if (usrGrid[ypos][xpos] == 2); //If a flag do nothing
		else
		{
			if ((turns == 0) && (gGrid[ypos][xpos] == -1)) //Move mine of first turn
			{
				gGrid[ypos][xpos] = 0;
				addMine(gGrid, x, y); //Moves mine to top left
				gridNum(gGrid, x, y); //Renumber the grid
			}
			usrGrid[ypos][xpos] = 1; //Reveal tile
			revealCount++; //Increment number of revealed tiles
			if (gGrid[ypos][xpos] == 0) //If tile is blank free surrounding
			{
				usrGrid[ypos][xpos] = 1;
				revealCount += revealSurround(gGrid, usrGrid, xpos, ypos, x, y);
			}
			else if (gGrid[ypos][xpos] == -1) //If a mine
			{
				gameOver = true;
			}
			turns++; //A turn has been taken
		}
	}
	else if (type == 2)
	{
		if (usrGrid[ypos][xpos] == 1); //If revealed do nothing
		else
		{
			if (usrGrid[ypos][xpos] == 2) //Toggle flag
			{
				usrGrid[ypos][xpos] = 0;
				flagCount--;
				
			}
			else //Add a flag
			{
				usrGrid[ypos][xpos] = 2;
				flagCount++;
			}
			turns++;
		}
	}
}

bool Mgame::testOver()
{
	return gameOver;
}

bool Mgame::testWin()
{
	bool win = true;
	//Loop through arrays and check each value for a win
	for (int ii = 0; ii < x; ii++)
	{
		for (int jj = 0; jj < y; jj++)
		{
			int a = gGrid[jj][ii];
			int b = usrGrid[jj][ii];
			switch (b)
			{
			case(0):
				if (a != -1) win = false;
				break;
			case(1):
				if (a == -1) win = false;
				break;
			case(2):
				if (a != -1) win = false;
				break;
			default:
				break;
			}
		}
	}
	if (win == true)
	{
		gameOver = true;
	}
	return win;
}

int Mgame::getRemMine()
{
	return mineNo - flagCount;
}

//Reveal any open tiles surrounding the selected
int revealSurround(int **gameArr, int **revealArr, int xpos, int ypos, int xmax, int ymax)
{
	int changed = 0;
	if (gameArr[ypos][xpos] == 0)
	{
		for (int ii = -1; ii <= 1; ii++)//Move through adjacent x
		{
			if (((xpos + ii) >= 0) && (xpos + ii) < xmax)
			{
				for (int jj = -1; jj <= 1; jj++) //Move through adjacent y
				{
					if (((ypos + jj) >= 0) && (ypos + jj) < ymax)
					{
						if ((revealArr[ypos + jj][xpos + ii] == 0) /*&& (gameArr[ypos + jj][xpos + ii] == 0)*/) //If unrevealed
						{
							revealArr[ypos + jj][xpos + ii] = 1;
							changed++;
							changed += revealSurround(gameArr, revealArr, (xpos + ii), (ypos + jj), xmax, ymax);

						}
					}
				}
			}
		}
		//check not at edge
		//Check surrounding tiles && check not freed
		//Reveal surrounding tiles
		//Repeat for each surrounding tile
	}
	return changed;
}

//Gives array of x * y, each position set to 0
int **giveArray(int x, int y) 
{
	int **mGrid = new int*[y];
	for (int ii = 0; ii < y; ii++)
	{
		mGrid[ii] = new int[x];
		for (int jj = 0; jj < x; jj++)
		{
			mGrid[ii][jj] = 0;
		}
	}
	return mGrid;
}

//Delete array of x * y: only use if allocated with new
void destroyArray(int **arr, int x, int y)
{
	for (int ii = 0; ii < y; ii++)
	{
		delete(arr[ii]);
	}
	arr = NULL;
}

//Fill n mines into **grid of size x by y
int fillMines(int **grid, int x, int y, int n)
{
	int placed = 0;
	srand((unsigned int)time(NULL));
	if ((y*x) / 2 >= n) //Mines take up fewer than half of available places: Default situation
	{
		for (int i = 0; placed < n; i++)
		{
			//Get random coords
			int yco = rand() % y, xco = rand() % x;
			//Check not occupied, place mine, incr placed if not
			if (grid[yco][xco] != -1) placed++;
			grid[yco][xco] = -1;

		}
	}
	else if ((y*x) / 2 < n) //Mines take up more than half of spaces: unlikely
	{
		for (int ii = 0; ii < y; ii++) //Fill with mines
		{
			for (int jj = 0; jj < x; jj++)
			{
				grid[ii][jj] = -1;
				placed++;
			}
		}
		for (int i = 0; placed > n; i++) //Remove mines randomly
		{
			//Get random coords
			int yco = rand() % y, xco = rand() % x;
			//Check occupied, place mine, decr placed if it is.
			if (grid[yco][xco] == -1) placed--;
			grid[yco][xco] = 0;

		}
	}
	else if (n > (y*x)) //If no. of mines is higher than number of places: this should never happen.
	{
		for (int ii = 0; ii < y; ii++)
		{
			for (int jj = 0; jj < x; jj++)
			{
				grid[ii][jj] = -1;
				placed++;
			}
		}
	}

	return placed;
}

//Adds 1 mine to **grid of size xmax by ymax
void addMine(int **grid, int xmax, int ymax)
{
	bool placed = false;
	for (int ii = 0; (ii < ymax) && (placed == false); ii++)
	{
		for (int jj = 0; (jj < xmax) && (placed == false); jj++)
		{
			if (grid[ii][jj] != -1)
			{
				grid[ii][jj] = -1;
				placed = true;
			}
		}
	}
}

//Calculates numbers surrounding mines of -1 in **grid of maximum size x by y
int gridNum(int **grid, int x, int y)
{
	int modified = 0;
	for (int ii = 0; ii < y; ii++)
	{
		for (int jj = 0; jj < x; jj++)
		{
			//Find non-mine, count surrounding mines, assign that value to the place
			if (grid[ii][jj] != -1)
			{
				//test
				int counted = 0;
				for (int ia = -1; ia < 2; ia++)
				{
					if (((ii + ia) < 0) || ((ii + ia) >= y)) {} //Skip if outside grid
					else
					{
						for (int ja = -1; ja < 2; ja++)
						{
							if (((jj + ja) < 0) || ((jj + ja) >= x)) {}
							else if (grid[ii + ia][jj + ja] == -1) counted++;
						}
					}
				}
				//Enter no of surrounding mines
				grid[ii][jj] = counted;
				modified++;
			}
		}
	}
	return modified;
}