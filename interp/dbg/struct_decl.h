#pragma once

#include <vector>
using std::vector;
#include <unordered_map>
using std::unordered_map;
#include <string>
using std::string;
#include <string.h>

#include "rapidxml.hpp"
using rapidxml::xml_node;

#include "variable.h"
#include "../machine.h"

class struct_decl
{
public:
	struct_decl( xml_node<char>* decl_node, unordered_map<string, struct_decl>& live_type_context );
	struct_decl() {}

	string to_string( Machine_State* cpu, uint32_t indent_level = 0 ) const;
	inline size_t size() const { return _size; }
	inline string name() const { return _name; }
        variable* var( const string& var_name );

private:
	vector<variable> _fields;
	size_t _size = 0;
	string _name;
};

