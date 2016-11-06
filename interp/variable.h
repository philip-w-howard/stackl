#pragma once

#include <string>
using std::string;
using std::to_string;
#include <unordered_map>
using std::unordered_map;
#include <algorithm>
using std::count;
#include <cstdlib>
using std::size_t;
#include <vector>
using std::vector;
#include <set>
using std::set;
#include <string.h>
//strcmp

#include "rapidxml.hpp"
using rapidxml::xml_node;
using rapidxml::xml_attribute;

class struct_decl;

class variable
{
public:
	variable( xml_node<char>* var_node, unordered_map<string, struct_decl>& global_type_context, unordered_map<string, struct_decl>* local_type_context = nullptr );
	variable() {}

	string to_string( uint32_t fp ) const;

	inline size_t size() const { return _size; }
	inline string name() const { return _name; }
	inline uint32_t offset() const { return _offset; }
	inline bool is_pointer() const { return _indirection != 0; }
	inline bool is_array() const { return _array_ranges.size() != 0; }
	inline bool is_struct() const { return _struct_decl != nullptr; }

private:

	void parse_base_decl( xml_node<char>* node );
	//it's only three args I promise. and it's two pointers and a ref
	//I could typedef this to shrink it but I think the full type is more useful.
	void parse_pointer_type( xml_node<char>* node, unordered_map<string, struct_decl>& global_type_context, unordered_map<string, struct_decl>* local_type_context );
	void parse_struct_type( xml_node<char>* node, unordered_map<string, struct_decl>& global_type_context, unordered_map<string, struct_decl>* local_type_context );
	void parse_array_type( xml_node<char>* node, unordered_map<string, struct_decl>& global_type_context, unordered_map<string, struct_decl>* local_type_context );
	void parse_node_type( xml_node<char>* node, unordered_map<string, struct_decl>& global_type_context, unordered_map<string, struct_decl>* local_type_context );

	string _type;
	uint32_t _offset;
	size_t _size = 0;
	string _name;

	uint8_t _indirection = 0; //0 if it's not a pointer, >0 if it is
	vector<size_t> _array_ranges; //.size() = 0 if its not an array, >0 if it is. each index is the range of that array level
	struct_decl* _struct_decl = nullptr; //nullptr if its not a struct, otherwise it points to the structs decl

	static set<string> builtins;
};

