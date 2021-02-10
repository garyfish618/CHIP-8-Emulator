#include "Chip8.h"
#include <string>
#include <stdexcept>
#include <iostream>
#include "WinSock2.h"


Chip8::Chip8(FILE* romPointer) {
	
	for (int i = 0; i < ROM_SIZE; i++) {
		buffer[i] = 0;

	}

	for (int i = 0; i < MEMORY_SIZE; i++) {
		mem[i] = 0;

	}

	for (int i = 0; i < STACK_SIZE; i++) {
		stack[i] = 0;

	}

	//Determine size of file
	fseek(romPointer, 0, SEEK_END);
	long end = ftell(romPointer);
	fseek(romPointer, 0, SEEK_SET);
	long start = ftell(romPointer);
	long length = end - start;
	std::cout << "LENGTH:" << length << "\n";

	if (fread(&buffer, sizeof(short), ROM_SIZE, romPointer) > ROM_SIZE) {
		std::cout << "Error reading ROM file";
	}

	//Convert to host endianess
	for (int i = 0; i < ROM_SIZE; i++) {
		buffer[i] = ntohs(buffer[i]);
	}


	/*for (int i = 0; i < ROM_SIZE; i++) {
		std::cout << std::hex << buffer[i] << "\n";
	}*/

	programCounter = 0x0200;

	registerV0 = 0;
	registerV1 = 0;
	registerV2 = 0;
	registerV3 = 0;
	registerV4 = 0;
	registerV5 = 0;
	registerV6 = 0;
	registerV7 = 0;
	registerV8 = 0;
	registerV9 = 0;
	registerVA = 0;
	registerVB = 0;
	registerVC = 0;
	registerVD = 0;
	registerVE = 0;
	registerVF = 0;
	registerI = 0;
	stackPointer = stack;
	
}


void Chip8::executeInstruction() {
	
	//Grab instruction
	unsigned short nextInstruction = buffer[ROM_START_ADDR - programCounter];
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
	unsigned char x = nextInstruction & 0x0F00;
	unsigned char y = nextInstruction & 0x00F0;
	unsigned short kk = nextInstruction & 0x00FF;

	
	switch (op) {
	
		case CLS:
			//Clear Display?
			break;

		case RET:
			programCounter = *stackPointer;
			stackPointer -= 1;

		/*case JP:
			programCounter = 
			*/

		
	
	}
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
