#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "formatstr.h"
#include "../version.h"

static char g_Input_File[256] = "";
static int g_Memory_Size = 65536;
static int g_Num_Errors = 0;
static int g_Line_Num = 0;
static int* g_Memory;
static int* g_Data_Memory;
static char g_Header[2048] = "";
static int g_Memory_Index;
static int g_Data_Memory_Index;
static int g_Use_Data = 0;

static int g_Make_Listing = 0;

#define NUM_OPCODES (sizeof(op_list)/sizeof(opcode_t))
static const char *DELIMS = " \t\n";
static const char *HELP_STR =
 "slasm [-defs] [-M<mem size>] [-version] [-list] <source file>\n";

typedef struct
{
    char *op_name;
    int  num_params;
} opcode_t;

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
    {"popcvarind", 0},
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
    {"trace_on", 0},
    {"trace_off", 0},
    {"clid", 0},
    {"seid", 0},
    {"illegal", 0}
};

//**************************************
// label processing
//**************************************
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

static void add_label_def(char *label)
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

    g_Num_Label_Defs++;
}

static void add_label_ref(char *label)
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

    g_Num_Label_Refs++;
}

static label_def_t *get_label_def(char *label)
{
    int ii;

    for (ii=0; ii<g_Num_Label_Defs; ii++)
    {
       if (strcmp(label, g_Label_Defs[ii].label) == 0) return &g_Label_Defs[ii];
    }

    fprintf(stderr, "Undefined label: %s\n", label);
    exit(1);

    return NULL;
}

static void update_labels()
{
    int ii;
    int mem_address;
    int label_address;

    label_def_t *label_def;

    for (ii=0; ii<g_Num_Label_Refs; ii++)
    {
        label_def = get_label_def(g_Label_Refs[ii].label);
        if (label_def == NULL) 
        {
            fprintf(stderr, "Undefined label: %s\n", g_Label_Refs[ii].label);
            exit(1);
        }

        mem_address = g_Label_Refs[ii].offset;
        if (g_Label_Refs[ii].is_data) mem_address += g_Memory_Index;

        label_address = label_def->offset;
        if (label_def->is_data) label_address += g_Memory_Index;
        label_address *= sizeof(int);

        g_Memory[mem_address] = label_address;
    }
}

//**************************************
// end label processing
//**************************************

static void report_error(const char *msg)
{
    fprintf(stderr, "Error %s in line %d\n", msg, g_Line_Num);
    g_Num_Errors++;
}

static void strupr(char *str)
{
    while (*str)
    {
        *str = toupper(*str);
        str++;
    }
}

static void make_def_file()
{
    FILE *def_file;
    int  ii;
    char opcode[50];

    def_file = fopen("opcode_defs.h", "w");
    if (def_file == NULL)
    {
        fprintf(stderr, "Unable to open opcode_defs.h\n");
        exit(1);
    }

    for (ii=0; ii<NUM_OPCODES; ii++)
    {
        strcpy(opcode, op_list[ii].op_name);
        strcat(opcode, "_OP");
        strupr(opcode);
        fprintf(def_file, "#define %s %d\n", opcode, ii);
    }

    fclose(def_file);
}

static void Process_Args(int argc, char **argv)
{
    int ii;
    for (ii=1; ii<argc; ii++)
    {
        if (argv[ii][0] == '-')
        {
            char *arg = &argv[ii][1];
            if (strcmp(arg, "defs") == 0)
            {
                make_def_file();
                exit(0);
            }
            else if (strcmp(arg, "help") == 0)
            {
                printf(HELP_STR);
                exit(0);
            }
            else if (strcmp(arg, "list") == 0)
                g_Make_Listing = 1;
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

static int get_op_index(char *op_name)
{
    int ii;
    for (ii=0; ii<NUM_OPCODES; ii++)
    {
        if (strcasecmp(op_name, op_list[ii].op_name) == 0) return ii;
    }

    return -1;
}

static void store(int value)
{
    if (g_Use_Data)
        g_Data_Memory[g_Data_Memory_Index++] = value;
    else
        g_Memory[g_Memory_Index++] = value;
}

static void process_data(char *str)
{
    int value;

    if (str[0] == '$')
        add_label_ref(&str[1]);
    else
        store(atoi(str));
}

static void process_string(char *str)
{
    int *int_ptr;
    char aligned_str[1000];
    int ii;

    str = format_string(str);
    if (str == NULL)
    {
        report_error("Invalid string format");
        return;
    }

    strcpy(aligned_str, str);
    int_ptr = (int *)aligned_str;

    // strlen (including NULL) rounded up to a word size.
    int str_len = (strlen(aligned_str)+sizeof(int))/sizeof(int);
    for (ii=0; ii<str_len; ii++)
    {
        store(int_ptr[ii]);
    }
}

static void process_dot_cmd(char *line)
{
    char *token;
    char *str;

    token = strtok(line, DELIMS);

    if (strcasecmp(token, ".dataseg") == 0)
        g_Use_Data = 1;
    else if (strcasecmp(token, ".codeseg") == 0)
        g_Use_Data = 0;
    else if (strcasecmp(token, ".string") == 0)
    {
        str = strtok(NULL, "\n");
        process_string(str);
    }
    else if (strcasecmp(token, ".block") == 0)
    {
        str = strtok(NULL, "\n");
        if (g_Use_Data)
            g_Data_Memory_Index += atoi(str);
        else
            g_Memory_Index += atoi(str);
    }
    else if (strcasecmp(token, ".data") == 0)
    {
        str = strtok(NULL, "\n");
        process_data(str);
    }
    else
        report_error("Unrecognized dot command");
}

static void process_label(char *line)
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

static void process_asm(char *line)
{
    char *op_name;
    char *op_param;
    int  op_index;

    op_name = strtok(line, DELIMS);
    if (op_name == NULL) return;

    op_index = get_op_index(op_name);

    if (op_index == -1)
    {
        report_error("Unrecognized opcode");
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
            add_label_ref(&op_param[1]);
        else
            store(atoi(op_param));
    }
}

static char *make_filename(char *in_filename, char *extension)
{
    static char out_filename[256];
    FILE *bin_file;
    int status;

    strcpy(out_filename, in_filename);
    
    char *end_of_root = strstr(out_filename, ".sl");
    if (end_of_root != NULL)
        strcpy(end_of_root, extension);
    else
        strcat(out_filename, extension);

    return out_filename;
}

static void write_output(char *in_filename)
{
    FILE *bin_file;
    int status;

    bin_file = fopen(make_filename(in_filename, ".slb"), "w");

    if (bin_file == NULL)
    {
        fprintf(stderr, "Unable to open output file\n");
        exit(1);
    }

    status = fprintf(bin_file, g_Header);
    if (status < 0)
    {
        fprintf(stderr, "Unable to create output file\n");
        fclose(bin_file);
        exit(1);
    }

    status = fwrite(g_Memory, 
            (g_Memory_Index + g_Data_Memory_Index) * sizeof(int), 
            1, bin_file);
    if (status != 1)
    {
        fprintf(stderr, "Unable to create output file\n");
        fclose(bin_file);
        exit(1);
    }

    fclose(bin_file);
}

static void write_listing(FILE *listing, char *line)
{
    int word_size = sizeof(g_Memory[0]);
    fprintf(listing, "%6d  %6d     %s", 
            g_Memory_Index*word_size, g_Data_Memory_Index*word_size, line);
}

static void write_symbol_table(FILE *listing)
{
    int word_size = sizeof(g_Memory[0]);
    int label_address;
    int ii;

    fprintf(listing, "\n********************************\nSymbol Table\n");

    for (ii=0; ii<g_Num_Label_Defs; ii++)
    {
        label_address = g_Label_Defs[ii].offset;
        if (g_Label_Defs[ii].is_data) label_address += g_Memory_Index;
        fprintf(listing, "%6d   %s\n", 
                label_address*word_size, g_Label_Defs[ii].label);
    }
}

int main(int argc, char** argv)
{
    FILE *input;
    FILE *listing = NULL;
    char  line[1000];
    char *comment;

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

    if (g_Make_Listing)
    {
        listing = fopen(make_filename(g_Input_File, ".lst"), "w");
        if (listing == NULL)
        {
            fprintf(stderr, "Unable to open listing file\n");
            exit(2);
        }
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

        if (g_Make_Listing) write_listing(listing, line);

        // trim comments
        comment = strchr(line, ';');
        if (comment != NULL) *comment = 0;

        if (line[0] == '.') 
            process_dot_cmd(line);
        else if (line[0] == '#')
            strcat(g_Header, &line[1]);
        else if (strchr(line, ':') != NULL)
            process_label(line);
        else
            process_asm(line);
    }

    fclose(input);

    // append the data memory to the actual memory
    memcpy(&g_Memory[g_Memory_Index], 
            g_Data_Memory, 
            g_Data_Memory_Index*sizeof(int));

    // fix-up the label references
    update_labels();

    if (g_Num_Errors > 0)
    {
        fprintf(stderr, "%d errors while processing %s\n", 
                g_Num_Errors, g_Input_File);
        exit(1);
    }

    write_output(g_Input_File);

    if (g_Make_Listing) 
    {
        write_symbol_table(listing);
        fclose(listing);
    }
    return 0;
}
