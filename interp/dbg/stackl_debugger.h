#pragma once
#include "abstract_syntax_tree.h"
#include "asm_list.h"
#include "debugger_command.h"

#include <string>
using std::string;
using std::stoi;
using std::to_string;
#include <vector>
using std::vector;
#include <algorithm>
using std::find;
#include <fstream>
using std::ifstream;
#include <unordered_map>
using std::unordered_map;
#include <chrono>
using std::chrono::high_resolution_clock;
using std::chrono::milliseconds;
using std::chrono::duration;
using std::chrono::duration_cast;

#include"string_utils.h"
#include "../machine.h"

class stackl_debugger
{

public:
    stackl_debugger( const char* filename );

    void debug_check( Machine_State* cpu  );
    void query_user( Machine_State* cpu );

    inline bool debugging() const { return get_flag( FLAG::DEBUGGING ); }
    inline bool loaded() const { return get_flag( FLAG::LOADED ); }
    inline bool opcode_mode() const { return get_flag( FLAG::OPCODE_DEBUG ); }
    inline string failure_reason() const { return _failure_reason; }

private:
    enum BREAKPOINT_RESULT
    {
        SUCCESS,
        NOT_FOUND,
        DUPLICATE
    };

    enum FLAG
    {
        LOADED,
        DEBUGGING,
        OPCODE_DEBUG,
        BREAK_NEXT_OP,
        STEP_OVER,
        STEP_INTO,
        RESUME_EXECUTING,
        TIMER
    };

    void set_flag( FLAG state, bool value );
    bool get_flag( FLAG state ) const;

    /*    Breakpoint text can be in the following formats:
    main.cpp:75 (filename:line_number)
    75    (line_number) -> this assumes current file
    main (function_name) -> no function overloading so this is all there is to it
    0x312312 (0xhex_address) -> breakpoint on an address
    */
    BREAKPOINT_RESULT add_breakpoint( const string& break_point_text, uint32_t cur_addr );
    BREAKPOINT_RESULT remove_breakpoint( const string& break_point_text, uint32_t cur_addr );

    inline void remove_all_breakpoints() { _break_points.clear(); }

    //is the current address in our list of break points?
    bool should_break( Machine_State* cpu );

    /* This will get the value of the var and return it as a printable string.

    var text can be in the following formats:
    0xAAFF (0Xhex_address) -> prints the 4 byte value at that memory location
    1414 (decimal address) -> prints the 4 byte value at that memory location

    variable_name -> just the name of a local/static/global var
        if variable_name is a pointer or array, it will print out the address that the variable holds
        if variable_name is a struct, it will recursively print out the fields of the struct, including contained structs
        if variable_name is a char pointer or char array, it will print out data as a string
        if variable_name is an array, it will print out every index of the array
    *pointer - dereferencing a pointer will print the value of the variable pointed to
        This supports multiple levels of indirection.
    array[X] will print the value of the variable at X index. This works with pointers or arrays.
    */
    string var_to_string( Machine_State* cpu, const string& var_text, bool prepend_def );

    //'list' in gdb.
    string get_nearby_lines( uint32_t cur_addr, int32_t range );

    //true if addr was added, false if it already exists
    bool add_breakpoint( uint32_t addr );
    //true if it was removed, false if it didnt exist
    bool remove_breakpoint( uint32_t addr );
    //converts a users breakpoint-formatted text into an instruction pointer
    uint32_t text_to_addr( const string& break_point_text, uint32_t cur_addr );
    //converts the instruction at addr to slasm text
    string opcode_to_string( uint32_t addr, Machine_State* cpu );
    //checks the compile time of the passed source code filename against its matching debug file.
    void check_compile_time( const string& binaryname, const string& filename );
    //returns true if the file exists
    bool file_exists( const string& filename );
    //asks the user if they would like to debug opcodes only
    void opcode_debug_mode( const string& filename);
    //gets the function that acts as the entry point of the program
    string get_init_func( Machine_State* cpu ) const;
    //gets a map of the watched variables that changed.
    unordered_map<string, string> changed_watches( Machine_State* cpu );
    //given what the user typed in, what's the text of the command they want to run
    string command_from_input( const string& input );
    //given what the user typed in, what's the text parameters to the command they want to run
    string params_from_input( const string& input );

    //this function populates the _commands field.
    void load_commands();
    vector<debugger_command> _commands;

    //each debugger command must have a function that is run when the command is invoked.
    void cmd_breakpoint( string& params, Machine_State* cpu );
    void cmd_removebreak( string& params, Machine_State* cpu );
    void cmd_print( string& params, Machine_State* cpu );
    void cmd_printi( string& params, Machine_State* cpu );
    void cmd_continue( string& params, Machine_State* cpu );
    void cmd_list( string& params, Machine_State* cpu );
    void cmd_next( string& params, Machine_State* cpu );
    void cmd_step( string& params, Machine_State* cpu );
    void cmd_locals( string& params, Machine_State* cpu );
    void cmd_globals( string& params, Machine_State* cpu );
    void cmd_funcs( string& params, Machine_State* cpu );
    void cmd_statics( string& params, Machine_State* cpu );
    void cmd_exit( string& params, Machine_State* cpu );
    void cmd_IP( string& params, Machine_State* cpu );
    void cmd_FLAG( string& params, Machine_State* cpu );
    void cmd_FP( string& params, Machine_State* cpu );
    void cmd_BP( string& params, Machine_State* cpu );
    void cmd_LP( string& params, Machine_State* cpu );
    void cmd_SP( string& params, Machine_State* cpu );
    void cmd_file( string& params, Machine_State* cpu );
    void cmd_program( string& params, Machine_State* cpu );
    void cmd_func( string& params, Machine_State* cpu );
    void cmd_help( string& params, Machine_State* cpu );
    void cmd_breakpointi( string& params, Machine_State* cpu );
    void cmd_removebreaki( string& params, Machine_State* cpu );
    void cmd_nexti( string& params, Machine_State* cpu );
    void cmd_restart( string& params, Machine_State* cpu );
    void cmd_watch( string& params, Machine_State* cpu );
    void cmd_removewatch( string& params, Machine_State* cpu );
    void cmd_watches( string& params, Machine_State* cpu );
    void cmd_backtrace( string& params, Machine_State* cpu );
    void cmd_up( string& params, Machine_State* cpu );
    void cmd_down( string& params, Machine_State* cpu );
    void cmd_timer( string& params, Machine_State* cpu );
    void cmd_clear( string& params, Machine_State* cpu );

    string _binary_name = ""; //the name of the current binary being debugged
    vector<uint32_t> _break_points; //list of instruction pointers to break at
    unordered_map<string, string> _watches;
    abstract_syntax_tree _ast; //the ast of the program currently being executed
    asm_list _lst; //the data structure containing relationships between instructions and source code

    string _prev_cmd = ""; //the text of the command the user most recently ran

    uint32_t _prev_line = 0;
    string _prev_file = "";
    uint32_t _prev_fp = 0; //used for step commands

    typedef struct context_s
    {
        int32_t IP, FP;
    } context_t;

    vector<context_t> _context_history; //keeps track of previous frame pointers when calling 'up'

    string _failure_reason; //the reason the debugger couldn't load. This is empty if the debugger loaded successfully.

    uint64_t _flags = 0; //the flags of the debugger itself

    //used if timer is enabled
    high_resolution_clock::time_point _start_time;
    uint32_t _instructions_executed = 0;
};
