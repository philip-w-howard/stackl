#include "asm_list.h"

#include <sstream>
using std::stringstream;
#include <algorithm>
using std::find;
#include <fstream>
using std::ifstream;

asm_list::asm_list( const string& filename )
{
    ifstream infile( filename );
    string line, token;
    uint32_t old_line_num = UINT32_MAX;
    string old_func_name = "";
    bool symbol_table = false;

    while( getline( infile, line ) )
    {
        stringstream ss( line );
        ss >> token;
        if( token == "Source:" )
        {
            string cur_func, cur_file;
            uint32_t ip, line_num;
            ss >> ip >> line_num >> cur_func >> cur_file;
            if( line_num != old_line_num && !cur_func.empty() && !cur_file.empty() )
            {
                auto& f = _file_and_line_to_addr[cur_file];
                f.push_back( addr_line_t{ ip, line_num } );
                old_line_num = line_num;
                if( old_func_name == "" || old_func_name != cur_func )
                {
                    _func_to_addr.push_back( func_addr_t{ ip, cur_func } );
                    old_func_name = cur_func;
                }
            }
        }
        else if (token == "FILE:")
        {
            string fname;
            ss >> fname;
            size_t idx = fname.find_last_of( '.' );
            if ( idx != string::npos )
                fname.erase( idx );
            _file_list.insert(fname + ".ast");
        }
        else if( symbol_table )
        {
            string global_name;
            ss >> global_name;
            _global_offsets[global_name] = stoi( token );

        }
        else if( token == "Symbol" ) //"Symbol Table"
        {
            symbol_table = true;
        }
    }
}

uint32_t asm_list::addr_of_func( const string & func_name )
{
    for( const func_addr_t& f_a : _func_to_addr )
        if( f_a.func_name == func_name )
            return f_a.addr;
    return UINT32_MAX;
}

//in this function we're looking for the line number that is the
//smallest amount BIGGER THAN line_number.
//aka: if we have { 12, 13, 17, 18 } and line_number is 14,
//return the address of line 17.
uint32_t asm_list::addr_of_line( const string & filename, uint32_t line_number )
{
    auto file_find = _file_and_line_to_addr.find( filename );
    if( file_find != _file_and_line_to_addr.end() )
    {
        uint32_t dif = UINT32_MAX; //we won't be able to debug a file with 2^32 empty lines :(
        uint32_t cur_best_line_addr = UINT32_MAX;
        for( addr_line_t& al_t : file_find->second )
        {
            if( al_t.line_num == line_number )
                return al_t.addr; //if we have the line obviously just return it

            if( al_t.line_num > line_number ) //is it a possible candidate?
            {
                if( ( al_t.line_num - line_number ) < dif ) //is it a new best choice?
                {
                    //record address in case this turns out to be the best choice
                    cur_best_line_addr = al_t.addr;
                    //this operation will always be positive,
                    //since we know that alt.line_num is bigger than line_number
                    dif = al_t.line_num - line_number;
                }
            }
        }
        //return our best case, since we didnt find it if we got here
        return cur_best_line_addr;
    }
    else return UINT32_MAX;
}

uint32_t asm_list::line_of_addr( const string& filename, uint32_t address )
{
    auto file_find = _file_and_line_to_addr.find( filename );
    if( file_find != _file_and_line_to_addr.end() )
    {
        uint32_t cur_best_line_num = UINT32_MAX;
        for( addr_line_t& al_t : file_find->second )
        { //If the address passed has multiple lines we return the smallest line number
            if( al_t.addr == address && al_t.line_num < cur_best_line_num )
            {
                cur_best_line_num = al_t.line_num;
            }
        }
        if( cur_best_line_num != UINT32_MAX )
        {   //return our best case, since we didnt find it if we got here
            return cur_best_line_num;
        }
        else
        {
            uint32_t dif = UINT32_MAX;
            for( addr_line_t& al_t : file_find->second )
            { //if the address passed isnt a registered line number, find the most recent line number update
                if( address > al_t.addr )
                {
                    if( address - al_t.addr < dif )
                    {
                        dif = address - al_t.addr;
                        cur_best_line_num = al_t.line_num;
                    }
                }
            }
            return cur_best_line_num;
        }
    }
    else return UINT32_MAX;
}

string asm_list::current_func( uint32_t cur_addr ) const
{
    func_addr_t* prev = nullptr;
    for( const func_addr_t& f_a : _func_to_addr )
    { //once we pass the address that was our lower bound, return the one we were looking at previously
        if( f_a.addr > cur_addr && prev != nullptr )
            return prev->func_name;
        else
            prev = const_cast<func_addr_t*>(&f_a);
    }
    if( prev == nullptr )
        return "";
    else return prev->func_name; //if we get to the end then our last function was the guy we want
}

string asm_list::current_file( uint32_t cur_addr ) const
{
    string best_filename = "unknown";
    uint32_t dif = INT32_MAX;
    for( const auto& f_dict : _file_and_line_to_addr )
    {
        for( const addr_line_t& alt : f_dict.second )
        {
            if( alt.addr == cur_addr )
                return f_dict.first; //filename
            else if( alt.addr > cur_addr )
            {
                if( alt.addr - cur_addr < dif )
                {
                    dif = alt.addr - cur_addr;
                    best_filename = f_dict.first;
                }
            }
        }
    }
    return best_filename;
}