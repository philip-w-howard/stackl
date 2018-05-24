#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "machine_def.h"
#include "opcodes.h"
#include "machine.h"
#include "vmem.h"
#include "loader.h"
#include "io_handler.h"
#include "dma_term.h"
#include "gen_io.h"
#include "pio_term_int.h"
#include "disk.h"
#include "formatstr.h"
#include "validversion.h"

#include "dbg/debugger_interface.h"

static int Do_Debug = 0;

void Loader_Debug()
{
    Do_Debug = 1;
}

// set DEBUG to "//" to turn off DEBUG
#define DEBUG(...) debug_print(__VA_ARGS__);
static void debug_print(int32_t location, const char *fmt, ...)
{
    if (!Do_Debug) return;

    va_list args;
    va_start(args, fmt);

    char format[200];
    sprintf(format, "Loading at %d: %s\n", location, fmt);
    vfprintf(stderr, format, args);
    va_end(args);
}

static void fatal_version_error()
{
    fprintf(stderr, ".slb assembled with incompatible version\n");
    fprintf(stderr, "Version must be at least %d.%d\n", 
            MIN_MAJOR_VERSION, MIN_MINOR_VERSION);
    exit(1);
}

static void compare_version(char *version)
{
    static const char *v_delims = "V.-\n";
    char *token;
    int version_num;

    token = strtok(version, v_delims);
    if (token != NULL)
    {
        version_num = atoi(token);
        if (version_num < MIN_MAJOR_VERSION) fatal_version_error();
        if (version_num > MAX_MAJOR_VERSION) fatal_version_error();
    }
    else
    {
        fatal_version_error();
    }

    token = strtok(NULL, v_delims);
    if (token != NULL)
    {
        version_num = atoi(token);
        if (version_num < MIN_MINOR_VERSION) fatal_version_error();
        if (version_num > MAX_MINOR_VERSION) fatal_version_error();
    }
    else
    {
        fatal_version_error();
    }
}

//*************************************************
// Load a binary file (.slb)
//
// Returns negative on failure, positive on success
int Load(const char *filename, int boot, int bp, int lp)
{
    int32_t top;
    int32_t value;
    int32_t addr;
    int32_t max_addr = 0;
    FILE *input = fopen(filename, "r");
    char string[256];
    char *token;
    const char *delims = " \n";
    int stack_size = 1000;

    Machine_State cpu;
    Get_Machine_State(&cpu);

    cpu.FLAG = 0;   // Want addresses to be absolute

    if (bp < 0)
        addr = cpu.BP;
    else
        addr = bp;

    if (lp < 0)
        top  = cpu.LP;
    else
        top = lp;

    if (input == NULL) 
    {
        strcpy(string, filename);
        strcat(string, ".slb");
        input = fopen(string, "r");
        if (input == NULL) return -1;
    }

    fgets(string, sizeof(string), input);
    token = strtok(string, delims);
    if (strcmp(token, "stackl") != 0)
    {
        fclose(input);
        return -2;
    }

    char *version = strtok(NULL, delims);
    compare_version(version);

    while (fgets(string, sizeof(string), input))
    {
        token = strtok(string, delims);
        if (strcmp(token, "begindata") == 0) 
        {
            break;
        }
        else if (strcmp(token, "stackl") == 0)
        {
            char *version = strtok(NULL, delims);
            compare_version(version);
        }
        else if (boot && strcmp(token, "feature") == 0)
        {
            token = strtok(NULL, delims);
            if (boot && strcmp(token, "gen_io") == 0)
            {
                Gen_IO_Init();
            }
            if (boot && strcmp(token, "pio_term") == 0)
            {
                PIO_T_Init();
            }
            else if (boot && strcmp(token, "dma_term") == 0)
            {
                DMA_T_Init();
            }
            else if (boot && strcmp(token, "disk") == 0)
            {
                Disk_Init();
            }
            else if (boot && strcmp(token, "inp") == 0)
            {
                IO_Enable_Inp();
            }
        }
        else if (strcmp(token, "stack_size") == 0)
        {
            token = strtok(NULL, delims);
            if (token != NULL) stack_size = atoi(token);
            if (stack_size < 0) stack_size = 0;
        }
    }

    while (fread(&value, 4, 1, input) == 1)
    {
        DEBUG(addr, "D: %d", value);
        Set_Word(&cpu, addr, value);
        addr += WORD_SIZE;
        max_addr = max_addr>addr ? max_addr : addr;
        if (max_addr > top)
        {
            //fprintf(stderr, "Memory overflow while loading\n");
            //exit(-1);

            fclose(input);
            return -3;
        }
    }

    // set the requested stack size in the first word above the code
    Set_Word(&cpu, addr, stack_size);

    dbg_load_info( &cpu, filename );
    Set_Debug_Info( cpu.debugger );

    fclose(input);
    return max_addr;
}

int Boot(const char *filename)
{
    int32_t top;
    Machine_State cpu;

    Get_Machine_State(&cpu);
    cpu.SP = cpu.LP - 8;
    cpu.FP = cpu.SP;
    Set_Machine_State(&cpu);

    top = Load(filename, 1, -1, -1);
    if (top <= 0) return 1;

    Get_Machine_State(&cpu);
    cpu.SP = top + WORD_SIZE;
    cpu.FP = cpu.SP;
    cpu.IP = cpu.BP + 2*WORD_SIZE;
    Set_Machine_State(&cpu);

    return 0;
}

int Boot_From_Disk()
{
    Machine_State cpu;
    int status;

    status = Disk_Load_Boot_Block();
    if (status != 0) 
    {
        Machine_Check(0x00000004, "Invalid boot block");

        // error booting is always fatal even if we're debugging
        exit(1);
    }

    Get_Machine_State(&cpu);
    cpu.SP = 1024;
    cpu.FP = cpu.SP;
    cpu.IP = 2*WORD_SIZE;
    Set_Machine_State(&cpu);

    return 0;
}
