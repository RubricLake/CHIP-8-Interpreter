/*
	File:		emulator.cpp
	By:			Ethan Kigotho (https://github.com/rubriclake)
	Date Made:	5/24/2025
*/

#include <iostream>
#include <cstdlib>
#include <fstream>
#include <string>
#include <stdint.h>
#include <map>
#include <stack>
#include <stdexcept>
#include <chrono>
#include "SDL3/SDL.h"
#include "emulator.h"


uint8_t fontData[80] =
{ 0xF0, 0x90, 0x90, 0x90, 0xF0,  // 0
 0x20, 0x60, 0x20, 0x20, 0x70,   // 1
 0xF0, 0x10, 0xF0, 0x80, 0xF0,   // 2
 0xF0, 0x10, 0xF0, 0x10, 0xF0,   // 3
 0x90, 0x90, 0xF0, 0x10, 0x10,   // 4
 0xF0, 0x80, 0xF0, 0x10, 0xF0,   // 5
 0xF0, 0x80, 0xF0, 0x90, 0xF0,   // 6
 0xF0, 0x10, 0x20, 0x40, 0x40,   // 7
 0xF0, 0x90, 0xF0, 0x90, 0xF0,   // 8
 0xF0, 0x90, 0xF0, 0x10, 0xF0,   // 9
 0xF0, 0x90, 0xF0, 0x90, 0x90,   // A
 0xE0, 0x90, 0xE0, 0x90, 0xE0,   // B
 0xF0, 0x80, 0x80, 0x80, 0xF0,   // C
 0xE0, 0x90, 0x90, 0x90, 0xE0,   // D
 0xF0, 0x80, 0xF0, 0x80, 0xF0,   // E
 0xF0, 0x80, 0xF0, 0x80, 0x80 }; // F

// Initialize Emulator and SDL
Emulator::Emulator() {
	// Init SDL
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
	window = SDL_CreateWindow("CHIP8-8", C8_WIDTH * 10, C8_HEIGHT * 10, 0);
	renderer = SDL_CreateRenderer(window, NULL);
	SDL_SetRenderLogicalPresentation(renderer, C8_WIDTH, C8_HEIGHT, SDL_LOGICAL_PRESENTATION_INTEGER_SCALE);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	running = false;
	listener = SDL_Event();

	// Emulator Values
	memset(RAM, 0, sizeof(RAM));
	memset(V, 0, sizeof(V));
	memset(screen, 0, sizeof(screen));
	lastFrame = {};
	lastTick = {};
	PC = 200;
	I = 0;
	delayTimer = 0;
	soundTimer = 0;
	shiftVY = false;
	resetVF = false;
	incrementOnlyByX = false;
	incrementNone = false;

	/*
		Key Map Layout
		1 2 3 4
		q w e r
		a s d f
		z x c v
	*/
	keyMap.insert(mapEntry(SDL_SCANCODE_1, keyInfo(0)));
	keyMap.insert(mapEntry(SDL_SCANCODE_2, keyInfo(1)));
	keyMap.insert(mapEntry(SDL_SCANCODE_3, keyInfo(2)));
	keyMap.insert(mapEntry(SDL_SCANCODE_4, keyInfo(3)));
	keyMap.insert(mapEntry(SDL_SCANCODE_Q, keyInfo(4)));
	keyMap.insert(mapEntry(SDL_SCANCODE_W, keyInfo(5)));
	keyMap.insert(mapEntry(SDL_SCANCODE_E, keyInfo(6)));
	keyMap.insert(mapEntry(SDL_SCANCODE_R, keyInfo(7)));
	keyMap.insert(mapEntry(SDL_SCANCODE_A, keyInfo(8)));
	keyMap.insert(mapEntry(SDL_SCANCODE_S, keyInfo(9)));
	keyMap.insert(mapEntry(SDL_SCANCODE_D, keyInfo(10)));
	keyMap.insert(mapEntry(SDL_SCANCODE_F, keyInfo(11)));
	keyMap.insert(mapEntry(SDL_SCANCODE_Z, keyInfo(12)));
	keyMap.insert(mapEntry(SDL_SCANCODE_X, keyInfo(13)));
	keyMap.insert(mapEntry(SDL_SCANCODE_C, keyInfo(14)));
	keyMap.insert(mapEntry(SDL_SCANCODE_V, keyInfo(15)));
}

// Load given file and predefined font into RAM.
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
	memcpy(&RAM[0], fontData, sizeof(fontData)); 
}

void Emulator::tick() {
	// Handle Time
	auto now = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::milli> tickDiff = now - lastTick;
	std::chrono::duration<double, std::milli> frameDiff = now - lastFrame;

	double tickDelta = tickDiff.count();
	double frameDelta = frameDiff.count();

	if (tickDelta < TICK_SPEED_MS)
		return;

	// Update Timers
	if (frameDelta >= SIXTY_HZ_MS) {
		if (delayTimer > 0) delayTimer--;
		if (soundTimer > 0) soundTimer--;
		swapBuffers();
		lastFrame = hires_clock::now();
	}

	if (waitingForKey) {
		lastTick = hires_clock::now();
		return;
	}

	// OPCODE Decision Tree
	try {
		execute();
	}
	catch (const std::runtime_error& e) {
		uint16_t code = std::stoi(e.what());
		std::string userInput;
		std::cout << "Unknown instruction: " << std::hex << code << std::dec << "\n";
		std::cout << "Skip instruction and continue? Y or N.\n" << "Input: ";
		std::cin >> userInput;
		if (userInput.at(0) != 'Y' || userInput.at(0) != 'y')
			running = false;
			
	}
	
	lastTick = std::chrono::high_resolution_clock::now();
}

void Emulator::execute() {
	// Useful parts of instruction
	uint8_t leftByte = RAM[PC];
	uint8_t rightByte = RAM[PC + 1];
	uint16_t bothByte = ((uint16_t) leftByte << 8) | rightByte;
	uint8_t firstNib = leftByte >> 4; // Leftmost
	uint8_t secondNib = leftByte & 0xf;
	uint8_t thirdNib = rightByte >> 4;
	uint8_t fourthNib = rightByte & 0xf; // Rightmost
	PC += 2;

	switch (firstNib)
	{
	case 0:
		if (leftByte == 0x00 && rightByte == 0xEE)
			returnFunc();
		else if (leftByte == 0x00 && rightByte == 0xE0)
			clearDisplay();
		else
			callFunc(0xCAFE); // Instruction Ignored
		break;
	case 1:
		jump(bothByte & 0x0FFF);
		break;
	case 2:
		callFuncAt(bothByte & 0x0FFF);
		break;
	case 3:
		skipEq(secondNib, rightByte);
		break;
	case 4:
		skipNeq(secondNib, rightByte);
		break;
	case 5:
		skipRegEq(secondNib, thirdNib);
		break;
	case 6:
		setRegX(secondNib, rightByte);
		break;
	case 7:
		addRegX(secondNib, rightByte);
		break;
	case 8:
		switch (fourthNib) {
		case 0x0:
			setRegXY(secondNib, thirdNib);
			break;
		case 0x1:
			regOr(secondNib, thirdNib);
			break;
		case 0x2:
			regAnd(secondNib, thirdNib);
			break;
		case 0x3:
			regXor(secondNib, thirdNib);
			break;
		case 0x4:
			addRegXY(secondNib, thirdNib);
			break;
		case 0x5:
			subRegXY(secondNib, thirdNib);
			break;
		case 0x6:
			shrRegXY(secondNib, thirdNib);
			break;
		case 0x7:
			subRegYX(secondNib, thirdNib);
			break;
		case 0xE:
			shlRegXY(secondNib, thirdNib);
			break;
		default:
			throw std::runtime_error(std::to_string(bothByte));
			break;
		}
		break;
	case 9:
		if (fourthNib == 0x0)
			skipRegNeq(secondNib, thirdNib);
		else
			throw std::runtime_error(std::to_string(bothByte));
		break;
	case 0xA:
		setI(bothByte & 0x0FFF);
		break;
	case 0xB:
		jumpPlus(bothByte & 0x0FFF);
		break;
	case 0xC:
		setXRand(secondNib, rightByte);
		break;
	case 0xD:
		draw(secondNib, thirdNib, fourthNib);
		break;
	case 0xE:
		if (rightByte == 0x9E)
			skipKeyEq(secondNib);
		else if (rightByte == 0xA1)
			skipKeyNeq(secondNib);
		else
			throw std::runtime_error(std::to_string(bothByte));
		break;
	case 0xF:
		switch (rightByte)
		{
		case 0x07:
			setXDelay(secondNib);
			break;
		case 0x0A:
			waitForKey(secondNib);
			break;
		case 0x15:
			setDelayX(secondNib);
			break;
		case 0x18:
			setSoundX(secondNib);
			break;
		case 0x1E:
			addXI(secondNib);
			break;
		case 0x29:
			setISprite(secondNib);
			break;
		case 0x33:
			setIBCD(secondNib);
			break;
		case 0x55:
			regDump(secondNib);
			break;
		case 0x65:
			regLoad(secondNib);
			break;
		default:
			throw std::runtime_error(std::to_string(bothByte));
			break;
		}
		break;
	default:
		throw std::runtime_error(std::to_string(bothByte));
		break;
	}

}
void Emulator::run() {
	running = true;
	while (running) { 
		pollEvents();
		tick();
	}
	std::cout << "Emulator shutting down..." << std::endl;
}

// Handles all input
void Emulator::pollEvents() {
	while (SDL_PollEvent(&listener)) {
		SDL_Scancode scancode;
		switch (listener.type) {
			case SDL_EVENT_QUIT:
				running = false;
				break;
			case SDL_EVENT_KEY_UP:
				scancode = listener.key.scancode;
				if (isValidKey(scancode))
					keyMap.at(scancode).down = false;
				break;
			case SDL_EVENT_KEY_DOWN:
				scancode = listener.key.scancode;
				if (isValidKey(scancode)) {
					keyMap.at(scancode).down = true;
					if (waitingForKey) { // FX0A Functionality 
						waitingForKey = false;
						V[waitingRegister] = keyMap.at(scancode).mappedNum;
					}
				}
				break;
		}
	}
}

// Draw the screen buffer to the screen
// Sets draw color to black.
void Emulator::swapBuffers() const {
	for (int i = 0; i < C8_HEIGHT; i++) {
		for (int j = 0; j < C8_WIDTH; j++) {
			uint8_t color = (screen[i][j] == 1) ? 255 : 0;
			const SDL_FRect pixel = { j, i, 1.0f, 1.0f };
			SDL_SetRenderDrawColor(renderer, color, color, color, 255);
			SDL_RenderFillRect(renderer, &pixel);
		}
	}
	SDL_RenderPresent(renderer);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_Delay(1);
}

bool Emulator::isValidKey(SDL_Scancode& key) const {
	return keyMap.find(key) != keyMap.end();
}

// Returns the bit in 'place' of 'number'
// The least significant bit is 'place' 0.
uint8_t Emulator::getBit(uint8_t number, uint8_t place) const {
	return (number >> place) & 1;
}

uint16_t Emulator::sprite_addr(uint8_t hex) const {
	return hex * 5;
}
////////////////////////////////
/*	        OPCODES          */
//////////////////////////////

void Emulator::callFunc(uint16_t NNN) { 
	std::cout << "INSTRUCTION IGNORED: 0NNN" << std::endl;
}


void Emulator::clearDisplay() {
	memset(screen, 0, sizeof(screen));
	swapBuffers();
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

void Emulator::draw(uint16_t X, uint16_t Y, uint16_t N) {
	uint8_t xOrig = V[X];
	uint8_t yOrig = V[Y];
	V[0xf] = 0;
	for (int i = 0; i < N; i++) {
		uint8_t row = RAM[I + i];
		for (int j = 0; j < 8; j++) {
			uint8_t bit = getBit(row, 7 - j);
			uint8_t& screenBit = screen[(yOrig + i) % C8_HEIGHT][(xOrig + j) % C8_WIDTH];
			if (screenBit == 1 && bit == 1) // Flip Check
				V[0xf] = 1;
			screenBit ^= bit;
		}
	}
}

void Emulator::skipKeyEq(uint16_t X) {
	int keyNum = V[X];
	for (const auto& [scancode, info] : keyMap) {
		if (info.mappedNum == keyNum && info.down) {
			PC += 2;
			break;
		}
	}
}

void Emulator::skipKeyNeq(uint16_t X) {
	int keyNum = V[X];
	for (const auto& [scancode, info] : keyMap) {
		if (info.mappedNum == keyNum && !info.down) {
			PC += 2;
			break;
		}
	}
}

void Emulator::setXDelay(uint16_t X) { V[X] = delayTimer; }

void Emulator::waitForKey(uint16_t X) {
	waitingForKey = true;
	waitingRegister = X;

	//SDL_Event e;
	//
	//while (true) {
	//	while (SDL_PollEvent(&e)) {
	//		if (e.type == SDL_EVENT_QUIT) {
	//			std::exit(0);
	//		}
	//
	//		if (e.type == SDL_EVENT_KEY_DOWN)
	//		{
	//			auto search = keyMap.find(e.key.scancode);
	//			if (search != keyMap.end()) {
	//				keyInfo& val = search->second;
	//				val.down = true;
	//				V[X] = val.mappedNum;
	//				return;
	//			}
	//		}
	//		SDL_Delay(1);
	//	}
	//}
}

void Emulator::setDelayX(uint16_t X) { delayTimer = V[X]; }

void Emulator::setSoundX(uint16_t X) { soundTimer = V[X]; }

void Emulator::addXI(uint16_t X) { I += V[X]; }

void Emulator::setISprite(uint16_t X) { I = sprite_addr(V[X]); }

void Emulator::setIBCD(uint16_t X) {
	uint8_t num = V[X];
	RAM[I] = num / 100;
	RAM[I + 1] = (num / 10) % 10;
	RAM[I + 2] = num % 10;	
}

void Emulator::regDump(uint16_t X) {
	for (uint8_t i = 0; i <= X; i++) {
		RAM[I + i] = V[i];
	}
	
	if (incrementOnlyByX) // Quirk 12
		I += X;
	else if (incrementNone) // Quirk 12
		;
	else
		I += X + 1;
}

void Emulator::regLoad(uint16_t X) {
	for (uint8_t i = 0; i <= X; i++) {
		V[i] = RAM[I + i];
	}

	if (incrementOnlyByX) // Quirk 12
		I += X;
	else if (incrementNone) // Quirk 12
		;
	else
		I += X + 1;
}
