#include "abstract_syntax_tree.h"
#include <iostream>

abstract_syntax_tree::abstract_syntax_tree( const string& filename )
{
	rapidxml::file<char> xml_file( filename.c_str() );
	xml_document<char> doc;
	doc.parse<0>( xml_file.data() );
	load( doc );
}

variable* abstract_syntax_tree::var( const string & func_name, const string & var_name )
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
			return nullptr; //variable not in locals or globals
		}
	}
	//return nullptr; //it's literally impossible to get here. Look at the 'else' path.
	//dumb visual studio warnings.
}

void abstract_syntax_tree::load( xml_document<char>& doc )
{
	for( xml_node<char>* node : doc.first_node()->all_nodes() )
	{
		if( strcmp( node->name(), "StructType" ) == 0 )
		{
			struct_decl decl( node, _struct_decls );
			_struct_decls[decl.name()] = decl;
		}
		else if( strcmp( node->name(), "VarDecl" ) == 0 )
		{
			variable var( node, _struct_decls );
			_globals[var.name()] = var;
		}
		else if( strcmp( node->name(), "FuncDecl" ) == 0 )
		{
			function func( node, _struct_decls );
			_functions[func.name()] = func;
		}
	}
}

void abstract_syntax_tree::print_funcs()
{
	for( auto& func : _functions )
		std::cout << func.first << '\n';
}