#include "function.h"

function::function( xml_node<char>* func_decl_node, unordered_map<string, struct_decl>& type_context )
{
	_name = func_decl_node->first_node( "Symbol" )->first_attribute( "name" )->value();

	xml_node<char>* decls_list = func_decl_node->first_node( "DeclsList" );  //all the arguments to the func
	if( decls_list != nullptr ) //function might not have args
	{
		for( xml_node<char>* decl : decls_list->all_nodes() )
		{
			variable var( decl, type_context );
			_locals[var.name()] = var;
		}
	}

	load_locals( func_decl_node->first_node( "StmtsList" ), type_context ); //start recursive search for local vars
}

variable* function::var( const string & var_name )
{
	auto res = _locals.find( var_name );
	if( res != _locals.end() )
		return &res->second;
	return nullptr;
}

//gotta do this recursively because nested scopes are inherently recursive
void function::load_locals( xml_node<char>* upper_node, unordered_map<string, struct_decl>& type_context )
{
	if( upper_node == nullptr )
		return;
	for( xml_node<char>* node : upper_node->all_nodes() )
	{
		if( strcmp( node->name(), "VarDecl" ) == 0 )
		{ //if it's a decl then obviously parse it

			//by default we don't have local types, obviously. (who declares structs inside a function?)
			unordered_map<string, struct_decl>* local_type_context = nullptr;
			//if we do have local types then pass in a pointer to that object
			if( _local_types.size() != 0 )
				local_type_context = &_local_types;

			variable var( node, type_context, local_type_context );
			_locals[var.name()] = var;
		}
		else if( strcmp( node->name(), "StructType" ) == 0 )
		{
			struct_decl decl( node, type_context );
			_local_types[decl.name()] = decl;
		}
		else
		{ //if it's not, then recurse down
			//the exit condition is that this node has no children, 
			//in which case the next level of recursion will exit immediately
			//and the stack will unwind
			load_locals( node, type_context ); 
		}
	}
}