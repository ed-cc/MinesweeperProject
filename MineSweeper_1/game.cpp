/*
	Init: - Take input of x&y dimension & mine no
	1.Fill array with mines(-1) + same size array of 0 for revealed / flags
	2.Assign numbers based upon surrounding mines (only check 0s)
	Play:
	1.Proccess user input - gain l / r and array position - calculate from screen position in UI code
	End:
	1.Free / Delete arrays
*/
#include <stdio.h>
#include <iostream>
#include <cstdlib>
#include <time.h>

int **giveArray(int x, int y);
void printArray(int **arr, int x, int y);
int fillMines(int **grid, int x, int y, int n);
int gridNum(int **grid, int x, int y);
int revealSurround(int **gameArr, int **revealArr, int xpos, int ypos, int xmax, int ymax);

int main()
{
	int **mGrid = giveArray(10, 10);
	int **uGrid = giveArray(10, 10);
	int test = fillMines(mGrid, 10, 10, 20);
	printArray(mGrid, 10, 10);
	printf("\n%d\n", test);
	gridNum(mGrid, 10, 10);
	printArray(mGrid, 10, 10);
	int xv, yv;
	printf("Please enter an x value: ");
	std::cin >> xv;
	printf("Please enter a y value: ");
	std::cin >> yv;
	int testa = revealSurround(mGrid, uGrid, xv, yv, 10, 10);
	printArray(uGrid, 10, 10);
	printf("\n%d\n", testa);

	return 0;
}

int **giveArray(int x, int y) //Gives array of x * y, each position set to 0
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

void printArray(int **arr, int x, int y) //Prints array arr dimensions x * y
{
	for (int ii = 0; ii < y; ii++)
	{
		for (int jj = 0; jj < x; jj++)
		{
			printf("%d ", arr[ii][jj]);
		}
		printf("\n");
	}
}

int fillMines(int **grid, int x, int y, int n)
{
	int placed = 0;
	srand(time(NULL));
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

int gridNum(int **grid, int x, int y)
{
	int modified = 0;
	for (int ii = 0; ii < y; ii++)
	{
		for (int jj = 0; jj < x; jj++)
		{
			//Find blank, count surrounding mines, assign that value to the place
			if (grid[ii][jj] == 0)
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
						if ((revealArr[ypos + jj][xpos + ii] == 0) &&(gameArr[ypos + jj][xpos + ii] == 0)) //If unrevealed
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