#include "debugger_interface.h"
#include "stackl_debugger.h"
#include "string_utils.h"
#include <iostream>
using std::cout;

static bool ENABLE = false;

void dbg_load_info( Machine_State* cpu, const char* filename )
{
    if( !ENABLE )
        return;

    if( cpu->debugger != nullptr )
    { //if a debugger object already exists
        string prompt = string( filename ) + " is being loaded. Would you like to debug it?";
        if( string_utils::get_yesno( prompt ) )
        { //they want to debug the new binary, delete the current debugger
            delete (stackl_debugger*)cpu->debugger;
            cpu->debugger = nullptr;
        }
        else return; //they want to keep debugging the existing binary, just stop here.
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