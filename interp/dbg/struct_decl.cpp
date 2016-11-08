#include "struct_decl.h"


struct_decl::struct_decl( xml_node<char>* struct_node, unordered_map<string, struct_decl>& live_type_context ): _size( 0 )
{
	//get the first symbol and read its 'name' attribute
	_name = struct_node->first_node( "Symbol" )->first_attribute( "name" )->value();
	
	//get every var decl in the decls list
	for( xml_node<char>* xml_var : struct_node->first_node( "DeclsList" )->all_nodes() )
	{
		variable var( xml_var, live_type_context );

		_size += var.size();

		_fields.push_back( var );
	}
}

string struct_decl::to_string( Machine_State* cpu ) const
{
	string ret = "";
	for( auto& var : _fields )
		ret += "\t" + var.to_string( cpu ) + '\n';
        ret += "}";
	return ret;
}
