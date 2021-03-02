#include "Chip8.h"
#include <iostream>
#include <errno.h>

int main() {
	errno = 0;
	FILE* romFile = fopen("Tests/BRIX.c8", "rb");

	if (romFile == NULL) {
		
		std::cout << "Could not open file: " << errno;
	}
	
	Chip8 emulator(romFile);

		
	emulator.start();
	

}