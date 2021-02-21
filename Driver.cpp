#include "Chip8.h"
#include <iostream>

int main() {
	FILE* romFile;
	errno_t fileError = fopen_s(&romFile, "Tests/Pong.ch8", "rb");

	if (fileError != 0) {
		
		throw std::invalid_argument("Invalid filename");
	}
	
	Chip8 emulator(romFile);

		
	emulator.start();

}