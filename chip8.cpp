#include "Chip8.h"
#include <string>

Chip8::Chip8() {
	programCounter = 0x0000;
	
}


void Chip8::executeInstruction() {}


std::string Chip8::determineInstruction(unsigned short instruction ) {
	std::string instructionName;

	switch (instruction & 0xF000) {
		case 0x0000:

			if (instruction == 0x00E0) {
				instructionName = "CLS";
			
			}

			else if (instruction == 0x00EE) {
				instructionName = "RET";
			}

			else {
				instructionName = "SYS";
			}
			break;

		case 0x1000:
			instructionName = "JP";
			break;

		case 0x2000:
			instructionName = "CALL";
			break;

		case 0x3000:
			instructionName = "SE";
			break;

		case 0x4000:
			instructionName = "SNE";
			break;

		case 0x5000:
			instructionName = "SE";
			break;

		case 0x6000:
			instructionName = "LD";
			break;

		case 0x7000:
			instructionName = "ADD";
			break;

		case 0x8000:
			switch (instruction & 0x000F) {
				case 0:
					instructionName = "LD";
					break;
				
				case 1:
					instructionName = "OR";
					break;

				case 2:
					instructionName = "AND";
					break;

				case 3:
					instructionName = "XOR";
					break;

				case 4:
					instructionName = "ADD";
					break;

				case 5:
					instructionName = "SUB";
					break;

				case 6:
					instructionName = "SHR";
					break;

				case 7:
					instructionName = "SUBN";
					break;

				case 0x000E:
					instructionName = "SHL";
					break;
					
			
			
			}


		case 0x9000:
			instructionName = "SNE";
			break;

		case 0xA000:
			instructionName = "LD";
			break;

		case 0xB000:
			instructionName = "JP";
			break; 

		case 0xC000:
			instructionName = "RND";
			break;

		case 0xD000:
			instructionName = "DRW";
			break;

		case 0xE000:
			instructionName = "SKP";
			break;

		case 0xF000:
			switch (instruction & 0x00FF) {
			
			case 0x0007: case 0x000A: case 0x0015: case 0x0018: case 0x0029: case 0x0033: case 0x0055: case 0x0065: 
					instructionName = "LD";
					break;

				case 0x001E:
					instructionName = "ADD";
					break;

							
			}
	}


}
