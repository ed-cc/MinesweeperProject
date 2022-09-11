#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string>
#include <cmath>
#include "Mgraphics.h"


//LTexture class functions
LTexture::LTexture()
{
	//Initialise
	mTexture = NULL;
	mWidth = 0;
	mHeight = 0;
}

LTexture::~LTexture()
{
	//Deallocate
	free();
}

//Loads file from path using renderer
bool LTexture::loadFromFile(std::string path, SDL_Renderer *renderer)
{
	free(); //Get rid of existing texture

	SDL_Texture *newTexture = NULL; //Final texture

	SDL_Surface *loadedsurface = IMG_Load(path.c_str());
	if (loadedsurface == NULL)
	{
		printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
		return false;
	}
	//Create new texture from surface
	newTexture = SDL_CreateTextureFromSurface(renderer, loadedsurface);
	if (newTexture == NULL)
	{
		printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
		return false;
	}
	//Get image dimensions
	mWidth = loadedsurface->w;
	mHeight = loadedsurface->h;
	//Get rid of old surface
	SDL_FreeSurface(loadedsurface);
	//Return Success
	mTexture = newTexture;
	return mTexture != NULL;
}

//Frees memory of texture
void LTexture::free()
{
	//Free texture if it exists
	if (mTexture != NULL)
	{
		SDL_DestroyTexture(mTexture);
		mTexture = NULL;
		mWidth = 0;
		mHeight = 0;
	}
}

//Outputs part of the texture to the renderer
void LTexture::render(SDL_Renderer *renderer, SDL_Rect *destination, SDL_Rect *clip, double angle, SDL_Point *centre, SDL_RendererFlip flip)
{
	//Set rendering space
	//SDL_Rect renderQuad = { x, y, mWidth, mHeight };
	//if (clip != NULL) //Set clip rendering dimensions
	//{
	//	renderQuad.w = clip->w;
	//	renderQuad.h = clip->h;
	//}
	//Render to screen
	SDL_RenderCopyEx(renderer, mTexture, clip, destination, 0, centre, flip);

}


int LTexture::getWidth()
{
	return mWidth;
}

int LTexture::getHeight()
{
	return mHeight;
}

LTimer::LTimer()
{
	//Intialise
	mStartTicks = 0;
	mPausedTicks = 0;
	mPaused = false;
	mStarted = false;
}

//Start timing
void LTimer::start()
{
	mStarted = true; //Start timer
	mPaused = false; //Unpause timer

	mStartTicks = SDL_GetTicks(); //Get current clock time
	mPausedTicks = 0;
}

//Stops timing and resets timer
void LTimer::stop()
{
	mStarted = false; //Stop timer
	mPaused = false; //Unpause timer
	//Clear tick variables
	mStartTicks = 0;
	mPausedTicks = 0;
}

//Pauses timer at curent time
void LTimer::pause()
{
	if (mStarted && !mPaused) //If timer is running and not already paused
	{
		mPaused = true; //Pause timer
		//Calculate paused ticks
		mPausedTicks = SDL_GetTicks() - mStartTicks;
		mStartTicks = 0;
	}
}

//LTimer class functions
void LTimer::unpause()
{
	if (mStarted && mPaused) //If timer started and is paused
	{
		mPaused = false; //Unpause timer
		//Reset starting ticks
		mStartTicks = SDL_GetTicks() - mPausedTicks;
		mPausedTicks = 0; //Reset paused ticks
	}
}

//Returns the number of ticks counted by the timer
Uint32 LTimer::getTicks()
{
	Uint32 time = 0; //Actual time
	if (mStarted) //If running
	{
		if (mPaused) //If paused
		{
			time = mPausedTicks; //Set time to the number of ticks when paused
		}
		else
		{
			time = SDL_GetTicks() - mStartTicks; //Return current time less start time
		}
	}

	return time;
}

//Check if the timer has been started
bool LTimer::isStarted()
{
	return mStarted;
}

//Check if the timer has been paused
bool LTimer::isPaused()
{
	return mPaused && mStarted;
}

//Mbutton class functions
MButton::MButton()
{
	mPosition.x = 0;
	mPosition.y = 0;
	tCurrent = 0;
	tPrevious = 0;
}

//Handles a mouse event from e
void MButton::handleEvent(SDL_Event *e)
{
	//If a mouse event occured
	if (e->type == SDL_MOUSEBUTTONDOWN)
	{
		//Get mouse position
		int x, y;
		SDL_GetMouseState(&x, &y);
		//Store Values
		mPosition.x = x;
		mPosition.y = y;
		//Store click timestamp
		tCurrent = e->button.timestamp;

		//Store type of click
		switch (e->button.button)
		{
		case(SDL_BUTTON_LEFT):
			mCurrent = BUTTON_LEFT_DOWN;
			break;
		case(SDL_BUTTON_RIGHT):
			mCurrent = BUTTON_RIGHT_DOWN;
			break;
		default:
			break;
		}
	}
	else if (e->type == SDL_MOUSEBUTTONUP)
	{
		//Get mouse position
		int x, y;
		SDL_GetMouseState(&x, &y);
		//Store Values
		mPosition.x = x;
		mPosition.y = y;
		//Store click timestamp
		tCurrent = e->button.timestamp;

		//Store type of click
		switch (e->button.button)
		{
		case(SDL_BUTTON_LEFT):
			mCurrent = BUTTON_LEFT_UP;
			break;
		case(SDL_BUTTON_RIGHT):
			mCurrent = BUTTON_RIGHT_UP;
			break;
		default:
			break;
		}
	}
}

//Sets x and y to current mouse position and returns type of click
MButtonType MButton::getMState(int *x, int *y)
{
	*x = mPosition.x;
	*y = mPosition.y;
	return mCurrent;
}

//Resets mouse data
void MButton::reset()
{
	mPPrevious = mPosition;
	mPosition.x = 0;
	mPosition.y = 0;
	tPrevious = tCurrent;
	tCurrent = 0;
	mPrevious = mCurrent;
	mCurrent = BLANK;
}

//LWindow class functions
LWindow::LWindow()
{
	//Initialise non-existant window
	mWindow = NULL;
	mRenderer = NULL;

	mMouseFocus = false;
	mKeyboardFocus = false;
	mFullScreen = false;
	mMinimised = false;
	mShown = false;
	mWindowID = -1;
	mWidth = 0;
	mHeight = 0;

	min_h = 0;
	min_w = 0;
}

//Initialise an LWindow
bool LWindow::init(std::string wTitle, int wWidth, int wHeight, bool shown, bool resizable, bool ontop, bool bordered, int xPos, int yPos)
{
	//Create window
	/*if (resizable)
	{
		if (ontop)
		{
			mWindow = SDL_CreateWindow(wTitle.c_str(), xPos, yPos, wWidth, wHeight, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALWAYS_ON_TOP);
		}
		else
		{
			mWindow = SDL_CreateWindow(wTitle.c_str(), xPos, yPos, wWidth, wHeight, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
		}
	}*/
	
	if (ontop)
	{
		mWindow = SDL_CreateWindow(wTitle.c_str(), xPos, yPos, wWidth, wHeight, SDL_WINDOW_SHOWN | SDL_WINDOW_ALWAYS_ON_TOP);
	}
	else
	{
		mWindow = SDL_CreateWindow(wTitle.c_str(), xPos, yPos, wWidth, wHeight, SDL_WINDOW_SHOWN);
	}
	
	if (!shown)
	{
		SDL_HideWindow(mWindow);
	}
	if (resizable)
	{
		SDL_SetWindowResizable(mWindow, SDL_TRUE);
	}
	if (!bordered)
	{
		SDL_SetWindowBordered(mWindow, SDL_FALSE);
	}

	if (mWindow != NULL)
	{
		mMouseFocus = true;
		mKeyboardFocus = true;
		mWidth = wWidth;
		mHeight = wHeight;

		mRenderer = SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
		if (mRenderer == NULL)
		{
			printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
			SDL_DestroyWindow(mWindow);
			mWindow = NULL;
		}
		else
		{
			//Initialise renderer colour
			SDL_SetRenderDrawColor(mRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
			//Grab window ID
			mWindowID = SDL_GetWindowID(mWindow);
			//Flag as opened
			mShown = true;
		}
	}
	else
	{
		printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
	}

	return mWindow != NULL && mRenderer != NULL;
}

//Returns a pointer to the renderer used by the window
SDL_Renderer *LWindow::getRenderer()
{
	return mRenderer;
}

//Returns the ID of this window
int LWindow::getWindowID()
{
	return mWindowID;
}

//Handles window events from e
void LWindow::handleEvent(SDL_Event &e)
{
	if (e.type == SDL_WINDOWEVENT && e.window.windowID == mWindowID)
	{
		switch (e.window.event)
		{
			//Window appeared
		case SDL_WINDOWEVENT_SHOWN:
			mShown = true;
			break;

			//Window disappeared
		case SDL_WINDOWEVENT_HIDDEN:
			mShown = false;
			break;
			
			//Get new dimensions and redraw on window size change
		case SDL_WINDOWEVENT_SIZE_CHANGED:
			mWidth = e.window.data1;
			mHeight = e.window.data2;
			SDL_RenderPresent(mRenderer);
			break;

			//Redraw when exposed
		case SDL_WINDOWEVENT_EXPOSED:
			SDL_RenderPresent(mRenderer);
			break;

			//Mouse entered window
		case SDL_WINDOWEVENT_ENTER:
			mMouseFocus = true;
			break;

			//Mouse left window
		case SDL_WINDOWEVENT_LEAVE:
			mMouseFocus = false;
			break;

			//Window has keyboard focus
		case SDL_WINDOWEVENT_FOCUS_GAINED:
			mKeyboardFocus = true;
			break;

			//Window lost keyboard focus
		case SDL_WINDOWEVENT_FOCUS_LOST:
			mKeyboardFocus = false;
			break;

			//Window minimised
		case SDL_WINDOWEVENT_MINIMIZED:
			mMinimised = true;
			break;

			//Window maximised
		case SDL_WINDOWEVENT_MAXIMIZED:
			mMinimised = false;
			break;

			//Window restored
		case SDL_WINDOWEVENT_RESTORED:
			mMinimised = false;
			break;

			//Hide on close
		case SDL_WINDOWEVENT_CLOSE:
			SDL_HideWindow(mWindow);
			break;

		default:
			break;
		}
	}

}

//Brings the window into focus
void LWindow::focus()
{
	//Restore window if needed
	if (!mShown)
	{
		SDL_ShowWindow(mWindow);
	}

	//Move window forward
	SDL_RaiseWindow(mWindow);
}

//Frees memory used by the window and its renderer
void LWindow::free()
{
	if (mWindow != NULL)
	{
		SDL_DestroyWindow(mWindow);
	}
	mWindow = NULL;

	if (mRenderer != NULL)
	{
		SDL_DestroyRenderer(mRenderer);
	}
	mRenderer = NULL;

	mMouseFocus = false;
	mKeyboardFocus = false;
	min_h = 0;
	min_w = 0;
	mWidth = 0;
	mHeight = 0;
}

//Returns window width
int LWindow::getWidth()
{
	return mWidth;
}

//Returns window height
int LWindow::getHeight()
{
	return mHeight;
}

//Returns window mouseOver state
bool LWindow::hasMouseFocus()
{
	return mMouseFocus;
}

//Returns window keyboard state
bool LWindow::hasKeyboardFocus()
{
	return mKeyboardFocus;
}

//Returns if window is minimised
bool LWindow::isMinimised()
{
	return mMinimised;
}

//Returns if the window is being shown
bool LWindow::isShown()
{
	return mShown;
}

//Sets a minimum width min_x and height min_y for the window
void LWindow::setMinimum(int min_x, int min_y)
{
	//Do nothing if thw window is fullscreen
	if (!mFullScreen)
	{
		//Ensure numbers are +ve
		if (min_x < 0) min_w = -min_x;
		else min_w = min_x;
		if (min_y < 0) min_h = -min_y;
		else min_h = min_y;
		SDL_SetWindowMinimumSize(mWindow, min_w, min_h);
	}
}