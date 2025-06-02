cmake_minimum_required(VERSION 3.30)

# Set project name
project(Chip8_Emulator)

# Set C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS OFF)

# Get SDL_DIR from environment
set(SDL_DIR "$ENV{SDL_DIR}")
if(NOT SDL_DIR)
    message(FATAL_ERROR "Please set the SDL_DIR environment variable to your SDL3 install path (e.g., C:/SDL)")
endif()

# Help CMake find SDL3Config.cmake inside cmake/SDL3/
list(APPEND CMAKE_PREFIX_PATH "${SDL_DIR}")

# Find SDL3 using its CMake config
find_package(SDL3 REQUIRED CONFIG)

# 6. Gather source and header files
file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "src/*.h")

if(NOT SOURCES)
    message(WARNING "No source files found in src/")
endif()

if(NOT HEADERS)
    message(WARNING "No header files found in src/")
endif()

# Create the executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Link SDL3 to your executable
target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${SDL_DIR}/bin/SDL3.dll"
        $<TARGET_FILE_DIR:${PROJECT_NAME}>
)
