/*
	File:		main.cpp
	By:			Ethan Kigotho (https://github.com/rubriclake)
	Date Made:	5/23/2025

	Project Description:
		CHIP-8 Interpreter / Emulator.
		Follows the wikipedia page on the subject.
		https://en.wikipedia.org/wiki/CHIP-8
		https://chip8.gulrak.net/  <--- Quirks
*/


#include <iostream>
#include <string>
#include "SDL3/SDL.h"
#include "emulator.h"

using std::string;

const string ROM_DIR = "../ROM/";

int main() {
	Emulator emu;
	emu.readROM(ROM_DIR + "4-flags.ch8");
	emu.run();
	return 0;
}