#include "abstract_syntax_tree.h"
#include <stdexcept>
using std::runtime_error;
#include <sstream>
using std::stringstream;
#include "string_utils.h"

#include <iostream>
using std::cout;

abstract_syntax_tree::abstract_syntax_tree( const string& filename )
{
    add_ast( filename );
}

void abstract_syntax_tree::add_ast( const string& filename )
{
    cout << "Loading symbols for " << filename << std::endl;

    rapidxml::file<char> xml_file( filename.c_str() );
    xml_document<char> doc;
    doc.parse<0>( xml_file.data() );
    load( filename, doc );
}

void abstract_syntax_tree::fixup_globals( unordered_map<string, int32_t>& offsets )
{
    for( auto& pair : _globals )
        pair.second.offset( offsets[pair.first] );

    //pair1.first = filename the statics were declared in
    //pair1.second = map from variable name to the variable object
    for( auto& pair1: _statics )
    {
        string pre = pair1.first + "#";

        //pair2.first = variable name
        //pair2.second = variable object
        for( auto& pair2 : pair1.second )
            pair2.second.offset( offsets[pre + pair2.first] );
    }
}

variable* abstract_syntax_tree::var( const string& cur_file, const string& func_name, const string & var_name )
{
    function* func = this->func( func_name );
    if( func == nullptr ) //function doesnt exist
        return nullptr;

    variable* var = func->var( var_name );
    if( var != nullptr ) //variable is in function
        return var;

    auto static_var_iter = _statics[cur_file].find( var_name );
    if( static_var_iter != _statics[cur_file].end() )
        return &static_var_iter->second;

    auto global_var_iter = _globals.find( var_name );
    if( global_var_iter != _globals.end() )
        return &global_var_iter->second;

    return nullptr;
}

function* abstract_syntax_tree::func( const string& func_name )
{
    auto func_iter = _functions.find( func_name );
    if( func_iter != _functions.end() )
        return &func_iter->second;
    return nullptr;
}

void abstract_syntax_tree::load( const string& filename, xml_document<char>& doc )
{
    const char* time_str = doc.first_node( "Program" )->first_node( "compiled" )->first_attribute( "time" )->value();
    struct tm tm;
    strptime( time_str, "%Y:%m:%d %H:%M:%S", &tm );
    _compile_times[filename] = timegm( &tm );

    string c_filename = string_utils::change_ext( filename, ".c" );

    for( xml_node<char>* node : doc.first_node( "Program" )->first_node( "DeclsList" )->all_nodes() )
    {
        if( strcmp( node->name(), "Typedef" ) == 0 )
        {
            xml_node<char>* type = node->first_node();
            if( strcmp( type->name(), "StructType" ) == 0 )
            {
                struct_decl decl( type, _struct_decls );
            }

        }
        else if( strcmp( node->name(), "StructType" ) == 0 )
        {
            struct_decl decl( node, _struct_decls );
        }
        else if( strcmp( node->name(), "VarDecl" ) == 0 )
        {
            variable var( node, _struct_decls );
            if( var.is_static() )
                _statics[c_filename][var.name()] = var;
            else if( var.is_global() )
                _globals[var.name()] = var;
            else
                throw runtime_error( string( "Variable " ) + var.definition() + " in unexpected location." );
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
    stringstream ss;

    auto find_res = _functions.find( func_name );

    if( find_res == _functions.end() )
        return "Couldn't find function " + func_name + ".\n";

    for( auto& var_pair : find_res->second.get_locals() )
        ss << var_pair.second.definition() << '\n';

    string ret = ss.str();
    if( ret.empty() )
        return "No local variables in " + func_name + '\n';
    else return ret;
}

string abstract_syntax_tree::all_globals()
{
    stringstream ss;

    for( auto& var_pair : _globals )
        ss << var_pair.second.definition() << '\n';

    string ret = ss.str();
    if( ret.empty() )
        return "No globals found\n";
    else return ret;
}

string abstract_syntax_tree::all_statics()
{
    stringstream ss;
    for( auto& pair1 : _statics )
    {
        ss << "Statics in: " << pair1.first << '\n';
        for( auto& var_pair : pair1.second )
            ss << "    " << var_pair.second.definition() << '\n';
    }

    string ret = ss.str();
    if( ret.empty() )
        return "No static variables exist.\n";
    else return ret;
}

string abstract_syntax_tree::statics_in( const string& filename )
{
    stringstream ss;
    for( auto& var_pair : _statics[filename] )
        ss << "    " << var_pair.second.definition() << '\n';

    string ret = ss.str();
    if( ret.empty() )
        return "No static variables exist in " + filename + '\n';
    return ret;
}

string abstract_syntax_tree::all_funcs()
{
    stringstream ss;
    for( auto& func_pair : _functions )
        ss << func_pair.second.to_string() << "\n";
    return ss.str(); //if this is ever empty string I'll eat my shorts
}
