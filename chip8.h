#include <string>

#define MEMORY_SIZE 4096
#define STACK_SIZE 16
#define SYS 

class Chip8 {
	
	private:
		unsigned char mem[MEMORY_SIZE];
		unsigned short stack[STACK_SIZE];

		unsigned char registerV0;
		unsigned char registerV1;
		unsigned char registerV2;
		unsigned char registerV3;
		unsigned char registerV4;
		unsigned char registerV5;
		unsigned char registerV6;
		unsigned char registerV7;
		unsigned char registerV8;
		unsigned char registerV9;
		unsigned char registerVA;
		unsigned char registerVB;
		unsigned char registerVC;
		unsigned char registerVD;
		unsigned char registerVE;
		unsigned char registerVF;
		unsigned short registerI;
		unsigned short programCounter;
		unsigned char stackPointer;
		//TODO Create timer and sound registers

		std::string determineInstruction(unsigned short instruction);
	
	public:
		void executeInstruction();
		


	



};