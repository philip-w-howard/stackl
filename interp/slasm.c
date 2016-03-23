#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../version.h"

static char g_Input_File[256] = "";
static int g_Memory_Size = 65536;
static int g_Num_Errors = 0;
static int g_Line_Num = 0;
static int* g_Memory;
static int* g_Data_Memory;
static int g_Memory_Index;
static int g_Data_Memory_Index;
static int g_Use_Data = 0;

typedef struct
{
    char *op_name;
    int  num_params;
} opcode_t;

static opcode_t op_list[] =
{
    {"nop", 0},
    {"plus", 0},
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
    {"popcvarind", 0},
    {"outs", 0},
    {"inp", 0},
    {"pushfp", 0},
    {"setmode", 1},
    {"jmpuser", 1},
    //{"traptoc", 0},
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
    {"popvarind", 0},
    {"popvarind", 0},
    {"comp", 0},
    {"push", 1},
    {"jump", 1},
    {"jumpe", 1},
    {"pushvar", 1},
    {"popvar", 1},
    {"adjsp", 1},
    {"call", 1},
    {"pushcvar", 1},
    {"popcvar", 1},
    {"traceon", 0},
    {"traceoff", 0},
    {"illegal", 0}
};

typedef struct
{
    char label[50];
    int  is_data;
    int  offset;
} label_def_t;

static label_def_t *g_Label_Defs = NULL;
static label_def_t *g_Label_Refs = NULL;
static int g_Num_Label_Defs = 0;
static int g_Num_Label_Refs = 0;
static int g_Label_Defs_Extent = 0;
static int g_Label_Refs_Extent = 0;

void add_label_def(char *label)
{
    if (g_Num_Label_Defs >= g_Label_Defs_Extent)
    {
        g_Label_Defs_Extent += 50;
        g_Label_Defs = realloc(g_Label_Defs, 
                g_Label_Defs_Extent*sizeof(label_def_t));
        if (g_Label_Defs == NULL) 
        {
            fprintf(stderr, "Unable to allocate memory\n");
            exit(1);
        }
    }

    strcpy(g_Label_Defs[g_Num_Label_Defs].label, label);
    if (g_Use_Data)
    {
        g_Label_Defs[g_Num_Label_Defs].is_data = 1;
        g_Label_Defs[g_Num_Label_Defs].offset = g_Data_Memory_Index;
    }
    else
    {
        g_Label_Defs[g_Num_Label_Defs].is_data = 0;
        g_Label_Defs[g_Num_Label_Defs].offset = g_Memory_Index;
    }
}

void add_label_ref(char *label)
{
    if (g_Num_Label_Refs >= g_Label_Refs_Extent)
    {
        g_Label_Refs_Extent += 50;
        g_Label_Refs = realloc(g_Label_Refs, 
                g_Label_Refs_Extent*sizeof(label_def_t));
        if (g_Label_Refs == NULL) 
        {
            fprintf(stderr, "Unable to allocate memory\n");
            exit(1);
        }
    }

    strcpy(g_Label_Refs[g_Num_Label_Refs].label, label);
    if (g_Use_Data)
    {
        g_Label_Refs[g_Num_Label_Refs].is_data = 1;
        g_Label_Refs[g_Num_Label_Refs].offset = g_Data_Memory_Index;
        g_Data_Memory_Index++;
    }
    else
    {
        g_Label_Refs[g_Num_Label_Refs].is_data = 0;
        g_Label_Refs[g_Num_Label_Refs].offset = g_Memory_Index;
        g_Memory_Index++;
    }
}

static const int NUM_OPCODES = sizeof(op_list)/sizeof(opcode_t);

static const char *DELIMS = " \t\n";

static const char *HELP_STR =
 "slasm [-M<mem size>] [-version] <source file>\n";

void report_error(const char *msg)
{
    fprintf(stderr, "Error %s in line %d\n", msg, g_Line_Num);
    g_Num_Errors++;
}

void Process_Args(int argc, char **argv)
{
    int ii;
    for (ii=1; ii<argc; ii++)
    {
        if (argv[ii][0] == '-')
        {
            char *arg = &argv[ii][1];
            if (strcmp(arg, "help") == 0)
            {
                printf(HELP_STR);
                exit(0);
            }
            else if (argv[ii][1] == 'M')
                g_Memory_Size = atoi(&argv[ii][2]);
            else if (strcmp(arg, "version") == 0)
            {
                printf("slasm %s %s %s\n", VERSION, __DATE__, __TIME__);
                exit(0);
            }
            else
            {
                fprintf(stderr, "Unrecognized option %s\n", argv[ii]);
                fprintf(stderr, HELP_STR);
                exit(-1);
            }
        }
        else
        {
            // assume input file name
            strcpy(g_Input_File, argv[ii]);
        }
    }
}

int get_op_index(char *op_name)
{
    int ii;
    for (ii=0; ii<NUM_OPCODES; ii++)
    {
        if (strcasecmp(op_name, op_list[ii].op_name) == 0) return ii;
    }

    return -1;
}

void store(int value)
{
    if (g_Use_Data)
        g_Data_Memory[g_Data_Memory_Index++] = value;
    else
        g_Memory[g_Memory_Index++] = value;
}

void output_label_ref(char *label)
{
}

void process_dot_cmd(char *line)
{
    if (strcasecmp(line, ".dataseg") == 0)
        g_Use_Data = 1;
    else if (strcasecmp(line, ".codeseg") == 0)
        g_Use_Data = 0;
    else
        report_error("Unrecognized dot command");
}

void process_label(char *line)
{
    char *label = strtok(line, ":");

    if (label != NULL) label = strtok(label, DELIMS);

    if (label == NULL)
    {
        report_error("Invalid label syntax");
        return;
    }

    add_label_def(label);
}

void process_asm(char *line)
{
    char *op_name;
    char *op_param;
    int  op_index;

    op_name = strtok(line, DELIMS);
    if (op_name == NULL) return;

    op_index = get_op_index(op_name);

    if (op_index == -1)
    {
        report_error("Unrecognized opcoe");
        return;
    }

    store(op_index);

    op_param = strtok(NULL, DELIMS);

    if (op_list[op_index].num_params > 0)
    {
        if (op_param == NULL)
        {
            report_error("opcode requires a parameter");
            return;
        }

        if (op_param[0] == '$')
            output_label_ref(&op_param[1]);
        else
            store(atoi(op_param));
    }
}

int main(int argc, char** argv)
{
    FILE *input;
    char  line[1000];
    char *comment;

    printf("Num opcodes: %d\n", NUM_OPCODES);

    Process_Args(argc, argv);

    if (strlen(g_Input_File) == 0)
    {
        printf(HELP_STR);
        exit(1);
    }

    input = fopen(g_Input_File, "r");
    if (input == NULL)
    {
        fprintf(stderr, "Unable to open input: %s\n", g_Input_File);
        exit(2);
    }

    g_Memory = (int *)malloc(g_Memory_Size/sizeof(int));
    g_Data_Memory = (int *)malloc(g_Memory_Size/sizeof(int));
    if (g_Memory == NULL || g_Data_Memory == NULL)
    {
        fprintf(stderr, "Out of memory\n");
        exit(1);
    }

    while (fgets(line, sizeof(line), input) != NULL)
    {
        g_Line_Num++;

        // trim comments
        comment = strchr(line, ';');
        if (comment != NULL) *comment = 0;

        if (line[0] == '.') 
            process_dot_cmd(line);
        else if (strchr(line, ':') != NULL)
            process_label(line);
        else
            process_asm(line);
    }

    fclose(input);

    if (g_Num_Errors > 0)
    {
        fprintf(stderr, "%d errors while processing %s\n", 
                g_Num_Errors, g_Input_File);
        exit(1);
    }

    return 0;
}
