#pragma once
#include <string>
using std::string;
#include <set>
using std::set;
#include "../machine.h"

//forward declaration because we're holding a reference to the debugger than contains this command.
class stackl_debugger;

//this points to a function in the stackl debugger that is invoked when the command is selected
typedef bool (stackl_debugger::*op)( string& params, Machine_State* cpu );

class debugger_command
{
public:

    debugger_command( stackl_debugger& dbg, op func, const set<string>& names, const string& help );
    bool called_by( const string& cmd ) const;
    bool run( string& params, Machine_State* cpu ) const;
    string to_string() const;

private:
    stackl_debugger& _dbg;
    op _func;
    set<string> _names;
    string _help;
};