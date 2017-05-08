#include "expression.h"
#include <algorithm>
using std::remove_if;

size_t matching_paren( const string& val, size_t begin_pos )
{
    if( begin_pos == string::npos )
        return string::npos;

    int32_t nesting = 1;
    size_t idx = begin_pos + 1;

    for( size_t len = val.length(); nesting != 0 && idx < len; ++idx )
    {
        if( val[idx] == '(' )
            ++nesting;
        else if( val[idx] == ')' )
            --nesting;
    }

    if( nesting != 0 )
        return string::npos;

    return idx - 1;
}

expression::expression( const string& exp ): _exp( exp )
{
    _exp.erase( remove_if( _exp.begin(), _exp.end(), isspace ), _exp.end() );
}

string expression::eval( Machine_State* cpu )
{
    string parse = "(" + _exp + ")";
    return reduce( cpu, parse, 0, parse.length() - 1 );
}

string expression::reduce( Machine_State* cpu, string val, size_t open_paren, size_t close_paren )
{
    string section = val.substr( open_paren + 1, close_paren - open_paren - 1 );
    size_t begin_pos;

    while( ( begin_pos = section.find_first_of( '(' ) ) != string::npos )
        section = reduce( cpu, section, begin_pos, matching_paren( section, begin_pos ) );

    op_tree tree( section, _heirarchy );

/*
    section.erase( section.find_last_not_of( '0' ) + 1, string::npos );
    if( section.back() == '.' )
        section.pop_back();
*/

    val.erase( open_paren, close_paren - open_paren + 1  );
    val.insert( open_paren, tree.eval( cpu ) );
    return val;
}