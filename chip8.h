#include <string>
#include <SDL.h>
#undef main


#define MEMORY_SIZE 4096
#define ROM_SIZE 3583
#define STACK_SIZE 16
#define ROM_START_ADDR 0x200
#define DISPLAY_HEIGHT_PX 32
#define DISPLAY_WIDTH_PX 64
#define SCALE 12

enum instructionType {CLS, RET, SYS, JP, CALL, SE, SNE, LD, OR, AND, XOR, ADD, SUB, SHR, SUBN, SHL, RND, DRW, SKP};

class Chip8 {
	
	private:
		unsigned char buffer[ROM_SIZE];
		unsigned char mem[MEMORY_SIZE];
		unsigned short stack[STACK_SIZE];
		unsigned char registers[15];
		unsigned short registerI;
		unsigned short programCounter;
		unsigned short* stackPointer;
		char display[DISPLAY_WIDTH_PX][DISPLAY_HEIGHT_PX];

		SDL_Window* window;
		SDL_Surface* sur;
		//TODO Create timer and sound registers

		int determineInstruction(unsigned short instruction);
		void clearDisplay();
	
	public:
		void executeInstruction();
		char viewMemoryCell(int cell);
		Chip8(FILE*);
		


	



};