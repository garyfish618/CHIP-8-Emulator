#include "Chip8.h"
#include <string>
#include <stdexcept>
#include <iostream>
#include "WinSock2.h"
#include <SDL.h>
#undef main

Chip8::Chip8(FILE* romPointer) {
	//Initialize arrays
	for (int i = 0; i < ROM_SIZE; i++) {
		buffer[i] = 0;

	}

	for (int i = 0; i < MEMORY_SIZE; i++) {
		mem[i] = 0;

	}

	for (int i = 0; i < STACK_SIZE; i++) {
		stack[i] = 0;

	}

	// Initialize SDL window
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		std::cout << "Error initializing SDL" << SDL_GetError() << std::endl;
		return;
		
	}

	 window = SDL_CreateWindow("CHIP-8 Display", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		DISPLAY_WIDTH_PX * SCALE, DISPLAY_HEIGHT_PX * SCALE, 0);

	if (!window) {
		std::cout << "Error initializing SDL window" << SDL_GetError();
	}

	 sur = SDL_GetWindowSurface(window);
	 SDL_FillRect(sur, NULL, SDL_MapRGB(sur->format, 255, 255, 255));
	
	

	clearDisplay();


	//Determine size of file
	fseek(romPointer, 0, SEEK_END);
	long end = ftell(romPointer);
	fseek(romPointer, 0, SEEK_SET);
	long start = ftell(romPointer);
	long length = end - start;
	std::cout << "LENGTH:" << length << "\n";

	if (fread(&buffer, sizeof(unsigned char) , ROM_SIZE, romPointer) > ROM_SIZE) {
		std::cout << "Error reading ROM file";
	}

	//Convert to host endianess
	//for (int i = 0; i < ROM_SIZE; i++) {
	//	buffer[i] = ntohs(buffer[i]);
	//}


	/*for (int i = 0; i < ROM_SIZE; i++) {
		std::cout << std::hex << buffer[i] << "\n";
	}*/

	programCounter = 0x0200;
	registerI = 0;
	stackPointer = stack - 1;
	
}


void Chip8::clearDisplay() {
	for (int i = 0; i < DISPLAY_HEIGHT_PX; i++) {
		for (int j = 0; j < DISPLAY_WIDTH_PX; j++) {
			display[j][i] = 0x00;
		}
	}
}

void Chip8::executeInstruction() {
	
	//Grab instruction
	unsigned short nextInstruction = buffer[programCounter - ROM_START_ADDR] << 4;
	nextInstruction = (nextInstruction << 4) | buffer[(programCounter + 1) - ROM_START_ADDR];
	int op = determineInstruction(nextInstruction);

	/*
		addr = 12 bit address (Lowest 12 bits)
		nib = 4 lowest bits of instruction
		x = lowest 4 bits of high byte
		y = upper 4 bits of low byte
		kk = lowest 8 bits of instruction
	*/

	unsigned short addr = nextInstruction & 0x0FFF;
	unsigned char nib = nextInstruction & 0x000F;
	unsigned char x = (nextInstruction & 0x0F00) >> 8;
	unsigned char y = (nextInstruction & 0x00F0) >> 4;
	unsigned char kk = (unsigned char)nextInstruction & 0x00FF;

	unsigned short sigByte = (nextInstruction & 0xF000) >> 12;

	
	switch (op) {
	
		case CLS:
			//Clear Display?
			break;

		case RET:
			programCounter = *stackPointer;
			stackPointer -= 1;
			break;

		case JP:
			if (sigByte == 0x1) {
				programCounter = addr;
			}

			else if (sigByte == 0xB) {
				programCounter = addr + registers[0];
			}
			break;

		case CALL:
			stackPointer++;
			*stackPointer = programCounter;
			programCounter = addr;
			break;

		case SE:

			if (sigByte == 0x5) {
				if (registers[x] == registers[y]) {
					programCounter++; //Skip next instruction
				}
			}

			else if (sigByte == 0x4) {
				if (registers[x] == kk) {
					programCounter++; //Skip next instruction
				}
			}

			break;

		case SNE:
			if (sigByte == 0x4) {
				if (registers[x] != kk) {
					programCounter++; //Skip next instruction
				}
			}

			else if (sigByte == 0x9) {
				if (registers[x] != registers[y]) {
					programCounter++;
				}
			}


			break;

		case SHL:
			if ((registers[x] & 0x80) >= 0x80) {
				registers[14] = 0x01;
			}

			else {
				registers[14] = 0x00;
			}

			registers[14] *= 2;
			break;
		
		case SHR:
			if ((registers[x] & 0x01) >= 1) {
				registers[14] = 0x01;
			}

			else {
				registers[14] = 0x00;
			}

			registers[x] /= 2;

			break;

		case LD:
			if (sigByte == 0x6) {
				registers[x] = kk;
			}

			else if (sigByte == 0x8) {
				registers[x] = registers[y];
			}

			else if (sigByte == 0xA) {
				registerI = ( (registerI & 0xFF00) | addr );
			}

			break;

		case ADD:
			if (sigByte == 0x7) {
				registers[x] = registers[x] += kk;
			}

			else if (sigByte == 0x8) {
				unsigned int newVal = registers[x] + registers[y];

				if (newVal > 255) {
					registers[14] = 0x1; //Register VF
				}

				else {
					registers[14] = 0x0;
				}

				registers[x] = (unsigned char)(newVal & 0xFFFFFF00);
			}
			break;

		case SUB:
			if (sigByte == 0x8) {
				if (registers[x] >= registers[y]) {
					registers[14] = 0x01; 
				}

				else {
					registers[14] = 0x0;
;				}

				registers[x] -= registers[y];

			}
			break;

		case SUBN:
			if (registers[y] >= registers[x]) {
				registers[14] = 0x01;
			
			}

			else {
				registers[14] = 0x0;
			}

			registers[y] -= registers[x];
			break;

		case OR:
			registers[x] = registers[x] | registers[y];
			break;

		case AND:
			registers[x] = registers[x] & registers[y];
			break;

		case XOR:
			registers[x] = registers[x] ^ registers[y];

		case RND:
		{
			unsigned char randChar = (unsigned char)(rand() % 255 + 1);
			registers[x] = randChar & kk;
			break;
		}

		case DRW:
		{
			bool collision = false;
			unsigned char* sprite = new unsigned char[(int)nib];
			unsigned char startX = registers[x];
			unsigned char startY = registers[y];

			//Check if sprite will get cut off on display
			if (registers[x] + 8 >= DISPLAY_WIDTH_PX) {
				startX = 0x00;
			
			}

			if (registers[y] + (int)nib >= DISPLAY_HEIGHT_PX) {
				startY = 0x00;
				
			}

			//Grab sprite
			for (int i = 0; i < nib; i++) {
				sprite[i] = buffer[(registerI + i) - ROM_START_ADDR];
				sprite[i+1] = buffer[(registerI + (i+1)) - ROM_START_ADDR];
			}

			//Draw in lines of 8 pixels nib times
			for (int i = 0; i < nib; i++) {
				unsigned char drawLine = sprite[i];
				unsigned char mask = 0x80;
				for (int j = 0; j < 8; j++) {
					
					if ((drawLine & mask) != 0 ) {
						SDL_Rect pix;
						pix.x = ( ( startX + j ) * SCALE);
						pix.y = (( startY + i )  * SCALE);
						pix.w = 1 * SCALE;
						pix.h = 1 * SCALE;
						SDL_FillRect(sur, &pix, SDL_MapRGB(sur->format, 0, 0, 0));

						//Collision occured
						if ( (display[startX + j][startY + i] ^ (drawLine & mask)) == 0) {
							collision = true;
						}

					}

					mask /= 2;

				}
			}

			registers[14] = collision;
			SDL_UpdateWindowSurface(window);

			 
		}

		default:
			break;
		
	}

	programCounter++;
}


char Chip8::viewMemoryCell(int cell) {
	// Valid cells are only 0- (MEMORY_SIZE - 1)
	if (cell >= MEMORY_SIZE) {
		throw std::out_of_range("Invalid cell provided");
	}

	return mem[cell];
	
}

int Chip8::determineInstruction(unsigned short instruction ) {
	int instructionName;

	switch (instruction & 0xF000) {
		case 0x0000:

			if (instruction == 0x00E0) {
				instructionName = CLS;
			
			}

			else if (instruction == 0x00EE) {
				instructionName = RET;
			}

			else {
				instructionName = SYS;
			}
			break;

		case 0x1000:
			instructionName = JP;
			break;

		case 0x2000:
			instructionName = CALL;
			break;

		case 0x3000:
			instructionName = SE;
			break;

		case 0x4000:
			instructionName = SNE;
			break;

		case 0x5000:
			instructionName = SE;
			break;

		case 0x6000:
			instructionName = LD;
			break;

		case 0x7000:
			instructionName = ADD;
			break;

		case 0x8000:
			switch (instruction & 0x000F) {
				case 0:
					instructionName = LD;
					break;
				
				case 1:
					instructionName = OR;
					break;

				case 2:
					instructionName = AND;
					break;

				case 3:
					instructionName = XOR;
					break;

				case 4:
					instructionName = ADD;
					break;

				case 5:
					instructionName = SUB;
					break;

				case 6:
					instructionName = SHR;
					break;

				case 7:
					instructionName = SUBN;
					break;

				case 0x000E:
					instructionName = SHL;
					break;
					
			
			
			}


		case 0x9000:
			instructionName = SNE;
			break;

		case 0xA000:
			instructionName = LD;
			break;

		case 0xB000:
			instructionName = JP;
			break; 

		case 0xC000:
			instructionName = RND;
			break;

		case 0xD000:
			instructionName = DRW;
			break;

		case 0xE000:
			instructionName = SKP;
			break;

		case 0xF000:
			switch (instruction & 0x00FF) {
			
			case 0x0007: case 0x000A: case 0x0015: case 0x0018: case 0x0029: case 0x0033: case 0x0055: case 0x0065: 
					instructionName = LD;
					break;

				case 0x001E:
					instructionName = ADD;
					break;

							
			}

		default:
			instructionName = -1;
	}
	
	return instructionName;


}
