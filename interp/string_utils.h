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
	static bool is_number( const string& num_text, int base = 10, int* res = nullptr );
};