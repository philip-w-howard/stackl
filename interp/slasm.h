// ********************************************************
// slasm.h
//
// definitions of opcode values
//
// This is a separate include file so that both slasm and the debugger have
// access to this info
//
// Author: Philip Howard 
// Email: phil.w.howard@gmail.com
//
typedef struct
{
    const char *op_name;    // text name of opcode
    int  num_params;        // number of instruction stream params for opcode
} opcode_t;

// Array position indicates numeric value of opcode.
// Actual opcode values should not be depended on. They can change in
// future releases
static opcode_t op_list[] =
{
    {"nop", 0},
    {"plus", 0},
    {"minus", 0},
    {"times", 0},
    {"divide", 0},
    {"mod", 0},
    {"eq", 0},
    {"ne", 0},
    {"gt", 0},
    {"lt", 0},
    {"ge", 0},
    {"le", 0},
    {"and", 0},
    {"or", 0},
    {"not", 0},
    {"swap", 0},
    {"dup", 0},
    {"halt", 0},
    {"pop", 0},
    {"return", 0},
    {"returnv", 0},
    {"neg", 0},
    {"pushcvarind", 0},
    {"outs", 0},
    {"inp", 0},
    {"pushfp", 0},
    {"jmpuser", 1},
    {"trap", 0},
    {"rti", 0},
    {"calli", 0},
    {"pushreg", 1},
    {"popreg", 1},
    {"band", 0},
    {"bor", 0},
    {"bxor", 0},
    {"shiftl", 0},
    {"shiftr", 0},
    {"pushvarind", 0},
    {"popcvarind", 0},
    {"popvarind", 0},
    {"comp", 0},
    {"push", 1},
    {"jump", 1},
    {"jumpe", 1},
    {"pushvar", 1},
    {"popvar", 1},
    {"adjsp", 1},
    {"popargs", 1},
    {"call", 1},
    {"pushcvar", 1},
    {"popcvar", 1},
    {"trace_on", 0},
    {"trace_off", 0},
    {"clid", 0},
    {"seid", 0},
    {"illegal", 0}
};


