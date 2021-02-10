#include <string>

#define MEMORY_SIZE 4096
#define ROM_SIZE 2559
#define STACK_SIZE 16
#define ROM_START_ADDR 0x200

enum instructionType {CLS, RET, SYS, JP, CALL, SE, SNE, LD, ADD, OR, AND, XOR, ADD, SUB, SHR, SUBN, SHL, RND, DRW, SKP};

class Chip8 {
	
	private:
		short buffer[ROM_SIZE];

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
		unsigned short* stackPointer;
		//TODO Create timer and sound registers

		int determineInstruction(unsigned short instruction);
	
	public:
		void executeInstruction();
		char viewMemoryCell(int cell);
		Chip8(FILE*);
		


	



};