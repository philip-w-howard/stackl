#include "abstract_syntax_tree.h"
#include <stdexcept>
using std::runtime_error;

#include <iostream>     // for debug purposes only

abstract_syntax_tree::abstract_syntax_tree( const string& filename, uint32_t max_ip )
{
    add_ast( filename, max_ip);
}

void abstract_syntax_tree::add_ast( const string& filename, uint32_t max_ip )
{
    std::cout << "Loading symbols for " << filename << std::endl;

    rapidxml::file<char> xml_file( filename.c_str() );
    xml_document<char> doc;
    doc.parse<0>( xml_file.data() );
    load( doc, max_ip );
}

variable* abstract_syntax_tree::var( const string& func_name, const string & var_name )
{
    auto func_iter = _functions.find( func_name );
    if( func_iter == _functions.end() )
        return nullptr; //function doesnt exist
    else
    {
        variable* var = func_iter->second.var( var_name );
        if( var != nullptr )
            return var; //variable is in function
        else //variable isn't in function
        {
            auto var_iter = _globals.find( var_name );
            if( var_iter != _globals.end() )
            return &var_iter->second; //variable isnt in globals
                else return nullptr; //variable not in locals or globals
        }
    }
    return nullptr;
}

void abstract_syntax_tree::load( xml_document<char>& doc, uint32_t max_ip )
{
    const char* time_str = doc.first_node( "Program" )->first_node( "compiled" )->first_attribute( "time" )->value();
    struct tm tm;
    strptime( time_str, "%Y:%m:%d %H:%M:%S", &tm );
    _compile_time = timegm( &tm );

    for( xml_node<char>* node : doc.first_node( "Program" )->first_node( "DeclsList" )->all_nodes() )
    {
        if( strcmp( node->name(), "Typedef" ) == 0 )
        {
            xml_node<char>* type = node->first_node();
            if( strcmp( type->name(), "StructType" ) == 0 )
            {
                struct_decl decl( type, _struct_decls );

                // constructor inserts the decl to handle forward refs
                //_struct_decls[decl.name()] = decl;
            }

        }
        else if( strcmp( node->name(), "StructType" ) == 0 )
        {
            struct_decl decl( node, _struct_decls );

            // constructor inserts the decl to handle forward refs
            //_struct_decls[decl.name()] = decl;
        }
        else if( strcmp( node->name(), "VarDecl" ) == 0 )
        {
            variable var( node, _struct_decls );
            var.offset( var.offset() + max_ip );
            _globals[var.name()] = var;
        }
        else if( strcmp( node->name(), "FuncDecl" ) == 0 )
        {
            if (function::is_definition( node ))
            {
                function func( node, _struct_decls );
                _functions[func.name()] = func;
            }
        }
    }
}

string abstract_syntax_tree::all_locals( const string& func_name )
{
    string ret = "";

    auto find_res = _functions.find( func_name );

    if( find_res == _functions.end() )
        return "Couldn't find function " + func_name + ".\n";

    for( auto& var_pair : find_res->second.get_locals() )
        ret += var_pair.second.definition() + '\n';
    if( ret.empty() )
        return "No local variables in " + func_name + '\n';
    return ret;
}

string abstract_syntax_tree::all_globals()
{
    string ret = "";
    for( auto& var_pair : _globals )
        ret += var_pair.second.definition() + '\n';
    if( ret.empty() )
        return "No globals found\n";
    return ret;
}

string abstract_syntax_tree::all_funcs()
{
    string ret = "";
    for( auto& func_pair : _functions )
        ret += func_pair.first + "()\n";
    return ret;
}
