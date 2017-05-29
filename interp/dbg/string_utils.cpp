#include "string_utils.h"
#include <cstdio>
#include <stdexcept>
using std::runtime_error;
#include <iostream>
using std::cout;
using std::cin;
extern "C"
{
    #include "../pio_term_int.h"
}

vector<string> string_utils::string_split( const string& s, const char delimiter )
{
    size_t start = 0;
    size_t end = s.find_first_of( delimiter );

    vector<string> output;

    while( end <= string::npos )
    {
        output.emplace_back( s.substr( start, end - start ) );

        if( end == string::npos )
            break;

        start = end + 1;
        end = s.find_first_of( delimiter, start );
    }

    return output;
}

//http://stackoverflow.com/a/217605
void string_utils::ltrim( string &s )
{
    s.erase( s.begin(), std::find_if( s.begin(), s.end(), std::not1( std::ptr_fun<int, int>( std::isspace ) ) ) );
}

bool string_utils::is_number( const string& num_text, int32_t base, int32_t* res )
{
    char* p;
    if( res != nullptr )
        *res = strtol( num_text.c_str(), &p, base );
    else
        strtol( num_text.c_str(), &p, base );
    return *p == 0;
}

bool string_utils::begins_with( const string & input, const string & begin )
{
    return input.length() >= begin.length() && input.substr( 0, begin.length() ) == begin;
}

bool string_utils::ends_with( const string & input, const string & end )
{
    return input.length() >= end.length() && input.substr( input.length() - end.length() ) == end;
}

string string_utils::to_hex( int32_t num )
{
    char buf[32];
    sprintf( buf, "%X", num );
    return string( "0x" ) + buf;
}

vector<uint32_t> string_utils::strip_array_indexes( string& var )
{
    vector<string> pieces = string_utils::string_split( var, '[' );
    var = pieces[0];

    vector<uint32_t> idxs;
    try
    {
        for( uint32_t i = 1; i < pieces.size(); ++i ) //convert the int between each open/close bracket to an int, store it.
            idxs.push_back( (uint32_t)stoi( string( pieces[i].begin(), pieces[i].end() - 1 ) ) );
    }
    catch( ... ) {}
    return idxs;
}

uint32_t string_utils::strip_indirection( string& var )
{
    uint32_t indirection = 0;
    while( var[indirection++] == '*' ); //count the number of leading asterisks
    var.erase( 0, --indirection ); //remove them
    return indirection;
}

int32_t string_utils::to_int( const string& text )
{
    int32_t addr;

    if( string_utils::begins_with( text, "0x" ) )
    {
        if( !string_utils::is_number( text, 16, &addr ) )
        {
            throw runtime_error( "Value is not a number" );
        }
    }
    else if( !string_utils::is_number( text, 10, &addr ) )
    {
        throw runtime_error( "Value is not a number" );
    }

    return addr;
}

bool string_utils::get_yesno( const string& question )
{
    int32_t previous_mode = pio_set_nonblock( 0 );
    char c;
    cout << question << " [Y/n] ";
    cin.get( c );
    cin.ignore( INT32_MAX, '\n' ); //ignore 2^31 characters up until the next newline
    pio_set_nonblock( previous_mode );
    return c == 'y' || c == 'Y';
}

string string_utils::change_ext( const string& filename, const string& new_ext )
{
    return filename.substr( 0, filename.find_last_of( "." ) ) + new_ext;
}