//*******************************************************
// Purpose: main program for lang compiler
//
// Author: Philip Howard
// Email:  phil.howard@oit.edu
//
// Date: 2/20/2015
//
//*******************************************************
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <unistd.h>     // for include path
#include <limits.h>     // for include path

#include "lex.h"
#include "cAstXml.h"
#include "cSemantics.h"
#include "cSizeOffset.h"
#include "cCodeGen.h"

#include "../version.h"

extern int yydebug;

char Include_Path[PATH_MAX] = "";

// set the include path to be the same as the directory where the EXE is.
void set_include_path()
{
    char *ptr;

    if (readlink("/proc/self/exe", Include_Path, PATH_MAX) == -1)
    {
        strcpy(Include_Path, "");
    }

    ptr = strrchr(Include_Path, '/');
    if (ptr != NULL)
    {
        ptr++;
        *ptr = 0;
    }
    strcat(Include_Path, "library/");
}

extern cDeclsList *yyast_root;

char Input_File[200] = "";
int  Do_Debug = 0;
int  Do_Ast = 0;
int  Do_Assembler = 1;
int  Do_Listing = 0;
int  Do_Type_Checks = 1;
void Process_Args(int argc, char **argv)
{
    static struct option long_opts[] = {
        {"ast",         no_argument, 0, 'a'},
        {"compile",     no_argument, 0, 'c'},
        {"debug",       no_argument, 0, 'g'},
        {"dbg",         no_argument, 0, 'g'},
        {"help",        no_argument, 0, 'h'},
        {"list",        no_argument, 0, 'l'},
        {"no_types",    no_argument, 0, 'n'},
        {"version",     no_argument, 0, 'v'},
        {"yydebug",     no_argument, 0, 'y'},
        {0, 0, 0, 0}
    };

    static char* HELP_MSG = (char *)
        "  -a --ast         output the AST\n"
        "  -c --compile     compile, but do not run the assembler\n"
        "  -g --dbg --debug Create output for the debugger\n"
        "  -h --help        Print this help message then exit\n"
        "  -l --list        Create listing file\n"
        "  -n --no_types    Skip type checks\n"
        "  -v --version     Print version then exit\n"
        "  -y --yydebug     Enable the yydebug of the parser\n"
        ;

    static char *single_opts = (char*)"acghlnvy";
    int opt;
    int opt_index;

    opt = getopt_long_only(argc, argv, single_opts, long_opts, &opt_index);
    while (opt != -1)
    {
        switch (opt)
        {
            case 'a':
                Do_Ast = 1;
                break;
            case 'c':
                Do_Assembler = 0;
                break;
            case 'g':
                Do_Debug = 1;
                Do_Ast = 1;
                break;
            case 'h':
                printf("%s\n", HELP_MSG);
                exit(0);
                break;
            case 'l':
                Do_Listing = 1;
                break;
            case 'n':
                Do_Type_Checks = 0;
                break;
            case 'v':
                std::cout << "stacklc " << VERSION << " " <<__DATE__ << " " 
                    << __TIME__ << std::endl;
                exit(0);
                break;
            case 'y':
                yydebug = 1;
                break;
        }
        opt = getopt_long_only(argc, argv, single_opts, long_opts, &opt_index);
    }

    // handle arguments without "-". Namely the filename
    if (optind < argc)
    {
        // assume input file name
        strcpy(Input_File, argv[optind]);
    }
}

//****************************************
int process_file(const char *filename, cDeclsList **program, int *total_errors)
{
    int result;

    // KLUDGE: should be handled within the scanner
    yylineno = 1;
    strcpy(yycurrentfile, filename);
    yyin = fopen(filename, "r");
    if (yyin == NULL)
    {
        std::cerr << "ERROR: Unable to open file " << filename << std::endl;
        exit(-1);
    }

    result = yyparse();
    while (yyast_root != NULL)
    {
        *total_errors += yynerrs;
        if (result == 0 && *total_errors == 0)
        {
            if (*program == NULL)
                *program = yyast_root;
            else
                (*program)->AddList(yyast_root);
        }
        result = yyparse();
    }

    fclose(yyin);

    if (result != 0) return result;
    if (*total_errors != 0) return 1;
    return 0;
}

//****************************************
int main(int argc, char **argv)
{
    char outfile_name[200];
    int result = 0;

    set_include_path();
    Process_Args(argc, argv);

    if (strlen(Input_File) == 0)
    {
        std::cerr << "Must specify an input file." << std::endl;
        exit(-1);
    }

    symbolTableRoot = new cSymbolTable();
    symbolTableRoot->InitDefaultTable();
    cDeclsList *program = NULL;
    int total_errors = 0;

    result = process_file(Input_File, &program, &total_errors);

    if (result == 0 && total_errors == 0 && program == NULL)
    {
        semantic_error("Source produced no output ", 0);
        exit(-2);
    }

    if (result == 0 && total_errors == 0)
    {
        yynerrs = 0;

        cSemantics semantics;
        semantics.VisitAllNodes(program);

        total_errors += yynerrs;
    }

    // need to recheck total_errors because semantics checks could add errors
    if (result == 0 && total_errors == 0)
    {
        cSizeOffset sizer;
        sizer.VisitAllNodes(program);

        if (Do_Ast)
        {
            strcpy(outfile_name, Input_File);
            char *ptr = strrchr(outfile_name, '.');
            if (ptr != NULL) *ptr = 0;
            strcat(outfile_name, ".ast");

            cAstXml astPrinter(outfile_name);
            astPrinter.VisitAllNodes(program);
        }

        // Create output filename
        strcpy(outfile_name, Input_File);
        char *ptr = strrchr(outfile_name, '.');
        if (ptr != NULL) *ptr = 0;
        strcat(outfile_name, ".sl");

        // coder must be in its own scope so it goes out of scope
        // and gets destructed
        {
            cCodeGen coder(outfile_name);
            coder.VisitAllNodes(program);
        }

        if (Do_Assembler)
        {
            string makebin = "slasm ";
            for (int ii=strlen(argv[0]); ii >= 0; ii-- )
            {
                if (argv[0][ii] == '/')
                {
                    argv[0][ii+1] = '\0';
                    makebin = argv[0] + makebin;
                    break;
                }
            }
            if (Do_Debug) makebin += "-dbg ";
            if (Do_Listing) makebin += "-list ";

            makebin += outfile_name;

            result = system(makebin.c_str());

            if (result < 0) fatal_error("Error creating binary output");
        }

    } else {
        std::cerr << total_errors << " Errors in compile" << std::endl;
        if (result == 0) result = total_errors;
    }

    return result;
}
