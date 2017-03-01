#pragma once
#include "abstract_syntax_tree.h"
#include "asm_list.h"
#include "debugger_command.h"

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
#include "../machine.h"


class stackl_debugger
{
public:
	stackl_debugger( const char* filename );

	void debug_check( Machine_State* cpu  );
	void query_user( Machine_State* cpu );

	inline bool loaded() const { return _loaded; }
	inline string failure_reason() const { return _failure_reason; }
private:
	enum BREAKPOINT_RESULT
	{
		SUCCESS,
		NOT_FOUND,
		DUPLICATE
	};

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
	bool should_break( uint32_t cur_addr );

	/* This will get the value of the var and return it as a printable string.

	var text can be in the following formats:
	0x312312 (0Xhex_address) -> prints the value at that memory location
	variable_name -> just the name of a local/global var
	if variable_name is a pointer or array, it will print out the address that the variable holds
	if variable_name is a struct, it will recursively print out the fields of the struct, including contained structs
	if variable_name is a char pointer or array, it will print out the string at that location
	*pointer - dereferencing a pointer will print the value of the variable pointed to
	This supports multiple levels of indirection.
	array[X] will print the value of the variable at X index
	*/
	string var_to_string( Machine_State* cpu, const string& var_text );

	//'list' in gdb. maybe 2 lines above and below? obviously configurable.
	string get_nearby_lines( uint32_t cur_addr, int32_t range );

	//true if addr was added, false if it already exists
	bool add_breakpoint( uint32_t addr );
	//true if it was removed, false if it didnt exist
	bool remove_breakpoint( uint32_t addr );
	uint32_t text_to_addr( const string& break_point_text, uint32_t cur_addr );

	void check_compile_time( const char* filename );

	void load_commands();
	vector<debugger_command> _commands;

	bool cmd_breakpoint( string& params, Machine_State* cpu );
	bool cmd_removebreak( string& params, Machine_State* cpu );
	bool cmd_print( string& params, Machine_State* cpu );
	bool cmd_continue( string& params, Machine_State* cpu );
	bool cmd_list( string& params, Machine_State* cpu );
	bool cmd_next( string& params, Machine_State* cpu );
	bool cmd_locals( string& params, Machine_State* cpu );
	bool cmd_globals( string& params, Machine_State* cpu );
	bool cmd_funcs( string& params, Machine_State* cpu );
	bool cmd_exit( string& params, Machine_State* cpu );
	bool cmd_IP( string& params, Machine_State* cpu );
	bool cmd_FP( string& params, Machine_State* cpu );
	bool cmd_BP( string& params, Machine_State* cpu );
	bool cmd_LP( string& params, Machine_State* cpu );
	bool cmd_SP( string& params, Machine_State* cpu );
	bool cmd_file( string& params, Machine_State* cpu );
	bool cmd_program( string& params, Machine_State* cpu );
	bool cmd_func( string& params, Machine_State* cpu );
	bool cmd_help( string& params, Machine_State* cpu );

	string _binary_name = "";
	vector<uint32_t> _break_points;
	abstract_syntax_tree _ast;
	asm_list _lst;
	bool _loaded = false;

	string _prev_cmd = "";

	bool _stepping = false;
	uint32_t _prev_line = 0;
	string _prev_file = "";

	string _failure_reason;
};
