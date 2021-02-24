#include <string>
#include <SDL.h>
#include <SDL_mixer.h>
#include "Display.h"


#undef main


#define MEMORY_SIZE 4096
#define ROM_SIZE 3583
#define STACK_SIZE 16
#define ROM_START_ADDR 0x200

enum instructionType {CLS, RET, SYS, JP, CALL, SE, SNE, LD, OR, AND, XOR, ADD, SUB, SHR, SUBN, SHL, RND, DRW, SKP, SKNP};

class Chip8 {
	
	private:
		unsigned char ram[MEMORY_SIZE];
		unsigned char buffer[ROM_SIZE];
		unsigned short stack[STACK_SIZE];
		unsigned char registers[16];
		unsigned char delayTimer;
		unsigned char soundTimer;	
		unsigned short registerI;
		unsigned short programCounter;
		unsigned short* stackPointer;
		
		
		Mix_Chunk* beep;
		Display disp;
		
		//TODO Create timer and sound registers

		int determineInstruction(unsigned short instruction);
		char determineKeyPress();
		void executeInstruction();
	
	public:
		void start();
		char viewMemoryCell(int cell);
		Chip8(FILE*);
		~Chip8();


		


	



};