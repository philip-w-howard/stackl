#include "stackl_debugger.h"

#include <signal.h>
#include <iostream>
using std::cout;
using std::cin;
#include <cmath>
#include <cstdio>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <cstdlib>
using std::runtime_error;
#include <algorithm>
using std::transform;
using std::remove_if;
#include <exception>
using std::exception;

#ifdef LREADLINE
    #include <readline/readline.h>
    #include <readline/history.h>
#endif

extern "C"
{
    #include "../vmem.h"
    #include "../slasm.h"
    #include "../pio_term_int.h"
    #include "../timer.h"
}

//I tried with the variable name here. I really did.
//It's the number of seconds that a debug file can be compiled BEFORE the binary is generated.
//Effectively this is a "max compile time" before the debugger will ask if the debug files are out of date.
//We want to keep it as low as is reasonable, otherwise users could accidentally run with out of date files
    //and not be notified. That's no good.
const static int MAX_DEBUG_FILE_TIME_DELTA = 10;

stackl_debugger::stackl_debugger( const char* filename ): _binary_name( string_utils::change_ext( filename, ".slb" ) )
{
    string dbg_file = string_utils::change_ext( filename, ".dbg" );
    try
    {
        if( !file_exists( dbg_file ) )
        {
            opcode_debug_mode( dbg_file );
        }
        else
        {
            _lst = asm_list( dbg_file );
            _ast = abstract_syntax_tree();

            for( const string& filename : _lst.file_list() )
            {
                if( file_exists( filename ) )
                {
                    cout << "Loading symbols for " << filename << '\n';
                    _ast.add_ast( filename );
                    check_compile_time( _binary_name, filename );
                }
                else
                    opcode_debug_mode( filename );
            }
            _ast.fixup_globals( _lst.offsets() );
            _lst.cleanup_offset_map(); //we're done with this part of the lst's memory
        }

        load_commands();

        //if we get to this point it's because there were no exceptions thrown above
        set_flag( LOADED, true );
        cout << "Debugger for " << _binary_name << " loaded. Type 'help' for help.\n";
    }
    catch( exception& ex )
    {
        set_flag( LOADED, false );
        _failure_reason = ex.what();
    }
}

void stackl_debugger::check_compile_time( const string& binaryfile, const string& filename )
{
    struct stat attrib;
    string source_file = string_utils::change_ext( filename, ".c" );

    if( file_exists( source_file ) ) //get last time the source code file was modified
        stat( source_file.c_str(), &attrib );
    else //if we can't find a matching source file then use the binary
        stat( binaryfile.c_str(), &attrib );

    if( attrib.st_mtime > ( _ast.compile_time( filename ) + MAX_DEBUG_FILE_TIME_DELTA ) )
    {
        string prompt = "Debug file " + filename + " may be out of date. Continue anyways?";
        if( !string_utils::get_yesno( prompt ) )
            throw runtime_error( string( "Debug file " ) + filename + " is out of date." );
    }
}

bool stackl_debugger::file_exists( const string& filename )
{
    struct stat buffer;
    return stat( filename.c_str(), &buffer ) == 0;
}

void stackl_debugger::opcode_debug_mode( const string& filename )
{
    string prompt = "Could not find the neccesary file " + filename + ". Would you like to debug the binary?";
    if( !string_utils::get_yesno( prompt ) )
        throw runtime_error( filename + " not found." );

    set_flag( OPCODE_DEBUG, true );
}

void stackl_debugger::load_commands()
{
    _commands.push_back( debugger_command( *this, &stackl_debugger::cmd_breakpoint, { "breakpoint", "break", "br", "b" }, "[line_num|file:line_num|func_name] - Adds breakpoint", false ) );
    _commands.push_back( debugger_command( *this, &stackl_debugger::cmd_breakpointi, { "breakpointi", "breaki", "bi" }, "[IP] - Adds breakpoint at the passed instruction pointer", true ) );
    _commands.push_back( debugger_command( *this, &stackl_debugger::cmd_removebreak, { "removebreak", "rbreak", "rb" }, "[line_num|file:line_num|func_name] - Removes breakpoint", false ) );
    _commands.push_back( debugger_command( *this, &stackl_debugger::cmd_removebreaki, { "removebreaki", "rbreaki", "rbi" }, "[IP] - Removes breakpoint at the passed instruction pointer", true ) );
    _commands.push_back( debugger_command( *this, &stackl_debugger::cmd_print, { "print", "p" }, "[var_name|0xaddress] - Prints the value of the variable", true ) );
    _commands.push_back( debugger_command( *this, &stackl_debugger::cmd_printi, { "printi", "pi" }, "[IP|0xaddress] - Prints the value of the instruction at the passed location", true ) );
    _commands.push_back( debugger_command( *this, &stackl_debugger::cmd_continue, { "continue", "cont", "c" }, "- Runs program until the next breakpoint", true ) );
    _commands.push_back( debugger_command( *this, &stackl_debugger::cmd_list, { "list" }, " optional[line_count] - Prints all source code within N lines of the current line", false ) );
    _commands.push_back( debugger_command( *this, &stackl_debugger::cmd_next, { "next", "n" }, "- Steps into a function or over a line", false ) );
    _commands.push_back( debugger_command( *this, &stackl_debugger::cmd_step, { "step", "s" }, "- Steps over the current line", false ) );
    _commands.push_back( debugger_command( *this, &stackl_debugger::cmd_locals, { "locals", "listlocals" }, "- Print all local vars in current or specified function", false ) );
    _commands.push_back( debugger_command( *this, &stackl_debugger::cmd_globals, { "globals", "listglobals" }, "- Print all global variables", false ) );
    _commands.push_back( debugger_command( *this, &stackl_debugger::cmd_statics, { "statics", "liststatics" }, "optional[filename] - Print all static variables in specified file or all files.", false ) );
    _commands.push_back( debugger_command( *this, &stackl_debugger::cmd_funcs, { "funcs", "functions", "listfuncs", "listfunctions" }, "- Print all functions", false ) );
    _commands.push_back( debugger_command( *this, &stackl_debugger::cmd_file, { "file", "currentfile" }, "- Prints the filename of the currently executing source code", false ) );
    _commands.push_back( debugger_command( *this, &stackl_debugger::cmd_program, { "program", "binary", "currentprogram", "currentbinary" }, "- Prints the filename of the loaded binary", true ) );
    _commands.push_back( debugger_command( *this, &stackl_debugger::cmd_func, { "func", "function", "currentfunc", "currentfunction" }, "- Prints the name of the currently executing function", false ) );
    _commands.push_back( debugger_command( *this, &stackl_debugger::cmd_exit, { "exit", "quit", "q" }, "- Exits current program", true ) );
    _commands.push_back( debugger_command( *this, &stackl_debugger::cmd_IP, { "IP", "ip" }, "optional[value] - Prints or sets the instruction pointer", true ) );
    _commands.push_back( debugger_command( *this, &stackl_debugger::cmd_FLAG, { "FLAG", "flag" }, "optional[value] - Prints or sets the flags register", true ) );
    _commands.push_back( debugger_command( *this, &stackl_debugger::cmd_FP, { "FP", "fp" }, "optional[value] - Prints or sets the frame pointer", true ) );
    _commands.push_back( debugger_command( *this, &stackl_debugger::cmd_BP, { "BP", "bp" }, "optional[value] - Prints or sets the base pointer", true ) );
    _commands.push_back( debugger_command( *this, &stackl_debugger::cmd_LP, { "LP", "lp" }, "optional[value] - Prints or sets the limit pointer", true ) );
    _commands.push_back( debugger_command( *this, &stackl_debugger::cmd_SP, { "SP", "sp" }, "optional[value] - Prints or sets the stack pointer", true) );
    _commands.push_back( debugger_command( *this, &stackl_debugger::cmd_help, { "help" }, "optional[command] - Prints the help text for all commands or the specified one", true ) );
    _commands.push_back( debugger_command( *this, &stackl_debugger::cmd_nexti, { "ni", "nexti" }, "- Runs the current instruction", true ) );
    _commands.push_back( debugger_command( *this, &stackl_debugger::cmd_restart, { "restart", "re" }, "- Restarts the program at the beginning", true ) );
    _commands.push_back( debugger_command( *this, &stackl_debugger::cmd_watch, { "watch", "w" }, "[var_name|0xaddress] - Breaks the program when the watched variable changes.", true ) );
    _commands.push_back( debugger_command( *this, &stackl_debugger::cmd_removewatch, { "removewatch", "rw" }, "[var_name|0xaddress] - Removes a variable from the watchlist.", true ) );
    _commands.push_back( debugger_command( *this, &stackl_debugger::cmd_watches, { "watches" }, "- Prints every variable being watched.", true ) );
    _commands.push_back( debugger_command( *this, &stackl_debugger::cmd_backtrace, { "backtrace", "callstack", "bt" }, "- Prints the callstack.", false ) );
    _commands.push_back( debugger_command( *this, &stackl_debugger::cmd_up, { "up" }, "optional[contexts]- Moves the framepointer up one or passed amount of contexts.", false ) );
    _commands.push_back( debugger_command( *this, &stackl_debugger::cmd_down, { "down" }, "- Moves the framepointer down one or passed amount of contexts", false ) );
    _commands.push_back( debugger_command( *this, &stackl_debugger::cmd_timer, { "timer" }, "- Enables the timer between breakpoints", true ) );
    _commands.push_back( debugger_command( *this, &stackl_debugger::cmd_clear, { "clear" }, "- Clears the screen", true ) );

    const size_t size = _commands.size();
    for( size_t i = 0; i < size; ++i )
    {
        for( size_t j = i + 1; j < size; ++j ) //only check i+1, since previous values have already been compared.
        {
            if( _commands[i].has_same_trigger( _commands[j] ) )
                throw runtime_error( _commands[i].to_string() + " has the same name as " + _commands[j].to_string() );
            if( _commands[i].func() == _commands[j].func() )
                throw runtime_error( _commands[i].to_string() + " has the same function as " + _commands[j].to_string() );
        }
    }
}

void stackl_debugger::cmd_breakpoint( string& params, Machine_State* cpu )
{
    if( params.length() == 0 )
    {
        cout << "Specifiy a breakpoint location.\n";
        return;
    }

    BREAKPOINT_RESULT res = add_breakpoint( params, cpu->IP );
    uint32_t final_addr;
    string file_name;

    switch( res )
    {
    case SUCCESS:
        final_addr = text_to_addr( params, cpu->IP );
        file_name = _lst.current_file( final_addr );
        cout << "Breakpoint added at " << file_name << ':' << _lst.line_of_addr( file_name, final_addr ) << ".\n";
        break;
    case DUPLICATE:
        cout << "Breakpoint already exists on that line.\n";
        break;
    case NOT_FOUND:
        cout << "Couldn't find breakpoint location.\n";
        break;
    }
}

void stackl_debugger::cmd_breakpointi( string& params, Machine_State* cpu )
{
    if( params.length() == 0 )
    {
        cout << "Specifiy a breakpoint location.\n";
        return;
    }

    int32_t addr = string_utils::to_int( params );
    if( addr % sizeof( int32_t ) != 0 )
        cout << "Misaligned instruction pointer.\n";
    else if( add_breakpoint( addr ) )
        cout << "Breakpoint added on instruction pointer " << addr << ".\n";
    else
        cout << "Breakpoint already exists on instruction pointer " << addr << ".\n";
}

void stackl_debugger::cmd_removebreak( string& params, Machine_State* cpu )
{
    if( params.length() == 0 )
    {
        cout << "Specifiy a breakpoint location.\n";
        return;
    }

    BREAKPOINT_RESULT res = remove_breakpoint( params, cpu->IP );
    uint32_t final_addr;
    string file_name;

    switch( res )
    {
        case SUCCESS:
            final_addr = text_to_addr( params, cpu->IP );
            file_name = _lst.current_file( final_addr );
            cout << "Breakpoint removed from " << file_name << ':' << _lst.line_of_addr( file_name, final_addr ) << ".\n";
            break;
        case NOT_FOUND:
            cout << "Breakpoint doesnt exist on that line.\n";
            break;
        case DUPLICATE:
            cout << "Not sure how you got here.\n";
            break;
    }
}


void stackl_debugger::cmd_removebreaki( string& params, Machine_State* cpu )
{
    if( params.length() == 0 )
    {
        cout << "Specifiy a breakpoint location.\n";
        return;
    }

    uint32_t addr = string_utils::to_int( params );
    if( addr % sizeof( int32_t ) != 0 )
        cout << "Misaligned instruction pointer.\n";
    else if( remove_breakpoint( addr ) )
        cout << "Breakpoint removed from instruction pointer " << addr << ".\n";
    else
        cout << "Breakpoint doesn't exist on instruction pointer " << addr << ".\n";
}

void stackl_debugger::cmd_print( string& params, Machine_State* cpu )
{
    if( params.length() == 0 )
        cout << "Specifiy a variable to print.\n";
    else
        cout << var_to_string( cpu, params, true ) << '\n';
}

void stackl_debugger::cmd_printi( string& params, Machine_State* cpu )
{
    int32_t addr = params.empty() ? cpu->IP : string_utils::to_int( params );

    if( addr % sizeof( int32_t ) != 0 )
        cout << "Misaligned instruction pointer.\n";
    else
        cout << opcode_to_string( addr, cpu ) << '\n';
}

void stackl_debugger::cmd_continue( string& params, Machine_State* cpu )
{
    set_flag( RESUME_EXECUTING, true );
}

void stackl_debugger::cmd_list( string& params, Machine_State* cpu )
{
    int32_t range;
    if( params.length() == 0 )
        cout << get_nearby_lines( cpu->IP, 2 );
    else if( string_utils::is_number( params, 10, &range ) )
        cout << get_nearby_lines( cpu->IP, range );
    else
        cout << "[list] [optional range]\n";
}

void stackl_debugger::cmd_next( string& params, Machine_State* cpu )
{
    _prev_file = _lst.current_file( cpu->IP );
    _prev_line = _lst.line_of_addr( _prev_file, cpu->IP );
    set_flag( STEP_INTO, true );
    set_flag( RESUME_EXECUTING, true );
}

void stackl_debugger::cmd_step( string& params, Machine_State* cpu )
{
    _prev_fp = cpu->FP;
    _prev_file = _lst.current_file( cpu->IP );
    _prev_line = _lst.line_of_addr( _prev_file, cpu->IP );
    set_flag( STEP_OVER, true );
    set_flag( RESUME_EXECUTING, true );
}

void stackl_debugger::cmd_nexti( string& params, Machine_State* cpu )
{
    set_flag( BREAK_NEXT_OP, true );
    set_flag( RESUME_EXECUTING, true );
}

void stackl_debugger::cmd_locals( string& params, Machine_State* cpu )
{
    if( params.length() == 0 )
    {
        string cur_func = _lst.current_func( cpu->IP );
        if( cur_func.empty() )
            cout << "Can't determine current function.\n";
        else
            cout << _ast.all_locals( cur_func );
    }
    else
    {
        cout << _ast.all_locals( params );
    }
}

void stackl_debugger::cmd_globals( string& params, Machine_State* cpu )
{
    cout << _ast.all_globals();
}

void stackl_debugger::cmd_funcs( string& params, Machine_State* cpu )
{
    cout << _ast.all_funcs();
}

void stackl_debugger::cmd_exit( string& params, Machine_State* cpu )
{
    exit( EXIT_SUCCESS );
}

void stackl_debugger::cmd_IP( string& params, Machine_State* cpu )
{
    if( params.empty() )
        cout << "Instruction Pointer: " << cpu->IP << '\n';
    else
        cpu->IP = string_utils::to_int( params );
}

void stackl_debugger::cmd_FLAG( string& params, Machine_State* cpu )
{
    if( params.empty() )
        cout << "Flags: " << cpu->FLAG << '\n';
    else
        cpu->FLAG = string_utils::to_int( params );
}

void stackl_debugger::cmd_FP( string& params, Machine_State* cpu )
{
    if( params.empty() )
        cout << "Frame Pointer: " << cpu->FP << '\n';
    else
        cpu->FP = string_utils::to_int( params );
}

void stackl_debugger::cmd_BP( string& params, Machine_State* cpu )
{
    if( params.empty() )
        cout << "Base Pointer: " << cpu->BP << '\n';
    else
        cpu->BP = string_utils::to_int( params );
}

void stackl_debugger::cmd_LP( string& params, Machine_State* cpu )
{
    if( params.empty() )
        cout << "Limit Pointer: " << cpu->LP << '\n';
    else
        cpu->LP = string_utils::to_int( params );
}

void stackl_debugger::cmd_SP( string& params, Machine_State* cpu )
{
    if( params.empty() )
        cout << "Stack Pointer: " << cpu->SP << '\n';
    else
        cpu->SP = string_utils::to_int( params );
}

void stackl_debugger::cmd_file( string& params, Machine_State* cpu )
{
    cout << _lst.current_file( cpu->IP ) << '\n';
}

void stackl_debugger::cmd_program( string& params, Machine_State* cpu )
{
    cout << _binary_name << '\n';
}

void stackl_debugger::cmd_func( string& params, Machine_State* cpu )
{
    function* func = _ast.func( _lst.current_func( cpu->IP ) );
    if( func != nullptr )
        cout << func->to_string() << '\n';
    else
        cout << "Couldn't determine current function.\n";
}

void stackl_debugger::cmd_help( string& params, Machine_State* cpu )
{
    if( params.empty() )
    {
        cout << "***********************************************************\n"
                "Stackl debugger written by Jacob Asmuth for Phil Howard\n"
                " and the OIT Operating Systems course.\n"
                "***********************************************************\n\n";
        for( const debugger_command& cmd : _commands )
            if( ( get_flag( OPCODE_DEBUG ) && cmd.allowed_in_opcode_mode() ) || !get_flag( OPCODE_DEBUG ) )
                cout << cmd.to_string() << '\n';
    }
    else
    {
        for( const debugger_command& cmd : _commands )
            if( cmd.called_by( params ) )
                cout << cmd.to_string() << '\n';
    }
}

void stackl_debugger::cmd_restart( string& params, Machine_State* cpu )
{
    cpu->IP = 8;
    set_flag( RESUME_EXECUTING, true );
}

void stackl_debugger::cmd_watch( string& params, Machine_State* cpu )
{
    if( params.length() == 0 )
    {
        cout << "Specifiy a variable to watch.\n";
        return;
    }

    _watches[params] = var_to_string( cpu, params, false );
    cout << params <<  " is now being watched.\n";
}

void stackl_debugger::cmd_removewatch( string& params, Machine_State* cpu )
{
    if( params.length() == 0 )
    {
        cout << "Specifiy a variable to remove from the watchlist.\n";
        return;
    }

    auto iter = _watches.find( params );
    if( iter != _watches.end() )
    {
        _watches.erase( iter );
        cout << params << " has been removed from the watchlist.\n";
    }
    else
        cout << "Unable to find " << params << " in the watchlist. Type 'watches' to view all watched variables.\n";
}

void stackl_debugger::cmd_watches( string& params, Machine_State* cpu )
{
    for( const auto& pair : _watches )
        cout << "'" << pair.first << "' last seen as: " << pair.second << '\n';
}

void stackl_debugger::cmd_backtrace( string& params, Machine_State* cpu )
{
    string init_func = get_init_func( cpu );

    uint32_t fp = cpu->FP;
    uint32_t ip = cpu->IP;
    uint32_t cur_line;
    int32_t i = 0;
    string cur_func, cur_file, res;
    vector<string> contexts;

    if( ip == 0 )
        return; //dont do anything if the program hasn't started running yet

    do
    {
        cur_func = _lst.current_func( ip );
        cur_file = _lst.current_file( ip );
        cur_line = _lst.line_of_addr( cur_file, ip );

        string full_func;
        function* func_ptr = _ast.func( cur_func );
        if( func_ptr != nullptr )
            full_func = func_ptr->to_string();
        else
            break; //we jumped to a garbage memory location so we're done

        contexts.push_back( std::to_string( ++i ) + ". " + full_func + " at " + cur_file + ":" + std::to_string( cur_line ) );

        ip = Get_Word( cpu, fp - 8 );
        fp = Get_Word( cpu, fp - 4 );

    } while( cur_func != init_func );

    for( auto it = contexts.rbegin(); it != contexts.rend(); ++it )
        cout << *it + '\n';
}

void stackl_debugger::cmd_up( string& params, Machine_State* cpu )
{
    string init_func = get_init_func( cpu );
    uint32_t num_contexts = params.empty() ? 1 : string_utils::to_int( params );
    string cur_file;

    for( uint32_t i = 0; i < num_contexts; ++i )
    {
        if( _lst.current_func( cpu->IP ) == init_func )
        {
            cout << "You are unable to go up that far.\n";
            break;
        }

        _context_history.push_back( { cpu->IP, cpu->FP } );
        cpu->IP = Get_Word( cpu, cpu->FP - 8 );
        cpu->FP = Get_Word( cpu, cpu->FP - 4 );
        cur_file = _lst.current_file( cpu->IP );
    }

    cout << "Now at: " << _lst.current_func( cpu->IP ) << " in " << cur_file << ':' << _lst.line_of_addr( cur_file, cpu->IP ) << '\n';
}

void stackl_debugger::cmd_down( string& params, Machine_State* cpu )
{
    if( _context_history.empty() )
        throw runtime_error( "Already in the lowest context" );

    uint32_t num_contexts = params.empty() ? 1 : string_utils::to_int( params );
    const size_t size = _context_history.size();
    if( num_contexts > size )
        throw runtime_error( "Can't go down that many contexts" );

    context_t& context = _context_history[size - num_contexts];
    cpu->IP = context.IP;
    cpu->FP = context.FP;

    _context_history.erase( _context_history.end() - num_contexts, _context_history.end() );

    string cur_file = _lst.current_file( cpu->IP );
    cout << "Now at: " << _lst.current_func( cpu->IP ) << " in " << cur_file << ':' << _lst.line_of_addr( cur_file, cpu->IP ) << '\n';
}

void stackl_debugger::cmd_timer( string& params, Machine_State* cpu )
{
    set_flag( TIMER, !get_flag( TIMER ) );
    cout << "Timer " << ( get_flag( TIMER ) ? "enabled" : "disabled" ) << ".\n";
}

void stackl_debugger::cmd_clear( string& params, Machine_State* cpu )
{
    system( "clear" );
}

void stackl_debugger::cmd_statics( string& params, Machine_State* cpu )
{
    if( params.empty() )
        cout << _ast.all_statics();
    else
        cout << _ast.statics_in( params );
}

inline void stackl_debugger::set_flag( FLAG flag, bool value )
{
    _flags ^= ( -(int32_t)value ^ _flags ) & ( 1 << (int32_t)flag );
}

inline bool stackl_debugger::get_flag( FLAG flag ) const
{
    return (bool)( ( _flags >> (int32_t)flag ) & 1 );
}

string stackl_debugger::opcode_to_string( uint32_t addr, Machine_State* cpu )
{
    string res = "";
    int32_t instr = Get_Word( cpu, addr );

    if( (int64_t)instr >= ( (int64_t)sizeof( op_list ) / (int64_t)sizeof( opcode_t ) ) || instr < 0 )
        return "Invalid opcode found.\n";

    opcode_t op = op_list[instr];
    res += op.op_name;
    transform( res.begin(), res.end(), res.begin(), ::toupper );

    for( int32_t i = 0; i < op.num_params; ++i )
    {
        addr += sizeof( int32_t );
        res += " " + to_string( Get_Word( cpu, addr ) );
    }

    return res;
}

stackl_debugger::BREAKPOINT_RESULT stackl_debugger::add_breakpoint( const string& break_point_text, uint32_t cur_addr )
{
    uint32_t bpl; //break point line
    try
    {
        bpl = text_to_addr( break_point_text, cur_addr );
    }
    catch( const exception& ex )
    {
        return BREAKPOINT_RESULT::NOT_FOUND;
    }

    if( bpl != UINT32_MAX )
    {
        if( add_breakpoint( bpl ) )
            return BREAKPOINT_RESULT::SUCCESS;
       else
           return BREAKPOINT_RESULT::DUPLICATE;
    }
    else return BREAKPOINT_RESULT::NOT_FOUND;
}

stackl_debugger::BREAKPOINT_RESULT stackl_debugger::remove_breakpoint( const string & break_point_text, uint32_t cur_addr )
{
    uint32_t bpl; //break point line
    try
    {
        bpl = text_to_addr( break_point_text, cur_addr );
    }
    catch( const exception& ex )
    {
        return BREAKPOINT_RESULT::NOT_FOUND;
    }

    if( bpl != UINT32_MAX )
    {
        if( remove_breakpoint( bpl ) )
            return BREAKPOINT_RESULT::SUCCESS;
    else
            return BREAKPOINT_RESULT::NOT_FOUND;
    }
    else return BREAKPOINT_RESULT::NOT_FOUND;
}

uint32_t stackl_debugger::text_to_addr( const string& break_point_text, uint32_t cur_addr )
{
    int res;

    if( string_utils::begins_with( break_point_text, "0x" ) && string_utils::is_number( break_point_text, 16, &res ) )
    {
        return res;
    }
    else if( break_point_text.find( ':' ) != string::npos )
    {
        vector<string> file_line = string_utils::string_split( break_point_text, ':' );
        int32_t user_line = string_utils::to_int( file_line[1] );
        return _lst.addr_of_line( file_line[0], user_line );
    }
    else if( string_utils::is_number( break_point_text, 10, &res ) )
    {
        return _lst.addr_of_line( _lst.current_file( cur_addr ), res );
    }
    else
    {
        return _lst.addr_of_func( break_point_text );
    }
}


bool stackl_debugger::add_breakpoint( uint32_t addr )
{
    if( find( _break_points.begin(), _break_points.end(), addr ) == _break_points.end() )
    {
        _break_points.push_back( addr );
        return true;
    }
    else return false;
}

bool stackl_debugger::remove_breakpoint( uint32_t addr )
{
    auto res = find( _break_points.begin(), _break_points.end(), addr );
    if( res != _break_points.end() )
    {
        _break_points.erase( res );
        return true;
    }
    else return false;
}

string stackl_debugger::var_to_string( Machine_State* cpu, const string& var_text, bool prepend_def )
{
    int32_t val;

    if( string_utils::begins_with( var_text, "0x" ) && string_utils::is_number( var_text, 16, &val ) )
        return string_utils::to_hex( Get_Word( cpu, val ) );
    else if (string_utils::is_number( var_text, 10, &val ) )
        return std::to_string( Get_Word( cpu, val ) );

    vector<string> var_fields = string_utils::string_split( var_text, '.' );

    bool address_of = false;
    if( var_fields[0][0] == '&' )
    {
        address_of = true;
        var_fields[0].erase( 0, 1 );
    }

    //parse arrow operator
    for( uint32_t i = 0; i < var_fields.size(); ++i )
    {
        string field = var_fields[i];
        size_t idx = field.find_first_of( '-' );

        if( idx != field.length() - 1 && field[idx+1] == '>' ) //'->' operator
        {
            field.replace( idx, 2, "." );
            field.insert( 0, "*" );
            vector<string> new_fields = string_utils::string_split( field, '.' );

            var_fields.erase( var_fields.begin() + i );
            var_fields.insert( var_fields.begin() + i, new_fields.begin(), new_fields.end() );

            i = 0;
        }
    }

    uint32_t indirection = string_utils::strip_indirection( var_fields[0] );
    vector<uint32_t> indexes = string_utils::strip_array_indexes( var_fields[0] );
    variable* var = _ast.var( _lst.current_file( cpu->IP ), _lst.current_func( cpu->IP ), var_fields[0] );

    if( var == nullptr )
        throw runtime_error( string( var_fields[0] ) + " not found in current scope" );

    variable res = var->from_indexes( indexes, cpu );
    res = res.deref( indirection, cpu );

    for( uint32_t i = 1; i < var_fields.size(); ++i )
        res = res.parse_field_access( cpu, var_fields[i] );

    if( address_of )
        return std::to_string( res.total_offset( cpu ) );
        //return string_utils::to_hex( res.total_offset( cpu ) );
    else return res.to_string( cpu, 0, prepend_def );
}

void stackl_debugger::debug_check( Machine_State* cpu )
{
    if( !loaded() ) return;

    if( should_break( cpu ) )
    {
        for( const auto& pair : changed_watches( cpu ) )
        {
            cout << "Watch on '" << pair.first << "' triggered.\n";
            cout << '\t' << pair.second << '\n';
            _watches[pair.first] = pair.second;
        }

        if( !get_flag( OPCODE_DEBUG ) )
        {
            string cur_file = _lst.current_file( cpu->IP );
            cout << "Breakpoint hit on " << cur_file << ":" << _lst.line_of_addr( cur_file, cpu->IP ) << '\n';
            cout << get_nearby_lines( cpu->IP, 0);
        }
        else
            cout << "Breakpoint hit on instruction " << opcode_to_string( cpu->IP, cpu ) << '\n';

        if( get_flag( TIMER ) )
        {
            duration<float> dur = high_resolution_clock::now() - _start_time;
            milliseconds ms = duration_cast<milliseconds>( dur );
            cout << ms.count() << "ms elapsed, " << Timer_Current_Time() - _instructions_executed << " instructions executed.\n";
        }

        query_user( cpu );

        if( !_context_history.empty() ) //if the user ran an 'up' command but didn't move back down
        {
            cpu->FP = _context_history[0].FP;
            cpu->IP = _context_history[0].IP;
            _context_history.clear();
        }
    }
}

string stackl_debugger::command_from_input( const string& input )
{
    //commands are always one word
    size_t idx = input.find_first_of( ' ' );
    if( idx == string::npos )
        return input;
    else
        return input.substr( 0, idx );
}

string stackl_debugger::params_from_input( const string& input )
{
    //commands are always one word
    size_t idx = input.find_first_of( ' ' );
    if( idx != string::npos )
    {
        string params = input.substr( idx + 1 );
        string_utils::ltrim( params );
        return params;
    }
    else return "";
}
static char __secret[] = {95,95,32,32,32,95,95,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,10,92,32,92,32,47,32,47,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,10,32,92,32,86,32,47,32,95,95,95,32,32,32,95,32,32,32,95,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,10,32,32,92,32,47,32,47,32,95,32,92,32,124,32,124,32,124,32,124,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,10,32,32,124,32,124,124,32,40,95,41,32,124,124,32,124,95,124,32,124,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,10,32,32,92,95,47,32,92,95,95,95,47,32,32,92,95,95,44,95,124,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,10,32,32,95,95,32,95,32,32,95,32,95,95,32,32,95,95,95,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,10,32,47,32,95,96,32,124,124,32,39,95,95,124,47,32,95,32,92,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,10,124,32,40,95,124,32,124,124,32,124,32,32,124,32,32,95,95,47,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,10,32,92,95,95,44,95,124,124,95,124,32,32,32,92,95,95,95,124,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,10,32,95,32,32,32,32,95,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,10,124,32,124,32,32,124,32,124,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,10,124,32,124,95,32,124,32,124,95,95,32,32,32,32,95,95,95,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,10,124,32,95,95,124,124,32,39,95,32,92,32,32,47,32,95,32,92,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,10,124,32,124,95,32,124,32,124,32,124,32,124,124,32,32,95,95,47,32,32,32,32,32,32,32,32,95,32,32,32,32,32,32,32,10,32,92,95,95,124,124,95,124,32,124,95,124,32,92,95,95,95,124,32,32,32,32,32,32,32,40,95,41,32,32,32,32,32,32,10,32,95,32,95,95,32,95,95,95,32,32,32,32,95,95,32,95,32,32,32,95,95,32,95,32,32,95,32,32,32,95,95,95,32,10,124,32,39,95,32,96,32,95,32,92,32,32,47,32,95,96,32,124,32,47,32,95,96,32,124,124,32,124,32,47,32,95,95,124,10,124,32,124,32,124,32,124,32,124,32,124,124,32,40,95,124,32,124,124,32,40,95,124,32,124,124,32,124,124,32,40,95,95,32,10,124,95,124,32,124,95,124,32,124,95,124,32,92,95,95,44,95,124,32,92,95,95,44,32,124,124,95,124,32,92,95,95,95,124,10,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,95,95,47,32,124,32,32,32,32,32,32,32,32,32,10,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,124,95,95,95,47,32,32,32,32,32,32,32,32,32,32,10,0};

void stackl_debugger::query_user( Machine_State* cpu )
{
    string input;

    set_flag( DEBUGGING, true );
    set_flag( RESUME_EXECUTING, false );

     //set to non-blocking so console output works like normal.
    int32_t previous_mode = pio_set_nonblock( 0 );

    while( !get_flag( RESUME_EXECUTING ) )
    {
        #ifdef LREADLINE
        {
            input = readline( "(dbg) " );
            if( !input.empty() )
                add_history( input.c_str() );
        }
        #else
        {
            cout << "(dbg) ";
            getline( cin, input );
        }
        #endif //#ifdef LREADLINE

        if( input == "" )
        {
            input = _prev_cmd;
            cout << "\e[A(dbg) " << _prev_cmd << '\n'; // "\e[A" moves the cursor up one line.
        }
        else
            _prev_cmd = input;

        string cmd_txt = command_from_input( input );
        string params = params_from_input( input );

        if( cmd_txt == "phil" || cmd_txt == "magic" )
        {
            cout << __secret;
            raise( SIGSEGV );
        }

        bool ran = false;
        //look at every command object and determine which one the user asked for
        for( const debugger_command& command : _commands )
        {
            if( command.called_by( cmd_txt ) )
            {
                ran = true;
                try
                {
                    command.run( params, cpu );
                }
                catch( exception& ex )
                {
                    cout << "Error: " << ex.what() << '\n';
                }
                break; //stop checking for more commands once we find it
            }
        }

        if( !ran )
            cout << "Unknown command: " << cmd_txt << '\n';
    }

    set_flag( DEBUGGING, false );
    if( get_flag( TIMER ) )
    {
        _start_time = high_resolution_clock::now();
        _instructions_executed = Timer_Current_Time();
    }
    pio_set_nonblock( previous_mode ); //reset console to whatever mode it was previously
}

bool stackl_debugger::should_break( Machine_State* cpu )
{
    uint32_t cur_addr = cpu->IP;
    if( get_flag( BREAK_NEXT_OP ) )
    {
        set_flag( BREAK_NEXT_OP, false );
        return true;
    }
    else if( get_flag( STEP_INTO ) || get_flag( STEP_OVER ) )
    {
        string cur_file = _lst.current_file( cur_addr );
        uint32_t cur_line = _lst.line_of_addr( cur_file, cur_addr );

        if( cur_line == UINT32_MAX ) //if we step into a location undefined in the debug file
            return false; //then don't break

        if( cur_line != _prev_line || cur_file != _prev_file )
        { //if our current line or file has changed
            if( get_flag( STEP_INTO ) )
            {
                set_flag( STEP_INTO, false );
                return true;
            }
            else if( get_flag( STEP_OVER ) && (uint32_t)cpu->FP <= _prev_fp )
            { //if our frame pointer is back to where we started
                set_flag( STEP_OVER, false );
                return true;
            }
            else return false;
        }
    }
    else if( find( _break_points.begin(), _break_points.end(), cur_addr ) != _break_points.end() )
        return true;
    else if( !_watches.empty() && !changed_watches( cpu ).empty() )
        return true;
    return false; //otherwise dont break
}

unordered_map<string, string> stackl_debugger::changed_watches( Machine_State* cpu )
{
    unordered_map<string, string> ret;
    for( const auto& pair : _watches )
    {
        string var_name = pair.first;
        try
        {
            string res = var_to_string( cpu, var_name, false );
            if( res != pair.second )
                ret[pair.first] = res;
        }
        catch( ... ) {}
    }
    return ret;
}

string stackl_debugger::get_nearby_lines( uint32_t cur_addr, int32_t range )
{
    string cur_file = _lst.current_file( cur_addr );
    int32_t cur_line = (int32_t)_lst.line_of_addr( cur_file, cur_addr );
    string ret = "";
    string line;
    ifstream file;

    try
    {
        file.open( cur_file ); //this can throw an exception
        if( !file.is_open() )
            throw 0; //this is kinda dumb... but it keeps us from duplicating the error message?
    }
    catch( ... )
    {
        return string( "Unable to open " ) + cur_file + '\n';
    }

    int i = 1; //files start on line 1
    while( getline( file, line ) )
    {
        if( i >= cur_line - range && i <= cur_line + range )
            ret += to_string( i ) + ". " + string( 3 - (int)log10( i ), ' ' ) + line + '\n';
        if( i > cur_line + range )
            break;
        ++i;
    }
    return ret;
}

//address 12 holds the address of the first function jumped to by the interpreter
string stackl_debugger::get_init_func( Machine_State* cpu ) const
{
    return _lst.current_func( Get_Word( cpu, 12 ) );
}
