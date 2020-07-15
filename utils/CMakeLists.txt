cmake_minimum_required(VERSION 3.10)
project(stackl_utils
	DESCRIPTION "stackl utils"
	LANGUAGES C
)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

#include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_executable(copy2disk 
	copy2disk.c
	)
set_target_properties(copy2disk PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../")
	
add_executable(makedisk 
	makedisk.c
	)
set_target_properties(makedisk PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../")

target_compile_options(makedisk PRIVATE -O0 -Wall -g -c)
target_compile_options(copy2disk PRIVATE -O0 -Wall -g -c)
