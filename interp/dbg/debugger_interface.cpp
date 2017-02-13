#include "debugger_interface.h"
#include "stackl_debugger.h"

#include <iostream>
using std::cout;

static bool ENABLE = false;

void dbg_load_info( Machine_State* cpu, const char* filename )
{
    if( !ENABLE )
        return;

    stackl_debugger* debugger = new stackl_debugger( filename );

    if( debugger == NULL )
    {
        std::cout << "Debugger memory allocation failed.\n";
        exit( EXIT_FAILURE );
    }
    
    if( !debugger->loaded() )
    {
        std::cout << "Error loading debugger: " << debugger->failure_reason() << '\n';
        free( (void*)debugger ); //using 'delete' seg faults here. Programming.
        exit( EXIT_FAILURE );
    }

    cpu->debugger = (stackl_debugger*)debugger;
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

void dbg_enable()
{
    ENABLE = true;
}