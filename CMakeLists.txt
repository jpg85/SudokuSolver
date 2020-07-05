cmake_minimum_required(VERSION 3.5)

set(CMAKE_CONFIGURATION_TYPES Debug Release CACHE TYPE INTERNAL FORCE )
project(SudokuSolver)

if(${CMAKE_SYSTEM_NAME} STREQUAL Windows)
    message(STATUS ": Building for Windows")
else()
    message(FATAL_ERROR ": Linux not supported")
endif()

#Set up a few basic flags and options
add_definitions(-D_CRT_SECURE_NO_WARNINGS)
add_compile_options("/Zi")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc")

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /DEBUG /OPT:REF /OPT:ICF")
set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} /DEBUG:FULL")

add_executable(SudokuSolver Board.cpp Board.h ConsoleHelper.cpp ConsoleHelper.h main.cpp)
