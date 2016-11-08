#pragma once
#include "../machine.h"

#ifdef __cplusplus
extern "C" {
#endif

void dbg_load_info( Machine_State* cpu, const char* filename );
void dbg_check_break( Machine_State* cpu );

#ifdef __cplusplus
}
#endif
