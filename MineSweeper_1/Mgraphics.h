#pragma once

//Mouse button type enum
enum MButtonType
{
	BLANK = -1,
	BUTTON_LEFT_DOWN,
	BUTTON_LEFT_UP,
	BUTTON_RIGHT_DOWN,
	BUTTON_RIGHT_UP
};

//Texture Wrapper Class
class LTexture
{
public:
	//Constructor
	LTexture();
	//Destructor
	~LTexture();
	//Load from specified path
	bool loadFromFile(std::string path, SDL_Renderer *renderer);
	//Deallocates texture
	void free();
	//Renders texture at given point
	void render(SDL_Renderer *renderer, SDL_Rect *destination, SDL_Rect *clip = NULL, double angle = 0, SDL_Point *centre = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);
	//Get image dimensions
	int getWidth();
	int getHeight();

private:
	//The hardware texture
	SDL_Texture *mTexture;
	//Image Dimensions
	int mWidth;
	int mHeight;
};


//Time based timer class
class LTimer
{
public:
	//Constructor
	LTimer();
	//Various clock actions
	void start();
	void stop();
	void pause();
	void unpause();
	//Get the timers time
	Uint32 getTicks();
	//Checks timer status
	bool isStarted();
	bool isPaused();

private:
	//Time when timer started
	Uint32 mStartTicks;
	//Time when timer paused
	Uint32 mPausedTicks;
	//Timer status
	bool mPaused;
	bool mStarted;
};

//Mouse button handling class
class MButton
{
public:
	//Constructor
	MButton();

	//Handle a mouse event
	void handleEvent(SDL_Event *e);

	//Give coordinates of mouse click
	MButtonType getMState(int *x, int *y);
	//Reset values
	void reset();
private:
	//Most recent event type
	MButtonType mCurrent;
	//Previous event type
	MButtonType mPrevious;
	//Timestamp of most recent event
	Uint32 tCurrent;
	//Timestamp of previous event
	Uint32 tPrevious;
	//Position
	SDL_Point mPosition;
	SDL_Point mPPrevious;
};

//Wiindow handling class
class LWindow
{
public:
	//Constructor
	LWindow();

	//Creates window and renderer
	bool init(std::string wTitle, int wWidth, int wHeight, bool shown = true, bool resizable = false, bool ontop = false, bool bordered = true, int xPos = SDL_WINDOWPOS_UNDEFINED, int yPos = SDL_WINDOWPOS_UNDEFINED);

	//Gets renderer from internal window
	SDL_Renderer *getRenderer();

	//Returns window ID
	int getWindowID();

	//Handles window events
	void handleEvent(SDL_Event &e);

	//Focuses on window
	void focus();

	//Deallocates internals
	void free();

	//Get window dimensions
	int getWidth();
	int getHeight();

	//Set window minimum size
	void setMinimum(int min_x, int min_y);

	//Window foci
	bool hasMouseFocus();
	bool hasKeyboardFocus();
	bool isMinimised();
	bool isShown();

private:
	//Window data
	SDL_Window *mWindow;
	SDL_Renderer *mRenderer;
	int mWindowID;

	//Window Dimensions
	int mWidth;
	int mHeight;
	int min_w, min_h;

	//Window focus
	bool mMouseFocus;
	bool mKeyboardFocus;
	bool mFullScreen;
	bool mMinimised;
	bool mShown;
};
