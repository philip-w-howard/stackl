#include "variable.h"
#include "struct_decl.h" //forward declaration found in variable.h
//since a variable needs to know its type context but also a struct_decl holds variables
#include "stackl_debugger.h"

#include <cstdio>
#include <stdexcept>
using std::runtime_error;

#include "string_utils.h"

#include "safe_read.h"

unordered_map<string, size_t> variable::builtins(
{
    { "char", 1 },
    { "int", 4 },
    { "void", 4 }
} );

variable::variable( xml_node<char>* var_node, unordered_map<string, struct_decl>& global_type_context, unordered_map<string, struct_decl>* local_type_context )
{
    xml_attribute<char>* global_att = var_node->first_attribute( "global" );
    if( global_att != nullptr )
    {
        _global = strcmp( global_att->value(), "true" ) == 0;
        xml_attribute<char>* static_att = var_node->first_attribute( "static" );
        if( static_att != nullptr )
            _static = strcmp( static_att->value(), "true" ) == 0;
    }

    _offset = atoi( var_node->first_attribute( "offset" )->value() );
    _name = var_node->first_node( "Symbol" )->first_attribute( "name" )->value();

    parse_node_type( var_node->first_node(), global_type_context, local_type_context );

    if( is_array() )
        for( uint32_t range : _array_ranges )
            _size *= range;
}

int32_t variable::total_offset( Machine_State* cpu ) const
{
    if( _global )
        return _offset;
    else return _offset + cpu->FP;
}

void variable::parse_base_decl( xml_node<char>* node )
{
    _size = atoi( node->first_attribute( "size" )->value() );
    _type = node->first_node( "Symbol" )->first_attribute( "name" )->value();
}

void variable::parse_pointer_type( xml_node<char>* node, unordered_map<string, struct_decl>& global_type_context, unordered_map<string, struct_decl>* local_type_context )
{
    _type = node->first_node( "Symbol" )->first_attribute( "name" )->value();

    //count the number of asterisks, that's our indirection level
    _indirection = count( _type.begin(), _type.end(), '*' );
    //then remove them
    _type.erase( std::remove( _type.begin(), _type.end(), '*' ), _type.end() );

    auto builtin_iter = builtins.find( _type );
    if( builtin_iter == builtins.end() ) //if the type is not a builtin type
    {
        auto global_type = global_type_context.find( _type );
        if( global_type != global_type_context.end() )
        {
            _struct_decl = &global_type->second; //'_struct_decl' now points to the decl object that declares the type of this var
        }
        else if( local_type_context != nullptr ) //it's a local type
        {
            _struct_decl = &local_type_context->find( _type )->second;
        }
        else
        {
            throw runtime_error( _type + " is an unknown type" );
            //we don't know what type it is?
            //kernel_panic();
            //throw "a flotation device";
            //exit( EXIT_FAILURE );
            //assert( false );
            //new uint64_t[SIZE_MAX]; -> does the user have 34.35 gigs of ram quick and easy test
        }
        _size = _struct_decl->size();
    }
    else
    {
        _size = builtin_iter->second;
    }
}

void variable::parse_struct_type( xml_node<char>* node, unordered_map<string, struct_decl>& global_type_context, unordered_map<string, struct_decl>* local_type_context )
{
    _type = node->first_node( "Symbol" )->first_attribute( "name" )->value();
    _struct_decl = &global_type_context[_type];
    _size = _struct_decl->size();
}

void variable::parse_array_type( xml_node<char>* node, unordered_map<string, struct_decl>& global_type_context, unordered_map<string, struct_decl>* local_type_context )
{
    //every time we come into this function we add a 'size' index to our vector.
    _array_ranges.push_back( atoi( node->first_attribute( "Size" )->value() ) );

    for( xml_node<char>* inner_node : node->all_nodes() )
    {
        if( strcmp( inner_node->name(), "Symbol" ) != 0 ) //NOT our 'Symbol' node
        {
            parse_node_type( inner_node, global_type_context, local_type_context );
            break;  //there's only going to be two nodes. A Symbol node and a node of the other type, which is the one we care about
        }
    }
}

void variable::parse_node_type( xml_node<char>* node, unordered_map<string, struct_decl>& global_type_context, unordered_map<string, struct_decl>* local_type_context )
{
    if( strcmp( node->name(), "BaseDecl" ) == 0 ) // (int, float, etc.)
    {
        parse_base_decl( node );
    }
    else if( strcmp( node->name(), "PointerType" ) == 0 )
    {
        parse_pointer_type( node, global_type_context, local_type_context );
    }
    else if( strcmp( node->name(), "StructType" ) == 0 )
    {
        parse_struct_type( node, global_type_context, local_type_context );
    }
    else if( strcmp( node->name(), "ArrayType" ) == 0 )
    {
        parse_array_type( node, global_type_context, local_type_context );
    }
    else throw runtime_error( string( "Invalid AST: " ) + node->name() );
}

string variable::definition() const
{
    string def = _type;

    def += string( _indirection, '*' );
    def += " " + _name;

    if( _array_ranges.size() != 0 )
        for( size_t idx : _array_ranges )
            def += "[" + std::to_string( idx ) + "]";

    return def;
}

bool variable::is_string() const
{
    if( _type != "char" )
        return false;
    if( _indirection == 1 && _array_ranges.size() == 0 ) //char*
        return true;
    if( _indirection == 0 && _array_ranges.size() == 1 ) //char[]
        return true;
    return false; //anything else aint a string.
}

variable variable::deref( uint32_t derefs, Machine_State* cpu ) const
{
    if( derefs == 0 ) //short cut.
        return *this; //the algorithm would work fine with deref == 0, this just saves time.
    if( derefs > _indirection )
        throw runtime_error( definition() + " does not have that many levels of indirection." );

    int32_t addr = total_offset( cpu );

    for( uint32_t i = 0; i < derefs; ++i )
        addr = safe_read_word( cpu, addr );


    if( !_global ) //if it's a local variable then return locality to the offset
        addr -= cpu->FP;

    variable deref_var = *this; //create a copy
    deref_var._offset = addr;
    deref_var._indirection -= derefs;

    return deref_var;
}

variable variable::from_indexes( vector<uint32_t>& indexes, Machine_State* cpu ) const
{
    if( indexes.size() == 0 )
        return *this;
    if( is_pointer() && !is_array() )
        return deref_ptr_from_index( indexes, cpu );
    if( indexes.size() > _array_ranges.size() )
        throw runtime_error( definition() + " does not have " + std::to_string( indexes.size() ) + " dimensions." );

    int32_t new_offset = _offset;
    size_t new_size = _size;
    for( uint32_t i = 0; i < indexes.size(); ++i )
    {
        if( indexes[i] >= _array_ranges[i] )
            throw runtime_error( definition() + " out of bounds on dimension " + std::to_string( i ) + "." );

        //every time we move down a dimension in our array,
        //we have to reduce the size of our offset modification by
        //the length of the dimension we just iterated over.
        new_size /= _array_ranges[i];
        new_offset += new_size * indexes[i];
    }

    variable indexed = *this;
    indexed._offset = new_offset;

    int32_t remaining_dimensions = _array_ranges.size() - indexes.size();

    indexed._array_ranges.erase( indexed._array_ranges.begin(), ( indexed._array_ranges.end() - remaining_dimensions ) );
    indexed._size = new_size;
    return indexed;
}

variable variable::deref_ptr_from_index( vector<uint32_t>& indexes, Machine_State* cpu ) const
{
    if( indexes.size() > _indirection )
        throw runtime_error( "Cannot index pointer on more dimensions than total level of indirection." );

    int32_t addr = safe_read_word( cpu, total_offset( cpu ) );
    for( uint32_t i = 0; i < indexes.size(); ++i )
    {
        if( _indirection - ( i + 1 ) == 0 ) //if its a deref to a type we have to move along the stack
            addr += indexes[i] * _size;
        else //if it's a deref to a pointer we have to follow the pointer
            addr = safe_read_word( cpu, addr + indexes[i] * sizeof( int32_t ) );
    }

    if( !_global ) //if it's a local variable then return locality to the offset
        addr -= cpu->FP;

    variable indexed = *this;
    indexed._offset = addr;
    indexed._indirection -= indexes.size();
    return indexed;
}

variable variable::parse_field_access( Machine_State* cpu, string& rhs ) const
{
    if( !is_struct() )
        throw runtime_error( string( "'" ) + definition() + "' is not a struct type." );

    uint32_t indirection = string_utils::strip_indirection( rhs );
    vector<uint32_t> indexes = string_utils::strip_array_indexes( rhs );
    variable* field = const_cast<variable*>( this )->decl()->var( rhs );

    if( field == nullptr )
        throw runtime_error( string( "'" ) + rhs + "' is not a field of type '" + type() + "'." );

    variable res = *field;
    res._offset += _offset;
    return res.from_indexes( indexes, cpu ).deref( indirection, cpu );
}

string variable::to_string( Machine_State* cpu, uint32_t indent_level, bool prepend_def ) const
{

    string tabs = string( indent_level*4, ' ' );
    string pre = tabs;
    if( prepend_def )
        pre += definition() + " = ";

    if( is_string() ) //special case: we want to print out the string
    {
        int32_t addr;
        char v;

        if( _indirection == 1 ) //is it a char*
            addr = safe_read_word( cpu, total_offset( cpu ) );
        else if( _array_ranges.size() == 1 ) //is it a char[]
            addr = total_offset( cpu );

        pre += "\"";
        for( uint32_t i = 0; ( v = safe_read_byte( cpu, addr + i ) ) != '\0'; ++i )
        {
            if( v == '\n' )
                pre += "\\n";
            else if( v == '\t' )
                pre += "\\t";
            else
                pre += v;
        }
        pre += "\"";
        return pre;
    }
    else if( is_pointer() && !is_array() )
    {
        //return pre + string_utils::to_hex( safe_read_word( cpu, total_offset( cpu ) ) );
        return pre + std::to_string( safe_read_word( cpu, total_offset( cpu ) ) );
    }
    else if( is_array() )
    {
        //pre += string_utils::to_hex( total_offset( cpu ) ) + " {\n";
        pre += std::to_string( total_offset( cpu ) ) + " {\n";

        //Initialize a vector with room for one number.
        // I know this is dumb. It's so that I don't have to make an overload to do essentially this behavior in this one case.
        vector<uint32_t> idx( 1 );

        if( _array_ranges.size() == 1 )
        {
            //iterate over the first dimension of our array
            for( uint32_t i = 0; i < _array_ranges[0]; ++i )
            {
                idx[0] = i; //use our 'from_indexes' function to ask the variable for itself[i]
                pre += from_indexes( idx, cpu ).to_string( cpu, indent_level + 1 ) + ",\n";
            }
            pre.erase( pre.end() - 2 ); //remove the last comma and newline
        }
        else
        {
            size_t index_size = _size / _array_ranges[0];
            for( uint32_t i = 0; i < _array_ranges[0]; ++i )
                //pre += tabs + '\t' + string_utils::to_hex( total_offset( cpu ) + ( i * index_size ) ) + ",\n";
                pre += tabs + '\t' + std::to_string( total_offset( cpu ) + ( i * index_size ) ) + ",\n";
        }
        return pre + tabs + "}";
    }
    else if( _struct_decl != nullptr )
    {
        Machine_State temp_state = *cpu;
        temp_state.FP = total_offset( cpu ); //a struct prints itself relative to the frame pointer
        //since its fields are offset relative to the start of the struct

        return pre + "{\n" + _struct_decl->to_string( &temp_state, indent_level + 1 ) + tabs + "}";
    }
    else if( _type == "int" )
    {
        return pre + std::to_string( safe_read_word( cpu, total_offset( cpu ) ) );
    }
    else if( _type == "char" )
    {
        return pre + "'" + (char)safe_read_byte( cpu, total_offset( cpu ) ) + "'";
    }
    else
        return string( "unable to print type " ) + _type;
}
