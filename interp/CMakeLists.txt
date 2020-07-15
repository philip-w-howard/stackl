cmake_minimum_required(VERSION 3.10)
project(interp
	DESCRIPTION "stackl interpreter"
	LANGUAGES CXX C
	)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/dbg)

set(SOURCES 
	disk.c
	formatstr.c
	io_handler.c
	machine.c
	pio_term.c
	vmem.c
	dma_term.c
	gen_io.c
	loader.c
	opcodes.c
	timer.c
	
	dbg/abstract_syntax_tree.cpp
	dbg/function.cpp
	dbg/struct_decl.cpp
	dbg/asm_list.cpp
	dbg/safe_read.cpp
	dbg/variable.cpp
	dbg/debugger_command.cpp
	dbg/stackl_debugger.cpp
	dbg/debugger_interface.cpp
	dbg/string_utils.cpp
	)

add_library(stackl_lib STATIC 
	${SOURCES}
	)

add_library(stackl_debugger_lib STATIC 
	dbg/stackl_debugger.cpp
	)

# Start Readline find package
# Search for the path containing library's headers Readline
find_path(Readline_ROOT_DIR
	NAMES include/readline/readline.h
	)

# Search for include directory Readline
find_path(Readline_INCLUDE_DIR
	NAMES readline/readline.h
	HINTS ${Readline_ROOT_DIR}/include
	)

# Search for library Readline
find_library(Readline_LIBRARY
	NAMES readline
	HINTS ${Readline_ROOT_DIR}/lib
	)

# Conditionally set READLINE_FOUND value
if(Readline_INCLUDE_DIR AND Readline_LIBRARY 
	AND Ncurses_LIBRARY)
	set(READLINE_FOUND TRUE)
else(Readline_INCLUDE_DIR AND Readline_LIBRARY 
	AND Ncurses_LIBRARY)
	FIND_LIBRARY(Readline_LIBRARY NAMES readline)
	include(FindPackageHandleStandardArgs)
	FIND_PACKAGE_HANDLE_STANDARD_ARGS(Readline DEFAULT_MSG 
	Readline_INCLUDE_DIR Readline_LIBRARY )
	MARK_AS_ADVANCED(Readline_INCLUDE_DIR Readline_LIBRARY)
endif(Readline_INCLUDE_DIR AND Readline_LIBRARY 
	AND Ncurses_LIBRARY)

# Hide these variables in cmake GUIs
mark_as_advanced(
	Readline_ROOT_DIR
	Readline_INCLUDE_DIR
	Readline_LIBRARY
	)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} 
	${CMAKE_CURRENT_SOURCE_DIR}/cmake
	)

# in readline is installed, it automatically added
find_package(Readline)
if (READLINE_FOUND)
	message("Readline found")
	target_compile_definitions(stackl_debugger_lib PRIVATE LREADLINE)
else (READLINE_FOUND)
	message("Readline not found")
endif (READLINE_FOUND)

add_library(stackl_debugger_norl_lib STATIC 
	dbg/stackl_debugger.cpp
	)

# slasm
add_executable(slasm 
	slasm.c
    formatstr.c
	)
add_dependencies(slasm version_file)
target_compile_options(slasm PRIVATE -O0 -g )
set_target_properties(slasm PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../")

set(OPCODE_DEF_FILE "opcode_defs.h")

add_custom_command(
	OUTPUT ${OPCODE_DEF_FILE}
	PRE_BUILD
	COMMAND ${CMAKE_SOURCE_DIR}/slasm -defs
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	DEPENDS slasm
	COMMENT "Create ${OPCODE_DEF_FILE}"
	)

add_custom_target(opcode_defs DEPENDS ${OPCODE_DEF_FILE})
add_dependencies(stackl_lib opcode_defs)

# stackl
add_executable(stackl 
	stackl.c
	)
set_target_properties(stackl PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../")

# stackl_norl - stackl without Readline
add_executable(stackl_norl
	stackl.c
	)
set_target_properties(stackl_norl PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../")


target_link_libraries(stackl stackl_lib pthread stdc++ m)

if (READLINE_FOUND)
	target_link_libraries(stackl readline)
endif (READLINE_FOUND)

target_link_libraries(stackl_norl stackl_lib pthread stdc++ m)

target_compile_options(stackl_norl PRIVATE -O0 -Wall -g -c)
target_compile_options(stackl PRIVATE -O0 -Wall -g -c)

