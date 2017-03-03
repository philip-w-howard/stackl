#pragma once
#include "../machine.h"

#ifdef __cplusplus
extern "C" {
#endif

void dbg_load_info( Machine_State* cpu, const char* filename );
void dbg_check_break( Machine_State* cpu );
int32_t dbg_machine_check( int32_t code, Machine_State* cpu );
void dbg_enable();

#ifdef __cplusplus
}
#endif
