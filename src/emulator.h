/*
	File:		emulator.h
	By:			Ethan Kigotho (https://github.com/rubriclake)
	Date Made:	5/24/2025
*/

#ifndef EMULATOR_H
#define EMULATOR_H

#include <map>
#include <stack>
#include <string>
#include "SDL3/SDL.h"

using std::map, std::stack;

class Emulator {
public:

	// Initialize System
	Emulator();

	// Read instructions from ROM to RAM.
	// Instructions start at address 0x200
	void readROM(const std::string& PathToROM);

	// Read Instruction from RAM
	// Increment PC
	// Execute Instruction
	void tick();

	// Begin emulation
	void run();

	// Some CHIP-8 programs or interpreters do slightly
	// different things for the bitwise instructions (Resetting VF). 
	// This method turns that quirk on or off.
	// https://chip8.gulrak.net/#quirk5
	void setBitwiseQuirk(bool setting) { resetVF = setting; }

	// Some CHIP-8 programs or interpreters do slightly
	// different things for the shift instructions (using VY). 
	// This method turns that quirk on or off.
	// https://chip8.gulrak.net/#quirk6
	void setShiftQuirk(bool setting) { shiftVY = setting; }


private:
	/* SDL */
	SDL_Renderer* renderer;
	SDL_Window* window;
	SDL_Event listener;
	bool running;

	/* Quirk Toggles */
	bool shiftVY;
	bool resetVF;
	bool incrementOnlyByX;
	bool incrementNone;

	/* Adjust as needed */
	float tickSpeed = 1000.0f;

	/* Emulated Hardware */
	stack<uint16_t> Stack;
	map<SDL_Scancode, int> keyMap;
	uint16_t PC;
	uint16_t I;
	uint8_t RAM[4096];
	uint8_t V[16];
	uint8_t screen[32][64];
	uint8_t delayTimer;
	uint8_t soundTimer;


	/* Helper Functions */
	void pollEvents();
	void swapBuffers() const;
	uint16_t sprite_addr(uint8_t hex) const;

	////////////////////////////////
	/*	        OPCODES          */
	//////////////////////////////

	// 0NNN
	void callFunc(uint16_t NNN);

	// 00E0
	void clearDisplay();

	// 00EE
	void returnFunc();

	// 1NNN
	void jump(uint16_t NNN);

	// 2NNN
	void callFuncAt(uint16_t NNN);

	// 3XNN
	void skipEq(uint16_t X, uint16_t NN);

	// 4XNN
	void skipNeq(uint16_t X, uint16_t NN);

	// 5XY0
	void skipRegEq(uint16_t X, uint16_t Y);
	
	// 6XNN
	void setRegX(uint16_t X, uint16_t NN); 

	// 7XNN 
	void addRegX(uint16_t X, uint16_t NN);

	// 8XY0
	void setRegXY(uint16_t X, uint16_t Y);

	// 8XY1
	void regOr(uint16_t X, uint16_t Y);

	// 8XY2
	void regAnd(uint16_t X, uint16_t Y);

	// 8XY3
	void regXor(uint16_t X, uint16_t Y);

	// 8XY4 
	void addRegXY(uint16_t X, uint16_t Y);

	// 8XY5
	void subRegXY(uint16_t X, uint16_t Y);

	// 8XY6
	void shrRegXY(uint16_t X, uint16_t Y);

	// 8XY7 
	void subRegYX(uint16_t X, uint16_t Y);

	// 8XYE
	void shlRegXY(uint16_t X, uint16_t Y);

	// 9XY0
	void skipRegNeq(uint16_t X, uint16_t Y); 

	// ANNN
	void setI(uint16_t NNN);

	// BNNN
	void jumpPlus(uint16_t NNN);

	// CXNN
	void setXRand(uint16_t X, uint16_t NN);

	// DXYN
	void draw(uint16_t X, uint16_t Y, uint16_t N);

	// EX9E
	void skipKeyEq(uint16_t X);

	// EXA1
	void skipKeyNeq(uint16_t X);

	// FX07
	void setXDelay(uint16_t X);

	// FX0A
	void waitForKey();

	// FX15
	void setDelayX(uint16_t X);

	// FX18
	void setSoundX(uint16_t X);

	// FX1E
	void addXI(uint16_t X);

	// FX29
	void setISprite(uint16_t X);

	// FX33
	void setIBCD(uint16_t X);

	// FX55
	void regDump(uint16_t X);

	// FX65
	void regLoad(uint16_t X);
};

#endif