#pragma once

#include <string>
using std::string;
#include <unordered_map>
using std::unordered_map;

#include "rapidxml.hpp"
using rapidxml::xml_node;
using rapidxml::xml_attribute;

#include "variable.h"
#include "struct_decl.h"

class function
{
public:
    function( xml_node<char>* func_decl_node, unordered_map<string, struct_decl>& type_context );
    function() {}

    inline string name() const { return _name; }
    inline unordered_map<string, variable>& locals() { return _locals; }

    variable* var( const string& var_name ); //returns nullptr if var not found
    static bool is_definition( xml_node<char>* func_decl_node);
    string to_string() const;

private:

    void load_locals( xml_node<char>* func_decl_node, unordered_map<string, struct_decl>& type_context );

    string _name;
    string _return_type;
    unordered_map<string, variable> _args; //key = var_name, value = variable
    unordered_map<string, variable> _locals; //key = var_name, value = variable
    unordered_map<string, struct_decl> _local_types; //key = type_name, value = type_decl
};

