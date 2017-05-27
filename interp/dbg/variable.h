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
#include <stdlib.h>

#include "rapidxml.hpp"
using rapidxml::xml_node;
using rapidxml::xml_attribute;
#include "../machine.h"

class struct_decl;

class variable
{
public:
    variable( xml_node<char>* var_node, unordered_map<string, struct_decl>& global_type_context, unordered_map<string, struct_decl>* local_type_context = nullptr );
    variable() {}

    string to_string( Machine_State* cpu, uint32_t indent_level = 0, bool prepend_def = false ) const;
    variable deref( uint32_t derefs, Machine_State* cpu ) const;
    variable from_indexes( vector<uint32_t>& indexes, Machine_State* cpu ) const;

    int32_t total_offset( Machine_State* cpu ) const;

    inline size_t size() const { return _size; }
    inline string name() const { return _name; }
    inline string type() const { return _type; }
    inline bool is_static() const { return _static; }
    inline bool is_global() const { return _global; }
    inline int32_t offset() const { return _offset; }
    inline void offset( int32_t offset ) { _offset = offset; }
    inline int32_t indirection() const { return _indirection; }
    inline void indirection( int32_t indirection ) { _indirection = indirection; }
    inline bool is_pointer() const { return _indirection != 0; }
    inline bool is_array() const { return _array_ranges.size() != 0; }
    inline bool is_struct() const { return _struct_decl != nullptr; }
    inline struct_decl* decl() { return _struct_decl; }
    bool is_string() const;
    string definition() const;

private:

    variable deref_ptr_from_index( vector<uint32_t>& indexes, Machine_State* cpu ) const;

    void parse_base_decl( xml_node<char>* node );
    //it's only three args I promise. and it's two pointers and a ref
    //I could typedef this to shrink it but I think the full type is more useful.
    void parse_pointer_type( xml_node<char>* node, unordered_map<string, struct_decl>& global_type_context, unordered_map<string, struct_decl>* local_type_context );
    void parse_struct_type( xml_node<char>* node, unordered_map<string, struct_decl>& global_type_context, unordered_map<string, struct_decl>* local_type_context );
    void parse_array_type( xml_node<char>* node, unordered_map<string, struct_decl>& global_type_context, unordered_map<string, struct_decl>* local_type_context );
    void parse_node_type( xml_node<char>* node, unordered_map<string, struct_decl>& global_type_context, unordered_map<string, struct_decl>* local_type_context );

    bool _global = false;
    bool _static = false;
    string _type = "";
    int32_t _offset = 0;
    size_t _size = 0;
    string _name = "";

    uint8_t _indirection = 0; //0 if it's not a pointer, >0 if it is
    vector<size_t> _array_ranges; //.size() = 0 if its not an array, >0 if it is. each index is the range of that array level
    struct_decl* _struct_decl = nullptr; //nullptr if its not a struct, otherwise it points to the structs decl

    static unordered_map<string, size_t> builtins;
};
