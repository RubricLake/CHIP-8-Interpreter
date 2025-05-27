/*
	File:		emulator.cpp
	By:			Ethan Kigotho (https://github.com/rubriclake)
	Date Made:	5/24/2025
*/

#include <iostream>
#include <fstream>
#include <string>
#include <stdint.h>
#include <algorithm>
#include <map>
#include <stack>
#include <stdexcept>
#include "SDL3/SDL.h"
#include "emulator.h"

using mapEntry = std::pair<SDL_Scancode, int>;

// Initialize Emulator
Emulator::Emulator() {
	memset(RAM, 0, sizeof(RAM));
	memset(V, 0, sizeof(V));
	memset(screen, 0, sizeof(screen));
	PC = 200;
	I = 0;
	delayTimer = 0;
	soundTimer = 0;
	shiftVY = false;
	resetVF = false;

	/*
		Key Map Layout
		1 2 3 4
		q w e r
		a s d f
		z x c v
	*/
	keyMap.insert(mapEntry(SDL_SCANCODE_1, 0));
	keyMap.insert(mapEntry(SDL_SCANCODE_2, 1));
	keyMap.insert(mapEntry(SDL_SCANCODE_3, 2));
	keyMap.insert(mapEntry(SDL_SCANCODE_4, 3));
	keyMap.insert(mapEntry(SDL_SCANCODE_Q, 4));
	keyMap.insert(mapEntry(SDL_SCANCODE_W, 5));
	keyMap.insert(mapEntry(SDL_SCANCODE_E, 6));
	keyMap.insert(mapEntry(SDL_SCANCODE_R, 7));
	keyMap.insert(mapEntry(SDL_SCANCODE_A, 8));
	keyMap.insert(mapEntry(SDL_SCANCODE_S, 9));
	keyMap.insert(mapEntry(SDL_SCANCODE_D, 10));
	keyMap.insert(mapEntry(SDL_SCANCODE_F, 11));
	keyMap.insert(mapEntry(SDL_SCANCODE_Z, 12));
	keyMap.insert(mapEntry(SDL_SCANCODE_X, 13));
	keyMap.insert(mapEntry(SDL_SCANCODE_C, 14));
	keyMap.insert(mapEntry(SDL_SCANCODE_V, 15));
	
}

void Emulator::readROM(const std::string& PathToROM) {
	std::ifstream inFile(PathToROM, std::ios::binary);
	if (!inFile.is_open())
		throw std::runtime_error("Unable to open ROM. Double check the file path.");

	inFile.seekg(0, inFile.end);
	std::streamsize inFileSize = inFile.tellg();
	inFile.seekg(0, inFile.beg);

	if (inFileSize < 0)
		throw std::runtime_error("Unable to read file size.");
	else if (inFileSize + 0x200 > 4096)
		throw std::runtime_error("ROM is too large to store in RAM.");
	
	inFile.seekg(0, std::ios::beg);
	if (!inFile.read(reinterpret_cast<char*>(&RAM[0x200]), inFileSize))
		throw std::runtime_error("Unable to open ROM");
	
	inFile.close();
}

void Emulator::tick() {}

/* OPCODE DEFINITIONS */

void Emulator::callFunc(uint16_t NNN) { 
	std::cout << "INSTRUCTION IGNORED: 0NNN" << std::endl;
}

void Emulator::clearDisplay() {
	memset(screen, 0, sizeof(screen));
	
	// TODO: Clear the screen with SDL3 (Create Function to update screen)
}

void Emulator::returnFunc() {
	try {
		PC = Stack.top(); 
		Stack.pop();
	}
	catch (std::exception&) {
		std::cout << "STACK EMPTY. OPCODE 00EE (returnFunc)." << std::endl;
	}
}

void Emulator::jump(uint16_t NNN) { PC = NNN; }

void Emulator::callFuncAt(uint16_t NNN) { 
	Stack.push(PC);
	PC = NNN;
}

void Emulator::skipEq(uint16_t X, uint16_t NN) {
	if (V[X] == NN)
		PC += 2;
}

void Emulator::skipNeq(uint16_t X, uint16_t NN) {
	if (V[X] != NN)
		PC += 2;
}

void Emulator::skipRegEq(uint16_t X, uint16_t Y) {
	if (V[X] == V[Y])
		PC += 2;
}

void Emulator::setRegX(uint16_t X, uint16_t NN) { V[X] = NN; }

void Emulator::addRegX(uint16_t X, uint16_t NN) { V[X] += NN; }

void Emulator::setRegXY(uint16_t X, uint16_t Y) { V[X] = V[Y]; }

void Emulator::regOr(uint16_t X, uint16_t Y) { 
	V[X] |= V[Y];
	if (resetVF)
		V[0xF] = 0; // QUIRK 5
}
void Emulator::regAnd(uint16_t X, uint16_t Y) { 
	V[X] &= V[Y];
	if (resetVF)
		V[0xF] = 0; // QUIRK 5
}

void Emulator::regXor(uint16_t X, uint16_t Y) { 
	V[X] ^= V[Y];

	if (resetVF)
		V[0xF] = 0; // QUIRK 5
}

void Emulator::addRegXY(uint16_t X, uint16_t Y) {
	uint16_t sum = V[X] + V[Y]; // Ensure flag stays set if X = F.
	V[X] += V[Y];
	V[0xF] = (sum > UINT8_MAX) ? 1 : 0;  
}

void Emulator::subRegXY(uint16_t X, uint16_t Y) {
	V[0xF] = (V[X] >= V[Y]) ? 1 : 0;

	if (X != 0xF) // Ensure flag stays set if X = F.
		V[X] -= V[Y];
}

void Emulator::shrRegXY(uint16_t X, uint16_t Y) {
	if (shiftVY) { // Quirk 6
		V[X] = V[Y] >> 1;
		V[0xF] = V[Y] & 1u;
	}
	else {
		uint8_t bit = V[X] & 1u; // Ensure flag stays set if X = F.
		V[X] >>= 1;
		V[0xF] = bit; 
	}
}


void Emulator::subRegYX(uint16_t X, uint16_t Y) {
	V[0xF] = (V[Y] >= V[X]) ? 1 : 0;

	if (X != 0xF) // Ensure flag stays set if X = F.
		V[X] = V[Y] - V[X];
}

void Emulator::shlRegXY(uint16_t X, uint16_t Y) {
	if (shiftVY) { // Quirk 6
		V[X] = V[Y] << 1;
		V[0xF] = V[Y] & (1u << 7);
	}
	else {
		uint8_t bit = V[X] & (1u << 7); // Ensure flag stays set if X = F.
		V[X] <<= 1;
		V[0xF] = bit;
	}
}

void Emulator::skipRegNeq(uint16_t X, uint16_t Y) {
	if (V[X] != V[Y])
		PC += 2;
}

void Emulator::setI(uint16_t NNN) { I = NNN; }

void Emulator::jumpPlus(uint16_t NNN) { PC = V[0] + NNN; }

void Emulator::setXRand(uint16_t X, uint16_t NN) { V[X] = (rand() % 256) & NN; }

void Emulator::draw(uint16_t X, uint16_t Y, uint16_t N) {}
void Emulator::skipKeyEq(uint16_t X) {}
void Emulator::skipKeyNeq(uint16_t X) {}
void Emulator::setXDelay(uint16_t X) {}
void Emulator::waitForKey() {}
void Emulator::setDelayX(uint16_t X) {}
void Emulator::setSoundX(uint16_t X) {}
void Emulator::addXI(uint16_t X) {}
void Emulator::setISprite() {}
void Emulator::setIBCD() {}
void Emulator::regDump(uint16_t X) {}
void Emulator::regLoad(uint16_t X) {}