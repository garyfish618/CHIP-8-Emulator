#include "includes/Chip8.h"
#include <string>
#include <stdexcept>
#include <iostream>
#include <SDL2/SDL.h>
#include <thread>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>

#undef main

Chip8::Chip8(FILE* romPointer) {
	delayTimer = 0x00;
	soundTimer = 0x00;
	registerI = 0x0000;
	programCounter = 0x0200;
	registerI = 0;
	stackPointer = stack - 1;
	beepPlaying = false;



	//Initialize arrays
	for (int i = 0; i < ROM_SIZE; i++) {
		ram[i] = 0x00;

	}

	for (int i = 0; i < STACK_SIZE; i++) {
		stack[i] = 0;

	}

	//Add fonts
	unsigned char fontset[] = {
		0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
		0x20, 0x60, 0x20, 0x20, 0x70, // 1
		0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
		0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
		0x90, 0x90, 0xF0, 0x10, 0x10, // 4
		0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
		0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
		0xF0, 0x10, 0x20, 0x40, 0x40, // 7
		0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
		0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
		0xF0, 0x90, 0xF0, 0x90, 0x90, // A
		0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
		0xF0, 0x80, 0x80, 0x80, 0xF0, // C
		0xE0, 0x90, 0x90, 0x90, 0xE0, // D
		0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
		0xF0, 0x80, 0xF0, 0x80, 0x80  // F

	};

	//Load fontset in 0x000 - 0x050
	for (int i = 0; i < 80; i++) {
		ram[i] = fontset[i];

	}

	// Initialize SDL
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		std::cout << "Error initializing SDL" << SDL_GetError() << std::endl;
		return;
		
	}
	
	 if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 512) < 0) {
		 std::cout << "SDL Mix Error";
	 }

	 beep = Mix_LoadWAV("Sounds/beep.wav");
	 if (beep == NULL) {
		 std::cout << "Error initialzing sound";
		 return;
	 }
	 
	 //Temporary -- Set volume to super low
	 Mix_Volume(-1, 1);


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

	fclose(romPointer);

	
	for (int i = 0x200; i < 0xFFF; i++) {
		ram[i] = buffer[i - 0x200];
	}


	
}

Chip8::~Chip8() {
	Mix_FreeChunk(beep);
	
	Mix_Quit();
	SDL_Quit();

}

void Chip8::start() {
	//Grab current time in milliseconds
	std::chrono::high_resolution_clock::time_point cpuTimeBefore = std::chrono::high_resolution_clock::now();
	std::chrono::high_resolution_clock::time_point timersTimeBefore = cpuTimeBefore;
	
	Mix_PlayChannel(-1, beep, -1);
	Mix_Pause(-1);
	
	while (1) {
		SDL_Event event;
		if (SDL_PollEvent(&event)) {
			if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
				return;
			}
		}
		bool skipCycle = false;

		std::chrono::high_resolution_clock::time_point timeBefore = std::chrono::high_resolution_clock::now();

		for(int i = 0; i < CPU_RATE/FRAME_RATE; i++) {
			executeInstruction();

		}

		if (soundTimer > 0x00) {
			soundTimer--;

			if (soundTimer == 0x00) {
				toggleSound(0);
			}
		}

		if (delayTimer > 0x00) {
			delayTimer--;
		}

		std::chrono::high_resolution_clock::time_point timeNow = std::chrono::high_resolution_clock::now();

		std::chrono::duration<double, std::milli> elapsed = timeNow - timeBefore;

		if(elapsed.count() > 16.66667) {
            auto delay = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::duration<double, std::milli>(16.66667));
			std::this_thread::sleep_for(std::chrono::milliseconds(delay));

		}

		else {
			auto delay = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::duration<double, std::milli>(16.66667 - elapsed.count()));	
			std::this_thread::sleep_for(std::chrono::milliseconds(delay));

		}		
	}
}

void Chip8::toggleSound(uint16_t state) {

	if(beepPlaying && state == 0) {
		Mix_Pause(-1);
		beepPlaying = false;
	}

	if(!beepPlaying && state == 1) {
		Mix_Resume(-1);
		beepPlaying = true;
	}

}

void Chip8::executeInstruction() {
	
	//Grab instruction
	unsigned short nextInstruction = ram[programCounter] << 4;
	nextInstruction = (nextInstruction << 4) | ram[programCounter + 1];
	int op = determineInstruction(nextInstruction);

	/*
		addr = 12 bit address (Lowest 12 bits)
		nib = 4 lowest bits of instruction
		x = lowest 4 bits of high byte
		y = upper 4 bits of low byte
		kk = lowest 8 bits of instruction
	*/

	uint16_t addr = nextInstruction & 0x0FFF;
	uint8_t nib = nextInstruction & 0x000F;
	uint8_t x = (nextInstruction & 0x0F00) >> 8;
	uint8_t y = (nextInstruction & 0x00F0) >> 4;
	uint8_t kk = (uint8_t)nextInstruction & 0x00FF;

	unsigned short sigByte = (nextInstruction & 0xF000) >> 12;
	
	switch (op) {
	
		case CLS:
			disp.clearDisplay();
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
			*stackPointer = programCounter+2;
			programCounter = addr;
			break;

		case SE:

			if (sigByte == 0x5) {
				if (registers[x] == registers[y]) {
					programCounter+= 2; //Skip next instruction
				}
			}

			else if (sigByte == 0x3) {
				if (registers[x] == kk) {
					programCounter+= 2; //Skip next instruction
				}
			}

			break;

		case SNE:
			if (sigByte == 0x4) {
				if (registers[x] != kk) {
					programCounter+= 2; //Skip next instruction
				}
			}

			else if (sigByte == 0x9) {
				if (registers[x] != registers[y]) {
					programCounter+= 2;
				}
			}


			break;

		case SHL:
			if ((registers[x] & 0x80) >= 0x80) {
				registers[15] = 0x01;
				disp.updateRegister(15, 0x01);
			}

			else {
				registers[15] = 0x00;
				disp.updateRegister(15, 0x00);
			}

			registers[x] *= 2;
			disp.updateRegister(x, registers[x]);
			break;
		
		case SHR:
			if ((registers[x] & 0x01) >= 1) {
				registers[15] = 0x01;
				disp.updateRegister(15, 0x01);
			}

			else {
				registers[15] = 0x00;
				disp.updateRegister(15, 0x00);
			}

			registers[x] /= 2;
			disp.updateRegister(x, registers[x]);
			break;

		case LD:
			if (sigByte == 0x6) {
				registers[x] = kk;
				disp.updateRegister(x, registers[x]);
			}

			else if (sigByte == 0x8) {
				registers[x] = registers[y];
				disp.updateRegister(x, registers[x]);
			}

			else if (sigByte == 0xA) {
				registerI = addr & 0x0FFF;
			}

			else if (sigByte == 0xF) {
				switch (kk) {
					case 0x07:
						registers[x] = delayTimer;
						disp.updateRegister(x, registers[x]);
						break;
					

					case 0x0A:
						while (1) {

							uint8_t val = determineKeyPress();

							if (val != 0xFF) {
								registers[x] = val;
								break;
							}
						}
						break;

					case 0x15:
						delayTimer = registers[x];
						break;

					case 0x18:
						soundTimer = registers[x];
						if (soundTimer != 0x00) {
							toggleSound(1);
						}
						break;

					case 0x29:
						registerI = (0x000 + (registers[x] * 5));
						break;

					case 0x33:
						ram[registerI] = registers[x] / 100;
						ram[registerI + 1] = (registers[x]%100) / 10;
						ram[registerI + 2] = (registers[x]%10) / 1;
						break;

					case 0x55:
						for (int i = 0; i <= x; i++)
						{
							ram[registerI + i] = registers[i];
						}
						break;

					case 0x65:
						for (int i = 0; i <= x; i++) {
							registers[i] = ram[registerI + i];
						}
						break;
				}
			}

			


			break;

		case ADD:
			if (sigByte == 0x7) {
				registers[x] = registers[x] += kk;
				disp.updateRegister(x, registers[x]);
			}

			else if (sigByte == 0x8) {
				uint16_t newVal = registers[x] + registers[y];

				if (newVal > 255) {
					registers[15] = 0x1; //Register VF
					disp.updateRegister(15, 0x01);
				}

				else {
					registers[15] = 0x0;
					disp.updateRegister(15, 0x00);
				}

				registers[x] = (uint8_t)(newVal & 0x000000FF);
			}

			else if (sigByte == 0xF) {
				registerI += registers[x];
			}

			break;

		case SUB:
			if (sigByte == 0x8) {
				if (registers[x] >= registers[y]) {
					registers[15] = 0x01; 
					disp.updateRegister(15, 0x01);
				}

				else {
					registers[15] = 0x0;
					disp.updateRegister(15, 0x00);
;				}

				registers[x] -= registers[y];
				disp.updateRegister(x, registers[x]);

			}
			break;

		case SUBN:
			if (registers[y] >= registers[x]) {
				registers[15] = 0x01;
				disp.updateRegister(15, 0x01);
			
			}

			else {
				registers[15] = 0x0;
				disp.updateRegister(15, 0x00);
			}

			registers[x] = registers[y] - registers[x];
			disp.updateRegister(x, registers[x]);
			break;

		case OR:
			registers[x] = registers[x] | registers[y];
			disp.updateRegister(x, registers[x]);
			break;

		case AND:
			registers[x] = registers[x] & registers[y];
			disp.updateRegister(x, registers[x]);
			break;

		case XOR:
			registers[x] = registers[x] ^ registers[y];
			disp.updateRegister(x, registers[x]);
			break;

		case RND:
		{
			uint8_t randChar = (uint8_t)(rand() % 255 + 1);
			registers[x] = randChar & kk;
			disp.updateRegister(x, registers[x]);
			break;
		}

		case DRW:
		{
			bool collision = false;
			uint8_t* sprite = new uint8_t[(uint16_t)nib];
			uint8_t startX = registers[x];
			uint8_t startY = registers[y];

			//Check if sprite will get cut off on display
		/*	if (registers[x] + 8 >= CHIP8_DISPLAY_WIDTH_PX) {
				startX = 0x00;
			
			}

			if (registers[y] + (int)nib >= CHIP8_DISPLAY_HEIGHT_PX) {
				startY = 0x00;
				
			}*/

			//Grab sprite
			for (uint16_t i = 0; i < nib; i++) {
				sprite[i] = ram[(registerI + i)];
				sprite[i+1] = ram[(registerI + (i+1))];
			}

			//Draw in lines of 8 pixels nib times
			for (uint16_t i = 0; i < nib; i++) {
				uint8_t drawLine = sprite[i];

				uint8_t mask = 0x80;
				for (uint16_t j = 0; j < 8; j++) {
					
					if ((drawLine & mask) != 0) {
						//Collision occured
						if ((disp.getDisplay( (startX + j)%CHIP8_DISPLAY_WIDTH_PX, (startY + i)%CHIP8_DISPLAY_HEIGHT_PX ) & (drawLine & mask) ) != 0) {
							collision = true;
						}


						disp.setDisplay((startX + j) % CHIP8_DISPLAY_WIDTH_PX, (startY + i) % CHIP8_DISPLAY_HEIGHT_PX);
					}

				
				
					mask /= 2;

				}
			}

			registers[15] = collision;
			disp.updateRegister(15, registers[15]);
			disp.updateScreen();
			
			break;

			 
		}

		case SKP:	
			if (registers[x] == determineKeyPress()) {
				programCounter+= 2;
			}
			break;

		case SKNP:
			if (registers[x] != determineKeyPress()) {
				programCounter+= 2;

			}
			break;
				

	
	}

	if (op != JP && op != RET && op != CALL) {
		programCounter += 2;
	
	}
}


uint8_t Chip8::viewMemoryCell(uint16_t cell) {
	// Valid cells are only 0- (MEMORY_SIZE - 1)
	if (cell >= MEMORY_SIZE) {
		throw std::out_of_range("Invalid cell provided");
	}

	return ram[cell];
	
}

uint16_t Chip8::determineInstruction(uint16_t instruction ) {
	int instructionName = -1;

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
			break;


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
			if ((instruction & 0x00FF) == 0x009E) {
				instructionName = SKP;
			}

			else {
				instructionName = SKNP;
			}
			
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
			break;

		default:
			instructionName = -1;
			break;
	}
	
	return instructionName;


}

uint8_t Chip8::determineKeyPress() {
	SDL_Event event;
	if (SDL_PollEvent(&event)) {
		if (event.type == SDL_KEYDOWN) {
			uint8_t val;
			switch (event.key.keysym.sym) {
				case SDLK_x:
					val = 0x00;
					break;

				case SDLK_1:
					val = 0x01;
					break;

				case SDLK_2:
					val = 0x02;
					break;

				case SDLK_3:
					val = 0x03;
					break;

				case SDLK_q:
					val = 0x04;
					break;

				case SDLK_w:
					val = 0x05;
					break;

				case SDLK_e:
					val = 0x06;
					break;

				case SDLK_a:
					val = 0x07;
					break;

				case SDLK_s:
					val = 0x08;
					break;

				case SDLK_d:
					val = 0x09;
					break;

				case SDLK_z:
					val = 0x0A;
					break;

				case SDLK_c:
					val = 0x0B;
					break;

				case SDLK_4:
					val = 0x0C;
					break;

				case SDLK_r:
					val = 0x0D;
					break;

				case SDLK_f:
					val = 0x0E;
					break;

				case SDLK_v:
					val= 0x0F;
					break;

				default:
					//Not a control key
					val = 0xFF;
					break;
			}
			return val;
		}
		
	}
	return 0xFF;
}
