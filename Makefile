OBJS = Driver.cpp chip8.cpp Display.cpp

OBJ_NAME = Driver

all: $(OBJS)
	g++ $(OBJS) -g -I "C:\Users\fishe\source\repos\CHIP-8 Emulator\CHIP-8 Emulator" -I "C:\Users\fishe\source\repos\CHIP-8 Emulator\CHIP-8 Emulator\include" -L "C:\Users\fishe\source\repos\CHIP-8 Emulator\CHIP-8 Emulator\lib" -lSDL2 -lSDL2_mixer -lSDL2_ttf -lmingw32