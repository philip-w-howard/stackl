#include "debugger_command.h"
#include <stdexcept>
using std::runtime_error;
#include "stackl_debugger.h"

debugger_command::debugger_command( stackl_debugger& dbg, op func, const set<string>& triggers, const string& help, bool op_dbg )
    : _dbg( dbg ), _func( func ), _triggers( triggers ), _help( help ), _op_dbg( op_dbg )
{
    for( const string& trigger : _triggers )
        if( trigger.find( ' ' ) != string::npos )
            throw runtime_error( string( "Command triggers must be one word: \"" ) + trigger + "\"" );
}

bool debugger_command::called_by( const string& cmd ) const
{
    return _triggers.find( cmd ) != _triggers.end();
}

void debugger_command::run( string& params, Machine_State* cpu ) const
{
    //if the debugger is in opcode mode AND the command is allowed in opcode mode
    // or if the debugger isn't in opcode mode
    if( ( _dbg.opcode_mode() && allowed_in_opcode_mode() ) || !_dbg.opcode_mode() )
        (_dbg.*_func)( params, cpu );
    else
        throw runtime_error( "This command can't be run without debug files" );
}

string debugger_command::to_string() const
{
    string ret = "[";
    for( const string& str : _triggers )
        ret += str + "|";
    ret.pop_back(); //remove last bar
    ret += "] " + _help;
    return ret;
}

bool debugger_command::has_same_trigger( const debugger_command& cmd ) const
{
    for( const string& trigger : _triggers )
        if( cmd.called_by( trigger ) )
            return true;
    return false;
}