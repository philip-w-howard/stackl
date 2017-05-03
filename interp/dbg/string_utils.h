#pragma once
#include <vector>
using std::vector;
#include <string>
using std::string;
#include <algorithm>
#include <functional>
#include <cctype>
#include <locale>

class string_utils
{
public:
	static vector<string> string_split( const string& s, const char delimiter );
	static void ltrim( string &s );
	static bool is_number( const string& num_text, int32_t base = 10, int32_t* res = nullptr );
	static bool begins_with( const string& input, const string& begin );
	static bool ends_with( const string& input, const string& end );
	static string to_hex( int32_t num );
	static vector<uint32_t> strip_array_indexes( string& var );
	static uint32_t strip_indirection( string& var );
	static int32_t to_int( const string& text );
    static bool get_yesno( const string& question );
};
