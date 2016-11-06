#pragma once
#include "abstract_syntax_tree.h"
#include "asm_list.h"

#include <string>
using std::string;
using std::stoi;
using std::to_string;
#include <vector>
using std::vector;
#include <algorithm>
using std::find;
#include <fstream>
using std::ifstream;
#include <iostream>
using std::cout;
using std::cin;

#include"string_utils.h"

//note: currently every function that needs the current state of the program requires the current instruction address.
//maybe I could just have a field that we set once after every instruction is executed?
//would cut down on duplicate args, internally would be identical. I dunno.
class stackl_debugger
{
public:

	enum BREAKPOINT_RESULT
	{
		SUCCESS,
		NOT_FOUND,
		DUPLICATE
	};


	stackl_debugger( const string& file_no_ext );

	/*	Breakpoint text can be in the following formats:
			main.cpp:75 (filename:line_number)
			75	(line_number) -> this assumes current file
			main (function_name) -> no function overloading so this is all there is to it
			0x312312 (0xhex_address) -> breakpoint on an address
	*/
	BREAKPOINT_RESULT add_breakpoint( const string& break_point_text, uint32_t cur_addr );
	BREAKPOINT_RESULT remove_breakpoint( const string& break_point_text, uint32_t cur_addr );

	inline void remove_all_breakpoints() { _break_points.clear(); }

	//is the current address in our list of break points?
	inline bool should_break( uint32_t cur_addr ) const { return find( _break_points.begin(), _break_points.end(), cur_addr ) != _break_points.end(); }
	
	/* This will get the value of the var and return it as a printable string.
		Structuring it this way FOR NOW because I'm not sure how we'll do IO yet.

		var text can be in the following formats:
			0x312312 (0Xhex_address) -> prints the value at that memory location
			variable_name -> just the name of a local/global var
				if variable_name is a pointer or array, it will print out the address that the variable holds
				if variable_name is a struct, it will recursively print out the fields of the struct, including contained structs
				if variable_name is a char pointer, it will print out the string at that location
			*pointer -> dereferencing a pointer will print the value of the variable pointed to
				note: this should inherently support multiple levels of indirection if written correctly. I think.
			array[X] will print the value of the variable at X index
			MAYBE: array[X:Y] will print the value of all variables between X and Y inclusive
			MAYBE MAYBE: array [:X] will print all variables up until X inclusive
			MAYBE MAYBE: array [X:] will print all variables after X inclusive
	*/
	string var_to_string( uint32_t cur_addr, const string& var_text, uint32_t frame_pointer );

	void debug_check( uint32_t instruction_pointer, uint32_t frame_pointer );
	void query_user( uint32_t instruction_pointer, uint32_t frame_pointer );

	//'list' in gdb. maybe 2 lines above and below? obviously configurable.
	string get_nearby_lines( uint32_t cur_addr );


private:

	bool add_breakpoint( uint32_t addr );
	bool remove_breakpoint( uint32_t addr );
	uint32_t text_to_addr( const string& break_point_text, uint32_t cur_addr );

	vector<uint32_t> _break_points;
	abstract_syntax_tree _ast;
	asm_list _lst;
};
