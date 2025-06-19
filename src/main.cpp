/*
	File:		main.cpp
	By:			Ethan Kigotho (https://github.com/rubriclake)
	Date Made:	5/23/2025

	Project Description:
		CHIP-8 Interpreter / Emulator.
		Links used for reference:
		https://en.wikipedia.org/wiki/CHIP-8
		https://chip8.gulrak.net/  <--- Quirks
		https://www.laurencescotford.net/2020/07/19/chip-8-on-the-cosmac-vip-drawing-sprites/ <--- DXYN Help
		https://github.com/Timendus/chip8-test-suite <--- Test ROMs
*/


#include <iostream>
#include <string>
#include "SDL3/SDL.h"
#include "emulator.h"

using std::string;

const string ROM_DIR = "../ROM/";
const string TEST_DIR = "../test/";

int main() {
	Emulator emu;
	emu.readROM("Path-To-Rom-Here");
	emu.setShiftQuirk(true);
	emu.setBitwiseQuirk(true);
	emu.setDrawOnCall(true);
	emu.run();
	return 0;
}