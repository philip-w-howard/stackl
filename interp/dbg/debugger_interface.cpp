#include "debugger_interface.h"
#include "stackl_debugger.h"

void dbg_load_info( Machine_State* cpu, const char* filename )
{
    stackl_debugger* debugger = new stackl_debugger( filename );
    if( debugger != NULL && debugger->loaded() )
    {
        cpu->debugger = (stackl_debugger*)debugger;
        debugger->query_user( cpu );
    }
    else
    {
        delete debugger;
        cpu->debugger = NULL;
    }
}

void dbg_check_break( Machine_State* cpu )
{
    if( cpu->debugger != NULL )
    {
        stackl_debugger* debugger = (stackl_debugger*)cpu->debugger;
        debugger->debug_check( cpu );
    }
}

