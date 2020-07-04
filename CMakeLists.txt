# Copyright 2018 Intel Corporation.  All Rights Reserved.
# 
# The source code contained or described herein and all documents related
# to the source code ("Material") are owned by Intel Corporation or its
# suppliers or licensors.  Title to the Material remains with Intel
# Corporation or its suppliers and licensors.  The Material is protected
# by worldwide copyright laws and treaty provisions.  No part of the
# Material may be used, copied, reproduced, modified, published, uploaded,
# posted, transmitted, distributed, or disclosed in any way without
# Intel's prior express written permission.
# 
# No license under any patent, copyright, trade secret or other
# intellectual property right is granted to or conferred upon you by
# disclosure or delivery of the Materials, either expressly, by
# implication, inducement, estoppel or otherwise.  Any license under such
# intellectual property rights must be express and approved by Intel in
# writing.

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
