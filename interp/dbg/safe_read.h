#pragma once
extern "C"
{
    #include "../machine.h"
}

int32_t safe_read_word( Machine_State* cpu, int32_t addr );
int32_t safe_read_byte( Machine_State* cpu, int32_t addr );
