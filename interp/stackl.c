#include <getopt.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "opcodes.h"
#include "loader.h"
#include "io_handler.h"
#include "dma_term.h"
#include "gen_io.h"
#include "pio_term_int.h"
#include "disk.h"
#include "timer.h"
#include "../version.h"
#include "dbg/debugger_interface.h"

// Use PATH_MAX from limits.h?
//
static char Input_File[256] = "";
static int Memory_Size = 0;
static int Use_Disk = 0;        // 1
static int Boot_Disk = 0;       // 1
static int Use_PIO_Term = 0;    // 1
static int Use_Gen_IO = 0;    // 1
static int Use_DMA_Term = 0;
static int Use_Inp_Instr = 0;

static const char *HELP_STR =
 "stackl\n"
 "    -b --boot         Enable the disk; load and execute Block 0\n"
 "    -d --dma_term     Enable the DMA terminal device\n"
 "    -D --no_disk      Disable the disk device\n"
 "    -G --gen_io       Enable the General IO device\n"
 "    -g --debug --dbg  Enable debugging\n"
 "    -h --help         Print this help message and exit\n"
 "    -i --inp          Enable the INP instruction\n"
 "    -I <n>            Specify the maximum number of instructions to execute\n"
 "    --max_inst <n>    Specify the maximum number of instructions to execute\n"
 "    -l --loader       Print additional information while loading programs\n"
 "    -M <size>         Set the memory size for the virtual machine\n"
 "    --memory <size>   Set the memory size for the virtual machine\n"
 "    -n --nopio_term   Disable the PIO terminal device\n"
 "    -o --opcodes      Write an instruction trace to stderr\n"
 "    -p --pio_term     Enable the PIO terminal device\n"
 "    -v --version      Print the interpreter version number then exit\n"
 "\n";

 //"stackl [-boot] [-dma_term] [-inp] [-gen_io] [-[no]pio_term] [-nodisk]\n"
 //"       [-opcodes] [-help] [-version] [-loader]\n"
 //"       [-M<mem size>] [-N<num instr>] [-dbg]\n";

void Process_Args(int argc, char **argv)
{
    static struct option long_opts[] = {
        {"boot",        no_argument, 0, 'b'},
        {"dma_term",    no_argument, 0, 'd'},
        {"nodisk",      no_argument, 0, 'D'},
        {"gen_io",      no_argument, 0, 'G'},
        {"debug",       no_argument, 0, 'g'},
        {"dbg",         no_argument, 0, 'g'},
        {"help",        no_argument, 0, 'h'},
        {"inp",         no_argument, 0, 'i'},
        {"max_inst",    required_argument, 0, 'I'},
        {"loader",      no_argument, 0, 'l'},
        {"memory",      required_argument, 0, 'M'},
        {"nopio_term",  no_argument, 0, 'n'},
        {"opcodes",     no_argument, 0, 'o'},
        {"pio_term",    no_argument, 0, 'p'},
        {"version",     no_argument, 0, 'v'},
        {0, 0, 0, 0}
    };

    static char *single_opts = "bdDgGhiI:lM:nopv";
    int opt;
    int opt_index;

    opt = getopt_long_only(argc, argv, single_opts, long_opts, &opt_index);
    while (opt != -1)
    {
        switch (opt)
        {
            case 'b':
                Use_Disk = 1;
                Boot_Disk = 1;
                break;
            case 'd':
                Use_Gen_IO = 0;
                Use_PIO_Term = 0;
                Use_DMA_Term = 1;
                break;
            case 'D':
                Use_Disk = 0;
                break;
            case 'g':
                dbg_enable();
                break;
            case 'G':
                Use_PIO_Term = 0;
                Use_DMA_Term = 0;
                Use_Gen_IO = 1;
                break;
            case 'h':
                printf("%s", HELP_STR);
                exit(0);
                break;
            case 'i':
                Use_Inp_Instr = 1;
                break;
            case 'I':
                Set_Max_Instr(atoi(optarg));
                break;
            case 'l':
                Loader_Debug();
                break;
            case 'M':
                Memory_Size = atoi(optarg);
                break;
            case 'n':
                Use_PIO_Term = 0;
                break;
            case 'o':
                Opcodes_Debug();
                break;
            case 'p':
                Use_Gen_IO = 0;
                Use_PIO_Term = 1;
                Use_DMA_Term = 0;
                break;
            case 'v':
                printf("stackl %s %s %s\n", VERSION, __DATE__, __TIME__);
                exit(0);
                break;
            default:
                printf("Unrecognized option\n");
                printf("%s", HELP_STR);
                exit(-1);
                break;
        }

        opt = getopt_long_only(argc, argv, single_opts, long_opts, &opt_index);
    }

    // handle arguments without "-". Namely the filename
    if (optind < argc)
    {
        // assume input file name
        strcpy(Input_File, argv[optind]);

        // If we're given a file to run, we won't boot from the disk
        Boot_Disk = 0;
    }
}
int main(int argc, char **argv)
{

    int result;

    Process_Args(argc, argv);

    Init_Machine(Memory_Size);
    Init_IO(Use_Inp_Instr);
    Timer_Init();

    if (Use_Gen_IO)
    {
        Gen_IO_Init();
    }

    if (Use_PIO_Term)
    {
        PIO_T_Init();
    }

    if (Use_DMA_Term)
    {
        DMA_T_Init();
    }

    if (Use_Disk)
    {
        Disk_Init();
    }

    if (Boot_Disk && Use_Disk)
    {
        Boot_From_Disk();
    }
    else if (strlen(Input_File) == 0)
    {
        printf("Need to specify an executable file\n");
        return 1;
    }
    else
    {
        result = Boot(Input_File);
        if (result != 0)
        {
            printf("Unable to execute %s\n", Input_File);
            return 3;
        }
    }

    Machine_Execute();

    return 0;
}
