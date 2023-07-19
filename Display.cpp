#include "includes/Display.h"
#include <iostream>
#include <string>


Display::Display() {


	//Initialize font
	TTF_Init();
	font = TTF_OpenFont("Fonts/8Bit.ttf", FONT_SIZE);

	window = SDL_CreateWindow("CHIP-8 Display", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		SCREEN_WIDTH_PX, SCREEN_HEIGHT_PX, SDL_WINDOW_MAXIMIZED);

	SDL_SetWindowFullscreen(window, 0);

	if (!window) {
		std::cout << "Error initializing SDL window" << SDL_GetError();
	}

	//Create game viewport
	SDL_Rect gameViewport;
	gameViewport.x = 0;
	gameViewport.y = 0;
	gameViewport.w = CHIP8_DISPLAY_WIDTH_PX * SCALE;
	gameViewport.h = CHIP8_DISPLAY_HEIGHT_PX * SCALE;

	gameRenderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (gameRenderer == NULL) {
		std::cout << "Error creating SDL Renderer\n" << SDL_GetError();
	}

	SDL_RenderSetViewport(gameRenderer, &gameViewport);
	SDL_SetRenderDrawColor(gameRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

	SDL_Surface* loadedSur = SDL_LoadBMP("Images/Square.bmp");

	if (loadedSur == NULL) {
		std::cout << "Failed to open image" << SDL_GetError();
	}
	pixSquare = SDL_CreateTextureFromSurface(gameRenderer, loadedSur);

	if (pixSquare == NULL) {
		std::cout << "Failed to create texture" << SDL_GetError();
	}

	SDL_FreeSurface(loadedSur);
	clearDisplay();
	


	

	//Create Register View
	//SDL_Rect regLocation = { 80, 80, 0, 0 };
	//struct Coordinates nextLine;
	//nextLine.x = 1500;
	//nextLine.y = 100;
	//addTextLine("+------------+--+", &nextLine, false);
	//for (int i = 0; i < 16; i++) {
	//	regCoords[i] = nextLine;
	//	char hexChar[16];
	//	sprintf_s(hexChar, "%x", i);
	//	std::string line = "| Register " + std::string(hexChar) + " |0x00|";
	//	addTextLine(line.c_str(), &nextLine, false);
	//	
	//	addTextLine("+------------+--+", &nextLine, false);
	//}


	//SDL_UpdateWindowSurface(window);
	
}

Display::~Display() {
	SDL_DestroyWindow(window);
	SDL_DestroyTexture(pixSquare);
	TTF_CloseFont(font);
}

void Display::addTextLine(const char* text, struct Coordinates* nextLine, bool highlighted) {
	//Displays a line of text and places coordinates of where to draw the next line of text in nextLine

	
	//SDL_Color foregroundColor = TEXT_COLOR;
	//SDL_Color backgroundColor;
	//
	//if (highlighted) {
	//	backgroundColor = HIGHLIGHT_COLOR;
	//}

	//else {
	//	backgroundColor = BACKGROUND_COLOR;
	//}

	//SDL_Rect pos;

	//pos = { nextLine->x, nextLine->y, 0, 0 };

	//SDL_Surface* su = TTF_RenderText_Shaded(font, text, foregroundColor, backgroundColor);
	//SDL_BlitSurface(su, NULL, sur, &pos);
	//nextLine->y =  pos.y + su->h;
	//return;
}

void Display::clearDisplay() {
	for (int i = 0; i < CHIP8_DISPLAY_HEIGHT_PX; i++) {
		for (int j = 0; j < CHIP8_DISPLAY_WIDTH_PX; j++) {
			dis[j][i] = 0x00;
		}
	}

	updateScreen();
}

void Display::updateScreen() {
	SDL_RenderClear(gameRenderer);


	for (int i = 0; i < CHIP8_DISPLAY_WIDTH_PX; i++) {
		for (int j = 0; j < CHIP8_DISPLAY_HEIGHT_PX; j++) {
			if (dis[i][j] != 0x00) {
				SDL_Rect dest = { i * SCALE, j * SCALE, SCALE,SCALE };

				SDL_RenderCopy(gameRenderer, pixSquare, NULL, &dest);
			}
		}
	}

	SDL_RenderPresent(gameRenderer);
}

void Display::setDisplay(int x, int y) {
	if (dis[x][y] != 0x00) {
		dis[x][y] = 0x00;
	}

	else {
		dis[x][y] = 0xFF;
	}
}



char Display::getDisplay(int x, int y) {
	return dis[x][y];
}

void Display::updateRegister(int reg, unsigned char val) {

	//char hexChar[16];
	//char hexValue[4];

	//sprintf_s(hexChar, "%X", reg);
	//sprintf_s(hexValue, "%02X", val);

	//struct Coordinates temp = { regCoords[reg].x, regCoords[reg].y };

	//std::string line = "| Register " + std::string(hexChar) + " |0x" + std::string(hexValue) + "|";
	//addTextLine(line.c_str(), &temp, false);
	//SDL_UpdateWindowSurface(window);
}
