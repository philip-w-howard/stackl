#include "stackl_debugger.h"

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
#include <set>
using std::set;
#include <algorithm>
using std::transform;

extern "C"
{
    #include "../vmem.h"
    #include "../slasm.h"
}

stackl_debugger::stackl_debugger( const char* filename )
{
    string fname = filename;
    _binary_name = fname;

    size_t idx =  fname.find_last_of( '.' );
    if( idx != string::npos )
        fname.erase( idx );

    try
    {

        if( !file_exists( fname + ".dbg" ) || !file_exists( fname + ".ast" ) )
            opcode_debug_mode( fname );
        else
        {
            _lst = asm_list( fname + ".dbg" );
            _ast = abstract_syntax_tree();

            for( const string& file : _lst.file_list() )
                _ast.add_ast( file );

            _ast.fixup_globals( _lst.global_offsets() );

            check_compile_time( filename );
        }

        load_commands();

        //if we get to this point it's because there were no exceptions thrown above
        _loaded = true;
        cout << "Debugger for " << _binary_name << " loaded. Type 'help' for help.\n";
    }
    catch( exception& ex )
    {
        _loaded = false;
        _failure_reason = ex.what();
    }
}

void stackl_debugger::check_compile_time( const char* filename )
{
    struct stat attrib;
    stat( filename, &attrib );
    if( attrib.st_mtime > _ast.compile_time() )
        throw runtime_error( "Debug files out of date." );
}

bool stackl_debugger::file_exists( const string& filename )
{
    struct stat buffer;
    return stat( filename.c_str(), &buffer ) == 0;
}

void stackl_debugger::opcode_debug_mode( const string& filename )
{
    char c;
    cout << "Could not find debug files. Would you like to debug the binary? [y/n] ";
    cin.get( c );
    cin.ignore( INT32_MAX, '\n' ); //ignore 2^31 characters up until the next newline
    if( c == 'n' )
        throw runtime_error( "Debug files not found" );

    _opcode_debug = true;
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
    _commands.push_back( debugger_command( *this, &stackl_debugger::cmd_next, { "next", "n" }, "[line_num|file:line_num|func_name] - Removes breakpoint", false ) );
    _commands.push_back( debugger_command( *this, &stackl_debugger::cmd_locals, { "locals", "listlocals" }, "- Print all local vars in current or specified function", false ) );
    _commands.push_back( debugger_command( *this, &stackl_debugger::cmd_globals, { "globals", "listglobals" }, "- Print all global variables", false ) );
    _commands.push_back( debugger_command( *this, &stackl_debugger::cmd_funcs, { "funcs", "functions", "listfuncs", "listfunctions" }, "- Print all functions", false ) );
    _commands.push_back( debugger_command( *this, &stackl_debugger::cmd_file, { "file", "currentfile" }, "- Prints the filename of the currently executing source code", false ) );
    _commands.push_back( debugger_command( *this, &stackl_debugger::cmd_program, { "program", "binary", "currentprogram", "currentbinary" }, "- Prints the filename of the loaded binary", true ) );
    _commands.push_back( debugger_command( *this, &stackl_debugger::cmd_func, { "func", "function", "currentfunc", "currentfunction" }, "- Prints the name of the currently executing function", false ) );
    _commands.push_back( debugger_command( *this, &stackl_debugger::cmd_exit, { "exit", "quit", "q" }, "- Exits current program", true ) );
    _commands.push_back( debugger_command( *this, &stackl_debugger::cmd_IP, { "IP" }, "optional[value] - Prints or sets the instruction pointer", true ) );
    _commands.push_back( debugger_command( *this, &stackl_debugger::cmd_FLAG, { "FLAG" }, "optional[value] - Prints or sets the flags register", true ) );
    _commands.push_back( debugger_command( *this, &stackl_debugger::cmd_FP, { "FP", "fp" }, "optional[value] - Prints or sets the frame pointer", true ) );
    _commands.push_back( debugger_command( *this, &stackl_debugger::cmd_BP, { "BP", "bp" }, "optional[value] - Prints or sets the base pointer", true ) );
    _commands.push_back( debugger_command( *this, &stackl_debugger::cmd_LP, { "LP", "lp" }, "optional[value] - Prints or sets the limit pointer", true ) );
    _commands.push_back( debugger_command( *this, &stackl_debugger::cmd_SP, { "SP", "sp" }, "optional[value] - Prints or sets the stack pointer", true) );
    _commands.push_back( debugger_command( *this, &stackl_debugger::cmd_help, { "help" }, "- Prints the help text", true ) );
    _commands.push_back( debugger_command( *this, &stackl_debugger::cmd_nexti, { "ni", "nexti" }, "- Runs the current instruction", true ) );
    _commands.push_back( debugger_command( *this, &stackl_debugger::cmd_restart, { "restart", "re" }, "- Restarts the program at the beginning", true ) );
}

bool stackl_debugger::cmd_breakpoint( string& params, Machine_State* cpu )
{
    if( params.length() == 0 )
    {
        cout << "Specifiy a breakpoint location.\n";
        return true;
    }

    BREAKPOINT_RESULT res = add_breakpoint( params, cpu->IP );
    uint32_t final_addr;
    string file_name;

    switch( res )
    {
    case SUCCESS:
        final_addr = text_to_addr( params, cpu->IP );
        file_name = _lst.current_file( final_addr );
        cout << "breakpoint added at " << file_name << ':' << _lst.line_of_addr( file_name, final_addr ) << ".\n";
        break;
    case DUPLICATE:
        cout << "breakpoint already exists on that line.\n";
        break;
    case NOT_FOUND:
        cout << "couldn't find breakpoint location.\n";
        break;
    }
    return true;
}

bool stackl_debugger::cmd_breakpointi( string& params, Machine_State* cpu )
{
    if( params.length() == 0 )
    {
        cout << "Specifiy a breakpoint location.\n";
        return true;
    }

    int32_t addr;
    if( string_utils::is_number( params, 10, &addr ) )
    {
        if( addr % sizeof( int32_t ) != 0 )
        {
            cout << "Misaligned instruction pointer.\n";
            return true;
        }
        add_breakpoint( addr );
        cout << "Breakpoint added on instruction pointer " << addr << ".\n";
        return true;
    }
    else
    {
        cout << "Enter a valid number.\n";
        return true;
    }
    return true;
}

bool stackl_debugger::cmd_removebreak( string& params, Machine_State* cpu )
{
    if( params.length() == 0 )
    {
        cout << "Specifiy a breakpoint location.\n";
        return true;
    }

    BREAKPOINT_RESULT res = remove_breakpoint( params, cpu->IP );
    uint32_t final_addr;
    string file_name;

    switch( res )
    {
        case SUCCESS:
            final_addr = text_to_addr( params, cpu->IP );
            file_name = _lst.current_file( final_addr );
            cout << "breakpoint removed from " << file_name << ':' << _lst.line_of_addr( file_name, final_addr ) << ".\n";
            break;
        case NOT_FOUND:
            cout << "breakpoint doesnt exist on that line.\n";
            break;
        case DUPLICATE:
            cout << "Not sure how you got here.\n";
            break;
    }
    return true;
}


bool stackl_debugger::cmd_removebreaki( string& params, Machine_State* cpu )
{
    if( params.length() == 0 )
    {
        cout << "Specifiy a breakpoint location.\n";
        return true;
    }

    try
    {
        uint32_t addr = stoi( params );
        if( addr % sizeof( int32_t ) != 0 )
        {
            cout << "Misaligned instruction pointer.\n";
            return true;
        }
        if( remove_breakpoint( addr ) )
            cout << "Breakpoint removed from instruction pointer " << addr << ".\n";
        else
            cout << "Breakpoint doesn't exist on instruction pointer " << addr << ".\n";
        return true;
    }
    catch( ... )
    {
        cout << "Enter a valid number.\n";
        return true;
    }
    return true;
}

bool stackl_debugger::cmd_print( string& params, Machine_State* cpu )
{
    if( params.length() == 0 )
    {
        cout << "Specifiy a variable to print.\n";
        return true;
    }

    try
    {
        cout << var_to_string( cpu, params ) << '\n';
    }
    catch( const char* err )
    {
        cout << err << '\n';
    }

    return true;
}

bool stackl_debugger::cmd_printi( string& params, Machine_State* cpu )
{
    int32_t addr;
    if( params.length() == 0 )
    {
        addr = cpu->IP;
    }
    else
    {
        try
        {
            addr = string_utils::to_int( params );
        }
        catch( runtime_error& )
        {
            cout << "Enter a valid number.\n";
            return true;
        }
    }

    if( addr % sizeof( int32_t ) != 0 )
    {
        cout << "Misaligned instruction pointer.\n";
        return true;
    }

    cout << opcode_to_string( addr, cpu ) << '\n';
    return true;
}

bool stackl_debugger::cmd_continue( string& params, Machine_State* cpu )
{
    return false;
}

bool stackl_debugger::cmd_list( string& params, Machine_State* cpu )
{
    int32_t range;
    if( params.length() == 0 )
        cout << get_nearby_lines( cpu->IP, 2 );

    else if( string_utils::is_number( params, 10, &range ) )
        cout << get_nearby_lines( cpu->IP, range );
    else
        cout << "[list] [optional range]\n";
    return true;
}

bool stackl_debugger::cmd_next( string& params, Machine_State* cpu )
{
    _prev_file = _lst.current_file( cpu->IP );
    _prev_line = _lst.line_of_addr( _prev_file, cpu->IP );
    _stepping = true;
    return false;
}

bool stackl_debugger::cmd_nexti( string& params, Machine_State* cpu )
{
    _break_next_op = true;
    return false;
}

bool stackl_debugger::cmd_locals( string& params, Machine_State* cpu )
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
    return true;
}

bool stackl_debugger::cmd_globals( string& params, Machine_State* cpu )
{
    cout << _ast.all_globals();
    return true;
}

bool stackl_debugger::cmd_funcs( string& params, Machine_State* cpu )
{
    cout << _ast.all_funcs();
    return true;
}

bool stackl_debugger::cmd_exit( string& params, Machine_State* cpu )
{
    exit( EXIT_SUCCESS );
    return true;
}

bool stackl_debugger::cmd_IP( string& params, Machine_State* cpu )
{
    if( params.empty() )
    {
        cout << "Instruction Pointer: " << cpu->IP << '\n';
        return true;
    }
    else
    {
        try
        {
            int32_t addr = string_utils::to_int( params );
            cpu->IP = addr;
            return true;
        }
        catch( runtime_error& )
        {
            cout << "Enter a valid number.\n";
            return true;
        }
    }
}

bool stackl_debugger::cmd_FLAG( string& params, Machine_State* cpu )
{
    if( params.empty() )
    {
        cout << "Flags: " << cpu->FLAG << '\n';
        return true;
    }
    else
    {
        try
        {
            int32_t val = string_utils::to_int( params );
            cpu->FLAG = val;
            return true;
        }
        catch( runtime_error& )
        {
            cout << "Enter a valid number.\n";
            return true;
        }
    }
}

bool stackl_debugger::cmd_FP( string& params, Machine_State* cpu )
{
    if( params.empty() )
    {
        cout << "Frame Pointer: " << cpu->FP << '\n';
        return true;
    }
    else
    {
        try
        {
            int32_t addr = string_utils::to_int( params );
            cpu->FP = addr;
            return true;
        }
        catch( runtime_error& )
        {
            cout << "Enter a valid number.\n";
            return true;
        }
    }
}

bool stackl_debugger::cmd_BP( string& params, Machine_State* cpu )
{
    if( params.empty() )
    {
        cout << "Base Pointer: " << cpu->BP << '\n';
        return true;
    }
    else
    {
        try
        {
            int32_t addr = string_utils::to_int( params );
            cpu->BP = addr;
            return true;
        }
        catch( runtime_error& )
        {
            cout << "Enter a valid number.\n";
            return true;
        }
    }
}

bool stackl_debugger::cmd_LP( string& params, Machine_State* cpu )
{
    if( params.empty() )
    {
        cout << "Limit Pointer: " << cpu->LP << '\n';
        return true;
    }
    else
    {
        try
        {
            int32_t addr = string_utils::to_int( params );
            cpu->LP = addr;
            return true;
        }
        catch( runtime_error& )
        {
            cout << "Enter a valid number.\n";
            return true;
        }
    }
}

bool stackl_debugger::cmd_SP( string& params, Machine_State* cpu )
{
    if( params.empty() )
    {
        cout << "Stack Pointer: " << cpu->SP << '\n';
        return true;
    }
    else
    {
        try
        {
            int32_t addr = string_utils::to_int( params );
            cpu->SP = addr;
            return true;
        }
        catch( runtime_error& )
        {
            cout << "Enter a valid number.\n";
            return true;
        }
    }
}

bool stackl_debugger::cmd_file( string& params, Machine_State* cpu )
{
    cout << _lst.current_file( cpu->IP ) << '\n';
    return true;
}

bool stackl_debugger::cmd_program( string& params, Machine_State* cpu )
{
    cout << _binary_name << '\n';
    return true;
}

bool stackl_debugger::cmd_func( string& params, Machine_State* cpu )
{
    cout << _lst.current_func( cpu->IP ) << '\n';
    return true;
}

bool stackl_debugger::cmd_help( string& params, Machine_State* cpu )
{
    for( const debugger_command& cmd : _commands )
        cout << cmd.to_string() << '\n';
    return true;
}

bool stackl_debugger::cmd_restart( string& params, Machine_State* cpu )
{
    cpu->IP = 8;
    return false;
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
    uint32_t bpl = text_to_addr( break_point_text, cur_addr );
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
    uint32_t bpl = text_to_addr( break_point_text, cur_addr );
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
	    return _lst.addr_of_line( file_line[0], stoi( file_line[1] ) );
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

string stackl_debugger::var_to_string( Machine_State* cpu, string& var_text )
{
    int32_t val;

    string_utils::ltrim( var_text );

    if( string_utils::begins_with( var_text, "0x" ) && string_utils::is_number( var_text, 16, &val ) )
        return string_utils::to_hex( Get_Word( cpu, val ) );
    else if (string_utils::is_number( var_text, 10, &val ) )
        return std::to_string(Get_Word( cpu, val ) );

    bool address_of = false;
    if( var_text[0] == '&' )
    {
        address_of = true;
        var_text.erase( 0, 1 );
    }

    uint32_t indirection = 0;
    while( var_text[indirection++] == '*' ); //count the number of leading asterisks
    var_text.erase( 0, --indirection ); //remove them

    vector<string> var_fields = string_utils::string_split( var_text, '.' );

    vector<uint32_t> indexes = string_utils::strip_array_indexes( var_fields[0] );
    variable* var = _ast.var( _lst.current_func( cpu->IP ), var_fields[0] );

    if( var == nullptr )
        return "Variable not found in current scope";

    variable res = var->from_indexes( indexes, cpu );

    int32_t total_offset = res.offset();
    for( uint32_t i = 1; i < var_fields.size(); ++i )
    {
        if( res.is_struct() ) //the guy left of the '.' operator
        {
            indexes = string_utils::strip_array_indexes( var_fields[i] ); //strip ending brackets to get the 'true' var name
            var = res.decl()->var( var_fields[i] ); //ask the type of the left for the variable object of the guy on the right

            if( var == nullptr )
                return string( "'" ) + var_fields[i] + "' is not a field of type '" + res.type() + "'.";

            res = var->from_indexes( indexes, cpu );
            total_offset += res.offset();
        }
        else
        {
            return string( "'" ) + res.definition() + "' is not a struct type.";
        }
    }

    res.offset( total_offset ); //modify its offset by the total dist from the FP
    res = res.deref( indirection, cpu );

    if( address_of )
        return std::to_string( res.total_offset( cpu ) );
        //return string_utils::to_hex( res.total_offset( cpu ) );
    else return res.to_string( cpu );
}

void stackl_debugger::debug_check( Machine_State* cpu )
{
    if( !loaded() ) return;

    if( should_break( cpu->IP ) )
    {
        if( _opcode_debug )
        {
            cout << "Breakpoint hit on instruction " << opcode_to_string( cpu->IP, cpu ) << '\n';
        }
        else
        {
            string cur_file = _lst.current_file( cpu->IP );
	        cout << "Breakpoint hit on " << cur_file << ":" << _lst.line_of_addr( cur_file, cpu->IP ) << '\n';
            cout << get_nearby_lines( cpu->IP, 0) << '\n';
        }
	    query_user( cpu );
    }
}

void stackl_debugger::query_user( Machine_State* cpu )
{
    string input;
    _debugging = true;
    while( true )
    {
        cout << "(dbg) ";
        getline( cin, input );

        if( input == "" )
        {
            input = _prev_cmd;
            cout << "\e[A(dbg) " << _prev_cmd << '\n'; // "\e[A" moves the cursor up one line.
        }
        else
            _prev_cmd = input;

        //commands are always one word
        size_t idx = input.find_first_of( ' ' );
        string cmd, params;
        if( idx == string::npos )
        {
            cmd = input;
            params = "";
        }
        else
        {
            cmd = input.substr( 0, idx );
            params = input.substr( idx + 1 );
            string_utils::ltrim( params );
        }

        int32_t res = -1;
        //look at every command object and determine which one the user asked for
        for( const debugger_command& command : _commands )
        {
            if( command.called_by( cmd ) )
            {
                if( _opcode_debug && !command.allowed_in_opcode_mode() )
                {
                    cout << "You can't run this command without debug files available.\n";
                    break;
                }
                else
                {
                    res = command.run( params, cpu );
                    break; //stop iterating once we find our command
                }
            }
        }

        if( res == -1 ) //if res wasn't modified
            cout << "Unknown command.\n";
        else if( res == false ) //return false means resume executing code
            break;
        else continue; //otherwise prompt for input again
    }
    _debugging = false;
}

bool stackl_debugger::should_break( uint32_t cur_addr )
{
    if( _break_next_op )
    {
        _break_next_op = false;
        return true;
    }
    else if( find( _break_points.begin(), _break_points.end(), cur_addr ) != _break_points.end() )
        return true;
    else if( _stepping )
    {
        string cur_file = _lst.current_file( cur_addr );
        uint32_t cur_line = _lst.line_of_addr( cur_file, cur_addr );
        if( cur_line == UINT32_MAX ) //if we step into a location undefined in the debug file
            return false; //then don't break
        if( cur_line != _prev_line || cur_file != _prev_file )
        { //if our current step has changed in either line or file, break.
            _stepping = false;
            return true;
        }
    }
    return false; //otherwise dont break
}

string stackl_debugger::get_nearby_lines( uint32_t cur_addr, int32_t range )
{
    string cur_file = _lst.current_file( cur_addr );
    int cur_line = (int)_lst.line_of_addr( cur_file, cur_addr );
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
