#include "debugger_interface.h"
#include "stackl_debugger.h"

#include <iostream>
using std::cout;
using std::cin;

static bool ENABLE = false;

void dbg_load_info( Machine_State* cpu, const char* filename )
{
    if( !ENABLE )
        return;

    if( cpu->debugger != nullptr )
    {
        char c;
        cout << "A different binary is already being debugged. Would you like to debug the new one? [y/n] ";
        cin.get( c );
        cin.ignore( INT32_MAX, '\n' ); //ignore 2^31 characters up until the next newline
        if( c == 'n' )
            return;
    }

    stackl_debugger* debugger = new stackl_debugger( filename );

    if( debugger == NULL )
    {
        cout << "Debugger memory allocation failed.\n";
        exit( EXIT_FAILURE );
    }

    if( !debugger->loaded() )
    {
        cout << "Error loading debugger: " << debugger->failure_reason() << '\n';
        delete debugger;
        exit( EXIT_FAILURE );
    }

    cpu->debugger = (void*)debugger;
    debugger->query_user( cpu );
}

void dbg_check_break( Machine_State* cpu )
{
    if( cpu->debugger != NULL )
    {
        stackl_debugger* debugger = (stackl_debugger*)cpu->debugger;
        debugger->debug_check( cpu );
    }
}

int32_t dbg_machine_check( int32_t code, Machine_State* cpu )
{
    stackl_debugger* dbg = (stackl_debugger*)cpu->debugger;
    if( dbg == nullptr )
        return 0; //no debugger? tell machine check to call exit()

    if( !dbg->debugging() )
        dbg->query_user( cpu );

    return 1;
}

void dbg_enable()
{
    ENABLE = true;
}