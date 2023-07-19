#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include "Display.h"
#include <chrono>


#undef main


#define MEMORY_SIZE 4096
#define ROM_SIZE 3583
#define STACK_SIZE 16
#define ROM_START_ADDR 0x200
#define FRAME_RATE 60
#define CPU_RATE 800

enum instructionType {CLS, RET, SYS, JP, CALL, SE, SNE, LD, OR, AND, XOR, ADD, SUB, SHR, SUBN, SHL, RND, DRW, SKP, SKNP};

class Chip8 {
	
	private:
		uint8_t ram[MEMORY_SIZE];
		uint8_t buffer[ROM_SIZE];
		uint16_t stack[STACK_SIZE];
		uint8_t registers[16];
		uint8_t delayTimer;
		uint8_t soundTimer;	
		uint16_t registerI;
		uint16_t programCounter;
		uint16_t* stackPointer;
		bool beepPlaying;
		
		Mix_Chunk* beep;
		Display disp;

		uint16_t determineInstruction(uint16_t instruction);
		uint8_t determineKeyPress();
		void executeInstruction();
		void toggleSound(uint16_t state);
	
	public:
		void start();
		uint8_t viewMemoryCell(uint16_t cell);
		Chip8(FILE*);
		~Chip8();
};
