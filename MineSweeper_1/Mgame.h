#pragma once

//Mgame.cpp
//Game functions class prototype
class Mgame
{
public:
	//Constructor
	Mgame();

	//Destructor
	~Mgame();

	//Test for gameover
	bool testOver();

	//Test if game is won
	bool testWin();

	//Initialise Mgame
	bool init(int xSize, int ySize, int mines); 
	
	//Turn of the game from user input
	void gameTurn(int xpos, int ypos, int type);

	//Returns a value to be displayed on the tile
	int getTile(int xpos, int ypos);

	//Returns if tile is revealed or flagged
	int getRevealed(int xpos, int ypos);

	//Returns number of mines - number of flags
	int getRemMine();

	//Free memory
	void free();

private:
	//Required variables
	int **gGrid, **usrGrid, x, y, mineNo, revealCount, flagCount, turns;
	//Flags for gameplay state
	bool gameOver, gameWin;
};

enum Tiles
{
	TILE_0,
	TILE_1,
	TILE_2,
	TILE_3,
	TILE_4,
	TILE_5,
	TILE_6,
	TILE_7,
	TILE_8,
	TILE_BLANK,
	TILE_FLAG,
	TILE_NOMINE,
	TILE_MINE,
	TILE_BADMINE,
	TILE_ERROR
};

