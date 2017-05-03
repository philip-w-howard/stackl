#include "struct_decl.h"


struct_decl::struct_decl( xml_node<char>* struct_node, unordered_map<string, struct_decl>& live_type_context ): _size( 0 )
{
    //get the first symbol and read its 'name' attribute
    _name = struct_node->first_node( "Symbol" )->first_attribute( "name" )->value();

        live_type_context[ _name ] = *this;

    //get every var decl in the decls list
    for( xml_node<char>* xml_var : struct_node->first_node( "DeclsList" )->all_nodes() )
    {
        variable var( xml_var, live_type_context );

        // need to update ourself and the struct_decl in the context
        _size += var.size();
        live_type_context[ _name ]._size += var.size();
        _fields.push_back( var );
        live_type_context[ _name ]._fields.push_back( var );
    }
}

string struct_decl::to_string( Machine_State* cpu, uint32_t indent_level ) const
{
    string ret = "";
    for( auto& var : _fields )
        ret += var.to_string( cpu, indent_level ) + '\n';
    return ret;
}

variable* struct_decl::var( const string& var_name )
{
    for( variable& local_var : _fields )
        if( local_var.name() == var_name )
            return &local_var;
    return nullptr;
}
