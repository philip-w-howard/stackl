#include "operation.h"
#include "string_utils.h"
#include "variable.h"
#include <cstdlib>
using std::runtime_error;
#include <cmath>

#include "stackl_debugger.h"


int64_t operation::calc( Machine_State* cpu, string left, string right )
{
    stackl_debugger* dbg = static_cast<stackl_debugger*>( cpu->debugger );

    string left_txt = dbg->var_to_string( cpu, left, false );
    int64_t left_val;
    if( !string_utils::is_number( left_txt, 10, &left_val ) )
        throw runtime_error( left_txt + " is not a number." );

    string right_txt = dbg->var_to_string( cpu, right, false );
    int64_t right_val;
    if( !string_utils::is_number( right_txt, 10, &right_val ) )
        throw runtime_error( right_txt + " is not a number." );

    switch( _op )
    {
        case '+': return left_val + right_val;
        case '-': return left_val - right_val;
        case '*': return left_val * right_val;
        case '/': return left_val / right_val;
        case '%': return left_val % right_val;
        case '^': return (int64_t)pow( left_val, right_val );
    }
    return 0;
}