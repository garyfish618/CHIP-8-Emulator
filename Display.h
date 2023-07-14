#include <SDL2/SDL_ttf.h>
#include<SDL2/SDL_mixer.h>

#define CHIP8_DISPLAY_HEIGHT_PX 32
#define CHIP8_DISPLAY_WIDTH_PX 64
#define SCREEN_WIDTH_PX 1920
#define SCREEN_HEIGHT_PX 1080 
#define FONT_SIZE 22
#define SCALE 12
#define BACKGROUND_COLOR { 80,80,80 }
#define TEXT_COLOR {255, 255, 255}
#define HIGHLIGHT_COLOR { 230, 225, 83 }
#define PIXEL_COLOR {0, 0, 0}

struct Coordinates {
	int x;
	int y;
};

class Display {
	private:

		SDL_Renderer* gameRenderer;
		SDL_Window* window;
		SDL_Texture* pixSquare;



		
		TTF_Font* font;
		char dis[CHIP8_DISPLAY_WIDTH_PX][CHIP8_DISPLAY_HEIGHT_PX];
		void addTextLine(const char* text, struct Coordinates* nextLine, bool highlighted);
		struct Coordinates regCoords[16];

	public:
		void clearDisplay();
		void updateScreen();
		void updateRegister(int reg, unsigned char val);
		void setDisplay(int x, int y);
		Display();
		~Display();
		char getDisplay(int x, int y);
		


};