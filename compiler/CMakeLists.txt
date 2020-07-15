cmake_minimum_required(VERSION 3.10)
project(stacklc
	DESCRIPTION "stackl c compiler "
	LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(BISON)
bison_target(bison_parser stackl.y ${CMAKE_CURRENT_BINARY_DIR}/parser.cpp DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/parser.h)

find_package(FLEX)
flex_target(flex_scanner stackl.l ${CMAKE_CURRENT_BINARY_DIR}/scanner.cpp)

add_flex_bison_dependency(flex_scanner bison_parser)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(SOURCES 
	cAstXml.cpp
	cDecl.cpp
	cSemantics.cpp
	cTypeDecl.cpp
	cBinaryExpr.cpp
	cGenAddr.cpp
	cSizeOffset.cpp
	cVisitor.cpp
	cCodeGen.cpp
	cSymbolTable.cpp
	)

add_library(scanlib STATIC 
	${FLEX_flex_scanner_OUTPUTS}
	)

add_library(parserlib STATIC 
	${BISON_bison_parser_OUTPUTS}
	)

target_compile_definitions(parserlib PRIVATE YYDEBUG=1)
target_compile_options(scanlib PRIVATE -Wno-sign-compare)

add_executable(stacklc 
	stacklc.cpp
	${SOURCES}
	)

target_compile_options(parserlib PRIVATE -O0 -Wall -g -c)
target_compile_options(scanlib PRIVATE -O0 -Wall -g -c)
target_compile_options(stacklc PRIVATE -O0 -Wall -g -c)

target_link_libraries(stacklc scanlib parserlib)

set_target_properties(stacklc PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../")


