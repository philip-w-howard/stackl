cmake_minimum_required(VERSION 3.10)
project(tests
	DESCRIPTION "stackl tests"
	)

enable_testing()

foreach(TEST_NUMBER RANGE 1 55)
	add_test(NAME test${TEST_NUMBER} 
		COMMAND ./test/runt test${TEST_NUMBER}
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../
		)
endforeach()

set(CLEAN_FILES
   "${CMAKE_CURRENT_SOURCE_DIR}/*.sl"
   "${CMAKE_CURRENT_SOURCE_DIR}/*.slb"
   "${CMAKE_CURRENT_SOURCE_DIR}/*.lst"
   "${CMAKE_CURRENT_SOURCE_DIR}/*.slb"
   "${CMAKE_CURRENT_SOURCE_DIR}/*.ast"
   "${CMAKE_CURRENT_SOURCE_DIR}/*.xml"
   "${CMAKE_CURRENT_SOURCE_DIR}/*.dbg"
   "${CMAKE_CURRENT_SOURCE_DIR}/*.slb"
   "${CMAKE_CURRENT_SOURCE_DIR}/../out"   
   )

add_custom_target(clean_test)

foreach(CLEAN_FILE ${CLEAN_FILES})
	add_custom_command(
		TARGET clean_test
		COMMAND rm -f ${CLEAN_FILE}
		COMMENT "Clean ${CLEAN_FILE}"
	)
endforeach()

if(TARGET clean_extra)
else()
	add_custom_target(clean_extra)
endif()

add_dependencies(clean_extra clean_test)
