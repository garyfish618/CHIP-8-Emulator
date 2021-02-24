#include "Display.h"
#include <iostream>
#include <string>

Display::Display() {
	//Initialize font
	TTF_Init();
	font = TTF_OpenFont("Fonts/8Bit.ttf", FONT_SIZE);

	SDL_DisplayMode dp;
	SDL_GetCurrentDisplayMode(0, &dp);
	window = SDL_CreateWindow("CHIP-8 Display", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		DISPLAY_WIDTH_PX * SCALE, DISPLAY_HEIGHT_PX * SCALE, SDL_WINDOW_MAXIMIZED);

	SDL_SetWindowFullscreen(window, 0);

	if (!window) {
		std::cout << "Error initializing SDL window" << SDL_GetError();
	}

	sur = SDL_GetWindowSurface(window);

	//Create Register View


	SDL_Rect textLocation = { 80, 80, 0, 0 };

	std::string registerView =
		"+------------+--+\n"
		"| Register 0 |  |\n"
		"+------------+--+\n"
		"| Register 1 |  |\n";
	/*+------------+--+
	| Register 2 |  |
	+------------+--+
	| Register 3 |  |
	+------------+--+
	| Register 4 |  |
	+------------+--+
	| Register 5 |  |
	+------------+--+
	| Register 6 |  |
	+------------+--+
	| Register 7 |  |
	+------------+--+
	| Register 8 |  |
	+------------+--+
	| Register 9 |  |
	+------------+--+
	| Register A |  |
	+------------+--+
	| Register B |  |
	+------------+--+
	| Register C |  |
	+------------+--+
	| Register D |  |
	+------------+--+
	| Register E |  |
	+------------+--+
	| Register F |  |
	+------------+--+
	| Register I |  |
	+------------+--+*/

	
	SDL_Color foregroundColor = { 0, 0, 0 };
	SDL_Color backgroundColor = { 230, 225, 83 };
	
	//Todo: Fix addTextLine
	SDL_Rect screenRegion = { 0,0,0,0 };
	SDL_Surface* textSurface = TTF_RenderText_Shaded(font, "Hello World", foregroundColor, backgroundColor);
	SDL_BlitSurface(textSurface, NULL, sur, &screenRegion);

	SDL_Surface* newSurface = addTextLine("New Line", textSurface, false); 
	SDL_UpdateWindowSurface(window);
	
}

Display::~Display() {
	SDL_DestroyWindow(window);

}

SDL_Surface* Display::addTextLine(const char* text, SDL_Surface* dest, bool highlighted) {
	SDL_Color foregroundColor = { 0, 0, 0 };
	SDL_Color backgroundColor = { 230, 225, 83 };


	SDL_Rect pos = { 0, 0 + dest->h, 0, 0};

	SDL_Surface* su = TTF_RenderText_Shaded(font, text, foregroundColor, backgroundColor);

	SDL_BlitSurface(su, NULL, sur, &pos);
	return su;
}

void Display::clearDisplay() {
	for (int i = 0; i < DISPLAY_HEIGHT_PX; i++) {
		for (int j = 0; j < DISPLAY_WIDTH_PX; j++) {
			dis[j][i] = 0x00;
		}
	}

	//Clear screen to white
	SDL_FillRect(sur, NULL, SDL_MapRGB(sur->format, 255, 255, 255));
	SDL_UpdateWindowSurface(window);

}

void Display::drawPixel(int x, int y) {
	SDL_Rect pix;
	pix.x = x * SCALE;
	pix.y = y * SCALE;
	pix.w = 1 * SCALE;
	pix.h = 1 * SCALE;
	SDL_FillRect(sur, &pix, SDL_MapRGB(sur->format, 0, 0, 0));
	SDL_UpdateWindowSurface(window);


}


char Display::getDisplay(int x, int y) {
	return dis[x][y];
}
