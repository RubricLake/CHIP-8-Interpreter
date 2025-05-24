/*
	File:		emulator.cpp
	By:			Ethan Kigotho (https://github.com/rubriclake)
	Date Made:	5/24/2025
*/

#include <stdint.h>
#include "SDL3/SDL.h"
#include "emulator.h"


Emulator::Emulator() {}

void Emulator::tick() {}

/* OPCODE DEFINITIONS */
void Emulator::callFunc(){}
void Emulator::clearDisplay(){}
void Emulator::returnFunc(){}
void Emulator::jump(uint16_t NNN){}
void Emulator::callFuncAt(uint16_t NNN){}
void Emulator::skipEq(uint16_t X, uint16_t NN){}
void Emulator::skipNeq(uint16_t X, uint16_t NN){}
void Emulator::skipRegEq(uint16_t X, uint16_t Y){}
void Emulator::setRegX(uint16_t X){}
void Emulator::addRegX(uint16_t X){}
void Emulator::setRegXY(uint16_t X, uint16_t Y){}
void Emulator::regOr(uint16_t X, uint16_t Y){}
void Emulator::regAnd(uint16_t X, uint16_t Y){}
void Emulator::regXor(uint16_t X, uint16_t Y){}
void Emulator::addRegXY(uint16_t X, uint16_t Y){}
void Emulator::subRegXY(uint16_t X, uint16_t Y){}
void Emulator::shrRegXY(uint16_t X, uint16_t Y){}
void Emulator::subRegYX(uint16_t X, uint16_t Y){}
void Emulator::shlRegXY(uint16_t X, uint16_t Y){}
void Emulator::skipRegNeq(uint16_t X, uint16_t Y){}
void Emulator::setI(uint16_t NNN){}
void Emulator::jumpPlus(uint16_t NNN){}
void Emulator::setXRand(uint16_t NN){}
void Emulator::draw(uint16_t X, uint16_t Y, uint16_t N){}
void Emulator::skipKeyEq(uint16_t X){}
void Emulator::skipKeyNeq(uint16_t X){}
void Emulator::setXDelay(uint16_t X){}
void Emulator::waitForKey(){}
void Emulator::setDelayX(uint16_t X){}
void Emulator::setSoundX(uint16_t X){}
void Emulator::addXI(uint16_t X){}
void Emulator::setISprite(){}
void Emulator::setIBCD(){}
void Emulator::regDump(uint16_t X){}
void Emulator::regLoad(uint16_t X){}