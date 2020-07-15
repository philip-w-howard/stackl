cmake_minimum_required(VERSION 3.10)
project(stdlibsl
	DESCRIPTION "standard libs for stacklc"
	LANGUAGES C
	)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(SOURCES_FILES 
	${CMAKE_CURRENT_SOURCE_DIR}/string
	${CMAKE_CURRENT_SOURCE_DIR}/sysio
	)

foreach(SLS_FILE ${SOURCES_FILES})
	set(SLS_FILE_SL ${SLS_FILE}.sl)
	get_filename_component(SLS_FILE_NAME ${SLS_FILE_SL} NAME)

	add_custom_command(
		OUTPUT ${SLS_FILE_SL}
		COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/../stacklc -c ${SLS_FILE}.c
		DEPENDS stacklc # This can make only with compiler stacklc
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
		COMMENT "Create ${SLS_FILE_SL}"
		)

	add_custom_target(${SLS_FILE_NAME} ALL DEPENDS ${SLS_FILE_SL})
endforeach()

set(INTERP_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../interp)

set(COPY_HEADER_FILES 
	pio_term.h
	timer.h
	gen_io.h
	disk.h
	inp_def.h
	machine_def.h
	)

add_custom_target(clean_lib)

foreach(HEADER_FILE ${COPY_HEADER_FILES})
	configure_file(${INTERP_DIR}/${HEADER_FILE} ${CMAKE_CURRENT_SOURCE_DIR})
	add_custom_command(
		TARGET clean_lib
		COMMAND rm -f ${CMAKE_CURRENT_SOURCE_DIR}/${HEADER_FILE}
		COMMENT "Clean ${HEADER_FILE}"
		)
endforeach()

if(TARGET clean_extra)
else()
	add_custom_target(clean_extra)
endif()

add_dependencies(clean_extra clean_lib)
