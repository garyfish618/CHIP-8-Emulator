#include <SDL_ttf.h>
#include<SDL_mixer.h>

#define DISPLAY_HEIGHT_PX 32
#define DISPLAY_WIDTH_PX 64
#define FONT_SIZE 34
#define SCALE 12

class Display {
	private:
		SDL_Window* window;
		SDL_Surface* sur;
		TTF_Font* font;
		char dis[DISPLAY_WIDTH_PX][DISPLAY_HEIGHT_PX];
		SDL_Surface* addTextLine(const char* text, SDL_Surface* dest, bool highlighted);

	public:
		void clearDisplay();
		void drawPixel(int x, int y);
		Display();
		~Display();
		char getDisplay(int x, int y);
		


};