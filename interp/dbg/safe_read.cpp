#include "safe_read.h"

extern "C"
{
    #include "../vmem.h"
}

#include <stdexcept>
using std::runtime_error;
#include <string>
using std::string;
#include <unordered_map>
using std::unordered_map;

static unordered_map<int32_t, string> error_messages =
{
    { MC_ILLEGAL_INST, "Illegal instruction" },
    { MC_ILLEGAL_ADDR, "Illegal address" },
    { MC_HW_FAILURE, "Hardware failure" },
    { MC_HW_WARNING, "Hardware warning" },
    { MC_PROT_INST, "Protected instruction" }
};

int32_t safe_read_word( Machine_State* cpu, int32_t addr )
{
    Machine_Check_Happened(); //clear the previous machine check state, we don't care about it
    int32_t val = Get_Word( cpu, addr );
    int32_t mch = Machine_Check_Happened();
    if( mch == 0 ) //no machine check
        return val;

    //otherwise we're gunna build an error message
    string msg = "";
    for( const auto& pair : error_messages )
        if( pair.first & mch ) //if the bit for that error is set
            msg += pair.second + ", "; //append its string to the message
    msg.erase( msg.end() - 2, msg.end() ); //remove the last ", "
    throw runtime_error( msg );
}

int32_t safe_read_byte( Machine_State* cpu, int32_t addr )
{
    Machine_Check_Happened(); //clear the previous machine check state, we don't care about it
    int32_t val = Get_Byte( cpu, addr );
    int32_t mch = Machine_Check_Happened();
    if( mch == 0 ) //no machine check
        return val;

    //otherwise we're gunna build an error message
    string msg = "";
    for( const auto& pair : error_messages )
        if( pair.first & mch )
            msg += pair.second + ", ";
    msg.erase( msg.end() - 2, msg.end() ); //remove the last ", "
    throw runtime_error( msg );
}
