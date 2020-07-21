cmake_minimum_required(VERSION 3.10)
project(stacklopt
	DESCRIPTION "stackl optimizator"
	LANGUAGES CXX
	)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_executable(stacklopt 
	optimizations.cpp
	stacklopt.cpp
	)
target_compile_options(stacklopt PRIVATE -O0 -Wall -g -c)
set_target_properties(stacklopt PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../")

