# CHIP-8 Interpreter (Windows)
**By: Ethan Kigotho**

--
## Background
I've been getting increasingly interested in the world of emulation. I read online that a CHIP-8 interpreter would be a good starting point, so this is my take on it.  
The project is written in **C++** and uses **SDL3** for graphics output.



## Installation Prerequisites
- [CMake](https://cmake.org/download/) **3.30** or higher
- A C++17-compatible compiler (e.g., MSVC, Clang, or GCC)



## How to Build

From the root directory of the project, run:

```bash
mkdir build
cd build
cmake ..
cmake --build .
```


## How to Use
In **Main.cpp**, edit the PathToROM variable to be the path to whatever ROM you'd like to run the interpreter on.

A "Test-Suite" collection of ROMs by Timendus can be found [here](https://github.com/Timendus/chip8-test-suite).
A bunch of games and demos that were aggregated by kripod can be found [here](https://github.com/kripod/chip8-roms).


## Additional Notes
Feel free to alter the code before building if you'd like to enable/disable any of the available quirks.