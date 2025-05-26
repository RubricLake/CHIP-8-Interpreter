/*
	File:		emulator.cpp
	By:			Ethan Kigotho (https://github.com/rubriclake)
	Date Made:	5/24/2025
*/

#include <iostream>
#include <string>
#include <stdint.h>
#include <algorithm>
#include <map>
#include <stack>
#include <stdexcept>
#include "SDL3/SDL.h"
#include "emulator.h"


Emulator::Emulator() {}

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