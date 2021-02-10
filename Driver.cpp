#include "Chip8.h"
#include <iostream>

int main() {
	FILE* romFile;
	errno_t fileError = fopen_s(&romFile, "c8_test.c8", "rb");

	if (fileError != 0) {
		
		throw std::invalid_argument("Invalid filename");
	}

	Chip8 emulator(romFile);
	unsigned char test = 0;
	//Memory test
	for (int i = 0; i < MEMORY_SIZE; i++) {

		std::cout << std::dec << "Memory cell " << i << ":";
		std::cout << std::hex << (int) test << "\n";

	}

	


	
}