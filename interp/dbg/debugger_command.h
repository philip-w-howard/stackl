#pragma once
#include <string>
using std::string;
#include <set>
using std::set;
#include "../machine.h"

//forward declaration because we're holding a reference to the debugger than contains this command.
class stackl_debugger;

//this points to a function in the stackl debugger that is invoked when the command is selected
typedef void (stackl_debugger::*op)( string& params, Machine_State* cpu );

class debugger_command
{
public:

    debugger_command( stackl_debugger& dbg, op func, const set<string>& _triggers, const string& help, bool op_dbg = false );
    bool called_by( const string& cmd ) const;
    void run( string& params, Machine_State* cpu ) const;
    bool has_same_trigger( const debugger_command& cmd ) const;
    string to_string() const;
    inline bool allowed_in_opcode_mode() const { return _op_dbg; }
    inline op func() const { return _func; }

private:
    stackl_debugger& _dbg;
    op _func;
    set<string> _triggers;
    string _help;
    bool _op_dbg;
};