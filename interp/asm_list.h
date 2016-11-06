#pragma once

#include <string>
using std::string;
using std::getline;
using std::stoul;
#include <fstream>
using std::ifstream;
#include <vector>
using std::vector;
#include <unordered_map>
using std::unordered_map;

#include "string_utils.h"

class asm_list
{
public:
	asm_list( const string& filename );

	uint32_t addr_of_func( const string& func );
	uint32_t addr_of_line( const string& filename, uint32_t line_number );
	uint32_t line_of_addr( const string& filename, uint32_t line_number );

	string current_func( uint32_t cur_addr );
	string current_file( uint32_t cur_addr );

private:

	typedef struct
	{
		uint32_t addr;
		string func_name;
	} func_addr_t;

	typedef struct
	{
		uint32_t addr;
		uint32_t line_num;
	} addr_line_t;

	vector<func_addr_t> _func_to_addr;
	unordered_map<string, vector<addr_line_t>> _file_and_line_to_addr;
};

