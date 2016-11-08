#include "stackl_debugger.h"

#include <iostream>
#include <cmath>

stackl_debugger::stackl_debugger( const char* filename )
{
    string fname = filename;

    size_t idx =  fname.find_last_of( '.' );
    if( idx != string::npos )
        fname.erase( idx );

    try
    {
        _ast = abstract_syntax_tree( fname + ".ast" );
        _lst = asm_list( fname + ".lst" );
        _loaded = true;
    }
    catch( ... )
    {
        _loaded= false;
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
    //THIS IS NAIVE INMPLIMENTATION ONLY WORKS ON NORMAL VARAIBLES
    //no pointer deref/array index/direct address access
    variable* var = _ast.var( _lst.current_func( cpu->IP ), var_text );
    if( var != nullptr )
    {
	return var->to_string( cpu );
    }
    else
    {
    	return "Variable not found in current scope";
    }
}

void stackl_debugger::debug_check( Machine_State* cpu )
{
    if( !loaded() ) return;

    if( should_break( cpu->IP ) )
    {
        string cur_file = _lst.current_file( cpu->IP );
	cout << "Breakpoint hit on line " << _lst.line_of_addr( cur_file, cpu->IP ) << '\n';
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
                continue; //no text? do nothing

	    BREAKPOINT_RESULT res = add_breakpoint( params, cpu->IP );
	    uint32_t final_addr;
	    string file_name;
	    switch( res )
	    {
		case SUCCESS:
		    final_addr = text_to_addr( params, cpu->IP );
		    file_name = _lst.current_file( final_addr );
		    cout << "breakpoint added at " << file_name << ':' << _lst.line_of_addr( file_name, final_addr ) << '\n';
		    break;
		case DUPLICATE:
		    cout << "breakpoint already exists on that line.\n";
		    break;
		case NOT_FOUND:
		    cout << "couldn't find breakpoint location.\n";
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
	else if( cmd == "print" || cmd == "p" )
	{
            if( params.length() == 0 )
                continue; //no text? do nothing
	    cout << var_to_string( cpu, params ) << '\n';
	}
        else if( cmd == "help" )
        {
            cout << "[breakpoint|break|b] [line_num|file:line_num|func_name] - adds breakpoint\n"
                << "[print|p] [var_name|0xaddress] - prints the value of the variable\n"
                << "[continue|cont|c] - runs program until the next breakpoint\n"
                << "[list] - prints lines of code near the current execution line\n";
        }
        else if( cmd == "IP" )
        {
            cout << "Instruction Pointer: " << cpu->IP << '\n';
        }
        else if( cmd == "FP" )
        {
            cout << "Frame Pointer: " << cpu->FP << '\n';
        }
        else
        {
            cout << "huh?\n";
        }
    }
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
	    throw 0; //this is kinda dumb... but it keeps us from duplicating code?
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
