#include "stackl_debugger.h"

stackl_debugger::stackl_debugger( const string& file_no_ext ): _ast( file_no_ext + ".ast" ), _lst( file_no_ext + ".lst" ) {}

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

	if( string_utils::is_number( break_point_text, 16, &res ) )
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

string stackl_debugger::var_to_string( uint32_t cur_addr, const string & var_text, uint32_t frame_pointer )
{
	//THIS IS NAIVE INMPLIMENTATION ONLY WORKS ON NORMAL VARAIBLES
	//no pointer deref/array index/direct address access
	variable* var = _ast.var( _lst.current_func( cur_addr ), var_text );
	if( var != nullptr )
	{
		return var->to_string( frame_pointer );
	}
	else
	{
		return "Variable not found in current scope";
	}
}

void stackl_debugger::debug_check( uint32_t instruction_pointer, uint32_t frame_pointer )
{
	if( should_break( instruction_pointer ) )
	{
		cout << "Breakpoint hit on line " << _lst.line_of_addr( _lst.current_file( instruction_pointer ), instruction_pointer ) << '\n';
		query_user( instruction_pointer, frame_pointer );
	}
}

void stackl_debugger::query_user( uint32_t instruction_pointer, uint32_t frame_pointer )
{
	string input;
	cout << "Enter command. -help for help.\n";
	while( true )
	{
		getline( cin, input );

		if( input == "-help" )
			cout << "that doesnt work yet geez dude\n";

		if( input.substr( 0, 5 ) == "break" )
		{
			input.erase( 0, 6 );
			BREAKPOINT_RESULT res = add_breakpoint( input, instruction_pointer );
			switch( res )
			{
				case SUCCESS:
					cout << "breakpoint added.\n";
					break;
				case DUPLICATE:
					cout << "breakpoint already exists on that line.\n";
					break;
				case NOT_FOUND:
					cout << "couldn't find breakpoint location.\n";
					break;
			}
		}
		else if( input == "continue" )
		{
			break;
		}
		else if( input == "list" )
		{
			cout << get_nearby_lines( instruction_pointer );
		}
		else if( input.substr( 0, 5 ) == "print" )
		{
			input.erase( 0, 6 );
			cout << var_to_string( instruction_pointer, input, frame_pointer ) << '\n';
		}
	}
}

string stackl_debugger::get_nearby_lines( uint32_t cur_addr )
{
	string cur_file = _lst.current_file( cur_addr );
	uint32_t cur_line = _lst.line_of_addr( cur_file, cur_addr );
	string ret = "";
	string line;
	uint32_t i = 0;
	ifstream file;
	try
	{
		file.open( cur_file );
		if( !file.is_open() )
			throw 0; //this is kinda dumb... but it keeps us from duplicating code?
	}
	catch( ... )
	{
		return string( "Unable to open " ) + cur_file;
	}

	while( getline( file, line ) )
	{
		if( i >= cur_line - 2 && i <= cur_line + 2 )
			ret += to_string( i ) + ". " + line + '\n';
		++i;
	}
	return ret;
}