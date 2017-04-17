#pragma once

#include <string>
using std::string;
using std::getline;
#include <vector>
using std::vector;
#include <unordered_map>
using std::unordered_map;
#include <unordered_set>
using std::unordered_set;

#include "string_utils.h"

class asm_list
{
public:
    asm_list() {}
    asm_list( const string& filename );

    uint32_t addr_of_func( const string& func );
    uint32_t addr_of_line( const string& filename, uint32_t line_number );
    uint32_t line_of_addr( const string& filename, uint32_t address );

    string current_func( uint32_t cur_addr ) const;
    string current_file( uint32_t cur_addr ) const;

    unordered_map<string, int32_t>& global_offsets() { return _global_offsets; }
    unordered_set<string>& file_list() { return _file_list; }


private:

    typedef struct
    {
        uint32_t addr;
        string func_name;
    } func_addr_t;

    typedef struct
    {
        uint32_t addr;
        uint32_t line_num;
    } addr_line_t;

    uint32_t _max_ip = 0;
    vector<func_addr_t> _func_to_addr;
    unordered_map<string, vector<addr_line_t>> _file_and_line_to_addr;
    unordered_map<string, int32_t> _global_offsets;
    unordered_set<string> _file_list;
};
