#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdint.h>
#include <unistd.h>     // for include path
#include <limits.h>     // for include path

#include "formatstr.h"
#include "../version.h"

#include "slasm.h"

#define LINES_PER_HEADING 50

static char g_Include_Path[PATH_MAX];

static char g_Function[256] = "";
static char g_Source_File[256] = "";
static int g_Source_Line = 0;
static int g_Memory_Size = 65536;
static int g_Num_Errors = 0;
static int g_Line_Num = 0;
static int32_t* g_Memory;
static int32_t* g_Data_Memory;
static char g_Header[2048] = "";
static int g_Memory_Index;
static int g_Data_Memory_Index;
static int g_Use_Data = 0;
static char g_Interrupt[256] = "";
static char g_Systrap[256] = "";
static char g_Startup[256] = "";

static int g_Make_Debug_Listing = 0;
static FILE *g_Debug_Listing = NULL;
static int g_Make_Listing = 0;
static FILE *g_Listing = NULL;

#define NUM_OPCODES (sizeof(op_list)/sizeof(opcode_t))
static const char *DELIMS = " \t\n";
static const char *HELP_STR =
 "slasm [-defs] [-M<mem size>] [-version] [-list] [-dbg] <source files>\n";

//**************************************
// label processing
//**************************************
typedef struct
{
    char label[50];
    int  is_data;
    int32_t  offset;
} label_def_t;

static label_def_t *g_Label_Defs = NULL;
static label_def_t *g_Label_Refs = NULL;
static int g_Num_Label_Defs = 0;
static int g_Num_Label_Refs = 0;
static int g_Label_Defs_Extent = 0;
static int g_Label_Refs_Extent = 0;

static void add_label_def(const char *label)
{
    if (g_Num_Label_Defs >= g_Label_Defs_Extent)
    {
        g_Label_Defs_Extent += 50;
        g_Label_Defs = (label_def_t*)realloc(g_Label_Defs, 
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

static void add_label_ref(const char *label)
{
    if (g_Num_Label_Refs >= g_Label_Refs_Extent)
    {
        g_Label_Refs_Extent += 50;
        g_Label_Refs = (label_def_t*)realloc(g_Label_Refs, 
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

static label_def_t *get_label_def(const char *label)
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

static void fixup_offsets()
{
    int ii;

    for (ii=0; ii<g_Num_Label_Refs; ii++)
    {
        if (g_Label_Refs[ii].is_data)
        {
            g_Label_Refs[ii].offset += g_Memory_Index;
            g_Label_Refs[ii].is_data = 0;
        }
    }

    for (ii=0; ii<g_Num_Label_Defs; ii++)
    {
        if (g_Label_Defs[ii].is_data)
        {
            g_Label_Defs[ii].offset += g_Memory_Index;
            g_Label_Defs[ii].is_data = 0;
        }
    }
}

static void clean_locals()
{
    int ii;

    for (ii=0; ii<g_Num_Label_Refs; ii++)
    {
        if (g_Label_Refs[ii].label[0] == '$')
            g_Label_Refs[ii].label[0] = '#';
    }

    for (ii=0; ii<g_Num_Label_Defs; ii++)
    {
        if (g_Label_Defs[ii].label[0] == '$')
            g_Label_Defs[ii].label[0] = '#';
    }
}

static void update_single_label(int label_index)
{
    int32_t mem_address;
    int32_t label_address;
    label_def_t *label_def;

    label_def = get_label_def(g_Label_Refs[label_index].label);

    if (label_def == NULL) 
    {
        fprintf(stderr, "Undefined label: %s\n", 
                g_Label_Refs[label_index].label);
        exit(1);
    }

    mem_address = g_Label_Refs[label_index].offset;
    if (g_Label_Refs[label_index].is_data) mem_address += g_Memory_Index;

    label_address = label_def->offset;
    if (label_def->is_data) label_address += g_Memory_Index;
    label_address *= sizeof(int32_t);

    g_Memory[mem_address] = label_address;
    if (g_Make_Listing)
    {
        fprintf(g_Listing, "Fixup %s at %d to %d\n",
                g_Label_Refs[label_index].label,
                mem_address*(int32_t)sizeof(int32_t), label_address);
    }
}

static void update_labels(int do_locals)
{
    int ii;
    int32_t mem_address;
    int32_t label_address;

    fixup_offsets();

    for (ii=0; ii<g_Num_Label_Refs; ii++)
    {
        if (do_locals && g_Label_Refs[ii].label[0] == '$')
        {
            update_single_label(ii);
        }
        else if (!do_locals && 
                (g_Label_Refs[ii].label[0] != '$') &&
                (g_Label_Refs[ii].label[0] != '#'))
        {
            update_single_label(ii);
        }
    }

    if (do_locals) clean_locals();
}

//**************************************
// end label processing
//**************************************

//**************************************
// File name processing
//**************************************

static char **g_File_List = NULL;
static int g_Num_Files = 0;
static int g_Files_Extent = 0;

static void Add_File(char *filename)
{
    int ii;

    char *ptr;
    char path[PATH_MAX] = "";

    if (*filename == '"')
    {
        ptr = strrchr(filename, '"');
        if (ptr != NULL) *ptr = 0;
        filename++;
    }
    else if (*filename == '<')
    {
        ptr = strrchr(filename, '>');
        if (ptr != NULL) *ptr = 0;
        filename++;
        strcpy(path, g_Include_Path);
    }

    strcat(path, filename);

    for (ii=0; ii<g_Num_Files; ii++)
    {
        // if the file is already in the list, don't re-add it.
        if (strcmp(g_File_List[ii], path) == 0) return;
    }

    if (g_Num_Files >= g_Files_Extent)
    {
        g_Files_Extent += 20;
        g_File_List = (char**)realloc(g_File_List, sizeof(char **)*g_Files_Extent);
        assert(g_File_List != NULL);
    }

    g_File_List[g_Num_Files] = (char*)malloc(strlen(path)+1);
    strcpy(g_File_List[g_Num_Files], path);
    g_Num_Files++;
}

//**************************************
// End file name processing
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
                printf("%s", HELP_STR);
                exit(0);
            }
            else if (strcmp(arg, "dbg") == 0)
                g_Make_Debug_Listing = 1;
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
                fprintf(stderr, "%s", HELP_STR);
                exit(-1);
            }
        }
        else
        {
            // assume input file name
            Add_File(argv[ii]);
        }
    }
}

static int32_t get_op_index(const char *op_name)
{
    int32_t ii;
    for (ii=0; ii<NUM_OPCODES; ii++)
    {
        if (strcasecmp(op_name, op_list[ii].op_name) == 0) return ii;
    }

    return -1;
}

static void store(int32_t value)
{
    if ((g_Data_Memory_Index + g_Memory_Index) > g_Memory_Size/4)
    {
        fprintf(stderr, "Execeeded stackl memory. Use -M to increase size\n");
        exit(1);
    }

    if (g_Use_Data)
        g_Data_Memory[g_Data_Memory_Index++] = value;
    else
        g_Memory[g_Memory_Index++] = value;
}

static void process_data(char *str)
{
    int32_t value;

    if (str[0] == '@')
        add_label_ref(&str[1]);
    else
        store(atoi(str));
}

static void process_string(char *str)
{
    int32_t *int_ptr;
    char aligned_str[1000];
    int32_t ii;

    str = format_string(str);
    if (str == NULL)
    {
        report_error("Invalid string format");
        return;
    }

    strcpy(aligned_str, str);
    int_ptr = (int32_t *)aligned_str;

    // strlen (including NULL) rounded up to a word size.
    int str_len = (strlen(aligned_str)+sizeof(int32_t))/sizeof(int32_t);
    for (ii=0; ii<str_len; ii++)
    {
        store(int_ptr[ii]);
    }
}

static void process_pound_cmd(char *line)
{
    char *name;

    if (strncmp(line, "#interrupt", strlen("#interrupt")) == 0)
    {
        if (strlen(g_Interrupt) != 0) 
        {
            report_error("Multiple interrupt definitions");
            return;
        }
        name = strtok(line, " \n");
        name = strtok(NULL, " \n");
        if (name == NULL || strlen(name)==0)
        {
            report_error("Invalid interrupt definition");
            return;
        }
        strcpy(g_Interrupt, name);
    }
    else if (strncmp(line, "#systrap", strlen("#systrap")) == 0)
    {
        if (strlen(g_Systrap) != 0) 
        {
            report_error("Multiple systrap definitions");
            return;
        }
        name = strtok(line, " \n");
        name = strtok(NULL, " \n");
        if (name == NULL || strlen(name)==0)
        {
            report_error("Invalid systrap definition");
            return;
        }
        strcpy(g_Systrap, name);
    }
    else if (strncmp(line, "#startup", strlen("#startup")) == 0)
    {
        if (strlen(g_Startup) != 0) 
        {
            report_error("Multiple startup definitions");
            return;
        }
        name = strtok(line, " \n");
        name = strtok(NULL, " \n");
        if (name == NULL || strlen(name)==0)
        {
            report_error("Invalid startup definition");
            return;
        }
        strcpy(g_Startup, name);
    }
    else if (strncmp(line, "#library", strlen("#library")) == 0)
    {
        name = strtok(line, " \n");
        name = strtok(NULL, " \n");
        if (name != NULL && strlen(name)!=0)
        {
            Add_File(name);
        }
    }
    else
    {
        strcat(g_Header, &line[1]);
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
    else if (strcasecmp(token, ".source") == 0)
    {
        str = strtok(NULL, "\n ");
        strcpy(g_Source_File, str);
        str = strtok(NULL, "\n ");
        g_Source_Line = atoi(str);
    }
    else if (strcasecmp(token, ".function") == 0)
    {
        str = strtok(NULL, "\n ");
        strcpy(g_Function, str);
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
    int32_t  op_index;

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

        if (op_param[0] == '@')
            add_label_ref(&op_param[1]);
        else
            store(atoi(op_param));
    }
}

static char *make_filename(char *in_filename, const char *extension)
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

    fprintf(bin_file, "stackl %s slasm\n", VERSION);

    status = fprintf(bin_file, "%s", g_Header);
    if (status < 0)
    {
        fprintf(stderr, "Unable to create output file\n");
        fclose(bin_file);
        exit(1);
    }

    status = fprintf(bin_file, "begindata\n");
    if (status < 0)
    {
        fprintf(stderr, "Unable to create output file\n");
        fclose(bin_file);
        exit(1);
    }

    status = fwrite(g_Memory, 
            (g_Memory_Index + g_Data_Memory_Index) * sizeof(int32_t), 
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
    static int line_count = 0;
    int32_t word_size = sizeof(g_Memory[0]);
    if (line_count % LINES_PER_HEADING == 0)
    {
        fprintf(listing, "%6s  %6s\n", "Code", "Data");
    }
    line_count++;

    fprintf(listing, "%6d  %6d     %s", 
            g_Memory_Index*word_size, g_Data_Memory_Index*word_size, line);
}

static void write_debug_listing(FILE *listing, char *line)
{
    int32_t word_size = sizeof(g_Memory[0]);
    fprintf(listing, "Source: %6d %6d %s %s\n", 
            g_Memory_Index*word_size, g_Source_Line, g_Function, g_Source_File);
}

static void write_symbol_table(FILE *listing)
{
    int32_t word_size = sizeof(g_Memory[0]);
    int32_t label_address;
    int32_t ii;

    fprintf(listing, "\n********************************\nSymbol Table\n");

    for (ii=0; ii<g_Num_Label_Defs; ii++)
    {
        label_address = g_Label_Defs[ii].offset;
        if (g_Label_Defs[ii].is_data) label_address += g_Memory_Index;
        fprintf(listing, "%6d   %s\n",
                label_address*word_size, g_Label_Defs[ii].label);
    }
}

static int process_file(char *filename, FILE *listing)
{
    FILE *input;
    char  line[1000];
    char *comment;

    //printf("Processing: %s\n", filename);
    input = fopen(filename, "r");
    if (input == NULL)
    {
        fprintf(stderr, "Unable to open input: %s\n", filename);
        exit(2);
    }

    g_Line_Num = 0;
    while (fgets(line, sizeof(line), input) != NULL)
    {
        g_Line_Num++;

        if (g_Make_Listing) write_listing(listing, line);
        if (g_Make_Debug_Listing) write_debug_listing(g_Debug_Listing, line);

        // trim comments
        comment = strchr(line, ';');
        if (comment != NULL) *comment = 0;

        if (line[0] == '.')
            process_dot_cmd(line);
        else if (line[0] == '#')
            process_pound_cmd(line);
        else if (strchr(line, ':') != NULL)
            process_label(line);
        else
            process_asm(line);
    }

    fclose(input);

    // append the data memory to the actual memory
    memcpy(&g_Memory[g_Memory_Index],
            g_Data_Memory,
            g_Data_Memory_Index*sizeof(int32_t));

    // fix-up the label references
    update_labels(1);

    // this needs to be here for now until I figure out how to handle non-locals
    // after updating g_Memory_Index
    //update_labels(0);

    g_Memory_Index += g_Data_Memory_Index;
    g_Data_Memory_Index = 0;

    if (g_Num_Errors > 0)
    {
        fprintf(stderr, "%d errors while processing %s\n", 
                g_Num_Errors, filename);
        exit(1);
    }

    return 0;
}

static void update_single_vector(int offset, const char *name)
{
    int32_t label_address;
    label_def_t *label_def;

    label_def = get_label_def(name);

    if (label_def == NULL)
    {
        fprintf(stderr, "Undefined label: %s\n", name);
        exit(1);
    }

    label_address = label_def->offset;
    label_address *= sizeof(int32_t);

    g_Memory[offset] = label_address;
    if (g_Make_Listing)
    {
        fprintf(g_Listing, "Fixup %s at %d to %d\n",
                name, offset*(int32_t)sizeof(int32_t), label_address);
    }
}
static void update_vectors()
{
    label_def_t *label_def;

    if (strlen(g_Interrupt) != 0)
        update_single_vector(0, g_Interrupt);
    else
        g_Memory[0] = -1;

    if (strlen(g_Systrap) != 0)
        update_single_vector(1, g_Systrap);
    else
        g_Memory[1] = -1;

    g_Memory[2] = get_op_index("jump");
    if (strlen(g_Startup) != 0)
    {
        update_single_vector(3, g_Startup);
    }
    else
    {
        g_Memory[3] = g_Memory_Index*sizeof(int32_t);
        g_Memory[g_Memory_Index++] = get_op_index("call");
        update_single_vector(g_Memory_Index++, "main");
        g_Memory[g_Memory_Index++] = get_op_index("halt");
    }
}

// set the include path to be the same as the directory where the EXE is.
void set_include_path()
{
    char *ptr;

    if (readlink("/proc/self/exe", g_Include_Path, PATH_MAX) == -1)
    {
        strcpy(g_Include_Path, "");
    }

    ptr = strrchr(g_Include_Path, '/');
    if (ptr != NULL)
    {
        ptr++;
        *ptr = 0;
    }
    strcat(g_Include_Path, "library/");
}

int main(int argc, char** argv)
{
    FILE *input;
    char  line[1000];
    char *comment;
    int ii;

    set_include_path();

    Process_Args(argc, argv);

    if (g_Num_Files == 0)
    {
        printf("%s", HELP_STR);
        exit(1);
    }

    if (g_Make_Debug_Listing)
    {
        g_Debug_Listing = fopen(make_filename(g_File_List[0], ".dbg"), "w");
        if (g_Debug_Listing == NULL)
        {
            fprintf(stderr, "Unable to open debug listing file\n");
            exit(2);
        }
        for (ii=0; ii<g_Num_Files; ii++)
        {
            fprintf(g_Debug_Listing, "FILE: %s\n", g_File_List[ii]);
        }
    }

    if (g_Make_Listing)
    {
        g_Listing = fopen(make_filename(g_File_List[0], ".lst"), "w");
        if (g_Listing == NULL)
        {
            fprintf(stderr, "Unable to open listing file\n");
            exit(2);
        }
        for (ii=0; ii<g_Num_Files; ii++)
        {
            fprintf(g_Listing, "; FILE: %s\n", g_File_List[ii]);
        }
    }

    g_Memory = (int32_t *)malloc(g_Memory_Size);
    g_Data_Memory = (int32_t *)malloc(g_Memory_Size);
    if (g_Memory == NULL || g_Data_Memory == NULL)
    {
        fprintf(stderr, "Out of memory\n");
        exit(1);
    }

    memset(g_Memory, 0xDF, g_Memory_Size);
    memset(g_Data_Memory, 0xBA, g_Memory_Size);

    // leave room for vector table and startup jump
    g_Memory_Index = 4;

    for (ii=0; ii<g_Num_Files; ii++)
    {
        process_file(g_File_List[ii], g_Listing);
    }

    update_labels(0);

    update_vectors();

    write_output(g_File_List[0]);

    if (g_Make_Listing)
    {
        write_symbol_table(g_Listing);
        fclose(g_Listing);
    }

    if (g_Make_Debug_Listing)
    {
        write_symbol_table(g_Debug_Listing);
        fclose(g_Debug_Listing);
    }
    return 0;
}
