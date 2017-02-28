#include "stackl_debugger.h"

#include <iostream>
#include <cmath>
#include <cstdio>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <cstdlib>

extern "C"
{
    #include "../vmem.h"
}

stackl_debugger::stackl_debugger( const char* filename )
{
    string fname = filename;

    size_t idx =  fname.find_last_of( '.' );
    if( idx != string::npos )
        fname.erase( idx );

    try
    {
        _ast = abstract_syntax_tree( fname + ".ast" );
        _lst = asm_list( fname + ".dbg" );

        struct stat attrib;
        stat( filename, &attrib );

        if( attrib.st_mtime > _ast.compile_time() )
        {
            _loaded = false;
            _failure_reason = "Debug files out of date.";
        }
        else
        {
            _loaded = true;
        }
    }
    catch( exception& ex )
    {
        _loaded = false;
        _failure_reason = ex.what();
    }
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

//this is a private func the user never calls.
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

string stackl_debugger::var_to_string( Machine_State* cpu, const string& var_text )
{
    //array index does not work currently

    int32_t val;
    if( string_utils::begins_with( var_text, "0x" ) && string_utils::is_number( var_text, 16, &val ) )
    {
        return string_utils::to_hex( Get_Word( cpu, val ) );
    }

    string txt = var_text;

    bool address_of = false;
    if( txt[0] == '&' )
    {
        address_of = true;
        txt.erase( 0, 1 );
    }

    uint32_t indirection = 0;
    while( txt[indirection++] == '*' ); //count the number of leading asterisks
    txt.erase( 0, --indirection ); //remove them

    vector<string> var_fields = string_utils::string_split( txt, '.' );
    variable* var = _ast.var( _lst.current_func( cpu->IP ), var_fields[0] );

    if( var == nullptr )
        return "Variable not found in current scope";

    int32_t total_offset = var->offset();
    for( uint32_t i = 1; i < var_fields.size(); ++i )
    {
        if( var->is_struct() && !var->is_pointer() && !var->is_array() )
        {
            var = var->decl()->var(var_fields[i]);
            total_offset += var->offset();
        }
        else
        {
            return string( "'" ) + var->definition() + "' is not a struct type.";
        }
    }

    if( var != nullptr )
    {
        variable res = *var; //create copy of var
        res.offset( total_offset ); //modify its offset by the total dist from the FP
        try
        {
            res = res.deref( indirection, cpu );
        }
        catch( const char* msg )
        {
            return msg;
        }

        if( address_of )
            return string_utils::to_hex( res.address_of( cpu ) );
        else return res.to_string( cpu );
    }
    else return "Variable not found in current scope";
}

void stackl_debugger::debug_check( Machine_State* cpu )
{
    if( !loaded() ) return;

    if( should_break( cpu->IP ) )
    {
        string cur_file = _lst.current_file( cpu->IP );
	    cout << "Breakpoint hit on " << cur_file << ":" << _lst.line_of_addr( cur_file, cpu->IP ) << '\n';
	    query_user( cpu );
    }
}

void stackl_debugger::query_user( Machine_State* cpu )
{
    string input;
    cout << "Enter command. 'help' for help.\n";
    while( true )
    {
        getline( cin, input );

        if( input == "" )
        {
            input = _prev_cmd;
            cout << '\b' << _prev_cmd << '\n';
        }
        else
            _prev_cmd = input;

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
        }

	    if( cmd == "breakpoint" || cmd == "break" || cmd == "b" )
	    {
            if( params.length() == 0 )
            {
                cout << "Specifiy a breakpoint location.\n";
                continue;
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
	    }
        else if( cmd == "removebreak" || cmd == "rbreak" || cmd == "rb" )
        {
            if( params.length() == 0 )
                continue;

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
        }
        else if( cmd == "continue" || cmd == "c" || cmd == "cont" )
        {
            break;
        }
        else if( cmd == "list" )
        {
            int32_t range;
            if( params.length() == 0 )
                cout << get_nearby_lines( cpu->IP, 2 );
            else if( string_utils::is_number( params, 10, &range ) )
	        cout << get_nearby_lines( cpu->IP, range );
            else
                cout << "[list] [optional range]\n";
	    }
        else if( cmd == "locals" || cmd == "listlocals" )
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
                if( _lst.addr_of_func( params ) != UINT32_MAX )
                {
                    cout << _ast.all_locals( params );
                }
            }
        }
        else if( cmd == "globals" || cmd == "listglobals" )
        {
            cout << _ast.all_globals();
        }
        else if( cmd== "funcs" || cmd == "functions" || cmd == "listfuncs" || cmd == "listfunctions" )
        {
            cout << _ast.all_funcs();
        }
	    else if( cmd == "print" || cmd == "p" )
	    {
            if( params.length() == 0 )
                continue; //no text? do nothing
	        cout << var_to_string( cpu, params ) << '\n';
	    }
        else if( cmd == "next" || cmd == "n" )
        {
            _prev_file = _lst.current_file( cpu->IP );
            _prev_line = _lst.line_of_addr( _prev_file, cpu->IP );
            _stepping = true;
            break;
        }
        else if( cmd == "help" )
        {
            cout << "[breakpoint|break|b] [line_num|file:line_num|func_name] - Adds breakpoint\n"
                << "[removebreak|rbreak|rb] [line_num|file:line_num|func_name] - Removes breakpoint\n"
                << "[print|p] [var_name|0xaddress] - Prints the value of the variable\n"
                << "[continue|cont|c] - Runs program until the next breakpoint\n"
                << "[list] optional[line_count]- Prints lines of with within N lines\n"
                << "[next|n] - Executes the current line of code\n"
                << "[locals|listlocals] optional[func_name] - Print all local vars in current or specified function\n"
                << "[globals|listglobals] - Print all global variables\n"
                << "[funcs|functions|listfuncs|listfunctions] - Print all functions\n"
                << "[exit|quit] - Exits current program\n"
                << "[IP] - Prints the instruction pointer\n"
                << "[FP] - Prints the frame pointer\n"
                << "[BP] - Prints the base pointer\n"
                << "[LP] - Prints the limit pointer\n"
                << "[SP] - Prints the stack pointer\n"
                << "Hitting enter repeats the last command entered.\n";
        }
        else if( cmd == "exit" || cmd == "quit" )
            exit( EXIT_SUCCESS );
        else if( cmd == "IP" )
            cout << "Instruction Pointer: " << cpu->IP << '\n';
        else if( cmd == "FP" )
            cout << "Frame Pointer: " << string_utils::to_hex( cpu->FP ) << '\n';
        else if( cmd == "BP" )
            cout << "Base Pointer: " << string_utils::to_hex( cpu->BP ) << '\n';
        else if( cmd == "LP" )
            cout << "Limit Pointer: " << string_utils::to_hex( cpu->LP ) << '\n';
        else if( cmd == "SP" )
            cout << "Stack Pointer: " << string_utils::to_hex( cpu->SP ) << '\n';
        else
        {
            cout << "huh?\n";
        }
    }
}

bool stackl_debugger::should_break( uint32_t cur_addr )
{
    if( find( _break_points.begin(), _break_points.end(), cur_addr ) != _break_points.end() )
        return true;
    else if( _stepping )
    {
        string cur_file = _lst.current_file( cur_addr );
        uint32_t cur_line = _lst.line_of_addr( cur_file, cur_addr );
        if( cur_line != _prev_line || cur_file != _prev_file )
        {
            _stepping = false;
            return true;
        }
    }
    return false;
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
	    file.open( cur_file );
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
	    ++i;
    }
    return ret;
}
