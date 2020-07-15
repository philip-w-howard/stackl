cmake_minimum_required(VERSION 3.10)
project(stackl
	DESCRIPTION "stackl project"
)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(VERSION_FILE "version.h")

add_custom_command(
	OUTPUT ${VERSION_FILE}
	PRE_BUILD
	COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/checkversion
	COMMENT "Create ${VERSION_FILE}"
	)
	
add_custom_target(version_file ALL DEPENDS ${VERSION_FILE})

add_subdirectory(compiler)
add_subdirectory(interp)
add_subdirectory(library)
add_subdirectory(optimizer)
add_subdirectory(utils)
enable_testing()
add_subdirectory(test)

# Release 
add_custom_target(release)

set(RELEASE_DIR
	${CMAKE_CURRENT_BINARY_DIR}/bin
	)

add_custom_command(
	TARGET release
	POST_BUILD
	COMMAND mkdir ${RELEASE_DIR}
	COMMAND mkdir ${RELEASE_DIR}/library
	COMMAND cp copy2disk ${RELEASE_DIR}
	COMMAND cp makedisk ${RELEASE_DIR}
	COMMAND cp slasm ${RELEASE_DIR}
	COMMAND cp stackl ${RELEASE_DIR}
	COMMAND cp stackl_norl ${RELEASE_DIR}
	COMMAND cp stacklc ${RELEASE_DIR}
	COMMAND cp library/*.h ${RELEASE_DIR}/library
	COMMAND cp library/*.sl ${RELEASE_DIR}/library
	COMMAND chmod 755 ${RELEASE_DIR}/copy2disk
	COMMAND chmod 755 ${RELEASE_DIR}/makedisk
	COMMAND chmod 755 ${RELEASE_DIR}/slasm
	COMMAND chmod 755 ${RELEASE_DIR}/stackl
	COMMAND chmod 755 ${RELEASE_DIR}/stackl_norl
	COMMAND chmod 755 ${RELEASE_DIR}/stacklc
	COMMAND chmod 644 ${RELEASE_DIR}/library/*
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	COMMENT "Copy to Release"
	)
