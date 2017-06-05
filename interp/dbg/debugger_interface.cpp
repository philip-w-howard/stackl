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

    stackl_debugger* debugger = static_cast<stackl_debugger*>( cpu->debugger );

    if( debugger != nullptr )
    { //if a debugger object already exists
        cout << filename << " is being loaded.\n";
        if( debugger->debug_new_binary() && string_utils::get_yesno( "Would you like to debug it?" ) )
        { //they want to debug the new binary, delete the current debugger
            cpu->debugger = nullptr;
            delete debugger;
        }
        else return; //they want to keep debugging the existing binary, just stop here.
    }

    debugger = new stackl_debugger( filename );

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
    //else this machine check was generated from something done inside the debugger

    return 1; //don't call exit, we might want to continue running code
}

void dbg_enable()
{
    ENABLE = true;
}
