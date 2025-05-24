/*
	File:		emulator.h
	By:			Ethan Kigotho (https://github.com/rubriclake)
	Date Made:	5/24/2025
*/

#include <map>
using std::map;

class Emulator {

private:
	// Random Access Memory
	uint8_t RAM[4096];

	// Program Counter (Instruction Pointer)
	uint16_t PC;

	// General purpose registers (VF is 'carry' or the 'no borrow' flag)
	uint8_t V[16];

	// Address Register
	uint16_t I;

	// Stack Pointer
	uint16_t sp;

	// Timers
	uint8_t delayTimer;
	uint8_t soundTimer;

	// Keyboard Map
	map<SDL_Scancode, bool> keyMap;

	// Display
	bool screen[64][32];
	
	/* OPCODES DECLARATIONS*/

	// 0NNN
	void callFunc();

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
	void setRegX(uint16_t X);

	// 7XNN (Carry Flag Unchanged)
	void addRegX(uint16_t X);

	// 8XY0
	void setRegXY(uint16_t X, uint16_t Y);

	// 8XY1
	void regOr(uint16_t X, uint16_t Y);

	// 8XY2
	void regAnd(uint16_t X, uint16_t Y);

	// 8XY3
	void regXor(uint16_t X, uint16_t Y);

	// 8XY4 (Overflow flag)
	void addRegXY(uint16_t X, uint16_t Y);

	// 8XY5
	void subRegXY(uint16_t X, uint16_t Y);

	// 8XY6
	void shrRegXY(uint16_t X, uint16_t Y);

	// 8XY7 (Underflow Flag)
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
	void setXRand(uint16_t NN);

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
	void setISprite();

	// FX33
	void setIBCD();

	// FX55
	void regDump(uint16_t X);

	// FX65
	void regLoad(uint16_t X);

public:

	// Initialize System
	Emulator();

	// Execute Instruction
	void tick();
};